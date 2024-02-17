# CIBRA

## SendKeys Funktion

Die `SendKeys`-Funktion simuliert Tastatureingaben und unterstützt eine breite Palette von Zeichen und speziellen Tastenbefehlen.

### Eingabeparameter

- **Zeichen**: Alle alphanumerischen Zeichen und die meisten Sonderzeichen.
- **Spezielle Tasten**: `{ENTER}`, `{TAB}`, `{BACKSPACE}`, `{CTRL}`, `{ALT}`, `{WIN}`, `{SHIFT}`.
- **Pfeiltasten**: `{UP}`, `{DOWN}`, `{LEFT}`, `{RIGHT}`.
- **Funktionstasten**: `{F1}` bis `{F12}`.

### Beispiel

```c
SendKeys("Hello{ENTER}");
SendKeys("{ALT}f{ALT}x");  // Öffnet das Dateimenü und wählt Beenden in vielen Anwendungen.
