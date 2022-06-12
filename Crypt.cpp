// Copyright (C) 2007 Andrew S. Bantly
#include "stdafx.h"
#include "Crypt.h"

using namespace std;

CCrypt::CCrypt(BYTE * pPassword,DWORD dwLen) : m_hCryptProv(NULL), m_hHash(NULL), m_hKey(NULL), m_hSessionKey(NULL)
{
	// Get handle to the default provider. 
	if (!CryptAcquireContext(&m_hCryptProv,NULL,MS_ENHANCED_PROV,PROV_RSA_FULL,CRYPT_VERIFYCONTEXT))
		return;
	HashData(pPassword,dwLen);
}

CCrypt::~CCrypt()
{
	// Destroy the MD5 Hash
	if (m_hHash)
	{
		CryptDestroyHash(m_hHash); 
		m_hHash = NULL;
	}

	// Destroy session key. 
	if (m_hSessionKey)
	{
		CryptDestroyKey(m_hSessionKey); 
		m_hSessionKey = NULL;
	}

	// Destroy the cryptographic session
	if (m_hKey) 
	{
		CryptDestroyKey(m_hKey);
		m_hKey = NULL;
	}

	// Release the cryptographic provider
	if (m_hCryptProv)
	{
		CryptReleaseContext(m_hCryptProv,0);
		m_hCryptProv = NULL;
	}
}

// Encrypt the data
bool CCrypt::Encrypt(CString & csData,vector<BYTE> & vecData)
{
	bool bRet = false;
	DWORD dwOut = csData.GetLength();
	if (dwOut == 0)
		return true;
	vecData.resize(dwOut);
Encrypt:
	memcpy(&vecData[0],csData.GetBuffer(),dwOut);
	if (!CryptEncrypt(m_hSessionKey,0,TRUE,0,&vecData[0],&dwOut,dwOut))
	{
		if (GetLastError() == ERROR_MORE_DATA)
		{
			vecData.resize(dwOut);
			goto Encrypt;
		}
	}
	else
		bRet = true;
	return bRet;
}

// Decrypt the data
bool CCrypt::Decrypt(vector<BYTE> & vecData,CString & csData)
{
	bool bRet = false;
	vector<BYTE> vecDecrypt;
	DWORD dwLength = (DWORD)vecData.size();
	if (dwLength == 0)
		return true;
	vecDecrypt.resize(dwLength);
	memcpy(&vecDecrypt[0],&vecData[0],dwLength);
	if (CryptDecrypt(m_hSessionKey,0,TRUE,0,&vecDecrypt[0],&dwLength))
	{
		char * pBuffer = csData.GetBufferSetLength(dwLength);
		memcpy(pBuffer,&vecDecrypt[0],dwLength);
		bRet = true;
	}
	return bRet;
}

// Create the hash and then the session key for encrypting/decrypting
bool CCrypt::HashData(BYTE * pPassword,DWORD dwLen)
{
	if (!m_hHash)
	{
		// Test for a good initialization
		if (!m_hCryptProv)
			return false;

		// Create the MD5 cryptographic hash
		if (!CryptCreateHash(m_hCryptProv,CALG_MD5,0,0,&m_hHash))
			return false;
	}

	// Add the password to the MD5 hash if it is being used
	if (pPassword)
	{
		bool bHash = CryptHashData(m_hHash,pPassword,dwLen,0) ? true : false;
		if (!bHash)
			return false;
	}
	return CreateSessionKey();
}

// Create the actual session key using the algorithm
bool CCrypt::CreateSessionKey()
{
	// Derive a session key from the hash object. 
	if (!CryptDeriveKey(m_hCryptProv,CALG_RC4,m_hHash,0x00800000,&m_hSessionKey))
		return false;
	CryptDestroyHash(m_hHash); 
	m_hHash = NULL;
	return true;
}
