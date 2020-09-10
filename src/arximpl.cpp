#include "StdAfx.h"
#include "rxregsvc.h"
//#include "AcExtensionModule.h"
#include "aced.h"
#include "dbsymtb.h"
#include "adslib.h"
#include "dbents.h"
#include "tchar.h"

//
#if defined(_DEBUG) && !defined(AC_FULL_DEBUG)
#error _DEBUG should not be defined except in internal Adesk debug builds
#endif

// Win32 message handler
static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

void nti56acad_cmd_load()
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

int sdl_main(int argc, char ** argv);
int win32_main(int argc, char *argv[]);
int win32_docking(int, char**);

void nti56acad_cmd_test()
{
	extern int is_chld;

	is_chld = 1;
	win32_docking(0, 0);
}

void initApp()
{
	//acedRegCmds->addCommand(_T("ASDK_DWG_COMMANDS"),
	//	_T("asdk_nti56load"), _T("nti56load"), ACRX_CMD_MODAL, nti56acad_cmd_load);
	acedRegCmds->addCommand(_T("ASDK_DWG_COMMANDS"),
		_T("asdk_nti56acad"), _T("nti56acad"), ACRX_CMD_MODAL, nti56acad_cmd_test);
}

void unloadApp()
{
	acedRegCmds->removeGroup(_T("ASDK_DWG_COMMANDS"));
}

extern "C" AcRx::AppRetCode acrxEntryPoint(AcRx::AppMsgCode msg, void* appId)
{
	switch (msg) {
	case AcRx::kInitAppMsg:
		acrxDynamicLinker->unlockApplication(appId);
		acrxDynamicLinker->registerAppMDIAware(appId);
		initApp();
		break;
	case AcRx::kUnloadAppMsg:
		unloadApp();
	}
	return AcRx::kRetOK;
}
