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


#### Sicheres Löschen von Dateien mit Militärstandard-Algorithmen

Die `secureErase`-Funktion bietet eine robuste Lösung zum sicheren Löschen von Dateien, indem sie fortschrittliche Datenlöschalgorithmen nutzt, die auf strengen Militärstandards basieren. Diese Algorithmen gewährleisten, dass gelöschte Daten mit einem Maß an Sicherheit entfernt werden, das den Anforderungen militärischer und geheimdienstlicher Organisationen entspricht. Unterstützte Standards umfassen:

- **DoD 5220.22-M (USA, Verteidigungsministerium)**: Ein 3-Pass-Algorithmus, der Daten mit den Mustern 0x00, 0xFF und erneut 0x00 überschreibt, um eine sichere Datenlöschung zu gewährleisten.
- **AFSSI-5020 (US Air Force)**: Ein Durchgang mit 0x00, bietet eine schnelle und effektive Methode zur Datenlöschung.
- **GOST R 50739-95 (Russischer Standard)**: Zwei Durchgänge mit 0x00, verwendet in russischen Regierungsorganisationen für die sichere Datenvernichtung.
- **British HMG IS5 (Britisches Innenministerium)**: Entspricht dem DoD 5220.22-M Standard mit drei Durchgängen (0x00, 0xFF, 0x00) für die Datenlöschung im Vereinigten Königreich.
- **Canadian RCMP TSSIT OPS-II (Royal Canadian Mounted Police)**: Ein 7-Pass-Algorithmus mit spezifischen Mustern für jede Passage, um höchste Sicherheitsanforderungen zu erfüllen.
- **Peter Gutmann Algorithmus**: Ein umfassender 35-Pass-Algorithmus, entwickelt von Peter Gutmann, der eine breite Palette von Musterkombinationen verwendet, um Daten auf fast jeder Art von Laufwerk sicher zu löschen.
- **US Army AR380-19 (US Army)**: Ähnelt dem DoD 5220.22-M Standard und bietet eine dreistufige Datenlöschung für die Verwendung in der US-Armee.

#### Verwendung

Um eine Datei sicher zu löschen, wählen Sie einfach den gewünschten Algorithmus und den Dateipfad:

```c
secureErase("pfad/zur/datei.txt", "DoD5220.22-M");
