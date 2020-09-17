/*!
 * This file is PART of nti56acad.arx project
 * @author hongjun.liao <docici@126.com>, @date 2020/09/16
 *
 * string util
 * */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "stdafx.h"
#include <string>	//std::string
#include "sds/win32_sds.h" //sds
#include "gbk-utf8/utf8.h"
#include "nti_str.h"
/////////////////////////////////////////////////////////////////////////////////////

std::string gb2utf8(char const * s)
{
	sds str = sdsMakeRoomFor(sdsempty(), strlen(s) * 2);
	gb_to_utf8(s, str, sdsavail(str));

	std::string ret(str);
	sdsfree(str);

	return ret;
}

#define U8(s) gb2utf8(s).c_str()
/////////////////////////////////////////////////////////////////////////////////////////

SStringW CvtA2W(const SStringA & str, unsigned int cp/*=CP_ACP*/, unsigned int cp2/*=0*/)
{
	UNREFERENCED_PARAMETER(cp2);
	wchar_t szBuf[1024];
	int nRet = MultiByteToWideChar(cp, 0, str.c_str(), str.length(), szBuf, 1024);
	if (nRet>0)
	{
		return SStringW(szBuf, nRet);
	}
	if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
	{
		int nRet = MultiByteToWideChar(cp, 0, str.c_str(), str.length(), NULL, 0);
		if (nRet>0)
		{
			wchar_t *pBuf = new wchar_t[nRet];
			MultiByteToWideChar(cp, 0, str.c_str(), str.length(), pBuf, nRet);
			SStringW strRet(pBuf, nRet);
			delete[]pBuf;
			return strRet;
		}
	}
	return L"";
}

SStringA CvtW2A(const SStringW & str, unsigned int cp/*=CP_ACP*/)
{
	char szBuf[1024];
	int nRet = WideCharToMultiByte(cp, 0, str.c_str(), str.length(), szBuf, 1024, NULL, NULL);
	if (nRet>0) return SStringA(szBuf, nRet);
	if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
	{
		int nRet = WideCharToMultiByte(cp, 0, str.c_str(), str.length(), NULL, 0, NULL, NULL);
		if (nRet>0)
		{
			char *pBuf = new char[nRet];
			WideCharToMultiByte(cp, 0, str.c_str(), str.length(), pBuf, nRet, NULL, NULL);
			SStringA strRet(pBuf, nRet);
			delete[]pBuf;
			return strRet;
		}
	}
	return "";
}

/////////////////////////////////////////////////////////////////////////////////////////
#ifndef NDEBUG
int test_nti_str_main(int argc, char ** argv)
{
	int rc = 0;

	SStringA s = CvtW2A(L"");

	return rc;
}
#endif //NDEBUG
