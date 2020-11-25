/*!
* This file is PART of nti56acad project
* @author hongjun.liao <docici@126.com>, @date 2020/09/11
*
* acad command entrypoint
* */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "stdafx.h"
#include "nti_imgui.h"		//nti_imgui_create
#include "nti_dockbar.h" //nti_dockbar
#include "nti_cmd.h"
#include "nti_arx.h"		//
#include "AdskDMgr.h"
#include <cinttypes>
#include "nti_reactor.h"		//
#include "nti_render.h"		//
#include "nti_cmn.h"	//nti_wnddata
#include "nti_test.h"
#include "nti_blocksbar.h"
#include "resource.h"

// per app
extern CDocReactor* gpDocReactor;
extern CEdReactor* gpEdReactor;
// per doc
extern CDbModReactor *gpDbReactor;

extern HWND g_hwnd;
extern int is_chld;
extern nti_wnddata * g_wnddata;
/////////////////////////////////////////////////////////////////////////////////////

void nti_cmd_null()
{
	//empty
	acutPrintf(_T("\nnti_cmd_null\n"));
}

void nti_cmd_about()
{
	nti_imgui_wnddata obj = { ("NTI About"), true }, *wnddata = &obj;
	nti_imgui_modal(sample_render, wnddata);
}

void nti_cmd_dx9_main()
{
	win32_dx9_main(0, 0);
}

void nti_cmd_blocks3()
{
	int rc;
	// Redirect the resource override   
	CAcModuleResourceOverride res;
	//// Create the dock ctrl bar   
	static nti_blocksbar * blocksbar = 0;
	if (!blocksbar) {
		blocksbar = new nti_blocksbar;
		// nti dockbar
		blocksbar->Create(acedGetAcadFrame(), _T("nti_dockbar"), 32141);

		//rc = nti_imgui_create(blocksbar->GetSafeHwnd(), acedGetAcadFrame()->GetSafeHwnd(), 0);
		//assert(rc == 0);
		//rc = nti_imgui_add(std::bind(&nti_blocksbar::render, blocksbar), blocksbar->GetSafeHwnd());

		blocksbar->EnableDocking(CBRS_ALIGN_ANY);
		blocksbar->SetWindowText(_T("nti56acad"));
		acedGetAcadFrame()->DockControlBar(blocksbar);
	}
	// Now display it   
	acedGetAcadFrame()->FloatControlBar(blocksbar, CPoint(), CBRS_ALIGN_LEFT);    // FloatControBar(...)   
	acedGetAcadFrame()->ShowControlBar(blocksbar, TRUE, FALSE);
}

void nti_cmd_blocks2()
{
	int rc;
	// Redirect the resource override   
	CAcModuleResourceOverride res;
	nti_blocksbar * blocksbar = (nti_blocksbar *)nti_imgui_()->user;
	if (blocksbar) {
		blocksbar->show();
	}
}

void nti_cmd_blocks() 
{ 
	return nti_cmd_blocks2();
	
	int rc;
	// Redirect the resource override   
	CAcModuleResourceOverride res;
	//// Create the dock ctrl bar   
	static nti_blocksbar * blocksbar = 0;
	if (!blocksbar) {
		blocksbar = new nti_blocksbar;
		// nti dockbar
		blocksbar->Create(acedGetAcadFrame(), _T("nti_dockbar"), 32141);

		rc = nti_imgui_add(std::bind(&nti_blocksbar::render, blocksbar), blocksbar->GetSafeHwnd());

		blocksbar->EnableDocking(CBRS_ALIGN_ANY);
		blocksbar->SetWindowText(_T("nti56acad"));
		acedGetAcadFrame()->DockControlBar(blocksbar);
	}
	// Now display it   
	acedGetAcadFrame()->FloatControlBar(blocksbar, CPoint(), CBRS_ALIGN_LEFT);    // FloatControBar(...)   
	acedGetAcadFrame()->ShowControlBar(blocksbar, TRUE, FALSE);
}

void nti_cmd_dockbar()
{
	int rc;
	// Redirect the resource override   
	CAcModuleResourceOverride res;
	//// Create the dock ctrl bar   
	static nti_dockbar * dockBar = 0;
	if (!dockBar) {
		dockBar = new nti_dockbar;
		dockBar->wnddata = g_wnddata;
		dockBar->Create(acedGetAcadFrame(), _T("nti_dockbar"), 32141);

		dockBar->EnableDocking(CBRS_ALIGN_ANY);
		dockBar->SetWindowText(_T("nti56acad"));
		acedGetAcadFrame()->DockControlBar(dockBar);
	}
	// Now display it   
	//acedGetAcadFrame()->FloatControlBar(dockBar, CPoint(750, 150), CBRS_ALIGN_RIGHT);    // FloatControBar(...)   
	acedGetAcadFrame()->ShowControlBar(dockBar, TRUE, FALSE);
}

// Win32 message handler
static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	nti_imgui_WndProc(hWnd, msg, wParam, lParam);

	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

void nti56acad_win32()
{
	int rc;
	if (!g_hwnd) {
		// Create application window
		int wstyle = (is_chld ? (WS_OVERLAPPEDWINDOW | WS_CHILD | WS_VISIBLE | WS_POPUP)
			: (WS_OVERLAPPEDWINDOW | WS_VISIBLE));
		static WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("nti56acad"), NULL };
		::RegisterClassEx(&wc);
		g_hwnd = ::CreateWindowEx(WS_EX_TOPMOST, wc.lpszClassName, _T("nti56acad"), wstyle, 100, 100, 400, 200, 0, NULL, wc.hInstance, NULL);

		rc = nti_imgui_create(g_hwnd, 0);

		rc = nti_imgui_add(nti_tabswnd_reactor, (nti_imgui_wnddata *)&g_wnddata->reactor);
		assert(rc == 0);

		nti_imgui_add(nti_tabswnd_render, 0);
		nti_imgui_add(nti_tabswnd_simple, 0);
		nti_imgui_add(nti_tabswnd_another, 0);

		SetTimer(g_hwnd, 1, 16, NULL);
	}
	// Show the window
	::ShowWindow(g_hwnd, SW_SHOWDEFAULT);
	::UpdateWindow(g_hwnd);
}