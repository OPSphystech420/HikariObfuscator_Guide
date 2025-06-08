# Hikari Plugin Android Studio 2024.3.2.15 NDK r29.0.13113456 (LLVM-20)

[Android Studio](https://developer.android.com/studio?hl=en)

[Android llvm-project](https://android.googlesource.com/toolchain/llvm-project)

> [!NOTE]
> We will be using [**clang-r547379**](https://android.googlesource.com/platform/prebuilts/clang/host/linux-x86/+/refs/heads/main/clang-r547379) – essentially LLVM/Clang 20.0.0 as packaged by AOSP/NDK, meaning it supports all LLVM 20–era front-end capabilities (including C++2a/C++2b features as of early 2025) and targets a broad range of backends for x86, ARM, AArch64, RISC-V, WebAssembly, etc. It also incorporates Android-specific toolchain adaptations such as updated libc++, sanitizers, LTO support, and BOLT optimizations, alongside numerous platform-specific patches (e.g., `BOLT-Increase-max-allocation-size-to-allow-BOLTing-clang-and-rustc.patch`) that ensure compatibility on Android devices. This Clang revision aligns with **NDK r29 (r29.0.13113456)**, and the upstream source commit for clang-r547379 is **b718bcaf8c198c82f3021447d943401e3ab5bd54** in the `llvm-project` repository.

# Guide

### MacOS ARM64 (should work universally on MacOS x86_64 and Linux)

> [!WARNING]
> If you don't have `CC` and `CXX` set in your [environment](#you-may-as-well-set-your-cc-and-cxx-for-future-uses), export default Xcode's clang or any other you wish
> ```bash
> export CC="$(xcrun --sdk macosx --find clang)"
> export CXX="$(xcrun --sdk macosx --find clang++)"
> ```
> So you don't get error, during cmake configuration
> ```error
> CMake Error (message):
>     Could not find compiler set in environment variable CC:
> ```
> ❗ Do not use our `clang-r522817` for that, as it is already included into our `CMakeLists.txt` and during llvm-project build you will get errors
> (You must use the same clang for Hikari plugin and llvm-project builds)

In our [`CMakeLists.txt`](https://github.com/OPSphystech420/HikariObfuscator_Guide/blob/build/android-ndk-llvm18/Hikari/CMakeLists.txt) you may change `CMAKE_OSX_ARCHITECTURES` to match your needs (set to arm64 as default)

```bash
git clone --recursive -b build/android-ndk-llvm20 https://github.com/OPSphystech420/HikariObfuscator_Guide.git
```

```bash
cd HikariObfuscator_Guide/Hikari
mkdir -p clang-r547379 && pushd clang-r547379
curl -L https://android.googlesource.com/platform/prebuilts/clang/host/darwin-x86/+archive/aff5a55a47b1a512815f21aae43475681a8bf238/clang-r547379.tar.gz | tar xz && popd
mkdir -p build && pushd build
cmake -G Ninja -DCMAKE_BUILD_TYPE=MinSizeRel ../
cmake --build .
popd
```

If during `cmake -G Ninja -DCMAKE_BUILD_TYPE=MinSizeRel ../` you get
> [!CAUTION]
>```error
>  CMake Error at clang-r547379/lib/cmake/llvm/LLVMExports.cmake:1238 (message):
>  The imported target "LLVMDemangle" references the file
>
>      "../HikariObfuscator_Guide/Hikari/clang-r547379/lib/libLLVMDemangle.a"
>
>  but this file does not exist.  Possible reasons include:
>
>  * The file was deleted, renamed, or moved to another location.
>
>  * An install or uninstall procedure did not complete successfully.
>
>  * The installation package was faulty and contained
>
>      "../HikariObfuscator_Guide/Hikari/clang-r547379/lib/cmake/llvm/LLVMExports.cmake"
>
>  but not all the files it references.
>
>  Call Stack (most recent call first):
>    clang-r547379/lib/cmake/llvm/LLVMConfig.cmake:287 (include)
>    CMakeLists.txt:21 (find_package)
>```


Do the following:
```bash
pushd ./clang-r547379/lib/cmake/llvm
cp LLVMExports.cmake LLVMExports.cmake.bak
sed -i.bak '/foreach(target ${_IMPORT_CHECK_TARGETS}/,/unset(_IMPORT_CHECK_TARGETS)/ s/^/#/' LLVMExports.cmake
popd
pushd build && rm -rf *
cmake -G Ninja -DCMAKE_BUILD_TYPE=MinSizeRel ../
cmake --build .
popd
```

Now you may find our plugin `libHikari.so` at `/HikariObfuscator_Guide/Hikari/build/Obfuscation`

---
> [!NOTE]
> If you load libHikari directly to the original NDK, you will encounter a number of errors and problems, including llvm not being able to recognize Hikari's obfuscation flags, for example `error: unable to load plugin 'libHikari.so': 'dlopen(libHikari.so, 0x0009): symbol not found in flat namespace '__ZTVN4llvm2cl6OptionE''`
>
> This is because the clang binary symbols included in the official NDK are incomplete. You need to build Android LLVM yourself to get the complete clang-18, clang and clang++ symbols to replace the corresponding three files included in the NDK. More info can be found [here](https://leadroyal.cn/p/1008/)

---

### Porting with Android Studio 2024.3.2.15 NDK r29.0.13113456

NDK r29.0.13113456 uses Clang version r547379, as if you followed previous steps, we have configured our libHikari.so to use this version prebuilt by Google for Android platform (AOSP).

(You may see `$ANDROID_SDK_ROOT/ndk/29.0.13113456/toolchains/llvm/prebuilt/darwin-x86_64/bin/clang --version`)

1. We will clone llvm-project and set it to NDK version we need (b718bcaf8c198c82f3021447d943401e3ab5bd54)
    ```bash
    mkdir -p android-llvm && cd android-llvm
    git clone https://android.googlesource.com/toolchain/llvm-project
    pushd llvm-project
    git checkout b718bcaf8c198c82f3021447d943401e3ab5bd54
    popd
    ```
2. Configure and build LLVM with CMake
    
   As default оn Apple platforms, CMake inspects the host compiler and the CMake binary’s own architecture to determine target slices. If you don’t set 
   `CMAKE_OSX_ARCHITECTURES`, it simply builds for the host’s architecture (the architecture of the clang/CMake used to configure).
   
   We will set `-DCMAKE_OSX_ARCHITECTURES=arm64`, you may change it to `x86_64` or `"arm64;x86_64"` aka universal
   ```bash
   mkdir -p build && cd build
   cmake -G "Ninja" ../llvm-project/llvm \
      -DCMAKE_INSTALL_PREFIX="./llvm_x64" \
      -DCMAKE_CXX_STANDARD=17 \
      -DCMAKE_BUILD_TYPE=MinSizeRel \
      -DCMAKE_OSX_ARCHITECTURES=arm64 \
      -DLLVM_APPEND_VC_REV=ON \
      -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra;compiler-rt;lld;mlir;openmp;polly;" \
      -DLLVM_ENABLE_RUNTIMES="libcxx;libcxxabi;libunwind" \
      -DLLVM_TARGETS_TO_BUILD="X86;ARM;AArch64" \
      -DLLVM_INSTALL_UTILS=ON \
      -DLLVM_INCLUDE_TESTS=OFF \
      -DLLVM_BUILD_TESTS=OFF \
      -DLLVM_INCLUDE_BENCHMARKS=OFF \
      -DLLVM_BUILD_BENCHMARKS=OFF \
      -DLLVM_INCLUDE_EXAMPLES=OFF \
      -DLLVM_ENABLE_BACKTRACES=OFF \
      -DLLVM_BUILD_DOCS=OFF
   cmake --build .
   ```
4. Install LLVM using Ninja
   ```bash
   ninja install
   ```

Now in your `./android-llvm/build/bin/` you may find `clang`, `clang++`, `clang-20`
```
clang --version
clang version 20.0.0git (https://android.googlesource.com/toolchain/llvm-project b718bcaf8c198c82f3021447d943401e3ab5bd54)
Target: x86_64-apple-darwin24.3.0
Thread model: posix
```

---

If you haven't installed NDK r29.0.13113456 yet, follow this steps

Open your `Android Studio` and go to `Tools` -> `SDK Manager`

![](https://github.com/OPSphystech420/HikariObfuscator_Guide/blob/main/imgs/tools.png)


Open `SDK Tools` tab, enable `Show Package Details`, find `NDK (Side by side)`/`29.0.13113456`, `Apply`, press `OK`

![](https://github.com/OPSphystech420/HikariObfuscator_Guide/blob/main/imgs/SDKmanagerNDKr29.png)


---

> [!TIP]
> If you haven't yet, set `$ANDROID_SDK_ROOT` path in your MacOS environment
>
> ```bash
> echo $0
> ```
> for `zsh` you may edit `~/.zshrc` or `~/.zprofile`, for bash – `~/.bashrc`, or `~/.bash_profile` on newer MacOS
> ```bash
> nano ~/.zshrc
> ```
> export the path, it is specified at `Tools` -> `SDK Manager`, for example
>
> `export ANDROID_SDK_ROOT="/Users/user/Library/Android/sdk"` 
>
> apply changes 
> ```bash
> source ~/.zshrc
> ```
> ```bash
> echo $ANDROID_SDK_ROOT
> ```
> `/Users/user/Library/Android/sdk`
> 
> ##### You may as well set your `CC` and `CXX` for future uses
> ```bash
> $CC --version
> ```

---

Now we will replace `clang`, `clang++` and `clang-20` in `$ANDROID_SDK_ROOT/ndk/27.0.12077973/toolchains/llvm/prebuilt/darwin-x86_64/bin` with those, which we've got from llvm-project

We will copy our NDK `29.0.13113456` into `29.0.13113456-obf` and apply changes to it
```bash
cd $ANDROID_SDK_ROOT/ndk
cp -r 29.0.13113456 29.0.13113456-obf
cd 29.0.13113456-obf/toolchains/llvm/prebuilt/darwin-x86_64/bin
mv clang clang.bak
mv clang++ clang++.bak
mv clang-20 clang-20.bak
```
Now copy `clang`, `clang++` and `clang-20` into this `bin` direcotry of `27.0.12077973-obf`
```bash
cp /your_path_to/HikariObfuscator_Guide/Hikari/android-llvm/build/bin/clang        .
cp /your_path_to/HikariObfuscator_Guide/Hikari/android-llvm/build/bin/clang++      .
cp /your_path_to/HikariObfuscator_Guide/Hikari/android-llvm/build/bin/clang-20     .
```

---

### Including Hikari obfuscation into your Android project

You may move `libHikari.so` to your project directory or store within `29.0.13113456-obf` libs, for example
```
cp    /your_path_to/HikariObfuscator_Guide/Hikari/build/Obfuscation/libHikari.so    $ANDROID_SDK_ROOT/ndk/29.0.13113456-obf/toolchains/llvm/prebuilt/darwin-x86_64/lib/
```

**Example porting with cmake `build.gradle.kts (Module :app)`, Android Kotlin Native C++ project**
```gradle
android {
    // ...
    val sdkDir = android.sdkDirectory
    ndkVersion = "29.0.13113456"
    ndkPath = "$sdkDir/ndk/29.0.13113456-obf" // you must use our modified NDK here
    // ...
    defaultConfig {
        externalNativeBuild {
            cmake {
                val ndkDir = android.ndkDirectory
                val obfLibDir =
                     "$ndkDir/toolchains/llvm/prebuilt/darwin-x86_64/lib/libHikari.so"
                val obfArgs = listOf(
                    "-fvisibility=hidden",
                    "-fpass-plugin=$obfLibDir",
                    "-Xclang",                         // -Xclang <arg>, -Xclang=<arg> - Pass <arg> to clang -cc1
                    "-load",
                    "-Xclang",
                    obfLibDir,
                    "-mllvm",
                    "-enable-strcry" // string encryption flag
                )
                cppFlags += obfArgs
                cFlags += obfArgs
            }
        }
    }
    // ...
}
```

**Example porting with cmake `app/build.gradle` and `app/../cpp/CMakeLists.txt`, Android Java Native C++ project**

in `app/build.gradle` specify
```gradle
android {
    // ...
    def sdkDir = android.sdkDirectory
    ndkVersion = "29.0.13113456"
    ndkPath = "$sdkDir/ndk/29.0.13113456-obf"
    // ...
```
You have two options, you may load the plugin and specify the flags with **gradle cmake** (1) or you may do it in CMakeLists.txt **target_compile_options** (2).

(1)
```gradle
android {
    // ...
    defaultConfig {
        // ...
        externalNativeBuild {
            cmake {
                def ndkDir = android.ndkDirectory
                def obfLib = "$ndkDir/toolchains/llvm/prebuilt/darwin-x86_64/lib/libHikari.so"

                cFlags  "-fvisibility=hidden",
                        "-fpass-plugin=${obfLib}",
                        "-Xclang", "-load", "-Xclang", "${obfLib}"  // -Xclang <arg>, -Xclang=<arg> - Pass <arg> to clang -cc1
                cppFlags "-fvisibility=hidden",
                        "-fpass-plugin=${obfLib}",
                        "-Xclang", "-load", "-Xclang", "${obfLib}"  // -Xclang <arg>, -Xclang=<arg> - Pass <arg> to clang -cc1

                cppFlags "-mllvm", "-enable-strcry"  // string encryption flag
            }
        }
    }
    // ...
}
```

(2)
```cmake
# -------------------------------
#  CMakeLists.txt for ProjectNative
# -------------------------------

cmake_minimum_required(VERSION 3.4.1)

project(ProjectNative LANGUAGES C CXX)

# your Project set
# ...

# your Project include_directories and add_library 
# ...

set(HIKARI_PLUGIN
        "${ANDROID_NDK}/toolchains/llvm/prebuilt/darwin-x86_64/lib/libHikari.so"     # set path
)

target_compile_options(Project PRIVATE
        # your C & C++ flags
        # ...

        # Obfuscation flags
        -fvisibility=hidden
        -fpass-plugin=${HIKARI_PLUGIN}
        -Xclang                             # -Xclang <arg>, -Xclang=<arg> - Pass <arg> to clang -cc1
        -load
        -Xclang=${HIKARI_PLUGIN}
        -mllvm
        -enable-strcry # string encryption flag
        
)

# ...

```

**Example porting with NDK-Build (`app/build.gradle` and `app/../jni/Android.mk`)**

As in previous example, if we follow (1), `ndkBuild` will cause
> [!CAUTION]
>```error
> GNUMAKE: Expected exactly one source file in compile step
>```

To resolve such issue, you must choose `-Xclang=<arg>` notation, for example 
```gradle
android {
    // ...
    def sdkDir = android.sdkDirectory
    ndkVersion = "29.0.13113456"
    ndkPath = "$sdkDir/ndk/29.0.13113456-obf"
    // ...
    defaultConfig {
        // ...
        externalNativeBuild {
            ndkBuild {         // ndkBuild

                def ndkDir = android.ndkDirectory
                def obfLib = "$ndkDir/toolchains/llvm/prebuilt/darwin-x86_64/lib/libHikari.so"

                cFlags  "-fvisibility=hidden",
                        "-fpass-plugin=${obfLib}",
                        "-Xclang", "-load", "-Xclang=${obfLib}" //  -Xclang=<arg>
                cppFlags "-fvisibility=hidden",
                        "-fpass-plugin=${obfLib}",
                        "-Xclang", "-load", "-Xclang=${obfLib}" //  -Xclang=<arg>

                cppFlags "-mllvm", "-enable-strcry"
            }
        }
    }
    // ...
}
```
In cases you are willing to use `Android.mk` to specify the flags 
```mk
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

# Your static libraries include
# ...

# Your library
include $(CLEAR_VARS)
LOCAL_MODULE    := Project

# Your C, CPP, LD... flags 
# LOCAL_CFLAGS := ...
# LOCAL_CPPFLAGS := ...
# ...

# Obfuscation
HIKARI_PLUGIN := $(NDK_ROOT)/toolchains/llvm/prebuilt/darwin-x86_64/lib/libHikari.so

LOCAL_CFLAGS   += -fpass-plugin=$(HIKARI_PLUGIN) -Xclang -load
LOCAL_CFLAGS   += -Xclang=$(HIKARI_PLUGIN)                         #  -Xclang=<arg>
LOCAL_CFLAGS   += -mllvm -enable-strcry

LOCAL_CPPFLAGS += -fpass-plugin=$(HIKARI_PLUGIN) -Xclang -load
LOCAL_CPPFLAGS += -Xclang=$(HIKARI_PLUGIN)                         #  -Xclang=<arg>
LOCAL_CPPFLAGS += -mllvm -enable-strcry

# Your includes
LOCAL_C_INCLUDES += $(LOCAL_PATH)
# ...

# Your files to compile
# LOCAL_SRC_FILES := ...

# Your static libraries
# LOCAL_STATIC_LIBRARIES := ...

include $(BUILD_SHARED_LIBRARY)
```

---

