#include <jni.h>
#include <string>
#include "../../include/App.h"

// Global app instance
static App* g_app = nullptr;

extern "C" {

JNIEXPORT void JNICALL
Java_com_multiplatform_gui_MainActivity_nativeInit(JNIEnv* env, jobject thiz) {
    if (!g_app) {
        g_app = new App("Gradle/Android");
        g_app->initialize();
    }
}

JNIEXPORT void JNICALL
Java_com_multiplatform_gui_MainActivity_nativeRun(JNIEnv* env, jobject thiz) {
    if (g_app) {
        g_app->run();
    }
}

JNIEXPORT void JNICALL
Java_com_multiplatform_gui_MainActivity_nativeCleanup(JNIEnv* env, jobject thiz) {
    if (g_app) {
        delete g_app;
        g_app = nullptr;
    }
}

} // extern "C"
