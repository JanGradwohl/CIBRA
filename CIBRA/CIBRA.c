#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>  // Für isupper und andere Charakterfunktionen
#include <stdint.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/rand.h>


#ifdef _WIN32

#include <windows.h>

#endif


#if defined(__linux__) && !defined(__ANDROID__)
//clang -Wall -Wextra -g CIBRA.c -o CIBRA -lX11 -lXtst
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XTest.h>
#include <unistd.h> // Für sleep

#endif

#define AES_BLOCK_SIZE 16
#define AES_KEY_SIZE 16
#define AES_ROUNDS 10

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

typedef struct KeyLogger {
    char* buffer;
    size_t size;
    size_t capacity;
} KeyLogger;

void handleErrors(void) {
    ERR_print_errors_fp(stderr);
    abort();
}

void overwriteFile(const char* filePath, const unsigned char* patterns, size_t patternLength, int passes);

void erase(const char* filePath, EraseAlgorithm algorithm) {
    switch (algorithm) {
    case DOD_5220_22_M: {
        const unsigned char patterns[3] = { 0x00, 0xFF, 0x00 };
        overwriteFile(filePath, patterns, sizeof(patterns), 3);
        break;
    }
    case AFSSI_5020: {
        const unsigned char pattern = 0x00;
        overwriteFile(filePath, &pattern, sizeof(pattern), 1);
        break;
    }
    case GOST_R_50739_95: {
        const unsigned char pattern = 0x00;
        overwriteFile(filePath, &pattern, sizeof(pattern), 2);
        break;
    }
    case BRITISH_HMG_IS5: {
        const unsigned char patterns[3] = { 0x00, 0xFF, 0x00 };
        overwriteFile(filePath, patterns, sizeof(patterns), 3);
        break;
    }
    case CANADIAN_RMCP_TSSIT_OPS_II: {
        const unsigned char patterns[7] = { 0x01, 0xFE, 0x01, 0xFE, 0x01, 0xFE, 0x00 };
        overwriteFile(filePath, patterns, sizeof(patterns), 7);
        break;
    }
    case PETER_GUTMANN: {
        //Not finished
        for (int i = 0; i < 35; i++) {
            const unsigned char pattern = rand() % 0xFF;
            overwriteFile(filePath, &pattern, sizeof(pattern), 1);
        }
        break;
    }
    case US_ARMY_AR380_19: {
        const unsigned char patterns[3] = { 0x00, 0xFF, 0x00 };
        overwriteFile(filePath, patterns, sizeof(patterns), 3);
        break;
    }
    default:
        fprintf(stderr, "Unsupported algorithm.\n");
    }

    remove(filePath);
}


int file_encrypt_decrypt(FILE* ifp, FILE* ofp, const unsigned char* key, const unsigned char* iv, EncryptionAlgorithm algo, int do_encrypt) {
    EVP_CIPHER_CTX* ctx;
    const EVP_CIPHER* cipher;
    unsigned char inbuf[1024], outbuf[1024 + EVP_MAX_BLOCK_LENGTH];
    int inlen, outlen;
    int totallen = 0;

    if (!(ctx = EVP_CIPHER_CTX_new())) {
        fprintf(stderr, "EVP_CIPHER_CTX_new failed\n");
        return -1;
    }

    switch (algo) {
    case ALGO_AES_256_CBC:
        cipher = EVP_aes_256_cbc();
        break;
    case ALGO_DES_EDE3_CBC:
        cipher = EVP_des_ede3_cbc();
        break;
    case ALGO_BLOWFISH_CBC:
        cipher = EVP_bf_cbc();
        break;
    case ALGO_CAMELLIA_256_CBC:
        cipher = EVP_camellia_256_cbc();
        break;
    case ALGO_AES_128_CBC:
        cipher = EVP_aes_128_cbc();
        break;
    case ALGO_SEED_CBC:
        cipher = EVP_seed_cbc();
        break;
    case ALGO_AES_192_CBC:
        cipher = EVP_aes_192_cbc();
        break;
    case ALGO_CAST5_CBC:
        cipher = EVP_cast5_cbc();
        break;
    case ALGO_RC2_CBC:
        cipher = EVP_rc2_cbc();
        break;
    case ALGO_IDEA_CBC:
        cipher = EVP_idea_cbc();
        break;
    case ALGO_RC4:
        cipher = EVP_rc4();
        break;
    case ALGO_AES_256_GCM:
        cipher = EVP_aes_256_gcm();
        break;
    case ALGO_CHACHA20_POLY1305:
        cipher = EVP_chacha20_poly1305();
        break;
    case ALGO_SM4_CBC:
        cipher = EVP_sm4_cbc();
        break;
    case ALGO_AES_256_XTS:
        cipher = EVP_aes_256_xts();
        break;
    default:
        fprintf(stderr, "Unsupported algorithm.\n");
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }

    if (1 != EVP_CipherInit_ex(ctx, cipher, NULL, key, iv, do_encrypt)) handleErrors();

    while ((inlen = fread(inbuf, 1, sizeof(inbuf), ifp)) > 0) {
        if (1 != EVP_CipherUpdate(ctx, outbuf, &outlen, inbuf, inlen)) handleErrors();
        fwrite(outbuf, 1, outlen, ofp);
        totallen += outlen;
    }

    if (1 != EVP_CipherFinal_ex(ctx, outbuf, &outlen)) handleErrors();
    fwrite(outbuf, 1, outlen, ofp);
    totallen += outlen;

    EVP_CIPHER_CTX_free(ctx);
    return totallen;
}



#ifdef _WIN32
void sendKeyEvent(WORD vkCode, BOOL isExtended, BOOL isKeyUp) {
    INPUT input = { 0 };

    input.type = INPUT_KEYBOARD;
    input.ki.wVk = vkCode;
    input.ki.dwFlags = (isExtended ? KEYEVENTF_EXTENDEDKEY : 0) | (isKeyUp ? KEYEVENTF_KEYUP : 0);

    SendInput(1, &input, sizeof(INPUT));
}

void sendMouseClick(DWORD flags) {
    INPUT input[2] = { 0 };

    input[0].type = INPUT_MOUSE;
    input[0].mi.dwFlags = flags;

    input[1].type = INPUT_MOUSE;
    input[1].mi.dwFlags = flags | MOUSEEVENTF_LEFTUP;

    SendInput(2, input, sizeof(INPUT));
}

void sendSpecialKey(const char* key) {
    if (strcmp(key, "LEFTCLICK") == 0) {
        sendMouseClick(MOUSEEVENTF_LEFTDOWN);
    }
    else if (strcmp(key, "RIGHTCLICK") == 0) 
    {
        sendMouseClick(MOUSEEVENTF_RIGHTDOWN);
    }
    else if (strcmp(key, "CTRL") == 0) {
        sendKeyEvent(VK_CONTROL, FALSE, FALSE);
        sendKeyEvent(VK_CONTROL, FALSE, TRUE);
    }
    else if (strcmp(key, "ALT") == 0) {
        sendKeyEvent(VK_MENU, FALSE, FALSE);
        sendKeyEvent(VK_MENU, FALSE, TRUE);
    }
    else if (strcmp(key, "WIN") == 0) {
        sendKeyEvent(VK_LWIN, FALSE, FALSE);
        sendKeyEvent(VK_LWIN, FALSE, TRUE);
    }
    else if (strcmp(key, "UP") == 0) {
        sendKeyEvent(VK_UP, FALSE, FALSE);
        sendKeyEvent(VK_UP, FALSE, TRUE);
    }
    else if (strcmp(key, "DOWN") == 0) {
        sendKeyEvent(VK_DOWN, FALSE, FALSE);
        sendKeyEvent(VK_DOWN, FALSE, TRUE);
    }
    else if (strcmp(key, "LEFT") == 0) {
        sendKeyEvent(VK_LEFT, FALSE, FALSE);
        sendKeyEvent(VK_LEFT, FALSE, TRUE);
    }
    else if (strcmp(key, "RIGHT") == 0) {
        sendKeyEvent(VK_RIGHT, FALSE, FALSE);
        sendKeyEvent(VK_RIGHT, FALSE, TRUE);
    }
    else if (strncmp(key, "F", 1) == 0 && strlen(key) <= 3) {
        int fnum = atoi(key + 1);
        if (fnum >= 1 && fnum <= 12) {
            sendKeyEvent(VK_F1 + (fnum - 1), FALSE, FALSE);
            sendKeyEvent(VK_F1 + (fnum - 1), FALSE, TRUE);
        }
    }
    else if (strcmp(key, "BACKSPACE") == 0) {
        sendKeyEvent(VK_BACK, FALSE, FALSE);
        sendKeyEvent(VK_BACK, FALSE, TRUE);
    }
    
}


void SendKeys(const char* keys) {
    while (*keys) {
        if (*keys == '{') {
            char specialKey[32] = { 0 };
            keys++;
            char* dest = specialKey;
            while (*keys && *keys != '}') {
                *dest++ = *keys++;
            }
            if (*keys == '}') keys++;
            sendSpecialKey(specialKey);
        }
        else {
            SHORT vk = VkKeyScan(*keys);
            WORD vkCode = LOBYTE(vk);
            BOOL isExtended = HIBYTE(vk) & 1;
            sendKeyEvent(vkCode, isExtended, FALSE);
            sendKeyEvent(vkCode, isExtended, TRUE);
            keys++;
        }
    }
}

//MOUSE POSITION
void SetMousePosition(int x, int y) 
{
    SetCursorPos(x, y);
}


char* os(const char* command) {
    static char* result = NULL;
    static size_t resultSize = 0;

    size_t bytesUsed = 0;
    FILE* pipe = _popen(command, "r");
    if (!pipe) return "ERROR";

    if (result == NULL) {
        result = malloc(1024);
        if (!result) {
            perror("Failed to allocate memory");
            return NULL;
        }
        resultSize = 1024;
    }
    result[0] = '\0';

    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        size_t bufferLen = strlen(buffer);
        if (bytesUsed + bufferLen + 1 > resultSize) {
            size_t newSize = resultSize + 1024;
            char* newResult = realloc(result, newSize);
            if (!newResult) {
                perror("Failed to reallocate memory");
                _pclose(pipe);
                return NULL;
            }
            result = newResult;
            resultSize = newSize;
        }
        strcat_s(result, resultSize, buffer);
        bytesUsed += bufferLen;
    }

    _pclose(pipe);
    return result;
}


char* startKeylogger() {
    size_t size = 1024;  // Anfangsgröße des Arrays
    char* recordedKeys = malloc(size);
    if (recordedKeys == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    size_t index = 0;
    while (1) {
        Sleep(5);  // Kleine Verzögerung, um die CPU-Last zu verringern

        for (int key = 32; key <= 127; key++) {  // ASCII-Zeichenbereich
            if (GetAsyncKeyState(key) & 0x0001) {  // Überprüft, ob die Taste gedrückt wurde
                if (index >= size - 1) {  // Überprüft, ob das Ende des Arrays erreicht ist
                    size *= 2;  // Verdoppelt die Größe des Arrays
                    char* temp = realloc(recordedKeys, size);
                    if (temp == NULL) {
                        fprintf(stderr, "Memory reallocation failed\n");
                        free(recordedKeys);
                        return NULL;
                    }
                    recordedKeys = temp;
                }

                recordedKeys[index++] = (char)key;
                recordedKeys[index] = '\0';  // Stellt sicher, dass die Zeichenfolge immer terminiert ist
                printf("%c", key);  // Druckt den Tastenanschlag zur Überprüfung
            }
        }
    }
    // return recordedKeys;  // Diese Zeile wird nie erreicht, da die Schleife endlos ist.
}


void overwriteFile(const char* filePath, const unsigned char* patterns, size_t patternLength, int passes) {
    FILE* file;
    errno_t err = fopen_s(&file, filePath, "r+b");

    if (err != 0 || file == NULL) {
        perror("Fehler beim Öffnen der Datei");
        return;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    unsigned char* buffer = (unsigned char*)malloc(patternLength * sizeof(unsigned char));
    if (buffer == NULL) {
        perror("Speicherzuweisung fehlgeschlagen");
        fclose(file);
        return;
    }

    for (int pass = 0; pass < passes; ++pass) {
        for (long i = 0; i < fileSize; i += patternLength) {
            memcpy(buffer, patterns + (pass % passes) * patternLength, patternLength);
            fwrite(buffer, 1, patternLength, file);
            fflush(file);
        }
        fseek(file, 0, SEEK_SET);
    }

    free(buffer);
    fclose(file);
}

void derive_key_iv(const char* password, unsigned char* key, unsigned char* iv, EncryptionAlgorithm algo) {
    const unsigned char salt[] = "12345678";
    const int iterations = 10000;

    if (!PKCS5_PBKDF2_HMAC(password, -1, salt, sizeof(salt), iterations, EVP_sha256(), EVP_MAX_KEY_LENGTH, key)) {
        fprintf(stderr, "Fehler bei der Schlüsselableitung.\n");
        exit(1);
    }

    
    memcpy(iv, key + EVP_MAX_KEY_LENGTH - EVP_MAX_IV_LENGTH, EVP_MAX_IV_LENGTH);
}


void file_encrypt(const char* infile, const char* password, EncryptionAlgorithm algo) {
    char outfile[260];
    sprintf_s(outfile, sizeof(outfile), "%s.CRYPT", infile);

    FILE* ifp, * ofp;
    fopen_s(&ifp, infile, "rb");
    fopen_s(&ofp, outfile, "wb");

    unsigned char key[EVP_MAX_KEY_LENGTH], iv[EVP_MAX_IV_LENGTH];
    derive_key_iv(password, key, iv, algo);

    if (file_encrypt_decrypt(ifp, ofp, key, iv, algo, 1) < 0) {
        fprintf(stderr, "Encryption failed.\n");
    }

    fclose(ifp);
    fclose(ofp);
}

void file_decrypt(const char* infileEncrypted, const char* password, EncryptionAlgorithm algo) {
    size_t len = strlen(infileEncrypted);
    if (len <= 6 || strcmp(infileEncrypted + len - 6, ".CRYPT") != 0) {
        fprintf(stderr, "Input file does not have the expected '.CRYPT' extension.\n");
        return;
    }

    char outfile[260];
    strncpy_s(outfile, sizeof(outfile), infileEncrypted, len - 6);

    FILE* ifp, * ofp;
    fopen_s(&ifp, infileEncrypted, "rb");
    fopen_s(&ofp, outfile, "wb");

    unsigned char key[EVP_MAX_KEY_LENGTH], iv[EVP_MAX_IV_LENGTH];
    derive_key_iv(password, key, iv, algo);

    if (file_encrypt_decrypt(ifp, ofp, key, iv, algo, 0) < 0) {
        fprintf(stderr, "Decryption failed.\n");
    }

    fclose(ifp);
    fclose(ofp);
}


#endif WIN



#ifdef __linux__

void sendKey(Display* display, KeySym keysym, Bool isShift) {
    KeyCode keycode = XKeysymToKeycode(display, keysym);

    if (isShift) {
        XTestFakeKeyEvent(display, XKeysymToKeycode(display, XK_Shift_L), True, 0);
    }

    XTestFakeKeyEvent(display, keycode, True, 0);
    XTestFakeKeyEvent(display, keycode, False, 0);

    if (isShift) {
        XTestFakeKeyEvent(display, XKeysymToKeycode(display, XK_Shift_L), False, 0);
    }

    XFlush(display);
}

void handleSpecialKey(Display* display, const char* key) {
    KeySym keysym = NoSymbol;

    if (strcmp(key, "CTRL") == 0) keysym = XK_Control_L;
    else if (strcmp(key, "ALT") == 0) keysym = XK_Alt_L;
    else if (strcmp(key, "ALTGR") == 0) keysym = XK_ISO_Level3_Shift;
    else if (strcmp(key, "WIN") == 0) keysym = XK_Super_L;
    else if (strcmp(key, "UP") == 0) keysym = XK_Up;
    else if (strcmp(key, "DOWN") == 0) keysym = XK_Down;
    else if (strcmp(key, "LEFT") == 0) keysym = XK_Left;
    else if (strcmp(key, "RIGHT") == 0) keysym = XK_Right;
    else if (strncmp(key, "F", 1) == 0 && strlen(key) <= 3) { // F1 bis F12
        int fnum = atoi(key + 1);
        if (fnum >= 1 && fnum <= 12) {
            keysym = XK_F1 + (fnum - 1);
        }
    }

    if (keysym != NoSymbol) {
        sendKey(display, keysym, False);
    }
}

KeySym charToKeySym(char c) {

    if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z')) {
        return c;
    }


    if (c >= 'a' && c <= 'z') {
        return c - 'a' + 'A';
    }


    if (c == ' ') {
        return XK_space;
    }

  
    switch (c) {
    case '!': return XK_exclam;
    case '@': return XK_at;
    case '#': return XK_numbersign;
    case '$': return XK_dollar;
    case '%': return XK_percent;
    case '^': return XK_asciicircum;
    case '&': return XK_ampersand;
    case '*': return XK_asterisk;
    case '(': return XK_parenleft;
    case ')': return XK_parenright;
    case '-': return XK_minus;
    case '_': return XK_underscore;
    case '=': return XK_equal;
    case '+': return XK_plus;
    case '[': return XK_bracketleft;
    case ']': return XK_bracketright;
    case '{': return XK_braceleft;
    case '}': return XK_braceright;
    case '\\': return XK_backslash;
    case '|': return XK_bar;
    case ';': return XK_semicolon;
    case ':': return XK_colon;
    case '\'': return XK_apostrophe;
    case '\"': return XK_quotedbl;
    case ',': return XK_comma;
    case '.': return XK_period;
    case '<': return XK_less;
    case '>': return XK_greater;
    case '/': return XK_slash;
    case '?': return XK_question;
    case '`': return XK_grave;
    case '~': return XK_asciitilde;
   
    default:
        fprintf(stderr, "Unsupported character: %c\n", c);
        return NoSymbol;
    }
}

void SendKeys(const char* str) {
    Display* display = XOpenDisplay(NULL);
    if (!display) {
        fprintf(stderr, "Cannot open display\n");
        exit(1);
    }

    while (*str != '\0') {
        if (*str == '{') {
            char buffer[256] = { 0 };
            str++;
            char* bufPtr = buffer;
            while (*str != '}' && *str != '\0') {
                *bufPtr++ = *str++;
            }
            *bufPtr = '\0';
            if (*str == '}') {
                str++;
            }
            handleSpecialKey(display, buffer);
        }
        else {
            KeySym keysym = charToKeySym(*str);
            if (keysym != NoSymbol) {
                Bool isShift = (isupper(*str) || strchr("!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~", *str) != NULL);
                sendKey(display, keysym, isShift);
            }
            else {
                fprintf(stderr, "Unsupported character: %c\n", *str);
            }
            str++;
        }
    }

    XCloseDisplay(display);
}

void setMousePosition(int x, int y) {
    Display* display = XOpenDisplay(NULL);
    if (display == NULL) {
        fprintf(stderr, "Cannot open display\n");
        exit(1);
    }

    Window root = DefaultRootWindow(display);
    XWarpPointer(display, None, root, 0, 0, 0, 0, x, y);
    XFlush(display);

    XCloseDisplay(display);
}


char* os(const char* command) {
    static char* result = NULL;
    static size_t resultSize = 0;

    size_t bytesUsed = 0;
    FILE* pipe = popen(command, "r");
    if (!pipe) return "ERROR";

    if (result == NULL) {
        result = malloc(1024);
        if (!result) {
            perror("Failed to allocate memory");
            return NULL;
        }
        resultSize = 1024;
    }
    result[0] = '\0';

    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        size_t bufferLen = strlen(buffer);
        if (bytesUsed + bufferLen + 1 > resultSize) {
            size_t newSize = resultSize + 1024;
            char* newResult = realloc(result, newSize);
            if (!newResult) {
                perror("Failed to reallocate memory");
                pclose(pipe);
                return NULL;
            }
            result = newResult;
            resultSize = newSize;
        }
        strcat_s(result, resultSize, buffer);
        bytesUsed += bufferLen;
    }

    pclose(pipe);
    return result;
}


void overwriteFile(const char* filePath, const unsigned char* patterns, size_t patternLength, int passes, int patternCount) {
    FILE* file = fopen(filePath, "r+b");
    if (file == NULL) {
        perror("Datei konnte nicht geöffnet werden");
        return;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);

    for (int pass = 0; pass < passes; ++pass) {
        for (long i = 0; i < fileSize; ++i) {
            fwrite(&patterns[pass % patternCount], 1, patternLength, file);
            rewind(file);
        }
        fflush(file);
    }

    fclose(file);
    
    remove(filePath);
}




#endif LINUX


#ifdef __APPLE__

void SendKeys(const char* sequence) {
    while (*sequence != '\0') {
        char command[4096] = "osascript -e 'tell application \"System Events\" to ";

        if (*sequence == ' ') {
            strcat(command, "keystroke space'");
        }
        else if (*sequence == '{') {
            char specialKey[32] = { 0 };
            int offset = 1;
            while (sequence[offset] != '}' && sequence[offset] != '\0' && offset < 31) {
                specialKey[offset - 1] = sequence[offset];
                offset++;
            }

            if (strcmp(specialKey, "SPACE") == 0) {
                strcat(command, "keystroke space'");
            }
            else if (strcmp(specialKey, "ENTER") == 0) {
                strcat(command, "keystroke return'");
            }
            else if (strcmp(specialKey, "TAB") == 0) {
                strcat(command, "keystroke tab'");
            }
            else if (strcmp(specialKey, "BACKSPACE") == 0) {
                strcat(command, "key code 51'");
            }
            else if (strcmp(specialKey, "ESC") == 0) {
                strcat(command, "key code 53'");
            }
            else if (strcmp(specialKey, "UP") == 0) {
                strcat(command, "key code 126'");
            }
            else if (strcmp(specialKey, "DOWN") == 0) {
                strcat(command, "key code 125'");
            }
            else if (strcmp(specialKey, "LEFT") == 0) {
                strcat(command, "key code 123'");
            }
            else if (strcmp(specialKey, "RIGHT") == 0) {
                strcat(command, "key code 124'");
            }
            else if (strcmp(specialKey, "DELETE") == 0) {
                strcat(command, "key code 117'");
            }
            else if (strncmp(specialKey, "F", 1) == 0 && strlen(specialKey) <= 3) {
                int fnum = atoi(specialKey + 1);
                if (fnum >= 1 && fnum <= 12) {
                    char fCommand[64];
                    sprintf(fCommand, "key code %d'", 122 + fnum - 1);
                    strcat(command, fCommand);
                }
            }
            sequence += offset + 1; // Move past the '}'
        }
        else {
            char keyStroke[64];
            snprintf(keyStroke, sizeof(keyStroke), "keystroke \"%c\"'", *sequence);
            strcat(command, keyStroke);
            sequence++;
        }

        strcat(command, " -e 'key up {shift, control, option, command}'");
        system(command);
    }
}

void setMousePosition(int x, int y) {
    char command[100];
    sprintf(command, "osascript -e 'tell application \"System Events\" to set position of mouse to {%d, %d}'", x, y);
    system(command);
}

#endif APPLE


#if defined(__APPLE__) || defined(__linux__)
void overwriteFile(const char* filePath, const unsigned char* patterns, size_t patternLength, int passes) {
    FILE* file = fopen(filePath, "r+b");

    if (file == NULL) {
        perror("Fehler beim Öffnen der Datei");
        return;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    unsigned char* buffer = (unsigned char*)malloc(patternLength * sizeof(unsigned char));
    if (buffer == NULL) {
        perror("Speicherzuweisung fehlgeschlagen");
        fclose(file);
        return;
    }

    for (int pass = 0; pass < passes; ++pass) {
        for (long i = 0; i < fileSize; i += patternLength) {
            memcpy(buffer, patterns + (pass % passes) * patternLength, patternLength);
            fwrite(buffer, 1, patternLength, file);
            fflush(file);
        }
        fseek(file, 0, SEEK_SET);
    }

    free(buffer);
    fclose(file);
}



void handleErrors(void) {
    ERR_print_errors_fp(stderr);
    abort();
}

void derive_key_iv(const char* password, unsigned char* key, unsigned char* iv, EncryptionAlgorithm algo) {
    const unsigned char salt[] = "12345678";
    const int iterations = 10000;

    if (!PKCS5_PBKDF2_HMAC(password, -1, salt, sizeof(salt), iterations, EVP_sha256(), EVP_MAX_KEY_LENGTH, key)) {
        fprintf(stderr, "Fehler bei der Schlüsselableitung.\n");
        exit(1);
    }

    
    memcpy(iv, key + EVP_MAX_KEY_LENGTH - EVP_MAX_IV_LENGTH, EVP_MAX_IV_LENGTH);
}


void file_encrypt(const char* infile, const char* password, EncryptionAlgorithm algo) {
    char outfile[260];
    snprintf(outfile, sizeof(outfile), "%s.CRYPT", infile);

    FILE* ifp = fopen(infile, "rb");
    FILE* ofp = fopen(outfile, "wb");

    if (!ifp || !ofp) {
        perror("Fehler beim Öffnen der Datei");
        if (ifp) fclose(ifp);
        if (ofp) fclose(ofp);
        return;
    }

    unsigned char key[EVP_MAX_KEY_LENGTH], iv[EVP_MAX_IV_LENGTH];
    derive_key_iv(password, key, iv, algo);

    if (file_encrypt_decrypt(ifp, ofp, key, iv, algo, 1) < 0) {
        fprintf(stderr, "Encryption failed.\n");
    }

    fclose(ifp);
    fclose(ofp);
}

void file_decrypt(const char* infileEncrypted, const char* password, EncryptionAlgorithm algo) {
    size_t len = strlen(infileEncrypted);
    char outfile[260];
    if (len <= 6 || strcmp(infileEncrypted + len - 6, ".CRYPT") != 0) {
        fprintf(stderr, "Input file does not have the expected '.CRYPT' extension.\n");
        return;
    }
    snprintf(outfile, sizeof(outfile), "%.*s", (int)len - 6, infileEncrypted);

    FILE* ifp = fopen(infileEncrypted, "rb");
    FILE* ofp = fopen(outfile, "wb");

    if (!ifp || !ofp) {
        perror("Fehler beim Öffnen der Datei");
        if (ifp) fclose(ifp);
        if (ofp) fclose(ofp);
        return;
    }

    unsigned char key[EVP_MAX_KEY_LENGTH], iv[EVP_MAX_IV_LENGTH];
    derive_key_iv(password, key, iv, algo);

    if (file_encrypt_decrypt(ifp, ofp, key, iv, algo, 0) < 0) {
        fprintf(stderr, "Decryption failed.\n");
    }

    fclose(ifp);
    fclose(ofp);
}


#endif

