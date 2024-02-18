#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

char* startKeylogger();

int main() {
    char* recordedKeys = startKeylogger();
    if (recordedKeys != NULL) {
        printf("\nRecorded keystrokes: %s\n", recordedKeys);
        free(recordedKeys);
    }

    return 0;
}


