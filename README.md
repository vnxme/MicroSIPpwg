# MicroSIP Password Grabber
MicroSIPpwg is a console tool to decrypt your SIP account password stored in MicroSIP.ini. As of November 2022 (MicroSIP 3.21.3 dated September 14, 2022), SIP account passwords were encrypted using RSA with a pre-defined secret key. So as not to infringe microsip.org copyright or create a security breach for massive audience, the secret key won't be published here. Please, use [IDA Free](https://hex-rays.com/ida-free/) to retrieve the secret key from a binary on your own.

##### Development tools used:
- Hex-Rays IDA 8.1 x64
- Microsoft Visual Studio 2022 x64

##### Operating systems tested:
- Windows 11 Pro 21H2 x64
- Windows 10 Pro 21H2 x64

##### Application versions tested:
- MicroSIP 3.21.3
