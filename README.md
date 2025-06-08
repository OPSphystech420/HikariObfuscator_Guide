# Hikari Plugin Android Studio 2024.3.2.15 NDK r27 (LLVM-18)

[Android Studio](https://developer.android.com/studio?hl=en)

[Android llvm-project](https://android.googlesource.com/toolchain/llvm-project)

> [!NOTE]
> We will be using [**clang-r522817**](https://android.googlesource.com/platform/prebuilts/clang/host/darwin-x86/+/1c8f09d76cb556336e677ef21111c1d7b20775e4/clang-r522817/) – essentially LLVM/Clang 18.0.1 as packaged by AOSP/NDK, meaning it supports all LLVM 18–era front-end capabilities (C 1x/C++2a/C++2b features as of early 2024), backends for x86, ARM, AArch64, RISC-V, WebAssembly, etc., and the Android-specific toolchain adaptations (libc++, sanitizers, LTO, BOLT, etc.)    (**NDK revision r27 - d8003a456d14a3deb8054cdaa529ffbf02d9b262**)

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
git clone --recursive -b build/android-ndk-llvm18 https://github.com/OPSphystech420/HikariObfuscator_Guide.git
```

```bash
cd HikariObfuscator_Guide/Hikari
mkdir -p clang-r522817 && pushd clang-r522817
curl -L https://android.googlesource.com/platform/prebuilts/clang/host/darwin-x86/+archive/1c8f09d76cb556336e677ef21111c1d7b20775e4/clang-r522817.tar.gz | tar xz && popd
mkdir -p build && pushd build
cmake -G Ninja -DCMAKE_BUILD_TYPE=MinSizeRel ../
cmake --build .
popd
```

If during `cmake -G Ninja -DCMAKE_BUILD_TYPE=MinSizeRel ../` you get
> [!CAUTION]
>```error
>  CMake Error at clang-r522817/lib/cmake/llvm/LLVMExports.cmake:1238 (message):
>  The imported target "LLVMDemangle" references the file
>
>      "../HikariObfuscator_Guide/Hikari/clang-r522817/lib/libLLVMDemangle.a"
>
>  but this file does not exist.  Possible reasons include:
>
>  * The file was deleted, renamed, or moved to another location.
>
>  * An install or uninstall procedure did not complete successfully.
>
>  * The installation package was faulty and contained
>
>      "../HikariObfuscator_Guide/Hikari/clang-r522817/lib/cmake/llvm/LLVMExports.cmake"
>
>  but not all the files it references.
>
>  Call Stack (most recent call first):
>    clang-r522817/lib/cmake/llvm/LLVMConfig.cmake:287 (include)
>    CMakeLists.txt:21 (find_package)
>```


Do the following:
```bash
pushd ./clang-r522817/lib/cmake/llvm
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

### Porting with Android Studio 2024.3.2.15 NDK r27

NDK r27 (`27.0.12077973`) uses Clang version r522817, as if you followed previous steps, we have configured our libHikari.so to use this version prebuilt by Google for Android platform (AOSP).

(You may see `$ANDROID_SDK_ROOT/ndk/27.0.12077973/toolchains/llvm/prebuilt/darwin-x86_64/bin/clang --version`)

1. We will clone llvm-project and set it to NDK version we need (d8003a456d14a3deb8054cdaa529ffbf02d9b262)
    ```bash
    mkdir -p android-llvm && cd android-llvm
    git clone https://android.googlesource.com/toolchain/llvm-project
    pushd llvm-project
    git checkout d8003a456d14a3deb8054cdaa529ffbf02d9b262
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
3. Install LLVM using Ninja
   ```bash
   ninja install
   ```

Now in your `./android-llvm/build/bin/` you may find `clang`, `clang++`, `clang-18`
```
clang --version
clang version 18.0.0git (https://android.googlesource.com/toolchain/llvm-project d8003a456d14a3deb8054cdaa529ffbf02d9b262)
Target: x86_64-apple-darwin24.3.0
Thread model: posix
```

---

If you haven't installed NDK r27 yet, follow this steps

Open your `Android Studio` and go to `Tools` -> `SDK Manager`

![](https://github.com/OPSphystech420/HikariObfuscator_Guide/blob/main/imgs/tools.png)


Open `SDK Tools` tab, enable `Show Package Details`, find `NDK (Side by side)`/`27.0.12077973`, `Apply`, press `OK`

![](https://github.com/OPSphystech420/HikariObfuscator_Guide/blob/main/imgs/SDKmanagerNDKr27.png)


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

Now we will replace `clang`, `clang++` and `clang-18` in `$ANDROID_SDK_ROOT/ndk/27.0.12077973/toolchains/llvm/prebuilt/darwin-x86_64/bin` with those, which we've got from llvm-project

We will copy our NDK `27.0.12077973` into `27.0.12077973-obf` and apply changes to it
```bash
cd $ANDROID_SDK_ROOT/ndk
cp -r 27.0.12077973 27.0.12077973-obf
cd 27.0.12077973-obf/toolchains/llvm/prebuilt/darwin-x86_64/bin
mv clang clang.bak
mv clang++ clang++.bak
mv clang-18 clang-18.bak
```
Now copy `clang`, `clang++` and `clang-18` into this `bin` direcotry of `27.0.12077973-obf`
```bash
cp /your_path_to/HikariObfuscator_Guide/Hikari/android-llvm/build/bin/clang        .
cp /your_path_to/HikariObfuscator_Guide/Hikari/android-llvm/build/bin/clang++      .
cp /your_path_to/HikariObfuscator_Guide/Hikari/android-llvm/build/bin/clang-18     .
```

---

### Including Hikari obfuscation into your Android project

You may move `libHikari.so` to your project directory or store within `27.0.12077973-obf` libs, for example
```
cp    /your_path_to/HikariObfuscator_Guide/Hikari/build/Obfuscation/libHikari.so    $ANDROID_SDK_ROOT/ndk/27.0.12077973-obf/toolchains/llvm/prebuilt/darwin-x86_64/lib/
```

Example porting with cmake `build.gradle.kts (Module :app)`, Android Kotlin Native C++ project
```gradle
android {
    // ...
    val sdkDir = android.sdkDirectory
    ndkVersion = "27.0.12077973"
    ndkPath = "$sdkDir/ndk/27.0.12077973-obf" // you must use our modified NDK here
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

Example porting with cmake `app/build.gradle` and `app/src/main/cpp/CMakeLists.txt`, Android Java Native C++ project

in `app/build.gradle` specify
```gradle
android {
    // ...
    def sdkDir = android.sdkDirectory
    ndkVersion = "27.0.12077973"
    ndkPath = "$sdkDir/ndk/27.0.12077973-obf"
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

**No examples porting to `Android.mk` and `Application.mk`**

`ndkBuild` will cause
> [!CAUTION]
>```error
> GNUMAKE: Expected exactly one source file in compile step
>```
Not resolved

