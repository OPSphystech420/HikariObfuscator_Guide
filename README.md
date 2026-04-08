## Hikari is an LLVM IR-level obfuscator implemented as a Clang/LLVM pass plugin

In short, Hikari is loaded into the compiler during the build process and transforms LLVM IR before final code generation. This makes it possible to apply obfuscation at compile time, without manually rewriting your source code.

> [!NOTE]
> In this repo I provide a **step-by-step** guide to start off with obfuscation in your **iOS/Android** projects, which will be enough to protect your binary from *skid* or *vibe-code* reverse engineering 

Supports **Windows-native** set up now!

> [!WARNING]
> For other (industrial) purposes I do **not** recommend this free to use obfuscator as it's experimental and outdated. I do **not** take actions for your vulnerabilities.
>
> You may aswell check out [O-MVLL/dProtect](https://obfuscator.re/) - maintained mobile open-source obfuscator (harder to set up, but more functionality and convenient use)
>
> Or check out other comercial and free obfuscators [here](https://obfuscator.re/resources/)





## This guide currently covers functionality such as:

- Function call obfuscation
- Bogus control flow
- Instruction substitution
- Control-flow flattening
- Basic block splitting
- String encryption
- Constant encryption
- Indirect branch transformation
- Function wrappers
- Anti-debugging
- Anti-hooking
- Anti-class-dump
- Core plugin / obfuscation infrastructure

And more...

## Guide

The build guides are split into branches:

### iOS - Build Hikari with Xcode Toolchain / [Theos](https://theos.dev/)
  - [LLVM-16](https://github.com/OPSphystech420/HikariObfuscator_Guide/tree/build/ios-xcode-llvm16)


### Android - Build and port Hikari Plugin to [Android Studio](https://developer.android.com/studio?hl=en) (Very detailed guide)
- MacOS/Linux
  - [NDK r27 (LLVM-18 Pass)](https://github.com/OPSphystech420/HikariObfuscator_Guide/tree/build/android-ndk-llvm18);
  - [NDK r29 (LLVM-20 Pass)](https://github.com/OPSphystech420/HikariObfuscator_Guide/tree/build/android-ndk-llvm20).

- Windows-Native
  - [NDK r29 (LLVM-20 Pass)](https://github.com/OPSphystech420/HikariObfuscator_Guide/tree/build/windows-native/android-ndk-llvm20).


### Flags and usage read [here](FLAGS.md)


---

> [!TIP]
> You may leave an issue for further help.


