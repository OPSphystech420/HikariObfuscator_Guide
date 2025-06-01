Not Complete...

---
MacOS ARM64

```bash
git clone --recursive -b build/ios-xcode-llvm16 https://github.com/OPSphystech420/HikariObfuscator_Guide.git
cd HikariObfuscator_Guide/Hikari
mkdir build && cd build
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_ARCHITECTURES=x86_64 -DLLVM_ENABLE_PROJECTS="clang;lld" -DLLVM_TARGETS_TO_BUILD="X86;ARM;AArch64" -DLLVM_ENABLE_ZSTD=OFF -DCMAKE_OSX_DEPLOYMENT_TARGET=12.0 -DLLVM_CREATE_XCODE_TOOLCHAIN=ON  -DLLVM_INCLUDE_TESTS=OFF -DLLVM_INCLUDE_EXAMPLES=OFF -DLLVM_INCLUDE_UTILS=OFF -DLLVM_INCLUDE_BENCHMARKS=OFF -DLLVM_BUILD_TOOLS=OFF -DLLVM_INSTALL_TOOLCHAIN_ONLY=ON -DCMAKE_INSTALL_PREFIX="./install" ../llvm
ninja
ninja install-xcode-toolchain
```

NOTE: `-DCMAKE_OSX_ARCHITECTURES` can be set to arm64 for Apple Silicon chips, but Hikari is built for x86_64. You have to use Rosseta Terminal via i386

See https://github.com/HikariObfuscator/Hikari/wiki
