# MicroSIP Password Grabber
MicroSIPpwg is a console tool to decrypt your SIP account password stored in MicroSIP.ini. As of November 2022 (MicroSIP 3.21.3 dated September 14, 2022), SIP account passwords were encrypted using RSA with a pre-defined secret key. So as not to infringe microsip.org copyright or create a security breach for massive audience, the secret key won't be published here. Please, use [IDA Free](https://hex-rays.com/ida-free/) to retrieve the secret key from a binary on your own.

## Usage

This tool only decrypts a password from **your own** `MicroSIP.ini` on **your own** machine. It reads the ini automatically from `%APPDATA%\MicroSIP\MicroSIP.ini`, selects the active account, and prints the account fields plus the decrypted password.

### 1. Prerequisites

- Windows (x86 or x64).
- To build from source: **Visual Studio 2022** (or the **Build Tools for Visual Studio 2022**) with:
  - *Desktop development with C++* / **MSVC v143** toolset,
  - **C++ MFC** for v143 (the project uses MFC, `UseOfMfc=Dynamic`),
  - a recent **Windows 10/11 SDK**.

  With `winget` you can install the Build Tools headlessly:

  ```
  winget install --id Microsoft.VisualStudio.2022.BuildTools -e ^
    --override "--quiet --wait --norestart --add Microsoft.VisualStudio.Workload.VCTools ^
    --add Microsoft.VisualStudio.Component.VC.ATLMFC --includeRecommended"
  ```

### 2. Provide the secret key

The secret key is **not** shipped with this project (see the note above). You extract it yourself from a `microsip.exe` and paste it into `const.h`:

```c
#define _GLOBAL_KEY L"[PUT SECRET KEY HERE]"
```

See [issue #1](https://github.com/vnxme/MicroSIPpwg/issues/1) for a step-by-step method using free tools. In short: the key is the string constant passed to `CCrypto::DeriveKey`, which MD5-hashes it and derives an RC2 key.

> **Representation matters.** The key is a **wide string of 8 code points (16 bytes)**, and some of them are non-printable. `DeriveKey` hashes those 16 bytes (`GetLength() * sizeof(wchar_t)`), so define `_GLOBAL_KEY` as a wide literal of the **8 code points** (e.g. `L"\uXXXX\uXXXX..."`). Do **not** rewrite the 16 bytes as sixteen one-byte escapes — that would hash 32 bytes and decryption would fail.

### 3. Build

Open `MicroSIPpwg.sln` in Visual Studio and build **Release | x64** (or **Win32** for 32-bit), or from a *Developer Command Prompt* / `msbuild`:

```
msbuild MicroSIPpwg.sln /t:Rebuild /p:Configuration=Release /p:Platform=x64
```

The executable is produced under `x64\Release\MicroSIPpwg.exe` (or `Release\` for Win32).

### 4. Run

Double-click `MicroSIPpwg.exe`, or run it from a terminal. Example output:

```
MicroSIP Password Grabber
Section:  Account1
Label:    My SIP account
Server:   sip.example.com
Proxy:    sip.example.com
Username: 1001
Domain:   sip.example.com
AuthID:   1001
Password: <your decrypted password>
```

If you see `WARNING: Can't decrypt the password string using the new format`, the key does not match the ini (wrong version or wrong bytes) — see step 2.

##### Development tools used:
- Hex-Rays IDA 8.1 x64
- Microsoft Visual Studio 2022 x64

##### Operating systems tested:
- Windows 11 Pro 21H2 x64
- Windows 10 Pro 21H2 x64

##### Application versions tested:
- MicroSIP 3.21.3
