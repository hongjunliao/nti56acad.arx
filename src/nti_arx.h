/*!
 * This file is PART of nti56acad.arx project
 * @author hongjun.liao <docici@126.com>, @date 2020/09/15
 *
 * arx
 * */

#ifndef NTI_ARX_H
#define NTI_ARX_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <adachar.h>

#ifdef __cplusplus
extern "C" {
#endif
/////////////////////////////////////////////////////////////////////////////////////

int nti_read(ACHAR const * file);

#define CUR_DB acdbHostApplicationServices()->workingDatabase()
/////////////////////////////////////////////////////////////////////////////////////////
#ifndef NDEBUG
int test_nti_arx_main(int argc, char ** argv);
#endif //NDEBUG

#ifdef __cplusplus
}
#endif
#endif //NTI_ARX_H