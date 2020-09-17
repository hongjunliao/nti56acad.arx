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
/////////////////////////////////////////////////////////////////////////////////////

#define nti_new(T) ((T *)calloc(1, sizeof(T)))

struct nti_wnddata_reactor {
	nti_imgui_wnddata base;
	char what[128];
	char cls[128];
	char obj_id[128];
	char handle[128];
};

struct nti_wnddata {
	nti_wnddata_reactor reactor;
};

#endif //NTI_CMN_H
