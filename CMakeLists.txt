cmake_minimum_required(VERSION 3.15)
project(MultiPlatformGUI VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# BUILD_PLATFORM: Where we're compiling (host machine)
set(BUILD_PLATFORM "${CMAKE_HOST_SYSTEM_NAME}")

# TARGET_PLATFORM: What we're compiling for (target platform)
# Determine from CMAKE_SYSTEM_NAME (which always refers to target)
if(CMAKE_TOOLCHAIN_FILE)
    # Cross-compiling: CMAKE_SYSTEM_NAME set by toolchain file
    if(CMAKE_SYSTEM_NAME STREQUAL "iOS")
        set(TARGET_PLATFORM "iOS")
        add_compile_definitions(PLATFORM_IOS)
    elseif(CMAKE_SYSTEM_NAME STREQUAL "Android")
        set(TARGET_PLATFORM "Android")
        add_compile_definitions(PLATFORM_ANDROID)
    elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
        set(TARGET_PLATFORM "Windows")
        add_compile_definitions(PLATFORM_WINDOWS)
    else()
        set(TARGET_PLATFORM "${CMAKE_SYSTEM_NAME}")
    endif()
else()
    # Native build: target = build platform, use convenience variables
    if(WIN32)
        set(TARGET_PLATFORM "Windows")
        add_compile_definitions(PLATFORM_WINDOWS)
    elseif(APPLE)
        set(TARGET_PLATFORM "MacOS")
        add_compile_definitions(PLATFORM_MACOS)
    elseif(UNIX)
        set(TARGET_PLATFORM "Linux")
        add_compile_definitions(PLATFORM_LINUX)
    endif()
endif()

message(STATUS "BUILD_PLATFORM (host): ${BUILD_PLATFORM}")
message(STATUS "TARGET_PLATFORM (target): ${TARGET_PLATFORM}")

# Define build tool
add_compile_definitions(BUILD_TOOL="CMake")

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Source files
set(SOURCES
    src/main.cpp
    src/App.cpp
    src/GuiManager.cpp
)

#
# SDL2 Build Targets (build SDL2 if not available)
#
set(SDL2_SOURCE_DIR "${CMAKE_SOURCE_DIR}/ext/SDL2")
set(SDL2_PREBUILT_DIR "${CMAKE_SOURCE_DIR}/ext/SDL2-install")

# Custom target to build SDL2 for current TARGET_PLATFORM
add_custom_target(build-sdl2
    COMMENT "Building SDL2 for ${TARGET_PLATFORM}..."
)

# Platform-specific SDL2 build commands
if(NOT CMAKE_TOOLCHAIN_FILE)
    # Only add these helper targets when not already cross-compiling

    add_custom_target(build-sdl2-macos
        COMMAND ${CMAKE_COMMAND} -S ${SDL2_SOURCE_DIR} -B ${SDL2_SOURCE_DIR}/build-macos
                -DCMAKE_BUILD_TYPE=Release
                -DCMAKE_INSTALL_PREFIX=${SDL2_PREBUILT_DIR}
                -DSDL_STATIC=ON -DSDL_SHARED=ON
        COMMAND ${CMAKE_COMMAND} --build ${SDL2_SOURCE_DIR}/build-macos --config Release
        COMMAND ${CMAKE_COMMAND} --install ${SDL2_SOURCE_DIR}/build-macos
        COMMENT "Built SDL2 for macOS → ${SDL2_PREBUILT_DIR}"
    )

    add_custom_target(build-sdl2-ios
        COMMAND ${CMAKE_COMMAND} -S ${SDL2_SOURCE_DIR} -B ${SDL2_SOURCE_DIR}/build-ios
                -DCMAKE_TOOLCHAIN_FILE=${CMAKE_SOURCE_DIR}/cmake/toolchains/ios.cmake
                -DCMAKE_BUILD_TYPE=Release
                -DCMAKE_INSTALL_PREFIX=${SDL2_PREBUILT_DIR}
                -DSDL_STATIC=ON -DSDL_SHARED=OFF
        COMMAND ${CMAKE_COMMAND} --build ${SDL2_SOURCE_DIR}/build-ios --config Release
        COMMAND ${CMAKE_COMMAND} --install ${SDL2_SOURCE_DIR}/build-ios
        COMMENT "Built SDL2 for iOS → ${SDL2_PREBUILT_DIR}"
    )

    add_custom_target(build-sdl2-android
        COMMAND ${CMAKE_COMMAND} -S ${SDL2_SOURCE_DIR} -B ${SDL2_SOURCE_DIR}/build-android
                -DCMAKE_TOOLCHAIN_FILE=${CMAKE_SOURCE_DIR}/cmake/toolchains/android.cmake
                -DCMAKE_BUILD_TYPE=Release
                -DCMAKE_INSTALL_PREFIX=${SDL2_PREBUILT_DIR}/android-temp
                -DSDL_STATIC=OFF -DSDL_SHARED=ON
        COMMAND ${CMAKE_COMMAND} --build ${SDL2_SOURCE_DIR}/build-android --config Release
        COMMAND ${CMAKE_COMMAND} --install ${SDL2_SOURCE_DIR}/build-android
        # Move libraries to ABI-specific directory
        COMMAND ${CMAKE_COMMAND} -E make_directory ${SDL2_PREBUILT_DIR}/lib/arm64-v8a
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${SDL2_PREBUILT_DIR}/android-temp/lib ${SDL2_PREBUILT_DIR}/lib/arm64-v8a
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${SDL2_PREBUILT_DIR}/android-temp/include ${SDL2_PREBUILT_DIR}/include
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${SDL2_PREBUILT_DIR}/android-temp
        COMMENT "Built SDL2 for Android arm64-v8a → ${SDL2_PREBUILT_DIR}"
    )

    add_custom_target(build-sdl2-mingw
        COMMAND ${CMAKE_COMMAND} -S ${SDL2_SOURCE_DIR} -B ${SDL2_SOURCE_DIR}/build-mingw
                -DCMAKE_TOOLCHAIN_FILE=${CMAKE_SOURCE_DIR}/cmake/toolchains/mingw.cmake
                -DCMAKE_BUILD_TYPE=Release
                -DCMAKE_INSTALL_PREFIX=${SDL2_PREBUILT_DIR}
                -DSDL_STATIC=ON -DSDL_SHARED=ON
        COMMAND ${CMAKE_COMMAND} --build ${SDL2_SOURCE_DIR}/build-mingw --config Release
        COMMAND ${CMAKE_COMMAND} --install ${SDL2_SOURCE_DIR}/build-mingw
        COMMENT "Built SDL2 for Windows (MinGW) → ${SDL2_PREBUILT_DIR}"
    )
endif()

# SDL2 Configuration based on TARGET_PLATFORM
# Strategy: 1) System libs, 2) Pre-built in ext/SDL2-install, 3) Build with build-sdl2-* targets

if(TARGET_PLATFORM STREQUAL "MacOS")
    # macOS: Try Homebrew first, then pre-built
    if(EXISTS "/opt/homebrew/include/SDL2" OR EXISTS "/usr/local/include/SDL2")
        set(SDL2_INCLUDE_DIRS /opt/homebrew/include /usr/local/include)
        set(SDL2_LIBRARIES "-L/opt/homebrew/lib -L/usr/local/lib -lSDL2")
        message(STATUS "SDL2: Using system libraries (Homebrew)")
    elseif(EXISTS "${SDL2_PREBUILT_DIR}/include/SDL2")
        set(SDL2_INCLUDE_DIRS ${SDL2_PREBUILT_DIR}/include)
        set(SDL2_LIBRARIES "-L${SDL2_PREBUILT_DIR}/lib -lSDL2")
        message(STATUS "SDL2: Using pre-built from ext/SDL2-install")
    else()
        message(FATAL_ERROR "SDL2 not found!\n  Install: brew install sdl2 sdl2_image\n  Or build: cmake --build build --target build-sdl2-macos")
    endif()
    include_directories(${SDL2_INCLUDE_DIRS})

elseif(TARGET_PLATFORM STREQUAL "iOS")
    # iOS: Use pre-built static library
    if(EXISTS "${SDL2_PREBUILT_DIR}/lib/libSDL2.a")
        set(SDL2_INCLUDE_DIRS ${SDL2_PREBUILT_DIR}/include)
        set(SDL2_LIBRARIES
            ${SDL2_PREBUILT_DIR}/lib/libSDL2main.a
            ${SDL2_PREBUILT_DIR}/lib/libSDL2.a
            iconv
        )
        message(STATUS "SDL2: Using pre-built iOS static library from ext/SDL2-install")
    else()
        message(FATAL_ERROR "SDL2 iOS library not found!\n  Build: cmake --build build --target build-sdl2-ios")
    endif()
    include_directories(${SDL2_INCLUDE_DIRS})

elseif(TARGET_PLATFORM STREQUAL "Android")
    # Android: Use pre-built NDK libraries
    if(EXISTS "${SDL2_PREBUILT_DIR}/lib/${CMAKE_ANDROID_ARCH_ABI}")
        set(SDL2_INCLUDE_DIRS ${SDL2_PREBUILT_DIR}/include)
        set(SDL2_LIBRARIES ${SDL2_PREBUILT_DIR}/lib/${CMAKE_ANDROID_ARCH_ABI}/libSDL2.so)
        message(STATUS "SDL2: Using pre-built Android libraries from ext/SDL2-install")
    else()
        message(FATAL_ERROR "SDL2 Android libraries not found!\n  Build: cmake --build build --target build-sdl2-android")
    endif()
    include_directories(${SDL2_INCLUDE_DIRS})

elseif(TARGET_PLATFORM STREQUAL "Windows")
    if(MINGW)
        # Windows cross-compilation: Use pre-built MinGW libs
        if(EXISTS "${SDL2_PREBUILT_DIR}/lib/libSDL2.a")
            set(SDL2_INCLUDE_DIRS ${SDL2_PREBUILT_DIR}/include)
            set(SDL2_LIBRARIES "-L${SDL2_PREBUILT_DIR}/lib -lSDL2")
            message(STATUS "SDL2: Using pre-built MinGW libraries from ext/SDL2-install")
        else()
            message(FATAL_ERROR "SDL2 MinGW libraries not found!\n  Build: cmake --build build --target build-sdl2-mingw")
        endif()
        include_directories(${SDL2_INCLUDE_DIRS})
    else()
        # Windows native
        set(SDL2_INCLUDE_DIRS "C:/SDL2/include")
        set(SDL2_LIBRARIES "-LC:/SDL2/lib -lSDL2main -lSDL2")
    endif()

elseif(TARGET_PLATFORM STREQUAL "Linux")
    # Linux: Try system libs first, then pre-built
    if(EXISTS "/usr/include/SDL2")
        set(SDL2_INCLUDE_DIRS "/usr/include/SDL2")
        set(SDL2_LIBRARIES "-lSDL2")
        message(STATUS "SDL2: Using system libraries")
    elseif(EXISTS "${SDL2_PREBUILT_DIR}/include/SDL2")
        set(SDL2_INCLUDE_DIRS ${SDL2_PREBUILT_DIR}/include)
        set(SDL2_LIBRARIES "-L${SDL2_PREBUILT_DIR}/lib -lSDL2")
        message(STATUS "SDL2: Using pre-built from ext/SDL2-install")
    else()
        message(FATAL_ERROR "SDL2 not found!\n  Install: sudo apt-get install libsdl2-dev libsdl2-image-dev\n  Or build: cmake --build build --target build-sdl2-macos")
    endif()
    include_directories(${SDL2_INCLUDE_DIRS})
endif()

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link SDL2 libraries
if(SDL2_LIBRARIES)
    if(SDL2_LIBRARIES MATCHES "^-L")
        # String format with -L flags
        separate_arguments(SDL2_LINK_FLAGS UNIX_COMMAND "${SDL2_LIBRARIES}")
        target_link_libraries(${PROJECT_NAME} ${SDL2_LINK_FLAGS})
    else()
        target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
    endif()
endif()

# Platform-specific linking based on TARGET_PLATFORM
if(TARGET_PLATFORM STREQUAL "MacOS")
    find_library(COCOA_LIBRARY Cocoa)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(COREVIDEO_LIBRARY CoreVideo)
    target_link_libraries(${PROJECT_NAME} ${COCOA_LIBRARY} ${IOKIT_LIBRARY} ${COREVIDEO_LIBRARY})
elseif(TARGET_PLATFORM STREQUAL "iOS")
    find_library(FOUNDATION_LIBRARY Foundation)
    find_library(UIKIT_LIBRARY UIKit)
    find_library(COREGRAPHICS_LIBRARY CoreGraphics)
    find_library(COREVIDEO_LIBRARY CoreVideo)
    find_library(COREAUDIO_LIBRARY CoreAudio)
    find_library(AUDIOTOOLBOX_LIBRARY AudioToolbox)
    find_library(AVFOUNDATION_LIBRARY AVFoundation)
    find_library(METAL_LIBRARY Metal)
    find_library(QUARTZCORE_LIBRARY QuartzCore)
    find_library(OPENGLES_LIBRARY OpenGLES)
    find_library(GAMECONTROLLER_LIBRARY GameController)
    find_library(COREMOTION_LIBRARY CoreMotion)
    find_library(COREBLUETOOTH_LIBRARY CoreBluetooth)
    find_library(COREHAPTICS_LIBRARY CoreHaptics)
    target_link_libraries(${PROJECT_NAME}
        ${FOUNDATION_LIBRARY}
        ${UIKIT_LIBRARY}
        ${COREGRAPHICS_LIBRARY}
        ${COREVIDEO_LIBRARY}
        ${COREAUDIO_LIBRARY}
        ${AUDIOTOOLBOX_LIBRARY}
        ${AVFOUNDATION_LIBRARY}
        ${METAL_LIBRARY}
        ${QUARTZCORE_LIBRARY}
        ${OPENGLES_LIBRARY}
        ${GAMECONTROLLER_LIBRARY}
        ${COREMOTION_LIBRARY}
        ${COREBLUETOOTH_LIBRARY}
        ${COREHAPTICS_LIBRARY}
    )
elseif(TARGET_PLATFORM STREQUAL "Windows" AND MINGW)
    target_link_libraries(${PROJECT_NAME} mingw32 ws2_32 winmm gdi32 imm32 ole32 oleaut32 version uuid)
elseif(TARGET_PLATFORM STREQUAL "Android")
    target_link_libraries(${PROJECT_NAME} log android EGL GLESv2)
endif()

# Copy resources to build directory
file(COPY ${CMAKE_SOURCE_DIR}/resources DESTINATION ${CMAKE_BINARY_DIR})

# Installation
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
install(DIRECTORY resources DESTINATION bin)

#
# Cross-compilation helper targets (only when not already cross-compiling)
#
if(NOT CMAKE_TOOLCHAIN_FILE)
    # Define helper targets for cross-compilation

    add_custom_target(macos
        COMMAND ${CMAKE_COMMAND} -E echo "Building for TARGET_PLATFORM=macOS on BUILD_PLATFORM=${BUILD_PLATFORM}..."
        COMMAND ${CMAKE_COMMAND} -S ${CMAKE_SOURCE_DIR} -B ${CMAKE_BINARY_DIR}/macos
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}/macos
        COMMENT "Building for macOS"
    )

    add_custom_target(ios
        COMMAND ${CMAKE_COMMAND} -E echo "Building for TARGET_PLATFORM=iOS on BUILD_PLATFORM=${BUILD_PLATFORM}..."
        COMMAND xcrun --version
        COMMAND ${CMAKE_COMMAND}
            -S ${CMAKE_SOURCE_DIR}
            -B ${CMAKE_BINARY_DIR}/ios
            -DCMAKE_TOOLCHAIN_FILE=${CMAKE_SOURCE_DIR}/cmake/toolchains/ios.cmake
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}/ios
        COMMENT "Cross-compiling for iOS"
    )

    add_custom_target(android
        COMMAND ${CMAKE_COMMAND} -E echo "Building for TARGET_PLATFORM=Android on BUILD_PLATFORM=${BUILD_PLATFORM}..."
        COMMAND ${CMAKE_COMMAND}
            -S ${CMAKE_SOURCE_DIR}
            -B ${CMAKE_BINARY_DIR}/android
            -DCMAKE_TOOLCHAIN_FILE=${CMAKE_SOURCE_DIR}/cmake/toolchains/android.cmake
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}/android
        COMMENT "Cross-compiling for Android"
    )

    add_custom_target(windows
        COMMAND ${CMAKE_COMMAND} -E echo "Building for TARGET_PLATFORM=Windows on BUILD_PLATFORM=${BUILD_PLATFORM}..."
        COMMAND bash -c "if ! command -v x86_64-w64-mingw32-gcc &> /dev/null; then echo 'ERROR: MinGW not found. Install with: brew install mingw-w64 (macOS) or apt-get install mingw-w64 (Linux)'; exit 1; fi"
        COMMAND ${CMAKE_COMMAND}
            -S ${CMAKE_SOURCE_DIR}
            -B ${CMAKE_BINARY_DIR}/windows
            -DCMAKE_TOOLCHAIN_FILE=${CMAKE_SOURCE_DIR}/cmake/toolchains/mingw.cmake
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}/windows
        COMMENT "Cross-compiling for Windows"
    )

    # Help target
    add_custom_target(help-platforms
        COMMAND ${CMAKE_COMMAND} -E echo ""
        COMMAND ${CMAKE_COMMAND} -E echo "==================================="
        COMMAND ${CMAKE_COMMAND} -E echo "Multi-Platform Build Targets"
        COMMAND ${CMAKE_COMMAND} -E echo "==================================="
        COMMAND ${CMAKE_COMMAND} -E echo "BUILD_PLATFORM (host): ${BUILD_PLATFORM}"
        COMMAND ${CMAKE_COMMAND} -E echo ""
        COMMAND ${CMAKE_COMMAND} -E echo "Available TARGET_PLATFORM builds:"
        COMMAND ${CMAKE_COMMAND} -E echo "  make macos    - Build for macOS"
        COMMAND ${CMAKE_COMMAND} -E echo "  make ios      - Build for iOS (requires Xcode)"
        COMMAND ${CMAKE_COMMAND} -E echo "  make android  - Build for Android (requires NDK)"
        COMMAND ${CMAKE_COMMAND} -E echo "  make windows  - Build for Windows (requires MinGW)"
        COMMAND ${CMAKE_COMMAND} -E echo ""
        COMMENT "Display available platform targets"
    )
endif()

# Print configuration summary
message(STATUS "")
message(STATUS "=================================")
message(STATUS "Configuration Summary")
message(STATUS "=================================")
message(STATUS "  Project: ${PROJECT_NAME} v${PROJECT_VERSION}")
message(STATUS "  Build Tool: CMake")
message(STATUS "  BUILD_PLATFORM (host): ${BUILD_PLATFORM}")
message(STATUS "  TARGET_PLATFORM (target): ${TARGET_PLATFORM}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Toolchain File: ${CMAKE_TOOLCHAIN_FILE}")
message(STATUS "=================================")
message(STATUS "")
if(NOT CMAKE_TOOLCHAIN_FILE)
    message(STATUS "Available targets: macos, ios, android, windows")
    message(STATUS "Run: cmake --build build --target <platform>")
    message(STATUS "")
endif()
