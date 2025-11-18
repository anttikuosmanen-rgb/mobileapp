# Multi-Platform GUI Application

A cross-platform GUI application built to compare different build systems (CMake, Gradle, Ant) across multiple target platforms (Windows, macOS, iOS, Android).

## Architecture

- **Shared C++ Core**: Common application logic in C++
- **SDL2 Framework**: Cross-platform GUI framework
- **Multiple Build Systems**: CMake, Gradle, and Ant can each build for all platforms

## Key Concepts

### BUILD_PLATFORM vs TARGET_PLATFORM

- **BUILD_PLATFORM (host)**: The platform where compilation happens (e.g., macOS, Windows, Linux)
- **TARGET_PLATFORM (target)**: The platform the executable will run on (e.g., iOS, Android, Windows)

**Examples:**
- Building on macOS for macOS: `BUILD_PLATFORM=macOS`, `TARGET_PLATFORM=macOS` (native build)
- Building on macOS for iOS: `BUILD_PLATFORM=macOS`, `TARGET_PLATFORM=iOS` (cross-compilation)
- Building on Linux for Android: `BUILD_PLATFORM=Linux`, `TARGET_PLATFORM=Android` (cross-compilation)

## Project Structure

```
mobileapp/
├── CMakeLists.txt              # CMake build configuration
├── build.gradle                # Gradle build configuration
├── build.xml                   # Ant build configuration
├── toolchain.properties        # Shared toolchain paths
├── cmake/toolchains/           # CMake cross-compilation toolchains
│   ├── ios.cmake
│   ├── android.cmake
│   └── mingw.cmake
├── src/                        # C++ source code
│   ├── main.cpp
│   ├── App.cpp
│   └── GuiManager.cpp
├── include/                    # C++ headers
│   ├── App.h
│   └── GuiManager.h
├── android/                    # Android-specific files
│   ├── AndroidManifest.xml
│   ├── build.gradle
│   └── src/main/java/...
└── resources/                  # Images and assets
```

## Prerequisites

### All Platforms
- **SDL2 and SDL2_image libraries** for your BUILD_PLATFORM

### macOS BUILD_PLATFORM
```bash
brew install cmake gradle ant
# For iOS builds:
xcode-select --install
# For Windows cross-compilation:
brew install mingw-w64
# For Android builds:
brew install openjdk@21  # Java 21 LTS required for Gradle 8.x
# Android SDK and NDK will be installed via setup scripts (see android/README.md)
```

### Linux BUILD_PLATFORM
```bash
sudo apt-get install libsdl2-dev libsdl2-image-dev cmake gradle ant
# For Windows cross-compilation:
sudo apt-get install mingw-w64
# For Android builds:
# Install Android Studio and NDK
```

### Windows BUILD_PLATFORM
- Install SDL2 and SDL2_image
- Install CMake, Gradle, Ant
- Install Android Studio and NDK for Android builds

## Build Commands

### CMake

```bash
# Native build for current platform
cmake -S . -B build
cmake --build build

# Cross-compile for specific TARGET_PLATFORM
cmake --build build --target macos      # TARGET_PLATFORM=macOS
cmake --build build --target ios        # TARGET_PLATFORM=iOS (macOS only)
cmake --build build --target android    # TARGET_PLATFORM=Android
cmake --build build --target windows    # TARGET_PLATFORM=Windows
```

### Gradle

```bash
# Build SDL2 for Android (required first time)
gradle buildSDL2Android

# Build Android APK
gradle buildAndroid

# Or build directly from android/ directory
cd android
JAVA_HOME=/usr/local/opt/openjdk@21/libexec/openjdk.jdk/Contents/Home ./gradlew assembleDebug
```

**Note:** For detailed Android build configuration and troubleshooting, see [`android/README.md`](android/README.md)

### Ant

```bash
# Native build (uses BUILD_PLATFORM as TARGET_PLATFORM)
ant build

# Cross-compile for specific TARGET_PLATFORM
ant build-macOS       # TARGET_PLATFORM=macOS
ant build-iOS         # TARGET_PLATFORM=iOS (macOS only)
ant build-Android     # TARGET_PLATFORM=Android
ant build-Windows     # TARGET_PLATFORM=Windows
ant build-Linux       # TARGET_PLATFORM=Linux
```

## Platform Support Matrix

| BUILD_PLATFORM | TARGET_PLATFORM | CMake | Gradle | Ant |
|----------------|-----------------|-------|--------|-----|
| macOS          | macOS           | ✅    | ✅     | ✅  |
| macOS          | iOS             | ✅    | ✅     | ✅  |
| macOS          | Android         | ✅    | ✅     | ✅  |
| macOS          | Windows         | ✅    | ✅     | ✅  |
| Windows        | Windows         | ✅    | ✅     | ✅  |
| Windows        | Android         | ✅    | ✅     | ✅  |
| Linux          | Linux           | ✅    | ✅     | ✅  |
| Linux          | Android         | ✅    | ✅     | ✅  |
| Linux          | Windows         | ✅    | ✅     | ✅  |

## Configuration

Edit `toolchain.properties` to customize paths for your system:
- Android NDK location
- MinGW compiler location
- SDL2 library paths per platform

## Application Features

The GUI application demonstrates:
- Background image display
- Exit button (X) - closes the application
- Info button (i) - displays "About" dialog showing:
  - App name and version
  - Build tool used (CMake/Gradle/Ant)
  - TARGET_PLATFORM information

## Output Locations

### CMake
- Native: `build/MultiPlatformGUI`
- iOS: `build/ios/MultiPlatformGUI`
- Android: `build/android/MultiPlatformGUI`
- Windows: `build/windows/MultiPlatformGUI.exe`

### Gradle
- Android APK (debug): `android/build/outputs/apk/debug/MultiPlatformGUI-Android-debug.apk`
- Android APK (release): `android/build/outputs/apk/release/MultiPlatformGUI-Android-release-unsigned.apk`
- Native library: `android/build/intermediates/cxx/Debug/*/obj/arm64-v8a/libMultiPlatformGUI.so`

### Ant
- All platforms: `build/ant/MultiPlatformGUI` (or `.exe` for Windows)

## Troubleshooting

### SDL2 Not Found
```bash
# macOS
brew install sdl2 sdl2_image

# Linux
sudo apt-get install libsdl2-dev libsdl2-image-dev

# Windows
# Download from https://www.libsdl.org/
```

### iOS Build Fails
- Ensure Xcode is installed: `xcode-select --install`
- Only works when BUILD_PLATFORM=macOS

### Android Build Fails
- Ensure Java 21 is installed: `brew install openjdk@21`
- Install Android SDK and NDK (see `android/README.md` for detailed instructions)
- Create `android/local.properties` with SDK and NDK paths
- Build SDL2 for Android first: `gradle buildSDL2Android`
- See `android/README.md` for comprehensive troubleshooting guide

### MinGW Not Found (Windows cross-compilation)
```bash
# macOS
brew install mingw-w64

# Linux
sudo apt-get install mingw-w64
```

## Comparing Build Systems

This project allows you to compare:
1. **Configuration complexity**: CMake vs Gradle vs Ant
2. **Cross-compilation support**: How each handles different TARGET_PLATFORMS
3. **Build speed**: Time to compile for each TARGET_PLATFORM
4. **Ease of use**: Command-line interface and error messages
5. **Platform support**: Which combinations work best

## License

This is a demonstration project for comparing build systems.
