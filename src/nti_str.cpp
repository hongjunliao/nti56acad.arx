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
#include "hp/sdsinc.h" //sds
//#include "gbk-utf8/utf8.h"
#include "nti_str.h"
/////////////////////////////////////////////////////////////////////////////////////

//std::string gb2utf8(char const * s)
//{
//	sds str = sdsMakeRoomFor(sdsempty(), strlen(s) * 2);
//	gb_to_utf8(s, str, sdsavail(str));
//
//	std::string ret(str);
//	sdsfree(str);
//
//	return ret;
//}

/////////////////////////////////////////////////////////////////////////////////////////
char *    nti_wcstr2a(const wchar_t * pwUNICODE)
{
	static char* s_pChar = NULL;

	delete[] s_pChar;
	s_pChar = NULL;

	if (pwUNICODE == NULL)
	{
		return NULL;
	}

	//get the length then and buffer
	int nLength = ::WideCharToMultiByte(CP_ACP, 0, pwUNICODE, -1, NULL, 0, NULL, NULL);
	s_pChar = new char[nLength*sizeof(char)];

	//do convert
	::WideCharToMultiByte(CP_ACP, 0, pwUNICODE, -1, s_pChar, nLength, NULL, NULL);

	return s_pChar;
}

wchar_t * nti_a2wcstr(const char *    ptrANSI)
{
	static wchar_t* s_pwChar = NULL;

	delete[] s_pwChar;
	s_pwChar = NULL;

	if (ptrANSI == NULL)
	{
		return NULL;
	}

	//get the length then and buffer
	int nLength = ::MultiByteToWideChar(CP_ACP, 0, ptrANSI, -1, NULL, 0);
	s_pwChar = new wchar_t[nLength*sizeof(wchar_t)];

	//do convert
	::MultiByteToWideChar(CP_ACP, 0, ptrANSI, -1, s_pwChar, nLength);

	return s_pwChar;
}

/////////////////////////////////////////////////////////////////////////////////////////
#ifndef NDEBUG
#include <assert.h> //assert

int test_nti_str_main(int argc, char ** argv)
{
	int rc = 0;

	{
		wchar_t ws[] = L"hello"; char * s = nti_wcstr2a(ws);
		assert(s && strlen(s) == 5 && strcmp(s, "hello") == 0);
	}
	{
		char s[] = "hello"; wchar_t * ws = nti_a2wcstr(s);
		assert(ws && wcslen(ws) == 5 && wcscmp(ws, L"hello") == 0);
	}
	return rc;
}
#endif //NDEBUG
