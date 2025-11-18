package com.multiplatform.gui;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.widget.Toast;

/**
 * MainActivity - Minimal Java wrapper for native C++ SDL2 application
 *
 * This activity serves as the entry point for the Android app and loads
 * the native library containing our C++ SDL2 code.
 */
public class MainActivity extends Activity {

    // Load native library
    static {
        try {
            System.loadLibrary("MultiPlatformGUI");
        } catch (UnsatisfiedLinkError e) {
            System.err.println("Native library failed to load: " + e);
        }
    }

    // Native method declarations (implemented in C++)
    private native void nativeInit();
    private native void nativeRun();
    private native void nativeCleanup();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        // Hide system UI for fullscreen experience
        getWindow().getDecorView().setSystemUiVisibility(
            View.SYSTEM_UI_FLAG_FULLSCREEN |
            View.SYSTEM_UI_FLAG_HIDE_NAVIGATION |
            View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
        );

        Toast.makeText(this, "MultiPlatform GUI - Built with CMake/Gradle/Ant",
                      Toast.LENGTH_SHORT).show();

        // Initialize native code
        // Note: Full SDL2 Android integration would use SDLActivity
        // This is a simplified version for demonstration
        nativeInit();
    }

    @Override
    protected void onResume() {
        super.onResume();
        // Resume native application
    }

    @Override
    protected void onPause() {
        super.onPause();
        // Pause native application
    }

    @Override
    protected void onDestroy() {
        nativeCleanup();
        super.onDestroy();
    }
}
