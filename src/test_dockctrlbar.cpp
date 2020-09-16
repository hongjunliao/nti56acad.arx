#include "stdafx.h"
#include "nti_cmd.h"
#include "nti_dockbar.h" //nti_dockbar
#include "nti_imgui.h"		//nti_imgui_create

extern HWND g_hwnd;
extern int is_chld;

int sdl_main(int argc, char ** argv);
int win32_main(int argc, char *argv[]);
int win32_docking(int, char**);
int win32_docking2(int, char**);

void nti56acad_dockctrlbar2()
{
	// Redirect the resource override   
	CAcModuleResourceOverride res;
	//// Create the dock ctrl bar   
	static nti_dockbar * dockBar = 0;
	if(!dockBar){
		dockBar = new nti_dockbar;
		dockBar->Create(acedGetAcadFrame(), _T("nti_dockbar"), 12345);
		dockBar->EnableDocking(CBRS_ALIGN_ANY);
		dockBar->SetWindowText(_T("nti56acad"));
	}
	// Now display it   
	acedGetAcadFrame()->FloatControlBar(dockBar, CPoint(750, 150), CBRS_ALIGN_RIGHT);    // FloatControBar(...)   
	acedGetAcadFrame()->ShowControlBar(dockBar, TRUE, FALSE);
}

void nti56acad_dockctrlbar()
{
	// Redirect the resource override   
	CAcModuleResourceOverride res;
	// Create the dock ctrl bar   
	static CAcUiDockControlBar * dockBar = 0;
	if(!dockBar){
		dockBar = new CAcUiDockControlBar;
		dockBar->Create(acedGetAcadFrame(), _T("DockControlBar"), 209349);
		dockBar->EnableDocking(CBRS_ALIGN_ANY);
		dockBar->SetWindowText(_T("Dock Tool"));
	}
	// Now display it   
	acedGetAcadFrame()->FloatControlBar(dockBar, CPoint(750, 150), CBRS_ALIGN_RIGHT);    // FloatControBar(...)   
	acedGetAcadFrame()->ShowControlBar(dockBar, TRUE, FALSE);
}

// Win32 message handler
static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

void nti56acad_win32()
{
	//CAcModuleResourceOverride resOverride;
	//acedGetAcadFrame

	// Create application window
	static WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("ImGui Example"), NULL };
	::RegisterClassEx(&wc);
	static HWND hwnd = ::CreateWindowEx(WS_EX_TOPMOST, wc.lpszClassName, _T("chld")
		, WS_OVERLAPPEDWINDOW | WS_CHILD | WS_VISIBLE | WS_POPUP
		, 100, 100, 200, 100, 0, NULL, wc.hInstance, NULL);
	// Show the window
	::ShowWindow(hwnd, SW_SHOWDEFAULT);
	::UpdateWindow(hwnd);
}

void nti56acad_imgui()
{
	is_chld = 1;
	win32_docking(0, 0);
}

void nti56acad_imgui2()
{
	is_chld = 1;
	win32_docking2(0, 0);
}