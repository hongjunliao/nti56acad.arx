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

#ifdef NTI56_ARX
#include <functional> //std::function
/////////////////////////////////////////////////////////////////////////////////////
int nti_foreach_symtbl(
	std::function<int(AcDbDatabase*& db, AcDbSymbolTable*& tbl)> on_get
	, std::function<int(AcDbSymbolTableRecord*& tblRec)> on_each);
#endif //#ifdef NTI56_ARX
/////////////////////////////////////////////////////////////////////////////////////////
#ifndef NDEBUG
int test_nti_arx_main(int argc, char ** argv);
#endif //NDEBUG

#endif //NTI_ARX_H