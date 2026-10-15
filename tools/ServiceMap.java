// Maps every online service call the client can make: finds each "<Name>Service" literal and the
// URL/path literals in targets, then for every function that references one records, in
// instruction order, every string literal the function touches (method names, JSON keys of the
// request body, which the client stores as {0x80000000|len, ptr} ZStrings) and its direct callees,
// and decompiles it. Run read-only on the existing project:
//   analyzeHeadless ghidra_proj hitman -process eboot.sym.elf -noanalysis -readOnly \
//     -scriptPath tools -postScript ServiceMap.java <outDir> <extraTargetsFile>
// extraTargetsFile lines: "S <literal>" (functions referencing it) or "D <0xaddr>" (decompile it).
import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.*;
import ghidra.program.model.address.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.mem.*;
import ghidra.program.model.scalar.Scalar;
import ghidra.program.model.symbol.*;
import java.io.*;
import java.nio.charset.StandardCharsets;
import java.nio.file.*;
import java.util.*;
import java.util.regex.*;

public class ServiceMap extends GhidraScript {
    // string start offset -> string, over all initialised non-executable blocks
    TreeMap<Long, String> strings = new TreeMap<>();
    Map<Function, Set<String>> why = new LinkedHashMap<>();

    void indexStrings() throws Exception {
        for (MemoryBlock b : currentProgram.getMemory().getBlocks()) {
            if (!b.isInitialized() || b.isExecute() || b.getSize() > Integer.MAX_VALUE) continue;
            byte[] buf = new byte[(int) b.getSize()];
            try { b.getBytes(b.getStart(), buf); } catch (Exception e) { continue; }
            long base = b.getStart().getOffset();
            int start = -1;
            for (int i = 0; i < buf.length; i++) {
                int c = buf[i] & 0xff;
                boolean pr = (c >= 0x20 && c < 0x7f) || c == '\t';
                if (pr) { if (start < 0) start = i; continue; }
                if (start >= 0 && c == 0 && i - start >= 2) {
                    strings.put(base + start, new String(buf, start, i - start, StandardCharsets.ISO_8859_1));
                }
                start = -1;
            }
        }
        println("indexed strings: " + strings.size());
    }

    // The string an address points into (suffix from that address), or null.
    String stringAt(long v) {
        Map.Entry<Long, String> e = strings.floorEntry(v);
        if (e == null) return null;
        long off = v - e.getKey();
        if (off < 0 || off >= e.getValue().length()) return null;
        return e.getValue().substring((int) off);
    }

    void refsTo(long target, String reason) {
        Address t = toAddr(target);
        for (Reference r : currentProgram.getReferenceManager().getReferencesTo(t)) {
            Function f = getFunctionContaining(r.getFromAddress());
            if (f == null) continue;
            why.computeIfAbsent(f, k -> new LinkedHashSet<>()).add(reason + " @" + r.getFromAddress());
        }
    }

    // Every string literal touched by f's instructions, in address order, plus callees.
    List<String> literals(Function f, List<String> callees) {
        List<String> out = new ArrayList<>();
        Set<String> seenCallee = new LinkedHashSet<>();
        InstructionIterator it = currentProgram.getListing().getInstructions(f.getBody(), true);
        while (it.hasNext()) {
            Instruction in = it.next();
            Set<Long> cands = new LinkedHashSet<>();
            for (int i = 0; i < in.getNumOperands(); i++) {
                for (Object o : in.getOpObjects(i)) {
                    if (o instanceof Scalar) cands.add(((Scalar) o).getUnsignedValue());
                    else if (o instanceof Address) cands.add(((Address) o).getOffset());
                }
            }
            for (Reference r : in.getReferencesFrom()) {
                if (r.getReferenceType().isCall()) {
                    Function cf = getFunctionAt(r.getToAddress());
                    seenCallee.add(cf == null ? r.getToAddress().toString() : cf.getName());
                } else {
                    cands.add(r.getToAddress().getOffset());
                }
            }
            for (long v : cands) {
                String s = stringAt(v);
                if (s != null && s.length() >= 2) out.add(in.getAddress() + "\t" + s);
            }
        }
        callees.addAll(seenCallee);
        return out;
    }

    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        File outDir = new File(args[0]);
        new File(outDir, "functions").mkdirs();
        indexStrings();

        Pattern svc = Pattern.compile("^[A-Z][A-Za-z0-9]+Service$");
        PrintWriter idx = new PrintWriter(new FileWriter(new File(outDir, "index.txt")));
        for (Map.Entry<Long, String> e : strings.entrySet()) {
            if (!svc.matcher(e.getValue()).matches()) continue;
            idx.println("SERVICE " + e.getValue() + " at " + toAddr(e.getKey()));
            refsTo(e.getKey(), "svc:" + e.getValue());
        }
        if (args.length > 1) {
            for (String line : Files.readAllLines(Paths.get(args[1]), StandardCharsets.UTF_8)) {
                if (line.length() < 3 || line.startsWith("#")) continue;
                String val = line.substring(2);
                if (line.charAt(0) == 'S') {
                    int hits = 0;
                    for (Map.Entry<Long, String> e : strings.entrySet()) {
                        if (!e.getValue().equals(val)) continue;
                        hits++;
                        refsTo(e.getKey(), "str:" + val);
                    }
                    idx.println("S \"" + val + "\" hits=" + hits);
                } else if (line.charAt(0) == 'D') {
                    Address a = toAddr(val);
                    Function f = getFunctionContaining(a);
                    if (f == null) f = createFunction(a, null);
                    if (f != null) why.computeIfAbsent(f, k -> new LinkedHashSet<>()).add("direct:" + val);
                    idx.println("D " + val + " -> " + (f == null ? "no function" : f.getName()));
                }
            }
        }

        DecompInterface ifc = new DecompInterface();
        ifc.setOptions(new DecompileOptions());
        ifc.openProgram(currentProgram);
        PrintWriter tsv = new PrintWriter(new FileWriter(new File(outDir, "functions.tsv")));
        tsv.println("ghidra_addr\tname\treasons\tcallers\tcallees\tliterals");
        int n = 0;
        for (Map.Entry<Function, Set<String>> e : why.entrySet()) {
            Function f = e.getKey();
            List<String> callees = new ArrayList<>();
            List<String> lits = literals(f, callees);
            Set<String> callers = new LinkedHashSet<>();
            for (Function c : f.getCallingFunctions(monitor)) callers.add(c.getName() + "@" + c.getEntryPoint());
            File out = new File(outDir, "functions/" + f.getEntryPoint() + ".c");
            try (PrintWriter pw = new PrintWriter(new FileWriter(out))) {
                pw.println("// " + f.getName() + " @ " + f.getEntryPoint() + " size=" + f.getBody().getNumAddresses());
                for (String w : e.getValue()) pw.println("// why: " + w);
                pw.println("// callers: " + String.join(", ", callers));
                pw.println("// callees: " + String.join(", ", callees));
                for (String l : lits) pw.println("// lit " + l.replace("\n", "\\n"));
                DecompileResults res = ifc.decompileFunction(f, 120, monitor);
                if (res != null && res.decompileCompleted()) pw.println(res.getDecompiledFunction().getC());
                else pw.println("// decompile failed: " + (res == null ? "null" : res.getErrorMessage()));
            }
            StringBuilder ls = new StringBuilder();
            for (String l : lits) { if (ls.length() > 0) ls.append(" | "); ls.append(l.substring(l.indexOf('\t') + 1).replace("\t", " ")); }
            tsv.println(f.getEntryPoint() + "\t" + f.getName() + "\t" + String.join("; ", e.getValue()) + "\t" +
                        String.join(", ", callers) + "\t" + String.join(", ", callees) + "\t" + ls);
            idx.println("FUNC " + f.getEntryPoint() + " " + f.getName() + " :: " + String.join(" | ", e.getValue()));
            if (++n % 25 == 0) println("decompiled " + n + "/" + why.size());
        }
        tsv.close();
        idx.close();
        println("DONE functions=" + why.size());
    }
}
