/*!
* This file is PART of nti56acad project
* @author hongjun.liao <docici@126.com>, @date 2020/09/11
*
* arx main
* */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "StdAfx.h"

#include "rxregsvc.h"
//#include "AcExtensionModule.h"
#include "aced.h"
#include "dbsymtb.h"
#include "adslib.h"
#include "dbents.h"
#include "tchar.h"
#include "sds/win32_sds.h"

#include "nti_imgui.h" /*nti_wnddata*/
#include "nti_cmd.h"
#include "nti_reactor.h" /**/
#include "nti_cmn.h"	//nti_wnddata

/////////////////////////////////////////////////////////////////////////////////////
static nti_wnddata g_wnddataobj = { 0 };

HWND g_hwnd = 0;
int is_chld = 1;
nti_wnddata * g_wnddata = &g_wnddataobj;
// per app
CDocReactor* gpDocReactor = NULL;
CEdReactor* gpEdReactor = NULL;
// per doc
CDbModReactor *gpDbReactor = NULL;
/////////////////////////////////////////////////////////////////////////////
// Define the sole extension module object.
AC_IMPLEMENT_EXTENSION_MODULE(modelessDll);

/////////////////////////////////////////////////////////////////////////////
//
// Rx interface
//
/////////////////////////////////////////////////////////////////////////////

void initApp()
{
	g_wnddata->reactor.block_list = listCreate();

	gpDocReactor = new CDocReactor();
	acDocManager->addReactor(gpDocReactor);

	gpEdReactor = new CEdReactor();
	acedEditor->addReactor(gpEdReactor);

	acedRegCmds->addCommand(_T("ASDK_NTI56ACAD"),
		_T("NTI56ACAD"), _T("NTI56ACAD"), ACRX_CMD_MODAL, nti56acad);

	//acedRegCmds->addCommand(_T("ASDK_DWG_COMMANDS"),
	//	_T("nti56acad_win32"), _T("nti56acad_win32"), ACRX_CMD_MODAL, nti56acad_win32);
	//acedRegCmds->addCommand(_T("ASDK_DWG_COMMANDS"),
	//	_T("nti56acad_imgui"), _T("nti56acad_imgui"), ACRX_CMD_MODAL, nti56acad_imgui);
	//acedRegCmds->addCommand(_T("ASDK_DWG_COMMANDS"),
	//	_T("nti56acad_imgui2"), _T("nti56acad_imgui2"), ACRX_CMD_MODAL, nti56acad_imgui2);
	//acedRegCmds->addCommand(_T("ASDK_DWG_COMMANDS"),
	//	_T("nti56acad_dockctrlbar"), _T("nti56acad_dockctrlbar"), ACRX_CMD_MODAL, nti56acad_dockctrlbar);
	//acedRegCmds->addCommand(_T("ASDK_DWG_COMMANDS"),
	//	_T("nti56acad_dockctrlbar2"), _T("nti56acad_dockctrlbar2"), ACRX_CMD_MODAL, nti56acad_dockctrlbar2);
}

void unloadApp()
{
	if (gpEdReactor)
	{
		acedEditor->removeReactor(gpEdReactor);
		delete gpEdReactor;
		gpEdReactor = NULL;
	}
	if (gpDocReactor)
	{
		acDocManager->removeReactor(gpDocReactor);
		delete gpDocReactor;
	}

	acedRegCmds->removeGroup(_T("ASDK_NTI56ACAD"));

	listRelease(g_wnddata->reactor.block_list);
}
/////////////////////////////////////////////////////////////////////////////
//
// Entry points
//
/////////////////////////////////////////////////////////////////////////////

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
		modelessDll.AttachInstance(hInstance);
	else if (dwReason == DLL_PROCESS_DETACH)
		modelessDll.DetachInstance();

	return 1;   // ok
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
