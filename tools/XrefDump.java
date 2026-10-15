// List all references (with ref type) to given addresses and decompile writer functions.
// Args: <outDir> <addr> [addr...]   (Ghidra addresses)
import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.*;
import ghidra.program.model.address.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.*;
import java.io.*;
import java.util.*;

public class XrefDump extends GhidraScript {
    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        File outDir = new File(args[0]);
        outDir.mkdirs();
        PrintWriter idx = new PrintWriter(new FileWriter(new File(outDir, "xrefs.txt")));
        Set<Function> writers = new LinkedHashSet<>();
        for (int i = 1; i < args.length; i++) {
            Address t = toAddr(args[i]);
            idx.println("== " + t);
            for (Reference r : currentProgram.getReferenceManager().getReferencesTo(t)) {
                Function f = getFunctionContaining(r.getFromAddress());
                idx.println("  " + r.getReferenceType() + " " + r.getFromAddress() + " in " + (f == null ? "?" : f.getName() + "@" + f.getEntryPoint()));
                if (f != null && (r.getReferenceType().isWrite() || r.getReferenceType() == RefType.DATA)) writers.add(f);
            }
        }
        DecompInterface ifc = new DecompInterface();
        ifc.setOptions(new DecompileOptions());
        ifc.openProgram(currentProgram);
        for (Function f : writers) {
            try (PrintWriter pw = new PrintWriter(new FileWriter(new File(outDir, "w_" + f.getEntryPoint() + ".c")))) {
                DecompileResults res = ifc.decompileFunction(f, 120, monitor);
                pw.println("// " + f.getName() + " @ " + f.getEntryPoint());
                if (res != null && res.decompileCompleted()) pw.println(res.getDecompiledFunction().getC());
                else pw.println("// failed");
            }
            idx.println("WRITER " + f.getEntryPoint());
        }
        idx.close();
        println("DONE writers=" + writers.size());
    }
}
