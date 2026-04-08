# Hikari Plugin Android Studio 2024.3.2.15 NDK r29.0.13113456 (LLVM-20) [Android Studio](https://developer.android.com/studio?hl=en) [Android llvm-project](https://android.googlesource.com/toolchain/llvm-project)

> [!NOTE]
> This is a **Windows-native** guide for the `build/android-ndk-llvm20` branch of [HikariObfuscator_Guide](https://github.com/OPSphystech420/HikariObfuscator_Guide/tree/build/android-ndk-llvm20).
>
> We use the AOSP/NDK **`clang-r547379`** Windows host prebuilt as the bootstrap compiler:
>
> - prebuilt host tree: [`platform/prebuilts/clang/host/windows-x86`](https://android.googlesource.com/platform/prebuilts/clang/host/windows-x86/)
> - upstream llvm-project commit: **`b718bcaf8c198c82f3021447d943401e3ab5bd54`**
>
> On Windows, the AOSP prebuilt alone is **not enough** to link `libHikari.dll`. We use it only to bootstrap a **full Windows host LLVM build**, then build Hikari against that host LLVM.

# Guide

### Windows x64 native (also validated in a Windows 11 ARM VM under x64 emulation)

> [!WARNING]
> Use **MSYS2 MINGW64** for the actual build steps.  
> Do **not** use plain `cmd`, plain PowerShell, Visual Studio Developer PowerShell, or MSYS2 `UCRT64` for the build itself.
>
> The working target/toolchain combination is:
>
> - target: `x86_64-w64-windows-gnu`
> - shell: `MSYS2 MINGW64`
> - bootstrap compiler: AOSP `clang-r547379`
>
> On Windows ARM, Android Studio host support is not official. If you are on Windows ARM, install the SDK/NDK with **command-line tools first** and treat the setup as experimental.

> [!WARNING]
> The `clang-r547379.tar.gz` Windows archive uses **symlinks** for several binaries (`clang++.exe`, `clang-cl.exe`, `lld-link.exe`, etc.).  
> Enable **Developer Mode** or run the extraction terminal **as Administrator**, otherwise extraction will fail.

---

## 0. What you will get at the end

After following this guide, you should have:

- `Hikari/build/Obfuscation/libHikari.dll`
- `Hikari/android-llvm/build/bin/clang.exe`
- `Hikari/android-llvm/build/bin/clang++.exe`
- `Hikari/android-llvm/build/bin/clang-cl.exe`
- `Hikari/android-llvm/build/bin/clang-cpp.exe`
- a modified NDK copy:
  - `.../ndk/29.0.13113456-obf/toolchains/llvm/prebuilt/windows-x86_64/...`

> [!NOTE]
> In this Windows flow, the host LLVM build **does not produce `clang-20.exe` automatically**.  
> We create it manually by copying `clang.exe` to `clang-20.exe` before replacing the NDK toolchain binaries.

---

## 1. Prerequisites

Install:

- **JDK 17 x64**
- **Android SDK Command-line Tools**
- **NDK r29.0.13113456**
- **7-Zip**
- **MSYS2 x64**
- **Git for Windows** (optional, useful for cloning the Hikari repo itself)

You may also use Android Studio later, but the core Windows-native build below does **not** require Visual Studio.

---

## 2. Set `ANDROID_SDK_ROOT` and install NDK r29.0.13113456

In **PowerShell**:

```powershell
setx ANDROID_SDK_ROOT "C:\Android\Sdk"
$env:ANDROID_SDK_ROOT = "C:\Android\Sdk"
```

Install a **JDK 17 x64**, then set `JAVA_HOME` in PowerShell:

```powershell
$env:JAVA_HOME = "C:\Program Files\Eclipse Adoptium\jdk-17.x.x"
$env:Path = "$env:JAVA_HOME\bin;$env:Path"
setx JAVA_HOME "C:\Program Files\Eclipse Adoptium\jdk-17.x.x"
```

Download **Android SDK Command-line Tools**, unpack them so this exists:

```text
C:\Android\Sdk\cmdline-tools\latest\bin\sdkmanager.bat
```

Then install NDK **r29.0.13113456**:

```powershell
cd "C:\Android\Sdk\cmdline-tools\latest\bin"

.\sdkmanager.bat --sdk_root=$env:ANDROID_SDK_ROOT --licenses
.\sdkmanager.bat --sdk_root=$env:ANDROID_SDK_ROOT --install "platform-tools" "ndk;29.0.13113456"
```

Verify the NDK host path:

```powershell
Get-ChildItem "$env:ANDROID_SDK_ROOT\ndk\29.0.13113456\toolchains\llvm\prebuilt"
```

You should see:

```text
windows-x86_64
```

---

## 3. Clone the repo and download the Windows AOSP bootstrap compiler

In **cmd.exe** or **PowerShell**:

```bash
git clone --recursive -b build/android-ndk-llvm20 https://github.com/OPSphystech420/HikariObfuscator_Guide.git
cd C:\HikariObfuscator_Guide\Hikari

curl.exe -L "https://android.googlesource.com/platform/prebuilts/clang/host/windows-x86/+archive/c8a25b3935d01457157f8094bff6b1ad5e6f7ca5/clang-r547379.tar.gz" -o clang-r547379.tar.gz
```

---

## 4. Extract `clang-r547379` correctly on Windows

Use an **Administrator cmd.exe** (or enable Developer Mode first), because the archive contains symlinks.

```bat
cd /d C:\HikariObfuscator_Guide\Hikari

"%ProgramFiles%\7-Zip\7z.exe" x clang-r547379.tar.gz -aoa
mkdir clang-r547379
"%ProgramFiles%\7-Zip\7z.exe" x clang-r547379.tar -oclang-r547379 -aoa
```

Verify the important files exist:

```bat
dir clang-r547379\bin\clang.exe
dir clang-r547379\bin\clang++.exe
dir clang-r547379\lib\cmake\llvm\LLVMConfig.cmake
```

> [!TIP]
> If 7-Zip warns about ignored/dangerous symlink paths under `lib\python3.11\site-packages\lldb\...`, you may ignore those warnings.  
> They are **not** needed for this Hikari flow.

---

## 5. Install MSYS2 packages and use the correct shell

Open **MSYS2 MINGW64** from the Start menu.

Update once or twice if MSYS2 asks you to restart the shell:

```bash
pacman -Suy
```

Then install the packages we actually used:

```bash
pacman -S --needed \
  mingw-w64-x86_64-toolchain \
  mingw-w64-x86_64-cmake \
  mingw-w64-x86_64-ninja \
  mingw-w64-x86_64-winpthreads \
  git \
  python
```

Verify the shell and tools:

```bash
echo $MSYSTEM
cmake --version
ninja --version
ls /mingw64/lib/libmingw32.a
ls /mingw64/lib/libkernel32.a
ls /mingw64/lib/libmsvcrt.a
```

You want:

- `MINGW64`
- `cmake` available
- `ninja` available
- the MinGW runtime libs above present

---

## 6. Build a Windows host LLVM from the exact Android commit

> [!NOTE]
> We do **not** clone the full `toolchain/llvm-project` repository with Git here.  
> It is very large and may fail inside a VM.  
> Instead, we download the exact pinned commit as a Gitiles archive.

In **MSYS2 MINGW64**:

```bash
cd /c/HikariObfuscator_Guide/Hikari
rm -rf android-llvm
mkdir -p android-llvm
cd android-llvm

curl -L "https://android.googlesource.com/toolchain/llvm-project/+archive/b718bcaf8c198c82f3021447d943401e3ab5bd54.tar.gz" -o llvm-project.tar.gz
mkdir -p llvm-project
tar -xzf llvm-project.tar.gz -C llvm-project

test -d llvm-project/llvm && echo OK
```

If that prints `OK`, configure the host LLVM build:

```bash
cmake -S llvm-project/llvm -B build -G Ninja \
  -DCMAKE_INSTALL_PREFIX="C:/HikariObfuscator_Guide/Hikari/android-llvm/install" \
  -DCMAKE_BUILD_TYPE=MinSizeRel \
  -DCMAKE_C_COMPILER="C:/HikariObfuscator_Guide/Hikari/clang-r547379/bin/clang.exe" \
  -DCMAKE_CXX_COMPILER="C:/HikariObfuscator_Guide/Hikari/clang-r547379/bin/clang++.exe" \
  -DCMAKE_C_COMPILER_TARGET=x86_64-w64-windows-gnu \
  -DCMAKE_CXX_COMPILER_TARGET=x86_64-w64-windows-gnu \
  -DTHREADS_PREFER_PTHREAD_FLAG=TRUE \
  -DLLVM_ENABLE_CURL=OFF \
  -DLLVM_ENABLE_LIBXML2=OFF \
  -DCMAKE_C_FLAGS_INIT="--gcc-toolchain=C:/msys64/mingw64 -BC:/msys64/mingw64/bin -pthread -femulated-tls" \
  -DCMAKE_CXX_FLAGS_INIT="--gcc-toolchain=C:/msys64/mingw64 -BC:/msys64/mingw64/bin -pthread -femulated-tls" \
  -DCMAKE_EXE_LINKER_FLAGS_INIT="--gcc-toolchain=C:/msys64/mingw64 -BC:/msys64/mingw64/bin -pthread" \
  -DCMAKE_SHARED_LINKER_FLAGS_INIT="--gcc-toolchain=C:/msys64/mingw64 -BC:/msys64/mingw64/bin -pthread" \
  -DCMAKE_MODULE_LINKER_FLAGS_INIT="--gcc-toolchain=C:/msys64/mingw64 -BC:/msys64/mingw64/bin -pthread" \
  -DLLVM_ENABLE_PROJECTS="clang;lld" \
  -DLLVM_TARGETS_TO_BUILD="X86;ARM;AArch64" \
  -DLLVM_INSTALL_UTILS=ON \
  -DLLVM_INCLUDE_TESTS=OFF \
  -DLLVM_BUILD_TESTS=OFF \
  -DLLVM_INCLUDE_BENCHMARKS=OFF \
  -DLLVM_BUILD_BENCHMARKS=OFF \
  -DLLVM_INCLUDE_EXAMPLES=OFF \
  -DLLVM_ENABLE_BACKTRACES=OFF \
  -DLLVM_BUILD_DOCS=OFF
```

Now build only the targets we actually need for Hikari:

```bash
cmake --build build --target \
  clang llvm-ar llvm-ranlib \
  LLVMSupport LLVMCore LLVMAnalysis LLVMTransformUtils LLVMScalarOpts LLVMInstCombine LLVMipo LLVMPasses \
  -v
```

Verify the results:

```bash
ls build/bin/clang.exe
ls build/bin/clang++.exe
ls build/lib/libLLVMSupport.a
ls build/lib/libLLVMCore.a
ls build/lib/libLLVMPasses.a
```

You should see all of them.

> [!NOTE]
> We use the **build tree** directly for Hikari:
>
> - compiler: `android-llvm/build/bin/clang.exe`
> - compiler: `android-llvm/build/bin/clang++.exe`
> - LLVM CMake package: `android-llvm/build/lib/cmake/llvm`
>
> We do **not** rely on the prebuilt Windows AOSP CMake package for the final Hikari link.

---

## 7. Patch Hikari for Windows-native linking

Replace `Hikari/CMakeLists.txt` with:

```cmake
cmake_minimum_required(VERSION 3.22)
project(Hikari-Obf-LLVM20-NDK29)

set(ENV{LLVM_HOME} "${CMAKE_CURRENT_LIST_DIR}/clang-r547379")
if(NOT DEFINED LLVM_DIR)
  set(LLVM_DIR "$ENV{LLVM_HOME}/lib/cmake/llvm")
endif()

find_package(LLVM REQUIRED CONFIG)
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})

add_definitions(${LLVM_DEFINITIONS_LIST})
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})

set(CMAKE_CXX_STANDARD 17)
add_compile_definitions(ENDIAN_LITTLE)

if(APPLE)
  set(CMAKE_OSX_ARCHITECTURES arm64)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

set(CMAKE_SKIP_RPATH ON)
add_subdirectory(Obfuscation)
```

Replace `Hikari/Obfuscation/CMakeLists.txt` with:

```cmake
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

add_library(Hikari MODULE
  Core/FunctionCallObfuscate.cpp
  Core/CryptoUtils.cpp
  Core/BogusControlFlow.cpp
  Core/SubstituteImpl.cpp
  Core/Substitution.cpp
  Core/Flattening.cpp
  Core/Utils.cpp
  Core/SplitBasicBlocks.cpp
  Core/AntiClassDump.cpp
  Core/AntiDebugging.cpp
  Core/AntiHooking.cpp
  Core/StringEncryption.cpp
  Core/IndirectBranch.cpp
  Core/FunctionWrapper.cpp
  Core/ConstantEncryption.cpp
  Core/Obfuscation.cpp
  Core/PluginEntry.cpp
)

target_include_directories(Hikari PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Headers)

target_compile_options(Hikari PRIVATE
  -fno-rtti
  -pthread
  -femulated-tls
)

if(APPLE)
  set_target_properties(Hikari PROPERTIES
    LINK_FLAGS "-undefined dynamic_lookup"
  )
else()
  llvm_map_components_to_libnames(HIKARI_LLVM_LIBS
    support
    core
    analysis
    transformutils
    scalaropts
    instcombine
    ipo
    passes
  )

  target_link_options(Hikari PRIVATE -pthread)

  target_link_libraries(Hikari PRIVATE
    ${HIKARI_LLVM_LIBS}
    Threads::Threads
    winpthread
  )
endif()

execute_process(
  COMMAND git log -1 --format=%H
  WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
  OUTPUT_VARIABLE HIKARI_GIT_COMMIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

target_compile_definitions(Hikari PRIVATE "-DGIT_COMMIT_HASH=\"${HIKARI_GIT_COMMIT_HASH}\"")
```

Replace `Hikari/Obfuscation/Core/PluginEntry.cpp` with:

```cpp
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "Obfuscation.h"

#if LLVM_VERSION_MAJOR >= 13

#if defined(_WIN32)
#define HIKARI_PLUGIN_EXPORT __declspec(dllexport)
#else
#define HIKARI_PLUGIN_EXPORT LLVM_ATTRIBUTE_WEAK
#endif

namespace llvm {

PassPluginLibraryInfo getHikariPluginInfo() {
    return {
        LLVM_PLUGIN_API_VERSION,
        "Hikari",
        LLVM_VERSION_STRING,
        [](PassBuilder &PB) {
            PB.registerPipelineStartEPCallback(
                [](ModulePassManager &PM, OptimizationLevel) {
                    PM.addPass(ObfuscationPass());
                });
        }
    };
}

extern "C" HIKARI_PLUGIN_EXPORT PassPluginLibraryInfo llvmGetPassPluginInfo() {
    return getHikariPluginInfo();
}

} // namespace llvm

#endif
```

> [!TIP]
> This `PluginEntry.cpp` still emits a harmless warning about `dllexport` redeclaration against `llvm/Passes/PassPlugin.h`, but the build succeeds and produces `libHikari.dll`.

---

## 8. Build `libHikari.dll` against the Windows host LLVM

In **MSYS2 MINGW64**:

```bash
cd /c/HikariObfuscator_Guide/Hikari
rm -rf build

cmake -S . -B build -G Ninja \
  -DCMAKE_BUILD_TYPE=MinSizeRel \
  -DCMAKE_C_COMPILER="C:/HikariObfuscator_Guide/Hikari/android-llvm/build/bin/clang.exe" \
  -DCMAKE_CXX_COMPILER="C:/HikariObfuscator_Guide/Hikari/android-llvm/build/bin/clang++.exe" \
  -DLLVM_DIR="C:/HikariObfuscator_Guide/Hikari/android-llvm/build/lib/cmake/llvm" \
  -DCMAKE_C_COMPILER_TARGET=x86_64-w64-windows-gnu \
  -DCMAKE_CXX_COMPILER_TARGET=x86_64-w64-windows-gnu \
  -DCMAKE_C_FLAGS_INIT="--gcc-toolchain=C:/msys64/mingw64 -BC:/msys64/mingw64/bin -pthread -femulated-tls" \
  -DCMAKE_CXX_FLAGS_INIT="--gcc-toolchain=C:/msys64/mingw64 -BC:/msys64/mingw64/bin -pthread -femulated-tls" \
  -DCMAKE_EXE_LINKER_FLAGS_INIT="--gcc-toolchain=C:/msys64/mingw64 -BC:/msys64/mingw64/bin -pthread" \
  -DCMAKE_SHARED_LINKER_FLAGS_INIT="--gcc-toolchain=C:/msys64/mingw64 -BC:/msys64/mingw64/bin -pthread"

cmake --build build --config MinSizeRel -v
find build -type f | grep -i hikari
```

You should now have:

```text
build/Obfuscation/libHikari.dll
```

---

## 9. Create `clang-20.exe` manually

The Windows host LLVM build above does **not** emit `clang-20.exe` automatically.

Create it manually:

```bash
cd /c/HikariObfuscator_Guide/Hikari/android-llvm/build/bin
cp clang.exe clang-20.exe
```

Now verify:

```bash
ls clang.exe
ls clang++.exe
ls clang-20.exe
```

---

## 10. Create a modified NDK copy and replace the compiler frontends

Set the MSYS2-style SDK path:

```bash
export ANDROID_SDK_ROOT="/c/Android/Sdk"
```

Copy the original NDK and patch the Windows host toolchain:

```bash
cd "$ANDROID_SDK_ROOT/ndk"
cp -r 29.0.13113456 29.0.13113456-obf

cd 29.0.13113456-obf/toolchains/llvm/prebuilt/windows-x86_64/bin

mv clang.exe clang.exe.bak
mv clang++.exe clang++.exe.bak
mv clang-20.exe clang-20.exe.bak

cp /c/HikariObfuscator_Guide/Hikari/android-llvm/build/bin/clang.exe .
cp /c/HikariObfuscator_Guide/Hikari/android-llvm/build/bin/clang++.exe .
cp /c/HikariObfuscator_Guide/Hikari/android-llvm/build/bin/clang-20.exe .
```

Copy the Hikari plugin into the modified NDK:

```bash
cp /c/HikariObfuscator_Guide/Hikari/build/Obfuscation/libHikari.dll \
  "$ANDROID_SDK_ROOT/ndk/29.0.13113456-obf/toolchains/llvm/prebuilt/windows-x86_64/lib/"
```

---

## 11. Port Hikari into your Android project

### Example with `build.gradle.kts` (Module `:app`) and Kotlin/Native C++

```gradle
android {
    // ...

    val sdkDir = android.sdkDirectory
    ndkVersion = "29.0.13113456"
    ndkPath = "$sdkDir/ndk/29.0.13113456-obf" // use the modified NDK

    defaultConfig {
        externalNativeBuild {
            cmake {
                val ndkDir = android.ndkDirectory
                val obfLibDir = "$ndkDir/toolchains/llvm/prebuilt/windows-x86_64/lib/libHikari.dll"
                val obfArgs = listOf(
                    "-fvisibility=hidden",
                    "-fpass-plugin=$obfLibDir",
                    "-Xclang",
                    "-load",
                    "-Xclang",
                    obfLibDir,
                    "-mllvm",
                    "-enable-strcry"
                )
                cppFlags += obfArgs
                cFlags += obfArgs
            }
        }
    }

    // ...
}
```

### Example with `app/build.gradle` and Java Native C++

In `app/build.gradle`:

```gradle
android {
    // ...
    def sdkDir = android.sdkDirectory
    ndkVersion = "29.0.13113456"
    ndkPath = "$sdkDir/ndk/29.0.13113456-obf"
    // ...
}
```

You may either pass the plugin through Gradle CMake flags or do it in `CMakeLists.txt`.

#### (1) Gradle passes the flags

```gradle
android {
    // ...
    defaultConfig {
        externalNativeBuild {
            cmake {
                def ndkDir = android.ndkDirectory
                def obfLib = "$ndkDir/toolchains/llvm/prebuilt/windows-x86_64/lib/libHikari.dll"

                cFlags "-fvisibility=hidden",
                       "-fpass-plugin=${obfLib}",
                       "-Xclang", "-load",
                       "-Xclang", "${obfLib}"

                cppFlags "-fvisibility=hidden",
                         "-fpass-plugin=${obfLib}",
                         "-Xclang", "-load",
                         "-Xclang", "${obfLib}",
                         "-mllvm", "-enable-strcry"
            }
        }
    }
    // ...
}
```

#### (2) Put the flags in your project `CMakeLists.txt`

```cmake
cmake_minimum_required(VERSION 3.4.1)
project(ProjectNative LANGUAGES C CXX)

# your project setup
# include_directories(...)
# add_library(...)

set(HIKARI_PLUGIN
    "${ANDROID_NDK}/toolchains/llvm/prebuilt/windows-x86_64/lib/libHikari.dll"
)

target_compile_options(Project PRIVATE
    # your own flags

    -fvisibility=hidden
    -fpass-plugin=${HIKARI_PLUGIN}
    -Xclang
    -load
    -Xclang=${HIKARI_PLUGIN}
    -mllvm
    -enable-strcry
)
```

---

## 12. Troubleshooting

### `JAVA_HOME is not set`
Install a **JDK 17 x64** and export `JAVA_HOME` before running `sdkmanager.bat`.

### `sdkmanager.bat` works but Android Studio still does not
If you are on **Windows ARM**, prefer installing the SDK/NDK with command-line tools first.  
Treat Android Studio itself as experimental on that host.

### `clang-r547379.tar.gz` extraction fails with symlink errors
Enable **Developer Mode** or run the extraction terminal **as Administrator** and use **7-Zip**, not plain `tar`.

### `cmake` not found in MSYS2
You are likely in the wrong shell.  
Use **MSYS2 MINGW64** and install:

```bash
pacman -S --needed mingw-w64-x86_64-cmake mingw-w64-x86_64-ninja
```

### Hikari link fails with `pthread_*` unresolved
Make sure you are using the Windows-patched `Obfuscation/CMakeLists.txt` from this guide, which links:

- `Threads::Threads`
- `winpthread`

and compiles/links with:

- `-pthread`
- `-femulated-tls`

### `clang-20.exe` is missing from `android-llvm/build/bin`
Create it manually:

```bash
cd /c/HikariObfuscator_Guide/Hikari/android-llvm/build/bin
cp clang.exe clang-20.exe
```

### Can I build Hikari directly against the original Windows AOSP prebuilt?
Not reliably.  
On Windows, use the AOSP prebuilt only as the **bootstrap compiler** to build a **full host LLVM**, then build `libHikari.dll` against that host LLVM.

---

## 13. Final result

If everything is correct, these should exist:

```text
C:\HikariObfuscator_Guide\Hikari\build\Obfuscation\libHikari.dll
C:\HikariObfuscator_Guide\Hikari\android-llvm\build\bin\clang.exe
C:\HikariObfuscator_Guide\Hikari\android-llvm\build\bin\clang++.exe
C:\HikariObfuscator_Guide\Hikari\android-llvm\build\bin\clang-20.exe
C:\Android\Sdk\ndk\29.0.13113456-obf\toolchains\llvm\prebuilt\windows-x86_64\bin\clang.exe
C:\Android\Sdk\ndk\29.0.13113456-obf\toolchains\llvm\prebuilt\windows-x86_64\bin\clang++.exe
C:\Android\Sdk\ndk\29.0.13113456-obf\toolchains\llvm\prebuilt\windows-x86_64\bin\clang-20.exe
C:\Android\Sdk\ndk\29.0.13113456-obf\toolchains\llvm\prebuilt\windows-x86_64\lib\libHikari.dll
```

That is the Windows-native equivalent of the original `darwin-x86_64 + libHikari.so` flow from the repository README.
