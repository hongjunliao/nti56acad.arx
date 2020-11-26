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
#include "AdAChar.h"
#include "AcString.h"
extern "C" {
#include "adlist.h"	//list
}

#ifdef __cplusplus
extern "C" {
#endif
/////////////////////////////////////////////////////////////////////////////////////

typedef struct nti_datalink nti_datalink;

int nti_read(ACHAR const * file);
int nti_insert_table();

#define CUR_DB() acdbHostApplicationServices()->workingDatabase()

/////////////////////////////////////////////////////////////////////////////////////////
struct nti_datalink {
	AcString name;
	AcString desc;
	AcString conn;
};

void createAndSetDataLink();
int nti_arx_update_datalinks(list * datalinks);

/////////////////////////////////////////////////////////////////////////////////////////
#ifndef NDEBUG
int test_nti_arx_main(int argc, char ** argv);
#endif //NDEBUG

#ifdef __cplusplus
}
#endif
#endif //NTI_ARX_H