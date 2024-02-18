// CIBRA.cpp : Diese Datei enthält die Funktion "main". Hier beginnt und endet die Ausführung des Programms.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>  // Für isupper und andere Charakterfunktionen

#ifdef _WIN32

#include <windows.h>
#include <conio.h>

#endif


#if defined(__linux__) && !defined(__ANDROID__)
//clang -Wall -Wextra -g CIBRA.c -o CIBRA -lX11 -lXtst
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XTest.h>
#include <unistd.h> // Für sleep

#endif


typedef struct KeyLogger {
    char* buffer;       // Dynamischer Puffer für Tastenanschläge
    size_t size;        // Anzahl der gespeicherten Tastenanschläge
    size_t capacity;    // Kapazität des Puffers
} KeyLogger;


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
    input[0].mi.dwFlags = flags;  // Drücken

    input[1].type = INPUT_MOUSE;
    input[1].mi.dwFlags = flags | MOUSEEVENTF_LEFTUP;  // Loslassen

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
    // Fügen Sie hier weitere spezielle Tasten hinzu
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
    FILE* pipe = _popen(command, "r");  // Verwende _popen auf Windows
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
                _pclose(pipe);  // Verwende _pclose auf Windows
                return NULL;
            }
            result = newResult;
            resultSize = newSize;
        }
        strcat_s(result, resultSize, buffer);
        bytesUsed += bufferLen;
    }

    _pclose(pipe);  // Verwende _pclose auf Windows
    return result;
}


void initializeLogger(KeyLogger* logger) {
    logger->capacity = 128; // Startkapazität
    logger->size = 0;
    logger->buffer = malloc(logger->capacity * sizeof(char));
    if (!logger->buffer) {
        fprintf(stderr, "Speicherzuweisung fehlgeschlagen\n");
        exit(EXIT_FAILURE);
    }
}

void logKey(KeyLogger* logger, char key) {
    if (logger->size >= logger->capacity) {
        logger->capacity *= 2;
        char* newBuffer = realloc(logger->buffer, logger->capacity * sizeof(char));
        if (!newBuffer) {
            fprintf(stderr, "Speichererweiterung fehlgeschlagen\n");
            free(logger->buffer);
            exit(EXIT_FAILURE);
        }
        logger->buffer = newBuffer;
    }
    logger->buffer[logger->size++] = key;
}

void stopLogging(KeyLogger* logger) {
    logger->buffer[logger->size] = '\0'; // Null-Terminator hinzufügen
    printf("Gespeicherte Tastenanschläge: %s\n", logger->buffer);
    free(logger->buffer); // Freigabe des Speichers
} 



#endif WIN



#ifdef __linux__

void sendKey(Display* display, KeySym keysym, Bool isShift) {
    KeyCode keycode = XKeysymToKeycode(display, keysym);

    if (isShift) {
        XTestFakeKeyEvent(display, XKeysymToKeycode(display, XK_Shift_L), True, 0);
    }

    XTestFakeKeyEvent(display, keycode, True, 0);  // Tastendruck
    XTestFakeKeyEvent(display, keycode, False, 0); // Tastenfreigabe

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
    // Direkte Unterstützung für Zahlen und Großbuchstaben
    if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z')) {
        return c;
    }

    // Konvertierung für Kleinbuchstaben zu ihren entsprechenden KeySyms
    if (c >= 'a' && c <= 'z') {
        return c - 'a' + 'A';
    }

    // Unterstützung für Leerzeichen
    if (c == ' ') {
        return XK_space;
    }

    // Erweiterte Zuordnung für Sonderzeichen
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
        // Fügen Sie hier weitere Fälle für Sonderzeichen hinzu, falls erforderlich
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
            str++;  // Überspringe das öffnende '{'
            char* bufPtr = buffer;
            while (*str != '}' && *str != '\0') {
                *bufPtr++ = *str++;
            }
            *bufPtr = '\0';  // Null-Terminator hinzufügen
            if (*str == '}') {
                str++;  // Überspringe das schließende '}'
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
    XFlush(display);  // Stellt sicher, dass der Befehl sofort ausgeführt wird

    XCloseDisplay(display);
}


char* os(const char* command) {
    static char* result = NULL;  // Statischer Pointer, um den aktuellen Speicherblock zu speichern
    static size_t resultSize = 0;  // Aktuelle Größe des Speicherblocks

    size_t bytesUsed = 0;  // Wie viel vom Speicher bereits verwendet wurde
    FILE* pipe = popen(command, "r");
    if (!pipe) return "ERROR";

    if (result == NULL) {  // Initialisiere den Speicher bei der ersten Verwendung
        result = malloc(1024);
        if (!result) {
            perror("Failed to allocate memory");
            return NULL;
        }
        resultSize = 1024;
    }
    result[0] = '\0';  // Reset des Ergebnis-Strings für diesen Aufruf

    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        size_t bufferLen = strlen(buffer);
        if (bytesUsed + bufferLen + 1 > resultSize) {  // +1 für den Nullterminator
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
            int offset = 1; // Start after '{'
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
                    sprintf(fCommand, "key code %d'", 122 + fnum - 1); // F1 starts at key code 122
                    strcat(command, fCommand);
                }
            } // Continue for other keys as needed
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
