# iOS Toolchain File for CMake
# Usage: cmake -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/ios.cmake ..

set(CMAKE_SYSTEM_NAME iOS)
set(CMAKE_SYSTEM_VERSION 14.0)
set(CMAKE_OSX_DEPLOYMENT_TARGET 14.0 CACHE STRING "Minimum iOS deployment version")

# Set the target architecture
set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "iOS architecture")

# Use Xcode SDK
execute_process(
    COMMAND xcrun --sdk iphoneos --show-sdk-path
    OUTPUT_VARIABLE CMAKE_OSX_SYSROOT
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(NOT CMAKE_OSX_SYSROOT)
    message(FATAL_ERROR "iOS SDK not found. Please install Xcode and command line tools:\n  xcode-select --install")
endif()

message(STATUS "iOS SDK: ${CMAKE_OSX_SYSROOT}")

# Set compilers
execute_process(
    COMMAND xcrun --sdk iphoneos --find clang
    OUTPUT_VARIABLE CMAKE_C_COMPILER
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
    COMMAND xcrun --sdk iphoneos --find clang++
    OUTPUT_VARIABLE CMAKE_CXX_COMPILER
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# iOS-specific flags
set(CMAKE_C_FLAGS_INIT "-mios-version-min=${CMAKE_OSX_DEPLOYMENT_TARGET}")
set(CMAKE_CXX_FLAGS_INIT "-mios-version-min=${CMAKE_OSX_DEPLOYMENT_TARGET}")

# Skip compiler checks (cross-compiling)
set(CMAKE_C_COMPILER_WORKS TRUE)
set(CMAKE_CXX_COMPILER_WORKS TRUE)

# Find root path for libraries
set(CMAKE_FIND_ROOT_PATH ${CMAKE_OSX_SYSROOT})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

message(STATUS "iOS Toolchain configured for arm64")
