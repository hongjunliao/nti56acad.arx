/*!
 * This file is PART of nti56acad project
 * @author hongjun.liao <docici@126.com>, @date 2020/09/11
 *
 * dock ctrl bar
 * */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "stdafx.h"
#include <assert.h>
#include "nti_dockbar.h" //nti_dockbar
#include "nti_imgui.h"		//nti_imgui_create
#include "nti_render.h"		//
#include "nti_cmn.h"	//nti_wnddata

/////////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(nti_dockbar, nti_dockbase)
	//{{AFX_MSG_MAP(nti_dockbar)
	ON_WM_CREATE()
	//ON_WM_PAINT()   
	ON_WM_TIMER()
	//}}AFX_MSG_MAP   
END_MESSAGE_MAP()

nti_dockbar::nti_dockbar()
{
}

nti_dockbar::~nti_dockbar()
{

}

int nti_dockbar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	int rc = nti_dockbase::OnCreate(lpCreateStruct);

	HWND hwnd = GetSafeHwnd();
	nti_imgui_create(hwnd);

	assert(wnddata);
	rc = nti_imgui_add_render(nti_tabswnd_reactor, (nti_imgui_wnddata *)&wnddata->reactor);
	assert(rc == 0);

	nti_imgui_add_render(nti_tabswnd_render, 0);
	nti_imgui_add_render(nti_tabswnd_simple, 0);
	nti_imgui_add_render(nti_tabswnd_another, 0);

	SetTimer(1, 16, NULL);
	return rc;
}

void nti_dockbar::OnTimer(UINT_PTR nIDEvent)
{
	nti_imgui_paint();
}