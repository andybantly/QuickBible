// Copyright (C) 2007 Andrew S. Bantly
#pragma once
#include <wincrypt.h>
#include <vector>

class CCrypt
{
public:
	CCrypt(BYTE * pPassword = NULL,DWORD dwLen = 0);
	~CCrypt();

	// Encrypt the data
	bool Encrypt(CString & csData,std::vector<BYTE> & vecData);

	// Decrypt the data
	bool Decrypt(std::vector<BYTE> & vecData,CString & csData);

private:

	// Create the hash and then the session key for encrypting/decrypting
	bool HashData(BYTE * pPassword,DWORD dwLen);

	// Create the actual session key using the algorithm
	bool CreateSessionKey();

private:
	bool m_bError;
	HCRYPTPROV m_hCryptProv;
	HCRYPTHASH m_hHash;
	HCRYPTKEY m_hSessionKey;
	HCRYPTKEY m_hKey;
};
