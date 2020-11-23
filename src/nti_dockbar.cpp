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
	//ON_WM_CREATE()
	//ON_WM_PAINT()   
	ON_WM_SIZE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP   
END_MESSAGE_MAP()

nti_dockbar::nti_dockbar()
{
}

nti_dockbar::~nti_dockbar()
{

}


//BOOL nti_dockbar::Create(CWnd* pParent, LPCTSTR lpszTitle)
//{
//	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, nti_imgui_WndProc, 0L, 0L
//		, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("ImGui Example"), NULL };
//	::RegisterClassEx(&wc);
//
//	RECT rect = { 0 };
//
//#ifndef NTI56_WITHOUT_ARX																
//	if (!nti_dockbase::Create(wc.lpszClassName, lpszTitle, WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN, rect, pParent, 2342)) {
//	//if (!nti_dockbase::Create(pParent, lpszTitle, 2342)){
//#else
//	if (CToolBar::Create(pParent)) {
//#endif
//		return (FALSE);
//	}
//	return (TRUE);
//}

//int nti_dockbar::OnCreate(LPCREATESTRUCT lpCreateStruct)
//{
//	int rc = nti_dockbase::OnCreate(lpCreateStruct);
//
//	HWND hwnd = GetSafeHwnd();
//
//	nti_imgui_create(hwnd, GetParent()->GetSafeHwnd());
//	rc = nti_imgui_add(nti_tabswnd_render, (nti_imgui_wnddata *)&wnddata->reactor);
//	assert(rc == 0);
//
//	//nti_imgui_add_render(nti_tabswnd_render, 0);
//
//	//nti_imgui_add_render(nti_tabswnd_simple, 0);
//	//nti_imgui_add_render(nti_tabswnd_another, 0);
//
//
////SetTimer(1, 16, NULL);
//	return rc;
//}

//void nti_dockbar::OnTimer(UINT_PTR nIDEvent)
//{
//	//nti_imgui_render();
//}

//void nti_dockbar::OnSize(UINT nType, int cx, int cy)
//{
//	CDockablePane::OnSize(nType, cx, cy);
//}
//
//LRESULT nti_dockbar::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
//{
//	nti_imgui_msghdl(GetSafeHwnd(), message, wParam, lParam);
//	return CDockablePane::WindowProc(message, wParam, lParam);
//}