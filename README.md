## SendKeys Funktion

Die `SendKeys`-Funktion simuliert Tastatureingaben und Mausinteraktionen. Sie unterstützt eine Vielzahl von Zeichen, speziellen Tastenbefehlen und Mausaktionen.

### Unterstützte Eingabeparameter

#### Tastatureingaben

- **Zeichen**: Alle alphanumerischen Zeichen (`a-z`, `A-Z`, `0-9`) und die meisten Sonderzeichen.
- **Spezielle Tasten**: `{ENTER}`, `{TAB}`, `{BACKSPACE}`, `{CTRL}`, `{ALT}`, `{WIN}`, `{SHIFT}`.
- **Pfeiltasten**: `{UP}`, `{DOWN}`, `{LEFT}`, `{RIGHT}`.
- **Funktionstasten**: `{F1}` bis `{F12}`.

#### Mausaktionen

- **Linksklick**: `{LEFTCLICK}` - Simuliert einen Klick mit der linken Maustaste.
- **Rechtsklick**: `{RIGHTCLICK}` - Simuliert einen Klick mit der rechten Maustaste.

### Beispiele

```c
SendKeys("Text{ENTER}"); // Gibt "Text" ein und drückt Enter.
SendKeys("{ALT}f{ALT}x"); // Öffnet das Dateimenü (Alt + f) und wählt Beenden (x) in vielen Anwendungen.
SendKeys("{LEFTCLICK}"); // Führt einen Linksklick aus.
SendKeys("{RIGHTCLICK}"); // Führt einen Rechtsklick aus.
