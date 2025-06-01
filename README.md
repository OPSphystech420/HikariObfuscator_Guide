Not Complete...

---
MacOS ARM64

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
```error
CMake Error at clang-r522817/lib/cmake/llvm/LLVMExports.cmake:1238 (message):
  The imported target "LLVMDemangle" references the file

     "/Users/grimreaper31/Desktop/HikariObfuscator_Guide/Hikari/clang-r522817/lib/libLLVMDemangle.a"

  but this file does not exist.  Possible reasons include:

  * The file was deleted, renamed, or moved to another location.

  * An install or uninstall procedure did not complete successfully.

  * The installation package was faulty and contained

     "/Users/grimreaper31/Desktop/HikariObfuscator_Guide/Hikari/clang-r522817/lib/cmake/llvm/LLVMExports.cmake"

  but not all the files it references.

Call Stack (most recent call first):
  clang-r522817/lib/cmake/llvm/LLVMConfig.cmake:287 (include)
  CMakeLists.txt:21 (find_package)
```
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
