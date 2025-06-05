# Hikari Plugin Android Studio 2024.3.2.15 NDK r27 (LLVM-18)

[Android Studio](https://developer.android.com/studio?hl=en)

[Android llvm-project](https://android.googlesource.com/toolchain/llvm-project)

We will be using [**clang-r522817**](https://android.googlesource.com/platform/prebuilts/clang/host/darwin-x86/+/1c8f09d76cb556336e677ef21111c1d7b20775e4/clang-r522817/) – essentially LLVM/Clang 18.0.1 as packaged by AOSP/NDK, meaning it supports all LLVM 18–era front-end capabilities (C 1x/C++2a/C++2b features as of early 2024), backends for x86, ARM, AArch64, RISC-V, WebAssembly, etc., and the Android-specific toolchain adaptations (libc++, sanitizers, LTO, BOLT, etc.)    (**NDK revision r27 - d8003a456d14a3deb8054cdaa529ffbf02d9b262**)

---
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
> ❗ Do not use our `clang-r522817` for that, otherwise during llvm-project build you will get errors
> (You must use the same clang for Hikari plugin and llvm-project builds)

```bash
git clone --recursive -b build/android-ndk-llvm18 https://github.com/OPSphystech420/HikariObfuscator_Guide.git
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

NDK r27 (`27.0.12077973`) uses Clang version r522817, as if you followed previous steps, we have configured our Hikari lib to use this version of Clang prebuilt by Google for Android platform use (AOSP).

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
   ```bash
   mkdir -p build && cd build
   cmake -G "Ninja" ../llvm-project/llvm \
      -DCMAKE_INSTALL_PREFIX="./llvm_x64" \
      -DCMAKE_CXX_STANDARD=17 \
      -DCMAKE_BUILD_TYPE=MinSizeRel \
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

Now we will replace `clang`, `clang++` and `clang++` in `$ANDROID_SDK_ROOT/ndk/27.0.12077973/toolchains/llvm/prebuilt/darwin-x86_64/bin` with those, which we've got from llvm-project

