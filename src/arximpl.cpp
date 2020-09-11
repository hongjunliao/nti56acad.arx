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

void nti56acad_dockctrlbar();
void nti56acad_win32();
void nti56acad_imgui();
void nti56acad_imgui2();

void initApp()
{
	acedRegCmds->addCommand(_T("ASDK_DWG_COMMANDS"),
	_T("nti56acad_imgui"), _T("nti56acad_imgui"), ACRX_CMD_MODAL, nti56acad_imgui);
	acedRegCmds->addCommand(_T("ASDK_DWG_COMMANDS"),
		_T("nti56acad_imgui2"), _T("nti56acad_imgui2"), ACRX_CMD_MODAL, nti56acad_imgui2);
	acedRegCmds->addCommand(_T("ASDK_DWG_COMMANDS"),
		_T("nti56acad_win32"), _T("nti56acad_win32"), ACRX_CMD_MODAL, nti56acad_win32);
	acedRegCmds->addCommand(_T("ASDK_DWG_COMMANDS"),
		_T("nti56acad_dockctrlbar"), _T("nti56acad_dockctrlbar"), ACRX_CMD_MODAL, nti56acad_dockctrlbar);
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
