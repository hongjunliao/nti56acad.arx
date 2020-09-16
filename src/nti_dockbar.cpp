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
#include "nti_dockbar.h" //nti_dockbar
#include "nti_imgui.h"		//nti_imgui_create

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
	int r = nti_dockbase::OnCreate(lpCreateStruct);

	HWND hwnd = GetSafeHwnd();
	nti_imgui_create(hwnd);

	SetTimer(1, 16, NULL);
	return r;
}

void nti_dockbar::OnTimer(UINT_PTR nIDEvent)
{
	nti_imgui_paint();
}