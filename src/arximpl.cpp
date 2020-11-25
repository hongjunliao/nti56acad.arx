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
#include "nti_EdUiContext.h"
#include "nti_render.h"
#include "nti_blocksbar.h"
#include "resource.h"

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
AC_IMPLEMENT_EXTENSION_MODULE(ntiacadDll);

/////////////////////////////////////////////////////////////////////////////
//
// Rx interface
//
/////////////////////////////////////////////////////////////////////////////

void initApp()
{
	int rc;
	//g_hwnd = acedGetAcadFrame()->GetMessageBar()->GetSafeHwnd();
	rc = nti_wnddata_init(g_wnddata);

	/////////////////////////////////////////////////////////////////////////////
	// Toolbar

	//static CToolBar toolbar;
	//rc = toolbar.CreateEx(acedGetAcadFrame());
	//if(rc) rc = toolbar.LoadToolBar(IDR_TOOLBAR_NTI);
	//if (!rc) 
	//	return;
	//acedGetAcadFrame()->DockControlBar(&toolbar);
	/////////////////////////////////////////////////////////////////////////////
	
	CAcUiDockControlBar * dock2 = new CAcUiDockControlBar;
	rc = dock2->Create(acedGetAcadFrame(), _T("dock2"), 34525);
	dock2->EnableDocking(CBRS_ALIGN_ANY);
	assert(rc);
	acedGetAcadFrame()->DockControlBar(dock2);

	/////////////////////////////////////////////////////////////////////////////
	// dockbar
	//CDialogBar * base = new CDialogBar;
	//CPoint pt(100, 100);
	//rc = base->Create(acedGetAcadFrame(), (UINT)IDD_DIALOG_NTI, 0, 0);
	//base->EnableDocking(CBRS_ALIGN_ANY);
	//acedGetAcadFrame()->FloatControlBar(base, pt, CBRS_ALIGN_LEFT);    // FloatControBar(...)   

	/////////////////////////////////////////////////////////////////////////////
	nti_blocksbar * blocksbar = 0;
	blocksbar = new nti_blocksbar;
	// nti dockbar
	rc = blocksbar->Create(acedGetAcadFrame(), _T("nti_dockbar"), 32141);
	assert(rc);
	blocksbar->EnableDocking(CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT);

	RECT R, *rect = &R;
	::GetWindowRect(acedGetAcadFrame()->GetSafeHwnd(), rect);
	acedGetAcadFrame()->FloatControlBar(blocksbar, CPoint(rect->left, rect->top + 200), CBRS_ALIGN_LEFT);
	acedGetAcadFrame()->DockControlBar(blocksbar);
	// init imgui
	rc = nti_imgui_create(0, acedGetAcadFrame()->GetSafeHwnd(), 0);
	assert(rc == 0);
	nti_imgui_()->user = blocksbar;

	rc = nti_imgui_add(std::bind(&nti_blocksbar::render, blocksbar), blocksbar->GetSafeHwnd());
	
	/////////////////////////////////////////////////////////////////////////////

	//nti_imgui_create(0, acedGetAcadFrame()->GetSafeHwnd());
	//rc = nti_imgui_add(nti_tabswnd_render, (nti_imgui_wnddata *)&g_wnddata->reactor);
	//assert(rc == 0);
	//nti_imgui_add_render(nti_tabswnd_simple, 0);
	//nti_imgui_add_render(nti_tabswnd_another, 0);

	gpDocReactor = new CDocReactor();
	acDocManager->addReactor(gpDocReactor);

	gpEdReactor = new CEdReactor();
	acedEditor->addReactor(gpEdReactor);

	acedRegCmds->addCommand(_T("ASDK_NTI56ACAD"), _T("nti_cmd_null"), _T("nti_cmd_null"), ACRX_CMD_MODAL, nti_cmd_null);
	acedRegCmds->addCommand(_T("ASDK_NTI56ACAD"), _T("nti_cmd_about"), _T("nti_cmd_about"), ACRX_CMD_MODAL, nti_cmd_about);
	acedRegCmds->addCommand(_T("ASDK_NTI56ACAD"), _T("nti_cmd_dx9_main"), _T("nti_cmd_dx9_main"), ACRX_CMD_MODAL, nti_cmd_dx9_main);
	acedRegCmds->addCommand(_T("ASDK_NTI56ACAD"), _T("nti_cmd_blocks"), _T("nti_cmd_blocks"), ACRX_CMD_MODAL, nti_cmd_blocks);
	acedRegCmds->addCommand(_T("ASDK_NTI56ACAD"), _T("nti_cmd_dockbar"), _T("nti_cmd_dockbar"), ACRX_CMD_MODAL, nti_cmd_dockbar);

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

	static nti_EdUiContextApp * m_uiContext = new nti_EdUiContextApp;
	rc = acedAddDefaultContextMenu(m_uiContext, m_uiContext, _T("ntiacad"));

	if ((m_uiContext->isValid() == false) ||
		(rc == false)) {
		ASSERT(0);

		delete m_uiContext;
		m_uiContext = NULL;
	}
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

	nti_wnddata_uninit(g_wnddata);
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
		ntiacadDll.AttachInstance(hInstance);
	else if (dwReason == DLL_PROCESS_DETACH)
		ntiacadDll.DetachInstance();

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
