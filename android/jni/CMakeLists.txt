cmake_minimum_required(VERSION 3.18.1)
project(MultiPlatformGUI)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add SDL2 include directories
include_directories(${CMAKE_SOURCE_DIR}/../../ext/SDL2-install/include)
include_directories(${CMAKE_SOURCE_DIR}/../../include)

# Source files
set(SOURCES
    ${CMAKE_SOURCE_DIR}/../../src/main.cpp
    ${CMAKE_SOURCE_DIR}/../../src/App.cpp
    ${CMAKE_SOURCE_DIR}/../../src/GuiManager.cpp
    android_native.cpp
)

# Create shared library
add_library(MultiPlatformGUI SHARED ${SOURCES})

# Link SDL2
set(SDL2_LIB_DIR "${CMAKE_SOURCE_DIR}/../../ext/SDL2-install/lib/${ANDROID_ABI}")
add_library(SDL2 SHARED IMPORTED)
set_target_properties(SDL2 PROPERTIES IMPORTED_LOCATION "${SDL2_LIB_DIR}/libSDL2.so")
target_link_libraries(MultiPlatformGUI SDL2 log android)

# Compile definitions
target_compile_definitions(MultiPlatformGUI PRIVATE
    PLATFORM_ANDROID
    BUILD_TOOL="Gradle/Android"
)
