# ./CMakeLists.txt
cmake_minimum_required(VERSION 3.22)
project(Hikari-Obf-LLVM20-NDK29)

# Uncomment the appropriate architecture for your builds
# set(CMAKE_OSX_ARCHITECTURES x86_64)
set(CMAKE_OSX_ARCHITECTURES arm64)

set(CMAKE_C_COMPILER ${CMAKE_CURRENT_LIST_DIR}/clang-r547379/bin/clang)
set(CMAKE_CXX_COMPILER ${CMAKE_CURRENT_LIST_DIR}/clang-r547379/bin/clang++)

# Set LLVM_HOME so that find_package(LLVM) can locate LLVMConfig.cmake.
set(ENV{LLVM_HOME} ${CMAKE_CURRENT_LIST_DIR}/clang-r547379)

if (NOT DEFINED ENV{LLVM_HOME})
    message(FATAL_ERROR "$LLVM_HOME is not defined")
else ()
    set(ENV{LLVM_DIR} $ENV{LLVM_HOME}/lib/cmake/llvm)
endif ()

find_package(LLVM REQUIRED CONFIG)
add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})

set(CMAKE_CXX_STANDARD 17)

# Uncomment this for macOS arm64
add_compile_definitions(ENDIAN_LITTLE)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
set(CMAKE_SKIP_RPATH ON)

add_subdirectory(Obfuscation)
