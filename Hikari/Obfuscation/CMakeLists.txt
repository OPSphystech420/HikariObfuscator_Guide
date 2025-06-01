# ./Obfuscation/CMakeLists.txt

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

# LLVM is (typically) built with no C++ RTTI. We need to match that;
# otherwise, we'll get linker errors about missing RTTI data.
set_target_properties(Hikari PROPERTIES
        COMPILE_FLAGS "-fno-rtti"
)

# Get proper shared-library behavior (where symbols are not necessarily
# resolved when the shared library is linked) on OS X.
if (APPLE)
    set_target_properties(Hikari PROPERTIES
            LINK_FLAGS "-undefined dynamic_lookup"
    )
endif (APPLE)

execute_process(
        COMMAND git log -1 --format=%H
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
        OUTPUT_VARIABLE HIKARI_GIT_COMMIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
)
target_compile_definitions(Hikari PRIVATE "-DGIT_COMMIT_HASH=\"${HIKARI_GIT_COMMIT_HASH}\"")
