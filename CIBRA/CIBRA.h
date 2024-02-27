#include "stdio.h"
#include "stdlib.h"


#ifndef CIBRA_H
#define CIBRA_H

typedef enum {
    DOD_5220_22_M,
    AFSSI_5020,
    GOST_R_50739_95,
    BRITISH_HMG_IS5,
    CANADIAN_RMCP_TSSIT_OPS_II,
    PETER_GUTMANN,
    US_ARMY_AR380_19,
    UNKNOWN_ALGORITHM
} EraseAlgorithm;


typedef enum {
    ALGO_AES_256_CBC,
    ALGO_DES_EDE3_CBC,
    ALGO_BLOWFISH_CBC,
    ALGO_CAMELLIA_256_CBC,
    ALGO_AES_128_CBC,
    ALGO_SEED_CBC,
    ALGO_AES_192_CBC,
    ALGO_CAST5_CBC,
    ALGO_RC2_CBC,
    ALGO_IDEA_CBC,
    ALGO_RC4,
    ALGO_AES_256_GCM,
    ALGO_CHACHA20_POLY1305,
    ALGO_SM4_CBC,
    ALGO_AES_256_XTS
} EncryptionAlgorithm;


void SendKeys(const char* keys);
void SetMousePosition(int x, int y);
char* os(const char* command);
void erase(const char* filePath, const char* algorithm);
void file_encrypt(const char* infile, const char* password, EncryptionAlgorithm algo);
void file_decrypt(const char* infileEncrypted, const char* password, EncryptionAlgorithm algo);
char* startKeylogger();

#endif
