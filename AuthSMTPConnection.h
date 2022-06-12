#pragma once

#include "atlsmtpconnection.h"

#define ATLSMTP_AUTH_UNPW "334"
#define ATLSMTP_AUTH_SUCC "235"

class CAuthSMTPConnection : public CSMTPConnection
{
public:
	CAuthSMTPConnection(int iPort = IPPORT_SMTP,const char * pszHostName = NULL,const char * pszName = NULL,const char * pszPwd = NULL) :
		m_pszHostName(NULL), m_pszName(NULL), m_pszPwd(NULL),
		m_nHostLen(0), m_nNameLen(0), m_nPwdLen(0)
	{
		if (pszHostName)
		{
			m_pszHostName = _strdup(pszHostName);
			m_nHostLen = (int)strlen(pszHostName);
		}

		if (pszName)
		{
			m_pszName = _strdup(pszName);
			m_nNameLen = (int)strlen(pszName);
		}

		if (pszPwd)
		{
			m_pszPwd = _strdup(pszPwd);
			m_nPwdLen = (int)strlen(pszPwd);
		}

		m_iPort = iPort;
	}

	~CAuthSMTPConnection()
	{
		if (m_pszHostName)
		{
			free(m_pszHostName);
			m_pszHostName = NULL;
		}

		if (m_pszName)
		{
			free(m_pszName);
			m_pszName = NULL;
		}

		if (m_pszPwd)
		{
			free(m_pszPwd);
			m_pszPwd = NULL;
		}
	}

	// Override Connect in CSMTPConnection to authenticate
	BOOL Connect() throw()
	{
		int nBufLen = 0;
		LPCTSTR lpszHostName = m_pszHostName;
		if (!lpszHostName)
			return FALSE;
		DWORD dwTimeout = 10000;
		ATLASSERT(lpszHostName != NULL);

		// If we're already connected
		if (Connected())
			return FALSE;

		if (!_g_smtp_init.Init())
			return FALSE;

		CSocketAddr address;
		if (address.FindAddr(lpszHostName,m_iPort,0,PF_UNSPEC,SOCK_STREAM,0))
			return FALSE;

		BOOL bRet = FALSE;
		int nIndex = 0;
		ADDRINFOT * pAI;
		while ((pAI = address.GetAddrInfo(nIndex++)) != NULL)
		{
			// Create the socket
			m_hSocket = WSASocket(pAI->ai_family,pAI->ai_socktype,pAI->ai_protocol,NULL,0,WSA_FLAG_OVERLAPPED);
			if (m_hSocket == INVALID_SOCKET)
				return FALSE;

			// Connect to the destination and wait for the connection event
			bRet = FALSE;
			WSAEVENT hEventConnect = WSACreateEvent();
			if (hEventConnect != NULL)
			{
				if (SOCKET_ERROR != WSAEventSelect(m_hSocket,hEventConnect,FD_CONNECT))
				{
					if (WSAConnect(m_hSocket,pAI->ai_addr,(int)pAI->ai_addrlen,NULL,NULL,NULL,NULL))
					{
						if (WSAGetLastError() == WSAEWOULDBLOCK)
						{
							DWORD dwWait = WaitForSingleObject((HANDLE)hEventConnect,dwTimeout);
							if (dwWait == WAIT_OBJECT_0)
							{
								// Test for connection errors
								WSANETWORKEVENTS wse;
								ZeroMemory(&wse, sizeof(wse));
								WSAEnumNetworkEvents(m_hSocket, NULL, &wse);
								if (wse.iErrorCode[FD_CONNECT_BIT]==0)
									bRet = TRUE;
							}
						}
					}
				}

				// Done with the event
				WSACloseEvent(hEventConnect);
			}

			// Test for an error
			if (bRet)
				break;
			
			// Close the socket on an error
			shutdown(m_hSocket, SD_BOTH);
			closesocket(m_hSocket);
			m_hSocket = INVALID_SOCKET;
		}
		
		// Create an event for asynchronous I/O
		if (bRet)
		{
			ATLASSUME(m_Overlapped.hEvent == NULL);
			m_Overlapped.hEvent = CreateEvent(NULL,TRUE,TRUE,NULL);
			if (m_Overlapped.hEvent == NULL)
				bRet = FALSE;
		}

		// Test for a created overlapped event
		if (bRet)
		{
			// See if the connect returns success
			nBufLen = ATLSMTP_MAX_LINE_LENGTH;
			bRet = AtlSmtpReadData((HANDLE)m_hSocket,m_szBuf,&nBufLen,&m_Overlapped);
			if (bRet)
			{
				if (strncmp(m_szBuf,ATLSMTP_CONN_SUCC,ATLSMTP_RETCODE_LEN))
					bRet = FALSE;
			}
		}

		// Get the computers host name for the SMTP servergethostname should return 0 on success
		char szLocalHost[ATLSMTP_MAX_SERVER_NAME_LENGTH + 1];
		if (bRet && gethostname(szLocalHost,ATLSMTP_MAX_SERVER_NAME_LENGTH))
			bRet = FALSE;

		if (bRet)
		{
			// Send the EHLO command
#if _SECURE_ATL
			nBufLen = sprintf_s(m_szBuf,ATLSMTP_MAX_LINE_LENGTH + 1,"EHLO %s\r\n",szLocalHost);
#else
			nBufLen = _snprintf(m_szBuf, ATLSMTP_MAX_LINE_LENGTH + 1,"EHLO %s\r\n",szLocalHost);
#endif
			bRet = AtlSmtpSendAndCheck((HANDLE)m_hSocket,m_szBuf,nBufLen,m_szBuf,&nBufLen,ATLSMTP_MAX_LINE_LENGTH,ATLSMTP_HELO_SUCC,&m_Overlapped);

			// Authenticate the user
			if (bRet)
				bRet = AuthenticateUser();
		}

		if (!bRet)
		{
			if (m_Overlapped.hEvent != NULL)
				CloseHandle(m_Overlapped.hEvent);
			shutdown(m_hSocket,SD_BOTH);
			closesocket(m_hSocket);
			m_hSocket = INVALID_SOCKET;
		}
		if (bRet)
			bRet = Connected();
		return bRet;
	}

	// Authenticate
	BOOL AuthenticateUser()
	{
		BOOL bRet = FALSE;
		int nBufLen = sizeof(m_szBuf);
		if (!strstr(m_szBuf,"250-AUTH"))
			AtlSmtpReadData((HANDLE)m_hSocket,m_szBuf,&nBufLen,&m_Overlapped);

#if _SECURE_ATL
		nBufLen = sprintf_s(m_szBuf, ATLSMTP_MAX_LINE_LENGTH + 1,"AUTH LOGIN\r\n");
#else
		nBufLen = _snprintf(m_szBuf, ATLSMTP_MAX_LINE_LENGTH + 1,"AUTH LOGIN\r\n");
#endif
		bRet = AtlSmtpSendAndCheck((HANDLE)m_hSocket,m_szBuf,nBufLen,m_szBuf,&nBufLen,ATLSMTP_MAX_LINE_LENGTH,ATLSMTP_AUTH_UNPW,&m_Overlapped);
		if (bRet)
		{
			const char * pasz[] = {"username:","password:"};
			for (int iAuth = 0;bRet && iAuth < 2;++iAuth)
			{
				// Get the challenge tests
				const char * psz = pasz[iAuth];
				int nLen = (int)strlen(psz);

				// Decode the response
				int nDestLen = sizeof(m_uzBuf);
				bRet = Base64Decode(&m_szBuf[4],nBufLen - 4,&m_uzBuf[0],&nDestLen);
				if (bRet)
				{
					if (nDestLen >= nLen)
					{
						bRet = _strnicmp((char *)&m_uzBuf[0],psz,nLen) == 0 ? TRUE : FALSE;
						if (bRet)
						{
							int nDestLen = sizeof(m_uzBuf);
							bRet = Base64Encode(iAuth == 0 ? (const BYTE *)m_pszName : (const BYTE *)m_pszPwd,iAuth == 0 ? m_nNameLen : m_nPwdLen,(LPSTR)&m_szBuf,&nDestLen);
							if (bRet)
							{
#if _SECURE_ATL
								strncpy_s(&m_szBuf[nDestLen],sizeof(m_szBuf) - nDestLen,"\r\n",2);
#else
								strncpy(&m_szBuf[nDestLen],"\r\n",2);
#endif
								if (bRet)
									bRet = AtlSmtpSendAndCheck((HANDLE)m_hSocket,m_szBuf,nDestLen + 2,m_szBuf,&nDestLen,ATLSMTP_MAX_LINE_LENGTH,iAuth == 0 ? ATLSMTP_AUTH_UNPW : ATLSMTP_AUTH_SUCC,&m_Overlapped);
							}
						}
					}
					else
						bRet = FALSE;
				}
			}
		}

		return bRet;
	}

protected:
	char * m_pszHostName, * m_pszName, * m_pszPwd;
	int m_nHostLen,m_nNameLen,m_nPwdLen;
	char m_szBuf[ATLSMTP_MAX_LINE_LENGTH + 1];
	BYTE m_uzBuf[ATLSMTP_MAX_LINE_LENGTH + 1];
	int m_iPort;
};
