/*!
 * This file is PART of nti56acad.arx project
 * @author hongjun.liao <docici@126.com>, @date 2020/09/16
 *
 * string util
 * */

#ifndef NTI_STR_H
#define NTI_STR_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <string>	//std::string

/////////////////////////////////////////////////////////////////////////////////////

//std::string gb2utf8(char const * s);
char *    nti_wcstr2a(const wchar_t * str);
wchar_t * nti_a2wcstr(const char *    str);
#define nti_wstr2a(str) std::string(nti_wcstr2a(str.c_str()));
#define nti_a2wstr(str) std::wstring(nti_a2wcstr(str.c_str()))

/////////////////////////////////////////////////////////////////////////////////////////
#ifndef NDEBUG
int test_nti_str_main(int argc, char ** argv);
#endif //NDEBUG

#endif //NTI_STR_H
