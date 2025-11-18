# Android Build Quick Start

Quick reference for building the Android APK.

## Prerequisites Checklist

- [ ] Java 21 LTS installed
- [ ] Android SDK installed at `~/Library/Android/sdk`
- [ ] Android NDK 25.2.9519653 installed
- [ ] CMake 3.18.1+ installed
- [ ] SDL2 built for Android (arm64-v8a)
- [ ] `local.properties` file created

## One-Time Setup

```bash
# 1. Install Java 21
brew install openjdk@21

# 2. Set Java home
export JAVA_HOME=/usr/local/opt/openjdk@21/libexec/openjdk.jdk/Contents/Home

# 3. Install Android SDK
mkdir -p ~/Library/Android/sdk && cd ~/Library/Android/sdk
curl -o cmdlinetools.zip https://dl.google.com/android/repository/commandlinetools-mac-11076708_latest.zip
unzip -q cmdlinetools.zip
mkdir -p cmdline-tools/latest
mv cmdline-tools/* cmdline-tools/latest/ 2>/dev/null || true
rm cmdlinetools.zip

# 4. Accept licenses and install SDK components
yes | ~/Library/Android/sdk/cmdline-tools/latest/bin/sdkmanager --licenses
~/Library/Android/sdk/cmdline-tools/latest/bin/sdkmanager "platform-tools" "platforms;android-33" "build-tools;33.0.1"

# 5. Create local.properties
cd /path/to/mobileapp/android
cat > local.properties << EOF
sdk.dir=$HOME/Library/Android/sdk
ndk.dir=$HOME/Library/Android/sdk/ndk/25.2.9519653
EOF

# 6. Build SDL2 for Android
cd ..
gradle buildSDL2Android
```

## Build Commands

```bash
# Set Java home (add to ~/.zshrc or ~/.bashrc for persistence)
export JAVA_HOME=/usr/local/opt/openjdk@21/libexec/openjdk.jdk/Contents/Home

# From project root
gradle buildAndroid

# OR from android/ directory
cd android
./gradlew assembleDebug
```

## Output

APK location: `android/build/outputs/apk/debug/MultiPlatformGUI-Android-debug.apk`

## Install on Device

```bash
# Connect device via USB (with USB debugging enabled)
adb install android/build/outputs/apk/debug/MultiPlatformGUI-Android-debug.apk

# View logs
adb logcat | grep -i multiplatform
```

## Common Issues

### "NDK is not installed"
```bash
# Verify NDK exists
ls ~/Library/Android/sdk/ndk/25.2.9519653

# If missing, NDK should be already installed. Check the path in local.properties
```

### "SDK location not found"
```bash
# Create local.properties
cd android
cat > local.properties << EOF
sdk.dir=$HOME/Library/Android/sdk
ndk.dir=$HOME/Library/Android/sdk/ndk/25.2.9519653
EOF
```

### "Unsupported class file major version"
```bash
# Wrong Java version - use Java 21
export JAVA_HOME=/usr/local/opt/openjdk@21/libexec/openjdk.jdk/Contents/Home
java -version  # Should show 21.x.x
```

### SDL2 linking errors
```bash
# Build SDL2 first
cd ..  # Go to project root
gradle buildSDL2Android

# Verify SDL2 was built
ls ext/SDL2-install/lib/arm64-v8a/libSDL2.so
```

### Clean build
```bash
cd android
./gradlew clean
rm -rf .gradle/ .cxx/ build/
./gradlew assembleDebug
```

## Environment Variables

For convenience, add to your `~/.zshrc` or `~/.bashrc`:

```bash
# Java 21 for Android builds
export JAVA_HOME=/usr/local/opt/openjdk@21/libexec/openjdk.jdk/Contents/Home

# Android SDK
export ANDROID_HOME=$HOME/Library/Android/sdk
export PATH=$PATH:$ANDROID_HOME/platform-tools
export PATH=$PATH:$ANDROID_HOME/cmdline-tools/latest/bin
```

## Verification Commands

```bash
# Check Java version
java -version

# Check Android SDK
ls ~/Library/Android/sdk/platforms/android-33

# Check NDK
ls ~/Library/Android/sdk/ndk/25.2.9519653

# Check CMake
cmake --version

# Check SDL2
ls ext/SDL2-install/lib/arm64-v8a/libSDL2.so

# Check Gradle wrapper
cd android && ./gradlew --version
```

## Build Variants

```bash
# Debug build (default)
./gradlew assembleDebug

# Release build (unsigned)
./gradlew assembleRelease

# Both
./gradlew assemble

# Clean build
./gradlew clean assembleDebug
```

## APK Information

```bash
# Get APK info
aapt dump badging android/build/outputs/apk/debug/MultiPlatformGUI-Android-debug.apk

# Check APK size
ls -lh android/build/outputs/apk/debug/*.apk

# List APK contents
unzip -l android/build/outputs/apk/debug/MultiPlatformGUI-Android-debug.apk

# Check native libraries in APK
unzip -l android/build/outputs/apk/debug/MultiPlatformGUI-Android-debug.apk | grep "\.so$"
```

## Detailed Documentation

For detailed configuration, troubleshooting, and architecture information, see:
- [`README.md`](README.md) - Comprehensive Android build documentation
- [`../README.md`](../README.md) - Project-wide build system comparison
