/*!
 * This file is PART of nti56acad project
 * @author hongjun.liao <docici@126.com>, @date 2020/09/11
 *
 * acad command entrypoint
 * */
#ifndef NTI_CMN_H
#define NTI_CMN_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */
#include "nti_imgui.h" //nti_imgui_wnddata
extern "C" {
#include "adlist.h"	//list
}

/////////////////////////////////////////////////////////////////////////////////////

#define nti_new(T) ((T *)calloc(1, sizeof(T)))
#define nti_newn(N,T) ((T *)calloc(N, sizeof(T)))
#define wcpy(buff, s) strncpy(buff, WA(s), _countof(buff));

/////////////////////////////////////////////////////////////////////////////////////
struct nti_wnddata_reactor {
	nti_imgui_wnddata base;
	list * block_list;
	listNode * curr_block;
	char what[128];
	char cls[128];
	char obj_id[128];
	char handle[128];
};

struct nti_wnddata {
	nti_wnddata_reactor reactor;
};

int nti_wnddata_init(nti_wnddata * wnddata);
int nti_wnddata_uninit(nti_wnddata * wnddata);
/////////////////////////////////////////////////////////////////////////////////////

#endif //NTI_CMN_H
