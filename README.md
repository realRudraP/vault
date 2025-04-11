# Vault - Secure Interactive File Vault

## Overview

Vault is a C++ application that provides a secure, **interactive command-line shell** for managing encrypted files. Unlike simple one-off tools, Vault maintains state across sessions, remembering the files stored within its encrypted structure. It uses the robust AES-GCM authenticated encryption algorithm (via OpenSSL) for securing individual files added to the vault.

A key feature is its method of storing added files: Vault encrypts the file, splits it into chunks, and distributes these chunks across various pre-configured system directories, adding a layer of obfuscation. User authentication is handled securely, prompting for a master password with **command-line echo disabling** to prevent shoulder-surfing.

The application runs an interactive loop, presenting a `Vault>` prompt where users can issue commands to manage files within the vault, perform standalone encryption/decryption tasks, or navigate the local filesystem.

## Features

* **Interactive Shell:** Provides a dedicated command-line environment (`Vault>`) for operations.
* **Strong Encryption:** Uses AES-GCM via OpenSSL for authenticated encryption of vault contents and metadata.
* **Secure Password Entry:** Disables terminal echo when prompting for the master password to enhance security.
* **State Management:** Remembers vault contents and configuration between sessions by loading/saving encrypted metadata.
* **File Chunking & Distribution:** Encrypted files added to the vault are split into chunks and scattered across designated hidden folders on the filesystem for obfuscation.
* **Secure Deletion:** Offers an option to securely wipe original files after adding them to the vault or via a dedicated command, using multiple overwrite passes.
* **Configuration:** Allows configuration of secure deletion behavior and storage locations during initial setup.
* **Filesystem Integration:** Includes basic commands (`ls`, `cd`) for navigating the host filesystem from within the shell.
* **Standalone Crypto:** Provides commands (`encrypt`, `decrypt`) for encrypting/decrypting individual files outside the main vault structure.
* **Dependency Management:** Uses `vcpkg` for managing external libraries (OpenSSL) [cite: uploaded:vault/vcpkg.json].

## Prerequisites

Before building and running Vault, ensure you have the following installed:

1.  **C++ Compiler:** A modern C++ compiler supporting C++17 or later (e.g., GCC, Clang, MSVC).
2.  **vcpkg:** The C++ library manager from Microsoft. Vault uses `vcpkg` in manifest mode (`vcpkg.json`) to acquire dependencies [cite: uploaded:vault/vcpkg.json]. Follow the official [vcpkg installation guide](https://github.com/microsoft/vcpkg#quick-start-windows).
3.  **Git:** Required for cloning the repository and for `vcpkg` to function correctly.
4.  **Build Tools:** Appropriate build tools for your platform (e.g., Make, Ninja, MSBuild/Visual Studio).

## Dependencies

The primary external dependency is:

* **OpenSSL:** Used for all cryptographic operations (AES-GCM, key derivation, secure random number generation).

This dependency is automatically managed by `vcpkg` via the `vcpkg.json` file [cite: uploaded:vault/vcpkg.json].

## Building the Project

*(Note: Specific build commands depend on your chosen build tools and environment setup.)*

1.  **Clone the Repository:**
    ```bash
    git clone <repository-url>
    cd vault
    ```

2.  **Integrate vcpkg:**
    Ensure your build environment is configured to use the `vcpkg` toolchain. Consult the `vcpkg` documentation for integration details specific to your build system.

3.  **Configure and Build:**
    Use your chosen build tools (like Make, Ninja, or Visual Studio's build tools) to configure and compile the project.

4.  **Executable Location:**
    The compiled executable (e.g., `vault` or `vault.exe`) will typically be found within your designated build output directory.

## Usage

1.  **Start the Application:** Run the compiled executable from your terminal.
    ```bash
    ./path/to/build/output/vault
    ```

2.  **Initial Setup / Login:**
    * **First Run:** The application will guide you through creating a master password (with confirmation and echo disabling) and configuring options like secure deletion and storage locations for file chunks. **Remember your master password - it cannot be recovered.**
    * **Subsequent Runs:** You will be prompted to enter your master password (with echo disabling) to unlock the vault and access the shell. Three incorrect attempts will cause the application to exit.

3.  **Interactive Shell:** Once authenticated, you will see a prompt indicating the current working directory followed by `Vault>`, e.g.:
    ```
    /home/user/documents: Vault> 
    ```
    You can now enter commands. Commands are case-insensitive.

4.  **Available Commands:** (Based on the `help` command output and parser logic)

    * **Vault Operations:**
        * `add <file_path> [-name <internal_name>] [-sdel]`
            * Adds the specified file from your filesystem to the vault.
            * `<file_path>`: Path to the file on your system.
            * `-name <internal_name>`: (Optional) Store the file under a different name inside the vault. Defaults to the original filename.
            * `-sdel`: (Optional) Securely delete the original file after successfully adding it to the vault.
        * `Workspace <internal_name>`
            * Retrieves a file from the vault by its internal name and saves it to the current working directory.
            * `<internal_name>`: The name of the file as stored in the vault.
        * `list`
            * Lists all files currently stored inside the vault by their internal names.

    * **File Security Operations (Standalone):**
        * `sdelete <file_path>`
            * Securely deletes the specified file *from your filesystem* (outside the vault). **WARNING: This is irreversible.**
        * `encrypt <file_path>`
            * Encrypts the specified file *on your filesystem*, creating a `<filename>.vaultaes` file. Does not add it to the vault structure.
        * `decrypt <file_path>`
            * Decrypts a file previously encrypted with the `encrypt` command (must have `.vaultaes` extension). Saves the result in the same directory without the extension.

    * **Filesystem Navigation:**
        * `ls [<directory_path>]` or `dir [<directory_path>]`
            * Lists the contents of the specified host filesystem directory or the current working directory.
        * `cd <directory_path>`
            * Changes the current working directory on the host filesystem.

    * **Application Control:**
        * `help`
            * Displays the built-in help message detailing available commands.
        * `exit`
            * Exits the Vault application shell.

## Project Structure
## Project Structure
vault/
├── include/            # Header files (.h) defining interfaces
│   ├── ... (list of .h files)
├── src/                # Source files (.cpp) implementing the logic
│   ├── ... (list of .cpp files including main.cpp)
├── vcpkg.json          # vcpkg manifest: defines dependencies (OpenSSL)
├── vcpkg-configuration.json # vcpkg configuration details (e.g., registries)
└── README.md           # This documentation file


