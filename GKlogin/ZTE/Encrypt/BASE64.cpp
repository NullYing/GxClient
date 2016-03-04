#include "stdafx.h"
#include "BASE64.h"

static const char *g_pCodes =
"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";

static const unsigned char g_pMap[256] =
{
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 62, 255, 255, 255, 63,
	52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 255, 255,
	255, 254, 255, 255, 255, 0, 1, 2, 3, 4, 5, 6,
	7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
	19, 20, 21, 22, 23, 24, 25, 255, 255, 255, 255, 255,
	255, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36,
	37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
	49, 50, 51, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255
};



bool CBase64::Encode(const unsigned char *pIn, unsigned long uInLen, unsigned char *pOut, unsigned long *uOutLen)
{
	unsigned long i, len2, leven;
	unsigned char *p;

	if (pOut == NULL || *uOutLen == 0)
		return false;

	//ASSERT((pIn != NULL) && (uInLen != 0) && (pOut != NULL) && (uOutLen != NULL));

	len2 = ((uInLen + 2) / 3) << 2;
	if ((*uOutLen) < (len2 + 1)) return false;

	p = pOut;
	leven = 3 * (uInLen / 3);
	for (i = 0; i < leven; i += 3)
	{
		*p++ = g_pCodes[pIn[0] >> 2];
		*p++ = g_pCodes[((pIn[0] & 3) << 4) + (pIn[1] >> 4)];
		*p++ = g_pCodes[((pIn[1] & 0xf) << 2) + (pIn[2] >> 6)];
		*p++ = g_pCodes[pIn[2] & 0x3f];
		pIn += 3;
	}

	if (i < uInLen)
	{
		unsigned char a = pIn[0];
		unsigned char b = ((i + 1) < uInLen) ? pIn[1] : 0;
		unsigned char c = 0;

		*p++ = g_pCodes[a >> 2];
		*p++ = g_pCodes[((a & 3) << 4) + (b >> 4)];
		*p++ = ((i + 1) < uInLen) ? g_pCodes[((b & 0xf) << 2) + (c >> 6)] : '=';
		*p++ = '=';
	}

	*p = 0; // Append NULL byte
	*uOutLen = p - pOut;
	return true;
}

bool CBase64::Encode(const unsigned char *pIn, unsigned long uInLen, CString& strOut)
{
	unsigned long i, len2, leven;
	strOut = "";

	//ASSERT((pIn != NULL) && (uInLen != 0) && (pOut != NULL) && (uOutLen != NULL));

	len2 = ((uInLen + 2) / 3) << 2;
	//if((*uOutLen) < (len2 + 1)) return false;

	//p = pOut;
	leven = 3 * (uInLen / 3);
	for (i = 0; i < leven; i += 3)
	{
		strOut += g_pCodes[pIn[0] >> 2];
		strOut += g_pCodes[((pIn[0] & 3) << 4) + (pIn[1] >> 4)];
		strOut += g_pCodes[((pIn[1] & 0xf) << 2) + (pIn[2] >> 6)];
		strOut += g_pCodes[pIn[2] & 0x3f];
		pIn += 3;
	}

	if (i < uInLen)
	{
		unsigned char a = pIn[0];
		unsigned char b = ((i + 1) < uInLen) ? pIn[1] : 0;
		unsigned char c = 0;

		strOut += g_pCodes[a >> 2];
		strOut += g_pCodes[((a & 3) << 4) + (b >> 4)];
		strOut += ((i + 1) < uInLen) ? g_pCodes[((b & 0xf) << 2) + (c >> 6)] : '=';
		strOut += '=';
	}

	//*p = 0; // Append NULL byte
	//*uOutLen = p - pOut;
	return true;
}

bool CBase64::Decode(const CString& strIn, unsigned char *pOut, unsigned long *uOutLen)
{
	unsigned long t, x, y, z;
	unsigned char c;
	unsigned long g = 3;

	//ASSERT((pIn != NULL) && (uInLen != 0) && (pOut != NULL) && (uOutLen != NULL));

	for (x = y = z = t = 0; x < (unsigned int)strIn.GetLength(); x++)
	{
		c = g_pMap[strIn[x]];
		if (c == 255) continue;
		if (c == 254) { c = 0; g--; }

		t = (t << 6) | c;

		if (++y == 4)
		{
			if ((z + g) > *uOutLen) { return false; } // Buffer overflow
			pOut[z++] = (unsigned char)((t >> 16) & 255);
			if (g > 1) pOut[z++] = (unsigned char)((t >> 8) & 255);
			if (g > 2) pOut[z++] = (unsigned char)(t & 255);
			y = t = 0;
		}
	}

	*uOutLen = z;
	return true;
}

CString CBase64::Encode(CString data)
{
	CString s;
	Encode((const unsigned char*)(data.GetBuffer()), (unsigned long)data.GetLength(), s);
	return s;
}

CString CBase64::Decode(CString data)
{
	CString s;
	unsigned char* bytBuf = NULL;
	unsigned long len = 0;
	char* pBuf = NULL;
	CString sRet;

	len = (data.GetLength() >> 2) * 3;
	bytBuf = new unsigned char[len];
	memset(bytBuf, 0, len);

	Decode(data, bytBuf, &len);

	pBuf = new char[len + 1];
	memset(pBuf, 0, len + 1);
	memcpy(pBuf, bytBuf, len);
	sRet = pBuf;

	delete[] bytBuf; bytBuf = NULL;
	delete[] pBuf;	pBuf = NULL;

	return sRet;
}

int CBase64::GetDecodeBufferLength(CString data)
{
	int len = 0;
	len = (data.GetLength() >> 2) * 3 - 2;
	return len;
}

CString CBase64::GBToUTF8(const char* str)
{
	CString result;
	WCHAR *strSrc;
	TCHAR *szRes;

	//获得临时变量的大小
	int i = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
	strSrc = new WCHAR[i + 1];
	MultiByteToWideChar(CP_ACP, 0, str, -1, strSrc, i);

	//获得临时变量的大小
	i = WideCharToMultiByte(CP_UTF8, 0, strSrc, -1, NULL, 0, NULL, NULL);
	szRes = new TCHAR[i + 1];
	int j = WideCharToMultiByte(CP_UTF8, 0, strSrc, -1, (LPSTR)(LPCTSTR)szRes, i, NULL, NULL);

	result = (char*)szRes;
	delete[]strSrc;
	delete[]szRes;

	return result;
}

CString CBase64::UTF8ToGB(const char* str)
{
	CString result;
	WCHAR *strSrc;
	TCHAR *szRes;

	//获得临时变量的大小
	int i = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
	strSrc = new WCHAR[i + 1];
	MultiByteToWideChar(CP_UTF8, 0, str, -1, strSrc, i);

	//获得临时变量的大小
	i = WideCharToMultiByte(CP_ACP, 0, strSrc, -1, NULL, 0, NULL, NULL);
	szRes = new TCHAR[i + 1];
	WideCharToMultiByte(CP_ACP, 0, strSrc, -1, (LPSTR)(LPCTSTR)szRes, i, NULL, NULL);

	result = (char*)szRes;
	delete[]strSrc;
	delete[]szRes;

	return result;
}

