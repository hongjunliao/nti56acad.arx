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
typedef std::string SStringA;
typedef std::wstring SStringW;

std::string gb2utf8(char const * s);


SStringA CvtW2A(const SStringW & str, unsigned int cp = CP_ACP);
SStringW CvtA2W(const SStringA & str, unsigned int cp = CP_ACP, unsigned int cp2 = 0);

#define U8(s) gb2utf8(s).c_str()
#define WA(s) CvtW2A(s).c_str()
#define AW(s) CvtA2W(s).c_str()
/////////////////////////////////////////////////////////////////////////////////////////
#ifndef NDEBUG
int test_nti_str_main(int argc, char ** argv);
#endif //NDEBUG

#endif //NTI_STR_H