// CIBRA.cpp : Diese Datei enthält die Funktion "main". Hier beginnt und endet die Ausführung des Programms.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>  // Für isupper und andere Charakterfunktionen

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


#ifdef _WIN32
void SendKeysWin(const char* keys) {
    INPUT input[2];
    int string_length = strlen(keys);
    memset(input, 0, sizeof(input));

    for (int i = 0; i <= string_length - 1; i++)
    {
        input[0].type = INPUT_KEYBOARD;
        input[0].ki.wVk = VkKeyScan(keys[i]); // Holt den virtuellen Tastencode

        input[1] = input[0];
        input[1].ki.dwFlags = KEYEVENTF_KEYUP; // Setzt das Key-Up Flag

        SendInput(2, input, sizeof(INPUT)); // Sendet das Tastenereignis
    }
}
#endif


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



void SendKeysLinux(const char* str) {
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

#endif


void SendKeys(const char* keys)
{

#ifdef _WIN32
    SendKeysWin(keys);
#endif

#ifdef __linux__
    SendKeysLinux(keys);
#endif

}


int main() {
    Sleep(5000);
    SendKeys("Hello, World! {CTRL}c{CTRL}{ALT}Tab{F1}{UP}cc{LEFT}{RIGHT}{WIN}r");
    return 0;
}

