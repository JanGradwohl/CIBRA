// CIBRA.cpp : Diese Datei enthält die Funktion "main". Hier beginnt und endet die Ausführung des Programms.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#ifdef _WIN32

#include <windows.h>

#endif


#if defined(__linux__) && !defined(__ANDROID__)

#include <X11/Xlib.h>
#include <X11/keysym.h>

#endif



bool isOSX() 
{
    #ifdef __APPLE__
        return true;
    #else
        return false;
    #endif
}

bool isWindows() 
{
    #ifdef _WIN32
        return true;
    #else
        return false;
    #endif
}

bool isLinux()
{
    #if defined(__linux__) && !defined(__ANDROID__)
        return true;
    #else
        return false;
    #endif
}



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


#ifdef __linux__



void SendKeys_LINUX(const char* keys) {
    Display* display = XOpenDisplay(NULL); // Öffnet eine Verbindung zum X-Server
    if (display == NULL) {
        fprintf(stderr, "Kann keine Verbindung zum X-Server herstellen.\n");
        exit(1); // Beendet das Programm, wenn keine Verbindung hergestellt werden kann
    }

    Window window = DefaultRootWindow(display);
    XEvent event;
    memset(&event, 0, sizeof(event));

    event.xkey.display = display;
    event.xkey.window = window;
    event.xkey.root = window;
    event.xkey.subwindow = None;
    event.xkey.time = CurrentTime;
    event.xkey.same_screen = True;

    for (int i = 0; i < strlen(keys); i++) {
        KeySym keysym = XStringToKeysym(&keys[i]); // Ermittelt den KeySym für das aktuelle Zeichen
        if (keysym == NoSymbol) {
            fprintf(stderr, "Kein KeySym für '%c'.\n", keys[i]);
            continue;
        }
        event.xkey.keycode = XKeysymToKeycode(display, keysym); // Ermittelt den KeyCode

        if (event.xkey.keycode == 0) {
            fprintf(stderr, "Kein KeyCode für '%c'.\n", keys[i]);
            continue;
        }

        // Sendet das KeyPress-Event
        event.type = KeyPress;
        XSendEvent(display, InputFocus, True, KeyPressMask, &event);

        // Sendet das KeyRelease-Event
        event.type = KeyRelease;
        XSendEvent(display, InputFocus, True, KeyReleaseMask, &event);
    }

    XFlush(display); // Stellt sicher, dass alle Events gesendet wurden
    XCloseDisplay(display); // Schließt die Verbindung zum X-Server
}
#endif 

void SendKeys(const char* keys)
{

#ifdef _WIN32
    SendKeysWin(keys);
#endif // DEBUG
        

#ifdef __linux__
    SendKeys_LINUX(keys);
#endif 

        


}



int main() {
    Sleep(5000); // Wartet 5 Sekunden, um Ihnen Zeit zu geben, ein Eingabefeld zu fokussieren
    SendKeys("Hi wie gehts dir?"); // Simuliert den Tastendruck "H"
    return 0;
}

