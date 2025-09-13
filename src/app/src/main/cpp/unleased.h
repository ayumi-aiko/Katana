//
// Created by Ayumi on 11/09/2025.
//

#ifndef UNLEASED_H
#define UNLEASED_H

// includes...
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

// externs...
extern bool enableLogging;
extern char *configScriptPath;
extern const char *daemonLogs;
extern const char *daemonLockFile;
extern const char *daemonLockFileStuck;
extern const char *daemonPackageLists;
extern const char *daemonLockFileSuccess;
enum elogLevel {
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_ABORT
};

// functions:
int putConfig(const char *variableName, int variableValue);
int putFileIntoVoid(const char *needle, const char *haystack);
int getTemporaryAccess(const char *filePath, bool removeTempFile);
int executeShellCommand(const char *command, const char *args[], bool requiresOutputOnUserConsole);
bool eraseFileContent(const void *handle, bool isFD);
int isPackageInList(const char *packageName);
int addPackageToList(const char *packageName);
int removePackageFromList(const char *packageName);
char *combineStringsFormatted(const char *format, ...);
char *grepProp(const char *variableName, const char *propFile);
void zeynaLog(enum elogLevel loglevel, const char *service, const char *message, ...);
void abortInst(const char *service, const char *format, ...);

#endif //UNLEASED_H