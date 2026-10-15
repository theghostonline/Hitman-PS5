// Dump decompiled functions that reference network/profile strings or call net/auth/curl entry points.
// Args: <outDir> <targetsFile>
// targetsFile lines: "S <literal string>" or "F <symbol name>" (callers of symbol) or "D <symbol or 0xaddr>" (decompile itself)
import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.*;
import ghidra.program.model.address.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.mem.*;
import ghidra.program.model.symbol.*;
import java.io.*;
import java.nio.charset.StandardCharsets;
import java.nio.file.*;
import java.util.*;

public class DumpNet extends GhidraScript {
    DecompInterface ifc;
    File outDir;
    Map<Function, Set<String>> why = new LinkedHashMap<>();
    PrintWriter idx;

    void note(Function f, String reason) {
        if (f == null) return;
        why.computeIfAbsent(f, k -> new LinkedHashSet<>()).add(reason);
    }

    List<Address> findAll(byte[] pat) throws Exception {
        List<Address> hits = new ArrayList<>();
        Memory mem = currentProgram.getMemory();
        for (MemoryBlock b : mem.getBlocks()) {
            if (!b.isInitialized()) continue;
            Address a = b.getStart();
            while (a != null && a.compareTo(b.getEnd()) <= 0) {
                Address h = mem.findBytes(a, b.getEnd(), pat, null, true, monitor);
                if (h == null) break;
                hits.add(h);
                a = h.add(1);
                if (hits.size() > 64) break;
            }
        }
        return hits;
    }

    void refsTo(Address target, String reason) {
        ReferenceManager rm = currentProgram.getReferenceManager();
        for (Reference r : rm.getReferencesTo(target)) {
            Function f = getFunctionContaining(r.getFromAddress());
            idx.println("  ref " + r.getFromAddress() + " -> " + target + " (" + reason + ") in " + (f == null ? "?" : f.getName() + "@" + f.getEntryPoint()));
            note(f, reason + " @" + r.getFromAddress());
        }
    }

    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        outDir = new File(args[0]);
        outDir.mkdirs();
        idx = new PrintWriter(new FileWriter(new File(outDir, "index.txt")));
        List<String> lines = Files.readAllLines(Paths.get(args[1]), StandardCharsets.UTF_8);
        SymbolTable st = currentProgram.getSymbolTable();
        for (String line : lines) {
            if (line.length() < 3 || line.startsWith("#")) continue;
            char kind = line.charAt(0);
            String val = line.substring(2);
            if (kind == 'S') {
                byte[] s = val.getBytes(StandardCharsets.UTF_8);
                byte[] pat = Arrays.copyOf(s, s.length + 1); // NUL-terminated
                List<Address> hits = findAll(pat);
                idx.println("S \"" + val + "\" hits=" + hits.size());
                for (Address h : hits) {
                    idx.println(" at " + h);
                    refsTo(h, "str:" + val);
                    // also refs to start of a containing string (tail-merged literals)
                    Data d = getDataContaining(h);
                    if (d != null && !d.getAddress().equals(h)) refsTo(d.getAddress(), "str(in):" + val);
                }
            } else if (kind == 'F' || kind == 'D') {
                List<Address> addrs = new ArrayList<>();
                if (val.startsWith("0x")) addrs.add(toAddr(val));
                else for (Symbol sym : st.getSymbols(val)) addrs.add(sym.getAddress());
                idx.println(kind + " " + val + " addrs=" + addrs);
                for (Address a : addrs) {
                    if (kind == 'D') { Function f = getFunctionContaining(a); if (f == null) f = createFunction(a, null); note(f, "direct:" + val); continue; }
                    Set<Address> targets = new LinkedHashSet<>();
                    targets.add(a);
                    Function tf = getFunctionAt(a);
                    if (tf != null && tf.isThunk()) targets.add(tf.getThunkedFunction(true).getEntryPoint());
                    // callers of thunks that point to this function
                    for (Reference r : currentProgram.getReferenceManager().getReferencesTo(a)) {
                        Function f = getFunctionContaining(r.getFromAddress());
                        if (f != null && f.isThunk()) targets.add(f.getEntryPoint());
                    }
                    int n = 0;
                    for (Address t : targets) {
                        for (Reference r : currentProgram.getReferenceManager().getReferencesTo(t)) {
                            Function f = getFunctionContaining(r.getFromAddress());
                            if (f == null || f.isThunk()) continue;
                            if (n++ < 400) note(f, "calls:" + val + " @" + r.getFromAddress());
                        }
                    }
                    idx.println("  callers=" + n);
                }
            }
        }
        ifc = new DecompInterface();
        DecompileOptions opts = new DecompileOptions();
        ifc.setOptions(opts);
        ifc.openProgram(currentProgram);
        int i = 0;
        for (Map.Entry<Function, Set<String>> e : why.entrySet()) {
            Function f = e.getKey();
            String name = f.getEntryPoint().toString();
            File out = new File(outDir, name + ".c");
            try (PrintWriter pw = new PrintWriter(new FileWriter(out))) {
                pw.println("// " + f.getName() + " @ " + f.getEntryPoint() + " size=" + f.getBody().getNumAddresses());
                for (String w : e.getValue()) pw.println("// why: " + w);
                DecompileResults res = ifc.decompileFunction(f, 90, monitor);
                if (res != null && res.decompileCompleted()) pw.println(res.getDecompiledFunction().getC());
                else pw.println("// decompile failed: " + (res == null ? "null" : res.getErrorMessage()));
            }
            idx.println("FUNC " + name + " " + f.getName() + " :: " + String.join(" | ", e.getValue()));
            if (++i % 50 == 0) println("decompiled " + i + "/" + why.size());
        }
        idx.close();
        println("DONE functions=" + why.size());
    }
}
