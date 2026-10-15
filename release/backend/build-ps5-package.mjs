// Builds the PS5 dynamic resources package (ps5/dynamic_resources_ps5.rpkg) from the player's own
// game files. Node port of tools/ps5_dynamic_resources.py + tools/locr.py, so the release needs
// nothing but the bundled Node and the system curl.
//
// After mounting the package it downloads, the PS5 client loads
// [assembly:/repository/pro.repo](fullgame,dynamic).ps5_repo and goes offline without it. No PS5
// game package carries it, so it is copied from the console's base repository in chunk0.rpkg (read
// over FTP byte ranges), next to the backend's LOCR and debrief JSON re-keyed to their ".ps5_" ids.
// The output is derived from the user's own game files: never share it.
//
//   node build-ps5-package.mjs <ps5-ip> [--chunk0 <path or ftp:// URL>] [--out <file>]
import { createHash } from "node:crypto"
import { execFileSync } from "node:child_process"
import { existsSync, mkdirSync, readFileSync, renameSync, writeFileSync } from "node:fs"
import { dirname, join } from "node:path"
import { fileURLToPath } from "node:url"
import { crc32 } from "node:zlib"

const HERE = dirname(fileURLToPath(import.meta.url))
const TITLE = "PPSA16387"
const XOR_KEY = [0xdc, 0x45, 0xa6, 0x9c, 0xd3, 0x72, 0x4c, 0xab]
const MASK56 = 0x00ffffffffffffffn

const runtimeId = (p) => createHash("md5").update(p.toLowerCase()).digest().readBigUInt64BE(0) & MASK56
const hex = (h) => h.toString(16).toUpperCase().padStart(16, "0")

const BASE_REPO = runtimeId("[assembly:/repository/pro.repo].ps5_repo")
const DYNAMIC_REPO = runtimeId("[assembly:/repository/pro.repo](fullgame,dynamic).ps5_repo")
const REKEY_PATHS = [
    "[assembly:/localization/hitman6/conversations/ui/pro/menutext_drp.sweetmenutext].{}_localized-textlist",
    "[assembly:/_pro/online/default/cloudstorage/resources/pages/common/views/debriefing/" +
        "debriefingfailedbuttons.json].{}_json",
]
const REKEY = new Map(
    REKEY_PATHS.map((p) => [runtimeId(p.replace("{}", "pc")), [runtimeId(p.replace("{}", "ps5")), p.replace("{}", "ps5")]]),
)

function curl(args) {
    return execFileSync("curl", ["-fsS", "-m", "600", ...args], { maxBuffer: 1 << 30, stdio: ["ignore", "pipe", "pipe"] })
}

function readRange(src, start, length) {
    if (src.startsWith("ftp://")) return curl(["-r", `${start}-${start + length - 1}`, src])
    const buf = readFileSync(src)
    return buf.subarray(start, start + length)
}

function parseHeader(head) {
    const magic = head.subarray(0, 4).toString("latin1")
    const off = magic === "2KPR" ? 13 : magic === "GKPR" ? 4 : -1
    if (off < 0) throw new Error("not an RPKG")
    return [off + 12, head.readUInt32LE(off), head.readUInt32LE(off + 4), head.readUInt32LE(off + 8)]
}

function parseTableEntry(table, off) {
    const rtype = Buffer.from(table.subarray(off, off + 4)).reverse().toString("latin1")
    const refSize = table.readUInt32LE(off + 4)
    const size = table.readUInt32LE(off + 12)
    const end = off + 24 + refSize
    const refs = refSize ? table.readUInt32LE(off + 24) & 0x3fffffff : 0
    return [{ type: rtype, raw: Buffer.from(table.subarray(off, end)), size, refs }, end]
}

function lz4Decompress(src, size) {
    const out = Buffer.alloc(size)
    let i = 0
    let o = 0
    while (i < src.length) {
        const token = src[i++]
        let lit = token >> 4
        if (lit === 15) for (let b = 255; b === 255; ) lit += b = src[i++]
        src.copy(out, o, i, i + lit)
        i += lit
        o += lit
        if (i >= src.length) break
        const offset = src[i] | (src[i + 1] << 8)
        i += 2
        let match = (token & 15) + 4
        if ((token & 15) === 15) for (let b = 255; b === 255; ) match += b = src[i++]
        const start = o - offset
        if (offset === 0 || start < 0) throw new Error("bad lz4 offset")
        for (let k = 0; k < match; k++) out[o++] = out[start + k]
    }
    if (o !== size) throw new Error(`lz4: got ${o} bytes, expected ${size}`)
    return out
}

const indexCache = new Map()
function loadIndex(src) {
    if (!indexCache.has(src)) {
        const [indexAt, count, indexSize, tableSize] = parseHeader(readRange(src, 0, 64))
        indexCache.set(src, { indexAt, count, indexSize, tableSize, index: readRange(src, indexAt, indexSize) })
    }
    return indexCache.get(src)
}

function contains(src, wanted) {
    const { count, index } = loadIndex(src)
    for (let i = 0; i < count; i++) if (index.readBigUInt64LE(i * 20) === wanted) return true
    return false
}

function extract(src, wanted) {
    const { indexAt, count, indexSize, tableSize, index } = loadIndex(src)
    let pos = -1
    let dataOff = 0
    let dsize = 0
    for (let i = 0; i < count; i++) {
        if (index.readBigUInt64LE(i * 20) === wanted) {
            pos = i
            dataOff = Number(index.readBigUInt64LE(i * 20 + 8))
            dsize = index.readUInt32LE(i * 20 + 16)
            break
        }
    }
    if (pos < 0) throw new Error(`${hex(wanted)} is not in ${src}`)
    const packed = dsize & 0x3fffffff
    const xored = (dsize & 0x80000000) !== 0
    const table = readRange(src, indexAt + indexSize, tableSize)
    let entry
    let off = 0
    for (let i = 0; i <= pos; i++) [entry, off] = parseTableEntry(table, off)
    let stored = Buffer.from(readRange(src, dataOff, packed || entry.size))
    if (xored) for (let k = 0; k < stored.length; k++) stored[k] ^= XOR_KEY[k % 8]
    return [entry, packed ? lz4Decompress(stored, entry.size) : stored]
}

function readPackage(path) {
    const blob = readFileSync(path)
    const [indexAt, count, indexSize, tableSize] = parseHeader(blob.subarray(0, 64))
    const table = blob.subarray(indexAt + indexSize, indexAt + indexSize + tableSize)
    const out = []
    let toff = 0
    for (let i = 0; i < count; i++) {
        const p = indexAt + i * 20
        const hash = blob.readBigUInt64LE(p)
        const doff = Number(blob.readBigUInt64LE(p + 8))
        const sizeField = blob.readUInt32LE(p + 16)
        let entry
        ;[entry, toff] = parseTableEntry(table, toff)
        const n = sizeField & 0x3fffffff
        out.push({ hash, sizeField, stored: Buffer.from(blob.subarray(doff, doff + (n || entry.size))), raw: entry.raw, type: entry.type })
    }
    return out
}

function writePackage(path, resources) {
    const indexSize = 20 * resources.length
    const table = Buffer.concat(resources.map((r) => r.raw))
    const dataAt = 25 + indexSize + table.length
    const index = Buffer.alloc(indexSize)
    let dataLen = 0
    resources.forEach((r, i) => {
        index.writeBigUInt64LE(r.hash, i * 20)
        index.writeBigUInt64LE(BigInt(dataAt + dataLen), i * 20 + 8)
        index.writeUInt32LE(r.sizeField >>> 0, i * 20 + 16)
        dataLen += r.stored.length
    })
    const head = Buffer.alloc(25)
    head.write("2KPR", 0, "latin1")
    head.writeUInt32LE(1, 4)
    head.write("xx", 11, "latin1")
    head.writeUInt32LE(resources.length, 13)
    head.writeUInt32LE(indexSize, 17)
    head.writeUInt32LE(table.length, 21)
    mkdirSync(dirname(path), { recursive: true })
    writeFileSync(path + ".tmp", Buffer.concat([head, index, table, ...resources.map((r) => r.stored)]))
    renameSync(path + ".tmp", path)
}

// LOCR: a 0 byte, one uint32 offset per language slot, then per language a uint32 count and per
// string CRC32(key) | length | XTEA-encrypted UTF-8 padded to 8 bytes | one 0 byte.
const LOCR_KEY = [0x53527737, 0x7506499e, 0xbd39aee3, 0xa59e7268]
const LANGS = ["xx", "en", "fr", "it", "de", "es", "ru", "cn", "tc", "jp"]
const LANG_NAMES = { en: "english", fr: "french", it: "italian", de: "german", es: "spanish", ru: "russian",
    cn: "chineseSimplified", tc: "chineseTraditional", jp: "japanese" }

function xteaEncrypt(bytes) {
    const b = Buffer.alloc(Math.ceil(bytes.length / 8) * 8)
    bytes.copy(b)
    for (let i = 0; i < b.length; i += 8) {
        let v0 = b.readUInt32LE(i)
        let v1 = b.readUInt32LE(i + 4)
        let s = 0
        for (let r = 0; r < 32; r++) {
            v0 = (v0 + ((((v1 << 4) ^ (v1 >>> 5)) + v1) ^ (s + LOCR_KEY[s & 3]))) >>> 0
            s = (s + 0x9e3779b9) >>> 0
            v1 = (v1 + ((((v0 << 4) ^ (v0 >>> 5)) + v0) ^ (s + LOCR_KEY[(s >>> 11) & 3]))) >>> 0
        }
        b.writeUInt32LE(v0, i)
        b.writeUInt32LE(v1, i + 4)
    }
    return b
}

const keyHash = (k) => crc32(Buffer.from(k.toUpperCase()))

function encodeLocr(langs) {
    const sections = langs.map(([, items]) => {
        const parts = [Buffer.alloc(4)]
        parts[0].writeUInt32LE(items.length)
        for (const [h, text] of items) {
            const enc = xteaEncrypt(Buffer.from(text, "utf8"))
            const hd = Buffer.alloc(8)
            hd.writeUInt32LE(h, 0)
            hd.writeUInt32LE(enc.length, 4)
            parts.push(hd, enc, Buffer.alloc(1))
        }
        return Buffer.concat(parts)
    })
    const head = Buffer.alloc(1 + 4 * sections.length)
    let offset = head.length
    sections.forEach((s, i) => {
        head.writeUInt32LE(offset, 1 + 4 * i)
        offset += s.length
    })
    return Buffer.concat([head, ...sections])
}

function buildLocr(locale, extraStrings) {
    const langs = [["xx", [[keyHash("UI_DRP_001"), "UI_DRP_001"]]]]
    for (const lang of LANGS.slice(1)) {
        const items = Object.entries(locale[LANG_NAMES[lang]]).map(([k, v]) => [keyHash(k), v])
        const seen = new Set(items.map(([h]) => h))
        for (const [k, v] of Object.entries(extraStrings)) {
            const h = keyHash(k)
            if (!seen.has(h)) {
                items.push([h, v])
                seen.add(h)
            }
        }
        langs.push([lang, items])
    }
    return encodeLocr(langs)
}

// Where the game's chunk0.rpkg can be over FTP: the running game's own mount, or a dump folder.
const FTP_PORTS = [2121, 1337, 21]
const DUMP_ROOTS = ["/data/homebrew", "/data/etaHEN/games", "/data/games",
    ...["ext0", "ext1", "usb0", "usb1", "usb2", "usb3"].flatMap((d) => [`/mnt/${d}/homebrew`, `/mnt/${d}/etaHEN/games`, `/mnt/${d}`])]

function isRpkg(url) {
    try {
        const head = curl(["-m", "15", "-r", "0-3", url]).toString("latin1")
        return head === "GKPR" || head === "2KPR"
    } catch {
        return false
    }
}

function listDir(url) {
    try {
        return curl(["-m", "15", "--list-only", url + "/"]).toString("utf8").split(/\r?\n/).filter(Boolean)
    } catch {
        return []
    }
}

export function findChunk0(ip) {
    for (const port of FTP_PORTS) {
        const base = `ftp://${ip}:${port}`
        try {
            curl(["-m", "5", "--list-only", base + "/"])
        } catch {
            continue
        }
        console.log(`FTP server found on port ${port}, looking for HITMAN 3 (${TITLE})...`)
        // The running game's app0 (installed or dump), then dump folders.
        const tries = [`${base}/mnt/sandbox/${TITLE}_000/app0/runtime/chunk0.rpkg`,
            `${base}/mnt/sandbox/pfsmnt/${TITLE}-app0/runtime/chunk0.rpkg`]
        for (const root of DUMP_ROOTS) {
            for (const name of listDir(base + root)) {
                if (name.toUpperCase().includes(TITLE)) {
                    tries.push(`${base}${root}/${name}/runtime/chunk0.rpkg`, `${base}${root}/${name}/app0/runtime/chunk0.rpkg`)
                }
            }
        }
        for (const url of tries) if (isRpkg(url)) return url
        throw new Error(
            `found an FTP server on port ${port} but not the game. Start HITMAN 3 on the PS5 and run this ` +
                `again, or pass --chunk0 ftp://${ip}:${port}/<game folder>/runtime/chunk0.rpkg`,
        )
    }
    throw new Error(`no FTP server on ${ip} (tried ports ${FTP_PORTS.join(", ")}). Start an FTP payload on the PS5 first.`)
}

export function buildPs5Package({ chunk0, out, resources = join(HERE, "resources"), log = console.log }) {
    const [entry, repo] = extract(chunk0, BASE_REPO)
    if (entry.type !== "REPO") throw new Error(`${hex(BASE_REPO)} is a ${entry.type}, not a REPO`)
    JSON.parse(repo.toString("utf8"))
    log(`base repository: ${repo.length} bytes, ${entry.refs} references`)

    const dyn = { hash: DYNAMIC_REPO, sizeField: 0, stored: repo, raw: entry.raw, type: "REPO" }
    const pkg = readPackage(join(resources, "dynamic_resources_h3.rpkg"))

    const locale = JSON.parse(readFileSync(join(resources, "locale.json"), "utf8"))
    const storePath = join(resources, "ps5_store_strings.json")
    const extra = existsSync(storePath)
        ? Object.fromEntries(Object.entries(JSON.parse(readFileSync(storePath, "utf8"))).filter(([k]) => !k.startsWith("_")))
        : {}
    const built = buildLocr(locale, extra)
    for (const r of pkg) {
        if (r.type !== "LOCR") continue
        r.stored = built
        r.sizeField = 0
        r.raw = Buffer.from(r.raw)
        r.raw.writeUInt32LE(built.length, 12)
    }

    for (const r of pkg) {
        const target = REKEY.get(r.hash)
        if (!target) {
            log(`  ${hex(r.hash)}.${r.type}: no PS5 path, kept as is`)
            continue
        }
        const [ps5Id, ps5Path] = target
        if (!contains(chunk0, ps5Id)) throw new Error(`${ps5Path} is not in the game's chunk0: wrong game version?`)
        const [ps5Entry] = extract(chunk0, ps5Id)
        if (ps5Entry.type !== r.type) throw new Error(`${hex(ps5Id)} is a ${ps5Entry.type} on the console, expected ${r.type}`)
        r.hash = ps5Id
    }
    writePackage(out, [dyn, ...pkg])

    const check = readPackage(out)
    const want = [dyn, ...pkg]
    if (check.length !== want.length || check.some((c, i) => c.hash !== want[i].hash || !c.stored.equals(want[i].stored))) {
        throw new Error(`readback of ${out} does not match`)
    }
    log(`wrote ${out} (${check.length} resources)`)
}

if (process.argv[1] && fileURLToPath(import.meta.url) === process.argv[1]) {
    const args = process.argv.slice(2)
    const opt = (name) => {
        const i = args.indexOf(name)
        return i >= 0 ? args.splice(i, 2)[1] : undefined
    }
    const chunk0Arg = opt("--chunk0")
    const outArg = opt("--out")
    const resArg = opt("--resources")
    const ip = args[0]
    if (!chunk0Arg && !ip) {
        console.error("usage: node build-ps5-package.mjs <ps5-ip> [--chunk0 <path or ftp:// URL>] [--out <file>]")
        process.exit(2)
    }
    try {
        const chunk0 = chunk0Arg || findChunk0(ip)
        console.log(`reading ${chunk0}`)
        buildPs5Package({ chunk0, out: outArg || join(HERE, "ps5", "dynamic_resources_ps5.rpkg"), resources: resArg })
    } catch (e) {
        console.error(`error: ${e.message}`)
        process.exit(1)
    }
}
