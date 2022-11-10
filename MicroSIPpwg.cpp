#include <iostream>
#include "stdafx.h"
#include "const.h"
#include "crypto.h"

// MicroSIP 3.21.3 -> settings.cpp -> Lines 35-44 //
static LONGLONG FileSize(const wchar_t* name)
{
	WIN32_FILE_ATTRIBUTE_DATA fad{};
	if (!GetFileAttributesEx(name, GetFileExInfoStandard, &fad))
		return -1; // error condition, could call GetLastError to find out more
	LARGE_INTEGER size{};
	size.HighPart = fad.nFileSizeHigh;
	size.LowPart = fad.nFileSizeLow;
	return size.QuadPart;
}
// ---------------------------------------------- //

// MicroSIP 3.21.3 -> lib/MSIP.cpp -> Lines 456-465 //
CString Bin2String(CByteArray* ca)
{
	CString res;
	int k = (int)ca->GetSize();
	for (int i = 0; i < k; i++) {
		unsigned char ch = ca->GetAt(i);
		res.AppendFormat(_T("%02x"), ca->GetAt(i));
	}
	return res;
}
// ------------------------------------------------ //

// MicroSIP 3.21.3 -> lib/MSIP.cpp -> Lines 467-478 //
void String2Bin(CString str, CByteArray* res)
{
	res->RemoveAll();
	int k = str.GetLength();
	CStringA rab;
	for (int i = 0; i < str.GetLength(); i += 2) {
		rab = CStringA(str.Mid(i, 2));
		char* p = NULL;
		unsigned long bin = strtoul(rab.GetBuffer(), &p, 16);
		res->Add((byte)bin);
	}
}
// ------------------------------------------------ //

// MicroSIP 3.21.3 -> lib/MSIP.cpp -> Lines 447-454 //
bool IniSectionExists(CString section, CString iniFile)
{
	CString str;
	LPTSTR ptr = str.GetBuffer(3);
	int result = GetPrivateProfileString(section, NULL, NULL, ptr, 3, iniFile);
	str.ReleaseBuffer();
	return result;
}
// ------------------------------------------------ //

// MicroSIP 3.21.3 -> settings.cpp -> Multiple lines //
int main()
{
	CString outputText;
	outputText.Format(_T("%s %s\n"), _T(_GLOBAL_NAME), _T(_GLOBAL_PWG));
	std::wcout << outputText.GetString();

	CString appDataRoaming;
	LPTSTR ptr;
	ptr = appDataRoaming.GetBuffer(MAX_PATH);
	SHGetSpecialFolderPath(
		0,
		ptr,
		CSIDL_APPDATA,
		FALSE);
	appDataRoaming.ReleaseBuffer();
	appDataRoaming.AppendFormat(_T("\\%s\\"), _T(_GLOBAL_NAME));

	CString iniFile;
	iniFile.Format(_T("%s.ini"), _T(_GLOBAL_NAME));
	iniFile = appDataRoaming + iniFile;
	
	if (!::PathFileExists(iniFile) || FileSize(iniFile) == 0) {
		outputText.Format(_T("ERROR: Can't find the settings file (%s)\n"), iniFile.GetString());
		std::wcout << outputText.GetString();
		exit(1);
	}
	else {
		CString settingsSection;
		settingsSection = _T("Settings");

		bool sectionExists;
		sectionExists = IniSectionExists(settingsSection, iniFile);
		if (!sectionExists) {
			std::wcout << "ERROR: The settings section doesn't exist\n";
			exit(1);
		}

		CString str;
		ptr = str.GetBuffer(255);
		GetPrivateProfileString(settingsSection, _T("enableLocalAccount"), NULL, ptr, 256, iniFile);
		str.ReleaseBuffer();
		bool enableLocalAccount = _wtoi(str);

		ptr = str.GetBuffer(255);
		GetPrivateProfileString(settingsSection, _T("accountId"), NULL, ptr, 256, iniFile);
		str.ReleaseBuffer();

		CString accountSection;
		if (str.IsEmpty()) {
			accountSection = settingsSection;
		}
		else {
			int accountId = _wtoi(str);
			if (!accountId && !enableLocalAccount) {
				accountId = 1;
			}
			if (accountId > 0) {
				accountSection.Format(_T("Account%d"), accountId);
				sectionExists = IniSectionExists(accountSection, iniFile);
				if (!sectionExists) {
					accountId = 0;
					accountSection.Format(_T("Account%d"), accountId);
					sectionExists = IniSectionExists(accountSection, iniFile);
					if (!sectionExists) {
						std::wcout << "ERROR: The account section doesn't exist\n";
						exit(1);
					}
				}
			}
		}
		outputText.Format(_T("Section:  %s\n"), accountSection.GetString());
		std::wcout << outputText.GetString();

		ptr = str.GetBuffer(255);
		GetPrivateProfileString(accountSection, _T("label"), NULL, ptr, 256, iniFile);
		str.ReleaseBuffer();

		CString labelText = str;
		outputText.Format(_T("Label:    %s\n"), labelText.GetString());
		std::wcout << outputText.GetString();

		ptr = str.GetBuffer(255);
		GetPrivateProfileString(accountSection, _T("server"), NULL, ptr, 256, iniFile);
		str.ReleaseBuffer();

		CString serverText = str;
		outputText.Format(_T("Server:   %s\n"), serverText.GetString());
		std::wcout << outputText.GetString();

		ptr = str.GetBuffer(255);
		GetPrivateProfileString(accountSection, _T("proxy"), NULL, ptr, 256, iniFile);
		str.ReleaseBuffer();

		CString proxyText = str;
		outputText.Format(_T("Proxy:    %s\n"), proxyText.GetString());
		std::wcout << outputText.GetString();

		ptr = str.GetBuffer(255);
		GetPrivateProfileString(accountSection, _T("username"), NULL, ptr, 256, iniFile);
		str.ReleaseBuffer();

		CString usernameText = str;
		outputText.Format(_T("Username: %s\n"), usernameText.GetString());
		std::wcout << outputText.GetString();

		ptr = str.GetBuffer(255);
		GetPrivateProfileString(accountSection, _T("domain"), NULL, ptr, 256, iniFile);
		str.ReleaseBuffer();

		CString domainText = str;
		outputText.Format(_T("Domain:   %s\n"), domainText.GetString());
		std::wcout << outputText.GetString();

		ptr = str.GetBuffer(255);
		GetPrivateProfileString(accountSection, _T("authID"), NULL, ptr, 256, iniFile);
		str.ReleaseBuffer();

		CString authidText = str;
		outputText.Format(_T("AuthID:   %s\n"), authidText.GetString());
		std::wcout << outputText.GetString();

		ptr = str.GetBuffer(1040);
		GetPrivateProfileString(accountSection, _T("password"), NULL, ptr, 1041, iniFile);
		str.ReleaseBuffer();

		CString passwordHash = str;
		CString passwordText = _T("");
		if (!passwordHash.IsEmpty()) {
			CByteArray arPassword;
			String2Bin(passwordHash, &arPassword);
			MFC::CCrypto crypto;
			if (crypto.DeriveKey((LPCTSTR)_GLOBAL_KEY)) {
				try {
					if (!crypto.Decrypt(arPassword, str)) {
						std::wcout << "WARNING: Can't decrypt the password string using the new format, trying the old one\n";

						//--decode from old format
						ptr = str.GetBuffer(255);
						GetPrivateProfileString(accountSection, _T("passwordSize"), NULL, ptr, 256, iniFile);
						str.ReleaseBuffer();
						if (!str.IsEmpty()) {
							int size = _wtoi(str);
							arPassword.SetSize(size > 0 ? size : 16);
							GetPrivateProfileStruct(accountSection, _T("password"), arPassword.GetData(), (int)arPassword.GetSize(), iniFile);
							crypto.Decrypt(arPassword, str);
						}
						//--end decode from old format
						//if (crypto.Encrypt(str, arPassword)) {
						//	WritePrivateProfileString(accountnSection, _T("password"), Bin2String(&arPassword), iniFile);
						//	//--delete old format addl.data
						//	WritePrivateProfileString(accountnSection, _T("passwordSize"), NULL, iniFile);
						//}
					}

					passwordText = str;
				}
				catch (...) {
					std::wcout << "ERROR: Can't decrypt the password string\n";
					exit(1);
				}
			}
			else {
				std::wcout << "ERROR: Can't derive a key from the secret string\n";
				exit(1);
			}
		}
		outputText.Format(_T("Password: %s\n"), passwordText.GetString());
		std::wcout << outputText.GetString();
	}
	std::cin.get();
	exit(0);
}
