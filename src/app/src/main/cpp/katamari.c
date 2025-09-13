#include <jni.h>
#include <stdlib.h>
#include "unleased.h"

// defines:
bool enableLogging = false;
const char *daemonLogs = "/sdcard/Android/data/ishimi.katamari/logs.katamari.log";
const char *daemonPackageLists = "/data/adb/Re-Malwack/remalwack-package-lists.txt";
const char *daemonLockFileStuck = "/data/adb/Re-Malwack/.daemon0";
const char *daemonLockFileSuccess = "/data/adb/Re-Malwack/.daemon1";
char *configScriptPath = "/data/adb/Re-Malwack/scripts/config.sh";

JNIEXPORT jboolean JNICALL Java_ishimi_katamari_MainActivity_testRoot(JNIEnv *env, jobject thiz) {
    return (system("su -c echo hi") == 0);
}

JNIEXPORT jboolean JNICALL Java_ishimi_katamari_MainActivity_isDaemonEnabled(JNIEnv *env, jobject thiz) {
    if(getTemporaryAccess(configScriptPath, true) != 0) return JNI_FALSE;
    const char *isDaemonEnabled = grepProp("enableDaemon", configScriptPath);
    if(!isDaemonEnabled) return JNI_FALSE;
    return (strcmp(isDaemonEnabled, "true") == 0);
}

JNIEXPORT jboolean JNICALL Java_ishimi_katamari_MainActivity_manageDaemon(JNIEnv *env, jobject thiz, jboolean enableDaemon) {
    if(enableDaemon) return (putConfig("enableDaemon", 1) == 1);
    else return (putConfig("enableDaemon", 1) == 0);
}

JNIEXPORT jboolean JNICALL Java_ishimi_katamari_MainActivity_addPackageIntoList(JNIEnv *env, jobject thiz, jstring packageToAdd) {
    const char *cPackage = (*env)->GetStringUTFChars(env, packageToAdd, 0);
    jboolean result = (addPackageToList(cPackage) == 0) ? JNI_TRUE : JNI_FALSE;
    (*env)->ReleaseStringUTFChars(env, packageToAdd, cPackage);
    return result;
}

JNIEXPORT jboolean JNICALL Java_ishimi_katamari_MainActivity_removePackageFromList(JNIEnv *env, jobject thiz, jstring packageToRemove) {
    const char *cPackage = (*env)->GetStringUTFChars(env, packageToRemove, 0);
    jboolean result = (removePackageFromList(cPackage) == 0) ? JNI_TRUE : JNI_FALSE;
    (*env)->ReleaseStringUTFChars(env, packageToRemove, cPackage);
    return result;
}