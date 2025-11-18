# Android Build Configuration

This document describes how to build the MultiPlatformGUI Android APK using Gradle.

## Prerequisites

### 1. Java Development Kit (JDK)
- **Required Version:** Java 21 LTS
- **Installation (macOS):**
  ```bash
  brew install openjdk@21
  ```
- **Verification:**
  ```bash
  java -version  # Should show version 21.x.x
  ```

### 2. Android SDK
- **Required Components:**
  - Platform Tools
  - Android SDK Platform 33
  - Android SDK Build Tools 33.0.1+

- **Installation via command-line tools:**
  ```bash
  # Download Android command-line tools
  mkdir -p ~/Library/Android/sdk
  cd ~/Library/Android/sdk
  curl -o commandlinetools.zip https://dl.google.com/android/repository/commandlinetools-mac-11076708_latest.zip
  unzip -q commandlinetools.zip
  mkdir -p cmdline-tools/latest
  mv cmdline-tools/* cmdline-tools/latest/ 2>/dev/null || true
  rm commandlinetools.zip

  # Accept licenses and install SDK components
  export JAVA_HOME=/usr/local/opt/openjdk@21/libexec/openjdk.jdk/Contents/Home
  yes | ~/Library/Android/sdk/cmdline-tools/latest/bin/sdkmanager --licenses
  ~/Library/Android/sdk/cmdline-tools/latest/bin/sdkmanager "platform-tools" "platforms;android-33" "build-tools;33.0.1"
  ```

### 3. Android NDK
- **Required Version:** NDK r25c (25.2.9519653)
- **Installation:**
  The NDK should already be installed at `~/Library/Android/sdk/ndk/25.2.9519653`

- **Verification:**
  ```bash
  ls ~/Library/Android/sdk/ndk/25.2.9519653
  ```

### 4. CMake
- **Required:** CMake 3.18.1+ (system CMake is used)
- **Verification:**
  ```bash
  cmake --version  # Should show version 3.18.1 or higher
  ```

### 5. SDL2 for Android
- **Required:** SDL2 built for arm64-v8a
- **Build SDL2:**
  ```bash
  cd ../  # Go to project root
  gradle buildSDL2Android
  ```
- **Verification:**
  ```bash
  ls ext/SDL2-install/lib/arm64-v8a/libSDL2.so
  ```

## Configuration Files

### local.properties
Create `local.properties` in the `android/` directory with the following content:

```properties
sdk.dir=/Users/<username>/Library/Android/sdk
ndk.dir=/Users/<username>/Library/Android/sdk/ndk/25.2.9519653
```

Replace `<username>` with your actual username.

### Gradle Wrapper
The project uses Gradle 8.11.1 configured in `gradle/wrapper/gradle-wrapper.properties`:

```properties
distributionUrl=https\://services.gradle.org/distributions/gradle-8.11.1-bin.zip
```

## Build Configuration Overview

### Key build.gradle Settings

```gradle
android {
    namespace 'com.multiplatform.gui'  // Required for AGP 8.x
    compileSdkVersion 33

    defaultConfig {
        applicationId "com.multiplatform.gui"
        minSdkVersion 21      // Android 5.0+
        targetSdkVersion 33   // Android 13

        externalNativeBuild {
            cmake {
                arguments "-DBUILD_TOOL=Gradle",
                          "-DCMAKE_TOOLCHAIN_FILE=${android.ndkDirectory}/build/cmake/android.toolchain.cmake"
                cppFlags "-std=c++11"
                abiFilters 'arm64-v8a'  // 64-bit ARM only
            }
        }

        ndk {
            abiFilters 'arm64-v8a'
        }
    }

    externalNativeBuild {
        cmake {
            path "jni/CMakeLists.txt"
            // No version specified - uses system CMake
        }
    }
}

repositories {
    google()
    mavenCentral()
}
```

### CMake Configuration (jni/CMakeLists.txt)

Key points:
- Uses system CMake (4.1.2+)
- Links against SDL2 built for arm64-v8a
- Creates `libMultiPlatformGUI.so` shared library
- Includes JNI bindings in `android_native.cpp`

## Building the APK

### Build Commands

From the `android/` directory:

```bash
# Set Java home
export JAVA_HOME=/usr/local/opt/openjdk@21/libexec/openjdk.jdk/Contents/Home

# Build debug APK
./gradlew assembleDebug

# Clean and rebuild
./gradlew clean assembleDebug

# Build release APK (unsigned)
./gradlew assembleRelease
```

### Build Output

- **Debug APK:** `build/outputs/apk/debug/MultiPlatformGUI-Android-debug.apk`
- **Release APK:** `build/outputs/apk/release/MultiPlatformGUI-Android-release-unsigned.apk`
- **Native Libraries:** `build/intermediates/cxx/Debug/*/obj/arm64-v8a/libMultiPlatformGUI.so`

## Target Architecture

- **Supported ABI:** arm64-v8a only
- **Why arm64-v8a only:** SDL2 is currently built only for this architecture. To support additional architectures (armeabi-v7a, x86_64), build SDL2 for those ABIs first.

## Troubleshooting

### Common Issues

1. **"NDK is not installed"**
   - Ensure `local.properties` exists with correct `ndk.dir` path
   - Verify NDK exists at the specified path

2. **"SDK location not found"**
   - Create `local.properties` with `sdk.dir` pointing to Android SDK

3. **"Unsupported class file major version"**
   - Wrong Java version. Use Java 21:
     ```bash
     export JAVA_HOME=/usr/local/opt/openjdk@21/libexec/openjdk.jdk/Contents/Home
     ```

4. **"CMake was not found"**
   - Remove version specification from `externalNativeBuild` in build.gradle
   - Install CMake: `brew install cmake`

5. **SDL2 linking errors**
   - Build SDL2 for Android first: `gradle buildSDL2Android`
   - Verify SDL2 library exists: `ls ../ext/SDL2-install/lib/arm64-v8a/libSDL2.so`

6. **"Cannot resolve external dependency aapt2"**
   - Add repositories block to build.gradle:
     ```gradle
     repositories {
         google()
         mavenCentral()
     }
     ```

### Build Cache Issues

If you encounter strange build errors:

```bash
# Clean Gradle cache
./gradlew clean
rm -rf .gradle/
rm -rf .cxx/
rm -rf build/

# Clean and rebuild
./gradlew assembleDebug
```

## Version Compatibility Matrix

| Component | Version | Notes |
|-----------|---------|-------|
| Java | 21 LTS | Required for Gradle 8.11.1 |
| Gradle | 8.11.1 | Via wrapper |
| Android Gradle Plugin | 8.1.0 | Requires Gradle 8.0+ |
| Android SDK Platform | 33 | Android 13 |
| Android Build Tools | 33.0.1+ | Auto-selected by AGP |
| NDK | 25.2.9519653 (r25c) | |
| CMake | 3.18.1+ | System version used |
| Min SDK | 21 | Android 5.0+ |
| Target SDK | 33 | Android 13 |

## Installing the APK

### On Physical Device

```bash
# Enable USB debugging on your device
# Connect device via USB

# Install APK
adb install build/outputs/apk/debug/MultiPlatformGUI-Android-debug.apk

# View logs
adb logcat | grep MultiPlatformGUI
```

### On Emulator

```bash
# Start emulator (if not running)
emulator -avd <avd_name>

# Install APK
adb install build/outputs/apk/debug/MultiPlatformGUI-Android-debug.apk
```

## Directory Structure

```
android/
├── AndroidManifest.xml           # App manifest
├── build.gradle                  # Android build configuration
├── gradle/
│   └── wrapper/
│       └── gradle-wrapper.properties  # Gradle version
├── local.properties              # SDK/NDK paths (not in git)
├── jni/
│   ├── CMakeLists.txt           # Native build configuration
│   └── android_native.cpp       # JNI bindings
├── src/main/
│   ├── java/com/multiplatform/gui/
│   │   └── MainActivity.java    # Android activity
│   └── res/                     # Android resources
└── build/                       # Build output (generated)
    └── outputs/apk/debug/
        └── MultiPlatformGUI-Android-debug.apk
```

## Additional Resources

- [Android Gradle Plugin DSL Reference](https://developer.android.com/reference/tools/gradle-api)
- [CMake External Native Build](https://developer.android.com/studio/projects/configure-cmake)
- [Android NDK Documentation](https://developer.android.com/ndk/guides)
- [Gradle Build Tool](https://gradle.org/guides/)
