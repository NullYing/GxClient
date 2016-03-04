#ifndef ___BASE64_H___
#define ___BASE64_H___

using namespace std;

class CBase64
{
public:
	static bool Encode(const unsigned char *pIn, unsigned long uInLen, CString& strOut);
	static bool Encode(const unsigned char *pIn, unsigned long uInLen, unsigned char *pOut, unsigned long *uOutLen);
	static bool Decode(const CString& strIn, unsigned char *pOut, unsigned long *uOutLen);

	static CString Encode(CString data);
	static CString Decode(CString data);
	static int GetDecodeBufferLength(CString data);

	static CString GBToUTF8(const char*); //将gb2312编码转为UTF-8字符
	static CString UTF8ToGB(const char*); //将UTF-8编码转为gb2312字符
};

#endif // ___BASE64_H___
