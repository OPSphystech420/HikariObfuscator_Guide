
## Flags list:
- `-mllvm -enable-bcfobf` — Enable BogusControlFlow
- `-mllvm -bcf_prob=100` — Obfuscate eligible basic blocks with 100% probability
- `-mllvm -enable-cffobf` — Enable Control Flow Flattening
- `-mllvm -enable-strcry` — Enable String Encryption
- `-mllvm -enable-subobf` — Enable Instruction Substitution
- `-mllvm -enable-fco` — Enable Function CallSite Obfuscation
- `-mllvm -enable-indibran` — Enable Indirect Branching
- `-mllvm -enable-antihook` — Enable AntiHooking
- `-mllvm -ah_inline` — Check inline hooks on AArch64
- `-mllvm -ah_objcruntime` — Check Objective-C runtime hooks
- `-mllvm -ah_antirebind` — Make fishhook / symbol rebinding unavailable
- `-mllvm -enable-adb` — Enable AntiDebugging
- `-mllvm -adb_prob=40` — Apply AntiDebugging to functions with 40% probability
- `-mllvm -enable-constenc` — Enable Constant Encryption
- `-mllvm -constenc_times=1` — Run ConstantEncryption 1 time
- `-mllvm -constenc_togv=1` — Replace ConstantInt values with GlobalVariable storage
- `-mllvm -constenc_togv_prob=50` — Apply ConstantInt -> GlobalVariable replacement with 50% probability
- `-mllvm -constenc_subxor=1` — Substitute xor operations used by ConstantEncryption
- `-mllvm -constenc_subxor_prob=40` — Apply xor substitution with 40% probability
- `-mllvm -indibran-use-stack` — Use stack-based indirect jumps
- `-mllvm -indibran-enc-jump-target` — Encrypt indirect jump targets

There is also
- `-mllvm -enable-allobf` — **Enables the scheduler-managed bulk obfuscation passes, but not every standalone pass-specific feature flag**

> [!NOTE]
> `-mllvm -enable-allobf` does **not** cover everything
> You still need:
> - `-mllvm -enable-antihook`
> - `-mllvm -enable-adb`
> - `-mllvm -enable-constenc`
>
> if you want AntiHooking, AntiDebugging, and ConstantEncryption enabled.


If you want only the passes listed, you do not need the other flags unless you want to tune behavior.

The currently defined flags come from the passes under `Hikari/Obfuscation/Core`

## Other flags:
Scheduler / top-level
- `-mllvm -aesSeed=<u64>` — Set the PRNG seed used by Hikari
- `-mllvm -enable-acdobf` — Enable AntiClassDump
- `-mllvm -enable-splitobf` — Enable BasicBlockSpliting
- `-mllvm -enable-funcwra` — Enable Function Wrapper

BogusControlFlow extras
- `-mllvm -bcf_loop=<n>` — Run BogusControlFlow multiple times
- `-mllvm -bcf_cond_compl=<n>` — Adjust opaque-condition complexity
- `-mllvm -bcf_onlyjunkasm` — Only add junk asm to the altered block
- `-mllvm -bcf_junkasm` — Add junk asm to the altered block
- `-mllvm -bcf_junkasm_maxnum=<n>` — Maximum junk-asm instructions per altered block
- `-mllvm -bcf_junkasm_minnum=<n>` — Minimum junk-asm instructions per altered block
- `-mllvm -bcf_createfunc` — Create a function for each opaque predicate

FunctionCallObfuscate extras
- `-mllvm -fco_flag=<value>` — Set the platform `RTLD_DEFAULT` value used by FCO
- `-mllvm -fcoconfig=<path>` — Path to the FunctionCallObfuscate symbol config JSON

AntiHooking / AntiDebugging extras
- `-mllvm -adhexrirpath=<path>` — External path to precompiled AntiHooking IR
- `-mllvm -adbextirpath=<path>` — External path to precompiled AntiDebugging IR

StringEncryption / Substitution / Split extras
- `-mllvm -strcry_prob=<0..100>` — Per-element StringEncryption probability
- `-mllvm -sub_loop=<n>` — Number of substitution loops
- `-mllvm -sub_prob=<0..100>` — Per-instruction substitution probability
- `-mllvm -split_num=<n>` — Split each basic block `n` times

FunctionWrapper extras
- `-mllvm -fw_prob=<0..100>` — FunctionWrapper application probability
- `-mllvm -fw_times=<n>` — Number of FunctionWrapper rounds / repetitions

AntiClassDump extras
- `-mllvm -acd-use-initialize` — Use the `+initialize` path for AntiClassDump handling
- `-mllvm -acd-rename-methodimp` — Rename method IMPs


> [!NOTE]
>  Also you may need to turn off optimizer so compiler's aggressive optimization won't break obfuscation (`-O0`)

---
## I prefer:
```
-mllvm -enable-cffobf -mllvm -enable-strcry -mllvm -enable-subobf -mllvm -enable-fco -mllvm -ah_objcruntime -mllvm -enable-antihook -mllvm -ah_inline -mllvm -enable-indibran -mllvm -indibran-enc-jump-target -mllvm -ah_antirebind -enable-constenc -mllvm -constenc_times=1 -mllvm -constenc_togv=1 -mllvm -constenc_togv_prob=50 -mllvm -constenc_subxor=1 -mllvm -constenc_subxor_prob=40
```
Other flags may take ages to compile or destroy your RAM
