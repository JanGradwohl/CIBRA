# CIBRA: Simplified System Control & Encryption in C

CIBRA, short for Comprehensive Interface for Binary and Resource Automation, is a sleek C library designed for system automation, encryption, and utility functions, leveraging OpenSSL for robust security. It streamlines complex tasks such as user input simulation, secure file handling, and system commands, enabling the easy integration of sophisticated features into C projects.

## SendKeys Function

The `SendKeys` function simulates keyboard inputs and mouse interactions. It supports a wide range of characters, special key commands, and mouse actions.

### Supported Input Parameters

#### Keyboard Inputs

- **Characters**: All alphanumeric characters (`a-z`, `A-Z`, `0-9`) and most special characters.
- **Special Keys**: `{ENTER}`, `{TAB}`, `{BACKSPACE}`, `{CTRL}`, `{ALT}`, `{WIN}`, `{SHIFT}`.
- **Arrow Keys**: `{UP}`, `{DOWN}`, `{LEFT}`, `{RIGHT}`.
- **Function Keys**: `{F1}` through `{F12}`.

#### Mouse Actions

- **Left Click**: `{LEFTCLICK}` - Simulates a click with the left mouse button.
- **Right Click**: `{RIGHTCLICK}` - Simulates a click with the right mouse button.

### Examples

```c
SendKeys("Text{ENTER}"); // Types "Text" and presses Enter.
SendKeys("{ALT}f{ALT}x"); // Opens the File menu (Alt + f) and selects Exit (x) in many applications.
SendKeys("{LEFTCLICK}"); // Performs a left click.
SendKeys("{RIGHTCLICK}"); // Performs a right click.
```


## Erase Function

### Secure File Deletion with Military-Standard Algorithms

The `erase` function offers a robust solution for securely deleting files by leveraging advanced data wiping algorithms based on strict military standards. These algorithms ensure that deleted data is removed with a level of security that meets the requirements of military and intelligence organizations. Supported standards include:

- **DoD 5220.22-M (USA, Department of Defense)**: A 3-pass algorithm that overwrites data with the patterns 0x00, 0xFF, and again 0x00, ensuring secure data deletion.
- **AFSSI-5020 (US Air Force)**: A single pass with 0x00, provides a fast and effective method for data wiping.
- **GOST R 50739-95 (Russian Standard)**: Two passes with 0x00, used in Russian government organizations for secure data destruction.
- **British HMG IS5 (British Home Office)**: Matches the DoD 5220.22-M standard with three passes (0x00, 0xFF, 0x00) for data wiping in the United Kingdom.
- **Canadian RCMP TSSIT OPS-II (Royal Canadian Mounted Police)**: A 7-pass algorithm with specific patterns for each pass, meeting the highest security requirements.
- **Peter Gutmann Algorithm**: A comprehensive 35-pass algorithm developed by Peter Gutmann that uses a wide range of pattern combinations to securely erase data on nearly any type of drive.
- **US Army AR380-19 (US Army)**: Similar to the DoD 5220.22-M standard, providing a three-stage data wiping process for use in the US Army.

#### Usage

To securely delete a file, simply select the desired algorithm and the file path:

```c
erase("path/to/file.txt", ALGO_DoD5220_22_M); // Securely deletes the file using the DoD 5220.22-M algorithm.
```

## SetMousePosition Function

The `SetMousePosition` function is designed to move the cursor to a specified location on the screen, given by the coordinates `x` and `y`.

### Usage

```c
SetMousePosition(200, 300); // Moves the cursor to the position (200, 300) on the screen.
```

## OS Function

The `os` function executes a given command in the system's command line and returns the output. This can be particularly useful for automating tasks, retrieving system information, or executing scripts and commands directly from within the application.

### Usage

```c
char* result = os("dir"); // Executes the 'dir' command (on Windows) or 'ls' command (on Unix/Linux) and returns the output.
```

## Encryption and Decryption Functions

The `file_encrypt` and `file_decrypt` functions provide the capability to securely encrypt and decrypt files using a variety of algorithms. Below is an overview of the supported encryption standards, each with its unique characteristics and use cases:

- **ALGO_AES_256_CBC**: Advanced Encryption Standard with a 256-bit key in Cipher Block Chaining mode. Widely used for its strength and efficiency.
- **ALGO_DES_EDE3_CBC**: Triple Data Encryption Standard in Cipher Block Chaining mode. An older standard that applies the DES algorithm three times to each data block.
- **ALGO_BLOWFISH_CBC**: A symmetric block cipher that provides strong encryption in Cipher Block Chaining mode and is known for its efficiency.
- **ALGO_CAMELLIA_256_CBC**: A symmetric key block cipher with a 256-bit key. Recognized for its security and performance, it's used in various cryptographic protocols.
- **ALGO_AES_128_CBC**: AES with a 128-bit key in Cipher Block Chaining mode. Offers a good balance between security and performance.
- **ALGO_SEED_CBC**: A block cipher developed by the Korean Information Security Agency, known for its security and efficiency in Cipher Block Chaining mode.
- **ALGO_AES_192_CBC**: AES with a 192-bit key in Cipher Block Chaining mode. Provides enhanced security over AES-128.
- **ALGO_CAST5_CBC**: A symmetric key block cipher known for its flexibility in key sizes and its use in Cipher Block Chaining mode.
- **ALGO_RC2_CBC**: A variable key-size cipher in Cipher Block Chaining mode, known for its adaptability.
- **ALGO_IDEA_CBC**: International Data Encryption Algorithm in Cipher Block Chaining mode. Known for its strength and efficiency, though less common today.
- **ALGO_RC4**: A stream cipher known for its simplicity and speed in operations, often used in protocols like SSL.
- **ALGO_AES_256_GCM**: AES with a 256-bit key in Galois/Counter Mode. Provides both encryption and authentication, offering high levels of security and performance.
- **ALGO_CHACHA20_POLY1305**: A combination of the ChaCha20 stream cipher and the Poly1305 MAC, providing high-speed encryption and authentication.
- **ALGO_SM4_CBC**: A block cipher standard developed by the Chinese government, mainly used in China, in Cipher Block Chaining mode.
- **ALGO_AES_256_XTS**: AES with a 256-bit key in XEX-based tweaked-codebook mode with ciphertext stealing. Commonly used for disk encryption.

### Usage

```c
// Encrypt a file
file_encrypt("path/to/file.txt", "password", ALGO_AES_256_CBC);

// Decrypt a file
file_decrypt("path/to/file_encrypted.txt", "password", ALGO_AES_256_CBC);
```
