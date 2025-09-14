#include <jni.h>
#include "unleased.h"

// defines:
bool enableLogging = false;
const char *daemonLogs = "/sdcard/Android/data/ishimi.katamari/files/mngrmlwk/logs.katamari.log";
const char *daemonPackageLists = "/data/adb/Re-Malwack/remalwack-package-lists.txt";
const char *daemonLockFileStuck = "/data/adb/Re-Malwack/.daemon0";
const char *daemonLockFileSuccess = "/data/adb/Re-Malwack/.daemon1";
const char *daemonLockFileFailure = "/data/adb/Re-Malwack/.daemon2";
const char *configScriptPath = "/data/adb/Re-Malwack/scripts/config.sh";
const char *tempFileFromPackage = "/sdcard/Android/data/ishimi.katamari/files/mngrmlwk/tempFile";

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

JNIEXPORT jint JNICALL Java_ishimi_katamari_MainActivity_importPackageList(JNIEnv *env, jobject thiz) {
    // signal failure to grab via return state! IOException == 127
    if(getTemporaryAccess(daemonPackageLists, true) != 0) return 41; // 41 UNC PLUGGNB RAPPER 🔥🔥🔥🔥🔥🔥🔥
    if(getTemporaryAccess(daemonPackageLists, false) != 0) return 66;
    // anyways, we got the file now.... BOIIIII THE RETURN CODE SOOO TUFFF 🔥🔥🔥🔥🔥🔥🔥
    if(executeShellCommand("su",(const char *[]) {"su","-c","[ -f /sdcard/katana.pkg ]",NULL}, false) != 0) return 67;
    // now copy it and call it a dayy..
    // signal the dawn import status by touching a file, we are just letting the daemon know that we are in the process of import.
    eraseFileContentEvilTwin(daemonLockFileStuck);
    if(executeShellCommand("su",(const char *[]) {"su","-c","cp", "-af", tempFileFromPackage, daemonPackageLists, NULL}, false) == 0) {
        removeFile(daemonLockFileStuck);
        eraseFileContentEvilTwin(daemonLockFileSuccess);
        return 0;
    }
    removeFile(daemonLockFileStuck);
    eraseFileContentEvilTwin(daemonLockFileFailure);
    return 1;
}

JNIEXPORT jint JNICALL Java_ishimi_katamari_MainActivity_exportPackageList(JNIEnv *env, jobject thiz) {
    int status = executeShellCommand("su",(const char*[]){"su","-c","cp","-af", daemonPackageLists, "/sdcard/katana.pkg", NULL},false);
    return status;
}

JNIEXPORT jobject JNICALL Java_ishimi_katamari_MainActivity_enableLogs(JNIEnv *env, jobject thiz) {
    enableLogging = true;
}