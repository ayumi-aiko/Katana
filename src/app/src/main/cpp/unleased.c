//
// Created by Ayumi on 11/09/2025.
//
#include "unleased.h"

int putConfig(const char *variableName, int variableValue) {
    if(getTemporaryAccess(configScriptPath, true) != 0) return -1;
    if(getTemporaryAccess(configScriptPath, false) != 0) return -1;
    FILE *fp = fopen(tempFileFromPackage, "r");
    if(!fp) {
        zeynaLog(LOG_LEVEL_ERROR, "putConfig", "Failed to open /sdcard/Android/data/ishimi.katamari/tempFile, please try again..");
        return 127;
    }
    // vars:
    char contentFromFile[10000];
    char **contentsOfConfig = malloc(400 * sizeof(char *));
    int array = 0;
    int arrayIndexOfVariable = -1;
    if(!contentsOfConfig) {
        zeynaLog(LOG_LEVEL_ERROR, "putConfig", "Failed to allocate to contentsOfConfig variable.");
        return -1;
    }
    while(fgets(contentFromFile, sizeof(contentFromFile), fp)) {
        contentFromFile[strcspn(contentFromFile, "\r\n")] = 0;
        // we will have to skip adding comments because fuck them idc
        if(strncmp(contentFromFile, "#", 1) == 0) continue;
        // we will replace the variable and it's value with this simple ahh trick.
        if(strncmp(contentFromFile, variableName, strlen(variableName)) == 0 && contentFromFile[strlen(variableName)] == '=') arrayIndexOfVariable = array;
        contentsOfConfig[array] = strdup(contentFromFile);
        if(!contentsOfConfig[array]) {
            fclose(fp);
            for(int j = 0; j < array; j++) free(contentsOfConfig[j]);
            free(contentsOfConfig);
            zeynaLog(LOG_LEVEL_ERROR, "putConfig", "Failed to allocate memory for %d in contentsOfConfig", array);
            return 127;
        }
        array++;
    }
    fclose(fp);
    bool found = false;
    FILE *fptr = fopen(tempFileFromPackage, "w");
    if(!fptr) {
        for(int i = 0; i < array; i++) free(contentsOfConfig[i]);
        zeynaLog(LOG_LEVEL_ERROR, "putConfig", "Failed to open /sdcard/Android/data/ishimi.katamari/tempFile, please try again..");
        free(contentsOfConfig);
        return 127;
    }
    for(int i = 0; i < array; i++) {
        // write the old content till we stumble upon the one we actually need!
        if(i != arrayIndexOfVariable) fprintf(fptr, "%s\n", contentsOfConfig[i]);
        // write the new content because we got into the correct index!
        else {
            fprintf(fptr, "%s=%d\n", variableName, variableValue);
            found = true;
        }
        free(contentsOfConfig[i]);
    }
    free(contentsOfConfig);
    fclose(fptr);
    // let's just hope that it deletes the file...
    getTemporaryAccess(configScriptPath, true);
    // let's copy the file to replace with the new one. FUCK GOOGLE 😭
    putFileIntoVoid(tempFileFromPackage, configScriptPath);
    // more verbose but it does the j*b. Returns 0 if set!
    return (found) ? 0 : 1;
}

int getTemporaryAccess(const char *filePath, bool removeTempFile) {
    int ret = 0;
    if(removeTempFile) {
        if(system("su -c 'rm -rf tempFileFromPackage'") != 0) return 125;
    }
    else {
        char *cmd = combineStringsFormatted("su -c 'cp -af \"%s\" tempFileFromPackage'",filePath);
        if(!cmd) return 127;
        ret = system(cmd);
        if(ret != 0) {
            free(cmd);
            return 127;
        }
        free(cmd);
    }
    return ret;
}

// effectively, let's just use shell commands to copy the dawn file.
int putFileIntoVoid(const char *needle, const char *haystack) {
    return executeShellCommand("su", (const char*[]) {"su", "-c", "cp", "-af", needle, haystack, NULL}, false);
}

int executeShellCommand(const char *command, const char *args[], bool requiresOutputOnUserConsole) {
    switch(fork()) {
        case -1:
            abortInst("executeShellCommand", "Failed to fork a child for executing command");
            return -1;
        break;
        case 0:
            // throw output to /dev/null when set to true
            if(requiresOutputOnUserConsole) {
                int devNull = open("/dev/null", O_WRONLY);
                if(devNull == -1) exit(EXIT_FAILURE);
                dup2(devNull, STDOUT_FILENO);
                dup2(devNull, STDERR_FILENO);
                close(devNull);
            }
            execvp(command, (char *const *) args);
            zeynaLog(LOG_LEVEL_ERROR, "executeShellCommands", "Failed to execute command: %s", command);
        break;
        default: {
            int status;
            wait(&status);
            return WIFEXITED(status) ? WEXITSTATUS(status) : 1;
        }
    }
    return 1;
}

int isPackageInList(const char *packageName) {
    if(getTemporaryAccess(daemonPackageLists, true) != 0) return -1;
    if(getTemporaryAccess(daemonPackageLists, false) != 0) return -1;
    FILE *packageFile = fopen(tempFileFromPackage, "r");
    if(!packageFile) {
        zeynaLog(LOG_LEVEL_ERROR, "isPackageInList", "Failed to open the package lists file, please run this command again or report this issue to the devs.");
        putFileIntoVoid(tempFileFromPackage, daemonPackageLists);
        return 127;
    }
    char contentFromFile[8000];
    while(fgets(contentFromFile, sizeof(contentFromFile), packageFile) != NULL) {
        contentFromFile[strcspn(contentFromFile, "\n")] = 0;
        if(strcmp(contentFromFile, packageName) == 0) {
            fclose(packageFile);
            putFileIntoVoid(tempFileFromPackage, daemonPackageLists);
            return 0;
        }
    }
    fclose(packageFile);
    putFileIntoVoid(tempFileFromPackage, daemonPackageLists);
    return 1;
}

int addPackageToList(const char *packageName) {
    if(getTemporaryAccess(daemonPackageLists, true) != 0) return -1;
    if(getTemporaryAccess(daemonPackageLists, false) != 0) return -1;
    FILE *packageFile = fopen(tempFileFromPackage, "a");
    if(!packageFile) {
        zeynaLog(LOG_LEVEL_ERROR, "addPackageToList", "Failed to open the package lists file, please run this command again or report this issue to the devs.");
        putFileIntoVoid(tempFileFromPackage, daemonPackageLists);
        return 127;
    }
    if(isPackageInList(packageName) == 1) {
        fprintf(packageFile, "\n%s\n", packageName);
        zeynaLog(LOG_LEVEL_WARN, "addPackageToList", "Successfully added %s into the list, the daemon will add the packages to the list for a short period of time.");
        fclose(packageFile);
        putFileIntoVoid(tempFileFromPackage, daemonPackageLists);
        if(getTemporaryAccess(daemonPackageLists, true) != 0) return -1;
        return 0;
    }
    else {
        zeynaLog(LOG_LEVEL_WARN, "addPackageToList", "%s is already present in the lists, please try again with a different application.");
        fclose(packageFile);
        putFileIntoVoid(tempFileFromPackage, daemonPackageLists);
        if(getTemporaryAccess(daemonPackageLists, true) != 0) return -1;
        return 1;
    }
}

int removePackageFromList(const char *packageName) {
    if(getTemporaryAccess(daemonPackageLists, true) != 0) return -1;
    if(getTemporaryAccess(daemonPackageLists, false) != 0) return -1;
    FILE *packageFile = fopen(tempFileFromPackage, "r");
    if(!packageFile) {
        zeynaLog(LOG_LEVEL_ERROR, "removePackageFromList", "Failed to open the package lists file, please run this command again or report this issue to the devs.");
        return 127;
    }
    // let's make an new file because we DON'T know if the temp file exists or not.
    // by using W, it creates if it's not present and removes the previous content stored on it.
    eraseFileContent("/sdcard/Android/data/ishimi.katamari/tempDaemonPackages", false);
    FILE *tempFile = fopen("/sdcard/Android/data/ishimi.katamari/tempDaemonPackages", "a");
    if(!tempFile) {
        zeynaLog(LOG_LEVEL_ERROR, "removePackageFromList", "Failed to open a temporary file, please run this command again or report this issue to the devs.");
        return 127;
    }
    char contentFromFile[8000];
    bool status = false;
    while(fgets(contentFromFile, sizeof(contentFromFile), packageFile)) {
        // setup daemon lock file. Used to temporarily pause the daemon when there's an
        // package update is happening.
        if(executeShellCommand("su", (const char*[]) {"su", "-c", "echo", "heyheyheyimonvaction", ">", daemonLockFileStuck, NULL}, false) != 0) return -1;
        contentFromFile[strcspn(contentFromFile, "\n")] = 0;
        if(strcmp(contentFromFile, packageName) == 0) {
            zeynaLog(LOG_LEVEL_DEBUG, "removePackageFromList", "Found %s on the list, removing the package from the list...");
            // skip writing to the temp file so we can write other strings aka the packages.
            // use a bool to indicate that we skipped copying it to the temp file.
            status = true;
        }
        else fprintf(tempFile, "%s\n", contentFromFile);
    }
    fclose(tempFile);
    fclose(packageFile);
    executeShellCommand("su", (const char*[]) {"su", "-c", "rm", "-rf", daemonLockFileStuck, NULL}, false);
    if(status) {
        putFileIntoVoid("/sdcard/Android/data/ishimi.katamari/tempDaemonPackages", daemonPackageLists);
        executeShellCommand("su", (const char*[]) {"su", "-c", "echo", "heyheyheyimonvaction", ">", daemonLockFileSuccess, NULL}, false);
        return 0;
    }
    else {
        zeynaLog(LOG_LEVEL_INFO, "removePackageFromList", "Seems like the package was't present in the list. Failed to remove the given package.");
        return 1;
    }
}

int isValidPackageName(const char *name) {
    size_t len = strlen(name);
    if(len < 3) return 0;
    int segmentLen = 0;
    int segmentCount = 0;
    for(size_t i = 0; i < len; i++) {
        char c = name[i];
        if(c == '.') {
            if(segmentLen == 0 || i == len - 1) return 0;
            segmentLen = 0;
            segmentCount++;
        }
        else {
            if(segmentLen == 0) {
                if(!islower(c)) return 0;
            }
            else if(!(islower(c) || isdigit(c) || c == '_')) return 0;
            segmentLen++;
        }
    }
    return (segmentCount >= 1);
}

bool eraseFileContent(const void *handle, bool isFD) {
    if(isFD) {
        int fd = *(const int *)handle;
        return ftruncate(fd, 0) == 0;
    }
    else {
        FILE *fptr = fopen((const char *)handle, "w");
        if (!fptr) return false;
        fclose(fptr);
        return true;
    }
}

bool eraseFileContentEvilTwin(const char *filePath) {
    if(!eraseFileContent(filePath, false)) {
        executeShellCommand("su", (const char*[]) {"su", "-c", "rm", "-rf", filePath, NULL}, false);
        return (executeShellCommand("su", (const char*[]) {"su", "-c", "echo", "", ">", filePath, NULL}, false) == 0);
    }
    return false;
}

bool removeFile(const char *filePath) {
    return (executeShellCommand("su", (const char*[]) {"su", "-c", "rm", "-rf", filePath, NULL}, false) == 0);
}

// returns stack! please clean it!
char *combineStringsFormatted(const char *format, ...) {
    va_list args;
    va_start(args, format);
    va_list args_copy;
    va_copy(args_copy, args);
    int len = vsnprintf(NULL, 0, format, args_copy);
    va_end(args_copy);
    if(len < 0) {
        va_end(args);
        return NULL;
    }
    char *result = malloc(len + 1);
    if(!result) {
        va_end(args);
        return NULL;
    }
    vsnprintf(result, len + 1, format, args);
    va_end(args);
    return result;
}

char *grepProp(const char *variableName, const char *propFile) {
    FILE *filePointer = fopen(propFile, "r");
    if(!filePointer) {
        zeynaLog(LOG_LEVEL_ERROR, "grepProp", "Failed to open properties file: %s", propFile);
        return NULL;
    }
    char theLine[1024];
    size_t lengthOfTheString = strlen(variableName);
    while(fgets(theLine, sizeof(theLine), filePointer)) {
        if(strncmp(theLine, variableName, lengthOfTheString) == 0 && theLine[lengthOfTheString] == '=') {
            strtok(theLine, "=");
            char *value = strtok(NULL, "\n");
            if(value) {
                char *result = strdup(value);
                fclose(filePointer);
                return result;
            }
        }
    }
    fclose(filePointer);
    return NULL;
}

void zeynaLog(enum elogLevel loglevel, const char *service, const char *message, ...) {
    if(!enableLogging) return;
    va_list args;
    va_start(args, message);
    FILE *out = fopen(daemonLogs, "a");
    if(!out) exit(EXIT_FAILURE);
    switch(loglevel) {
        case LOG_LEVEL_INFO:
            fprintf(out, "INFO: %s: ", service);
        break;
        case LOG_LEVEL_WARN:
            fprintf(out, "WARNING: %s: ", service);
        break;
        case LOG_LEVEL_DEBUG:
            fprintf(out, "DEBUG: %s: ", service);
        break;
        case LOG_LEVEL_ERROR:
            fprintf(out, "ERROR: %s: ", service);
        break;
        case LOG_LEVEL_ABORT:
            fprintf(out, "ABORT: %s: ", service);
        break;
    }
    vfprintf(out, message, args);
    fprintf(out, "\n");
    va_end(args);
    fclose(out);
}

void abortInst(const char *service, const char *format, ...) {
    va_list args;
    va_start(args, format);
    zeynaLog(LOG_LEVEL_ABORT, "%s", "%s %s", service, format, args);
    va_end(args);
    exit(EXIT_FAILURE);
}