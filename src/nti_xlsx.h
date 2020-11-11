/*!
 * This file is PART of nti56acad.arx project
 * @author hongjun.liao <docici@126.com>, @date 2020/09/15
 *
 * arx
 * */

#ifndef NTI_XLSX_H
#define NTI_XLSX_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "sds/win32_sds.h" //sds

#ifdef __cplusplus
extern "C" {
#endif
/////////////////////////////////////////////////////////////////////////////////////

int nti_import_from_excel(char const * path, sds * errstr);
/////////////////////////////////////////////////////////////////////////////////////////
#ifndef NDEBUG
int test_nti_xlsx_main(int argc, char ** argv);
#endif //NDEBUG

#ifdef __cplusplus
}
#endif
#endif //NTI_XLSX_H