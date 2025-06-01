// ./Hikari/PluginEntry.cpp

#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "Obfuscation.h"

#if LLVM_VERSION_MAJOR >= 13

namespace llvm {

PassPluginLibraryInfo getHikariPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "Hikari", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineStartEPCallback(
                [](ModulePassManager &PM, OptimizationLevel) {
                  PM.addPass(ObfuscationPass());
                });
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return getHikariPluginInfo();
}

} // namespace llvm

#endif
