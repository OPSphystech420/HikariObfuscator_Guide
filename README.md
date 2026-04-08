guide will be updated later

---

## Building

```bash
git clone --recursive -b build/ios-xcode-llvm16 https://github.com/OPSphystech420/HikariObfuscator_Guide.git
cd HikariObfuscator_Guide/Hikari
mkdir build && cd build
cmake -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_OSX_ARCHITECTURES=x86_64 \
  -DLLVM_ENABLE_PROJECTS="clang;lld" \
  -DLLVM_TARGETS_TO_BUILD="X86;ARM;AArch64" \
  -DLLVM_ENABLE_ZSTD=OFF \
  -DCMAKE_OSX_DEPLOYMENT_TARGET=12.0 \
  -DLLVM_CREATE_XCODE_TOOLCHAIN=ON \
  -DLLVM_INCLUDE_TESTS=OFF \
  -DLLVM_INCLUDE_EXAMPLES=OFF \
  -DLLVM_INCLUDE_UTILS=OFF \
  -DLLVM_INCLUDE_BENCHMARKS=OFF \
  -DLLVM_BUILD_TOOLS=OFF \
  -DLLVM_INSTALL_TOOLCHAIN_ONLY=ON \
  -DCMAKE_INSTALL_PREFIX="./install" \
  ../llvm
ninja
ninja install-xcode-toolchain

mv your_path/build/Toolchains/Hikari_LLVM16.0.0.xctoolchain /Library/Developer/Toolchains/Hikari_LLVM16.0.0.xctoolchain
```

NOTE: `-DCMAKE_OSX_ARCHITECTURES` can be set to arm64 for Apple Silicon chips, but Hikari is built for x86_64. You have to use Rosseta Terminal via i386

See https://github.com/HikariObfuscator/Hikari/wiki

## Theos usage:

In your Makefile
```Make
ARCHS = arm64
TARGET = iphone:clang:16.5:15

export TARGET_CC=/Library/Developer/Toolchains/Hikari_LLVM16.0.0.xctoolchain/usr/bin/clang
export TARGET_CXX=/Library/Developer/Toolchains/Hikari_LLVM16.0.0.xctoolchain/usr/bin/clang++

..

OBFUSCATED_CFLAGS = -mllvm -enable-cffobf -mllvm -enable-strcry ..
OBFUSCATED_CCFLAGS = $(OBFUSCATED_CFLAGS)


```

## Xcode usage:
Switch to

![](https://github.com/OPSphystech420/HikariObfuscator_Guide/blob/main/imgs/toolchain.png)

Add flags here

![](https://github.com/OPSphystech420/HikariObfuscator_Guide/blob/main/imgs/xflags.png)

---

> [!TIP]
> More about flags [here](https://github.com/OPSphystech420/HikariObfuscator_Guide/blob/main/FLAGS.md)
