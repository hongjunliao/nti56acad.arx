/*!
 * This file is PART of nti56acad project
 * @author hongjun.liao <docici@126.com>, @date 2020/11/13
 *
 * */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>

#include "nti_cmn.h" //nti_imgui_wnddata

/////////////////////////////////////////////////////////////////////////////////////

/**
*/
static void * list_dup(void *ptr)
{
	return strdup((char *)ptr);
}

static void list_free(void *ptr)
{
	free(ptr);
}

static int list_match(void *ptr, void *key)
{
	return strncmp((char *)ptr, (char *)key, strlen((char *)ptr)) == 0;
}

/////////////////////////////////////////////////////////////////////////////////////


int nti_wnddata_init(nti_wnddata * wnddata)
{
	if(!wnddata)
		return -1;
	strcpy(wnddata->reactor.base.title, "ntiacad");
	wnddata->reactor.block_list = listCreate();
	listSetDupMethod(wnddata->reactor.block_list, list_dup);
	listSetFreeMethod(wnddata->reactor.block_list, list_free);
	listSetMatchMethod(wnddata->reactor.block_list, list_match);

	return 0;
}

int nti_wnddata_uninit(nti_wnddata * wnddata)
{
	if(!wnddata)
		return -1;
	listRelease(wnddata->reactor.block_list);

	return 0;
}
/////////////////////////////////////////////////////////////////////////////////////

