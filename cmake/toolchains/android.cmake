# Android Toolchain File for CMake
# Usage: cmake -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/android.cmake ..

# Read Android NDK path from toolchain.properties (if it exists)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/toolchain.properties")
    file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/toolchain.properties" TOOLCHAIN_PROPS)
    foreach(line ${TOOLCHAIN_PROPS})
        if(line MATCHES "^android\\.ndk\\.dir=(.*)$")
            set(ANDROID_NDK_PATH "${CMAKE_MATCH_1}")
            # Expand ${user.name} variable if present
            if(ANDROID_NDK_PATH MATCHES "\\$\\{user\\.name\\}")
                string(REPLACE "\${user.name}" "$ENV{USER}" ANDROID_NDK_PATH "${ANDROID_NDK_PATH}")
            endif()
            # Also handle /Users/${user.name} → $HOME pattern
            string(REGEX REPLACE "/Users/\\$\\{user\\.name\\}" "$ENV{HOME}" ANDROID_NDK_PATH "${ANDROID_NDK_PATH}")
        endif()
    endforeach()
endif()

# Check if NDK exists
if(NOT ANDROID_NDK_PATH OR NOT EXISTS "${ANDROID_NDK_PATH}")
    # Try common locations
    if(EXISTS "$ENV{HOME}/Library/Android/sdk/ndk")
        file(GLOB NDK_VERSIONS "$ENV{HOME}/Library/Android/sdk/ndk/*")
        list(GET NDK_VERSIONS 0 ANDROID_NDK_PATH)
    elseif(EXISTS "$ENV{HOME}/Android/Sdk/ndk")
        file(GLOB NDK_VERSIONS "$ENV{HOME}/Android/Sdk/ndk/*")
        list(GET NDK_VERSIONS 0 ANDROID_NDK_PATH)
    elseif(EXISTS "$ENV{USERPROFILE}/AppData/Local/Android/Sdk/ndk")
        file(GLOB NDK_VERSIONS "$ENV{USERPROFILE}/AppData/Local/Android/Sdk/ndk/*")
        list(GET NDK_VERSIONS 0 ANDROID_NDK_PATH)
    endif()
endif()

if(NOT ANDROID_NDK_PATH OR NOT EXISTS "${ANDROID_NDK_PATH}")
    message(FATAL_ERROR
        "Android NDK not found!\n"
        "Please install Android NDK:\n"
        "  1. Download from: https://developer.android.com/ndk/downloads\n"
        "  2. Or use Android Studio SDK Manager\n"
        "  3. Update android.ndk.dir in toolchain.properties\n"
        "Current path checked: ${ANDROID_NDK_PATH}"
    )
endif()

message(STATUS "Android NDK: ${ANDROID_NDK_PATH}")

# Set Android NDK toolchain
set(CMAKE_ANDROID_NDK "${ANDROID_NDK_PATH}")
set(CMAKE_SYSTEM_NAME Android)
set(CMAKE_SYSTEM_VERSION 21)  # Android 5.0 minimum
set(CMAKE_ANDROID_ARCH_ABI arm64-v8a)  # Can be: armeabi-v7a, arm64-v8a, x86, x86_64
set(CMAKE_ANDROID_STL_TYPE c++_shared)

# Use NDK's toolchain file
if(EXISTS "${CMAKE_ANDROID_NDK}/build/cmake/android.toolchain.cmake")
    include("${CMAKE_ANDROID_NDK}/build/cmake/android.toolchain.cmake")
else()
    message(FATAL_ERROR "NDK toolchain file not found in ${CMAKE_ANDROID_NDK}/build/cmake/")
endif()

message(STATUS "Android Toolchain configured for ${CMAKE_ANDROID_ARCH_ABI}, API ${CMAKE_SYSTEM_VERSION}")
