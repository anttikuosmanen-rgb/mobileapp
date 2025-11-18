# MinGW Cross-Compilation Toolchain for Windows
# Usage: cmake -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/mingw.cmake ..
# For building Windows executables from macOS or Linux

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# Read MinGW configuration from toolchain.properties
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/toolchain.properties" TOOLCHAIN_PROPS)
set(MINGW_PREFIX "x86_64-w64-mingw32")
set(MINGW_BIN_DIR "")

foreach(line ${TOOLCHAIN_PROPS})
    if(line MATCHES "^mingw\\.prefix=(.*)$")
        set(MINGW_PREFIX "${CMAKE_MATCH_1}")
    elseif(line MATCHES "^mingw\\.bin\\.dir=(.*)$")
        set(MINGW_BIN_DIR "${CMAKE_MATCH_1}")
    endif()
endforeach()

# Locate MinGW compilers
find_program(CMAKE_C_COMPILER
    NAMES ${MINGW_PREFIX}-gcc
    PATHS ${MINGW_BIN_DIR} /opt/homebrew/bin /usr/local/bin /usr/bin
    NO_DEFAULT_PATH
)

find_program(CMAKE_CXX_COMPILER
    NAMES ${MINGW_PREFIX}-g++
    PATHS ${MINGW_BIN_DIR} /opt/homebrew/bin /usr/local/bin /usr/bin
    NO_DEFAULT_PATH
)

find_program(CMAKE_RC_COMPILER
    NAMES ${MINGW_PREFIX}-windres
    PATHS ${MINGW_BIN_DIR} /opt/homebrew/bin /usr/local/bin /usr/bin
    NO_DEFAULT_PATH
)

if(NOT CMAKE_C_COMPILER OR NOT CMAKE_CXX_COMPILER)
    message(FATAL_ERROR
        "MinGW-w64 cross-compiler not found!\n"
        "Please install MinGW-w64:\n"
        "  macOS: brew install mingw-w64\n"
        "  Ubuntu/Debian: sudo apt-get install mingw-w64\n"
        "  Fedora: sudo dnf install mingw64-gcc mingw64-gcc-c++\n"
        "Searched for: ${MINGW_PREFIX}-gcc, ${MINGW_PREFIX}-g++\n"
        "In paths: ${MINGW_BIN_DIR}, /opt/homebrew/bin, /usr/local/bin, /usr/bin"
    )
endif()

message(STATUS "MinGW C compiler: ${CMAKE_C_COMPILER}")
message(STATUS "MinGW C++ compiler: ${CMAKE_CXX_COMPILER}")

# Set compilers to work correctly
set(CMAKE_C_COMPILER_WORKS TRUE)
set(CMAKE_CXX_COMPILER_WORKS TRUE)

# Adjust the default behaviour of FIND_XXX() commands
set(CMAKE_FIND_ROOT_PATH /usr/${MINGW_PREFIX})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# Windows-specific settings
set(CMAKE_EXECUTABLE_SUFFIX ".exe")

message(STATUS "MinGW Toolchain configured for Windows (${MINGW_PREFIX})")
