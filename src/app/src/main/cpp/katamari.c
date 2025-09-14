#include <jni.h>
#include "unleased.h"

// defines:
bool enableLogging = true;
const char *daemonLogs = "/sdcard/Android/data/ishimi.katamari/files/mngrmlwk/logs.katamari.log";
const char *daemonPackageLists = "/data/adb/Re-Malwack/remalwack-package-lists.txt";
const char *daemonLockFileStuck = "/data/adb/Re-Malwack/.daemon0";
const char *daemonLockFileSuccess = "/data/adb/Re-Malwack/.daemon1";
const char *daemonLockFileFailure = "/data/adb/Re-Malwack/.daemon2";
const char *configScriptPath = "/data/adb/Re-Malwack/scripts/config.sh";
const char *tempFileFromPackage = "/sdcard/Android/data/ishimi.katamari/files/mngrmlwk/tempFile";
const char *currentDaemonPIDFile = "/data/adb/Re-Malwack/currentDaemonPID";
const char *daemonStarterScript = "/data/adb/Re-Malwack/daemonStarter.sh";

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
    if(enableDaemon) return (system(daemonStarterScript) == 0 && putConfig("enableDaemon", 1) == 0);
    else {
        getTemporaryAccess(currentDaemonPIDFile, true);
        getTemporaryAccess(currentDaemonPIDFile, false);
        FILE *fptr = fopen(tempFileFromPackage, "r");
        if(!fptr) {
            zeynaLog(LOG_LEVEL_INFO, "JNI-manageDaemon()", "Daemon PID file not found, assuming it's not running.");
            return (putConfig("enableDaemon", 0) == 0);
        }
        char buffer[1000];
        char *endptr;
        if(fgets(buffer, sizeof(buffer), fptr) != NULL) {
            long pid_long = strtol(buffer, &endptr, 10);
            if(endptr == buffer || *endptr != '\0' || pid_long <= 0) zeynaLog(LOG_LEVEL_WARN, "JNI-manageDaemon()", "Invalid PID found in file: %s", buffer);
            else {
                pid_t pid = (pid_t)pid_long;
                if(kill(pid, SIGTERM) == -1) {
                    // If kill returns -1, an error occurred. Use strerror(errno) to get the error message.
                    zeynaLog(LOG_LEVEL_ERROR, "JNI-manageDaemon()", "Failed to send SIGTERM to daemon with PID %d. Error: %s", pid, strerror(errno));
                }
                else zeynaLog(LOG_LEVEL_INFO, "JNI-manageDaemon()", "Successfully sent SIGTERM to daemon with PID %d.", pid);
            }
        }
        else zeynaLog(LOG_LEVEL_WARN, "JNI-manageDaemon()", "Failed to read PID from file.");
        fclose(fptr);
        getTemporaryAccess(currentDaemonPIDFile, true);
        return (putConfig("enableDaemon", 0) == 0);
    }
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

JNIEXPORT jboolean JNICALL Java_ishimi_katamari_MainActivity_doesModuleExists(JNIEnv *env, jobject thiz) {
    FILE *fptr = fopen("/data/adb/Re-Malwack/module.prop", "r");
    if(!fptr) {
        zeynaLog(LOG_LEVEL_ERROR, "doesModuleExists()", "Failed to open the module property file, please open the app again or just install Re-Malwack to proceed!");
        return JNI_FALSE;
    }
    fclose(fptr);
    return JNI_TRUE;
}