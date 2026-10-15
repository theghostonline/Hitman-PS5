// Starts the HITMAN PS5 backend. On the first run it asks for the PS5's IP address and builds the PS5
// resource package from the console's own game files (build-ps5-package.mjs), then runs the server.
//
//   node start.mjs [ps5-ip]        PORT (default 8410) changes the listen port
import { spawn } from "node:child_process"
import { existsSync, readFileSync, writeFileSync } from "node:fs"
import { networkInterfaces } from "node:os"
import { dirname, join } from "node:path"
import { createInterface } from "node:readline/promises"
import { fileURLToPath } from "node:url"
import { buildPs5Package, findChunk0 } from "./build-ps5-package.mjs"

const HERE = dirname(fileURLToPath(import.meta.url))
const PORT = process.env.PORT || "8410"
const PKG = join(HERE, "ps5", "dynamic_resources_ps5.rpkg")
const IP_FILE = join(HERE, "ps5-ip.txt")

async function ask(question) {
    const rl = createInterface({ input: process.stdin, output: process.stdout })
    try {
        return (await rl.question(question)).trim()
    } finally {
        rl.close()
    }
}

function lanAddresses() {
    return Object.values(networkInterfaces())
        .flat()
        .filter((a) => a && a.family === "IPv4" && !a.internal)
        .map((a) => a.address)
}

if (!existsSync(PKG) || process.argv.includes("--rebuild")) {
    console.log("First run: building the PS5 resource package from your game files.")
    console.log("The PS5 needs to be on with an FTP server running (etaHEN or ftpsrv).\n")
    let ip = process.argv.slice(2).find((a) => !a.startsWith("--"))
    if (!ip && existsSync(IP_FILE)) ip = readFileSync(IP_FILE, "utf8").trim()
    if (!ip) ip = await ask("PS5 IP address: ")
    try {
        const chunk0 = findChunk0(ip)
        console.log(`found the game at ${chunk0}`)
        buildPs5Package({ chunk0, out: PKG })
        writeFileSync(IP_FILE, ip + "\n")
    } catch (e) {
        console.error(`\nCould not build the PS5 package: ${e.message}`)
        console.error("Without it the game goes offline after login. Fix the above and start again.")
        if (process.platform === "win32") await ask("Press Enter to close.")
        process.exit(1)
    }
    console.log("")
}

const ips = lanAddresses()
console.log("HITMAN PS5 backend")
console.log(`On the PS5, /data/hitman-server/config.ini should contain:`)
for (const ip of ips.length ? ips : ["<this computer's IP>"]) console.log(`    upstream = http://${ip}:${PORT}`)
console.log("Leave this window open while you play.\n")

const child = spawn(process.execPath, [join(HERE, "chunk0.js")], {
    cwd: HERE,
    stdio: "inherit",
    env: { ...process.env, PORT, HOST: "0.0.0.0" },
})
child.on("exit", (code) => process.exit(code ?? 0))
for (const sig of ["SIGINT", "SIGTERM"]) process.on(sig, () => child.kill(sig))
