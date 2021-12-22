/*!
* This file is PART of nti56acad.arx project
* @author hongjun.liao <docici@126.com>, @date 2020/09/15
*
* arx
* */
//////////////////////////////////////////////////////////////////////////////
//
#include "stdafx.h"
#include "resource.h"

#if defined(_DEBUG) && !defined(AC_FULL_DEBUG)
#error _DEBUG should not be defined except in internal Adesk debug builds
#endif

#include "nti_EdUiContext.h"
#include "ARXDBG/Inc/ArxDbgEdUiContext.h"
#include "AcExtensionModule.h"
#include "nti_cmn.h"

AC_DECLARE_EXTENSION_MODULE(ntiacadDll);
//ACRX_CONS_DEFINE_MEMBERS(nti_EdUiContextApp, AcEdUiContext, 1)

/**************************************************************************
**
**  nti_EdUiContextApp::nti_EdUiContextApp
**
**  **jma
**
*************************************/

nti_EdUiContextApp::nti_EdUiContextApp()
{
	CAcModuleResourceOverride(ntiacadDll.ModuleResourceInstance());

    m_cMenu = new CMenu;
    BOOL res = m_cMenu->LoadMenu(NTI_MNU_APP);
    VERIFY(res);
    if (!res) {
        delete m_cMenu;
        m_cMenu = NULL;
    }
}

/**************************************************************************
**
**  nti_EdUiContextApp::nti_EdUiContextApp
**
**  **jma
**
*************************************/

nti_EdUiContextApp::~nti_EdUiContextApp()
{
    if (m_cMenu) 
        delete m_cMenu;
}

/**************************************************************************
**
**  nti_EdUiContextApp::getMenuContext
**
**  **jma
**
*************************************/

void* 
nti_EdUiContextApp::getMenuContext(const AcRxClass* classtype, const AcDbObjectIdArray& selectedObjs)
{
    m_hMenu = m_cMenu->GetSubMenu(0)->GetSafeHmenu();
    return &m_hMenu;
}

/**************************************************************************
**
**  nti_EdUiContextApp::onCommand
**
**  **jma
**
*************************************/

void  
nti_EdUiContextApp::onCommand(Adesk::UInt32 cmdId)
{
	LPCTSTR cmdStr = NULL;
	CString str;

	extern nti_wnddata * g_wnddata;
	if (cmdId == ID_NTI_BLOCKS) {
		cmdStr = _T("nti_cmd_blocks");
		g_wnddata->reactor.base.is_open = 1;
	}
	else if (cmdId == ID_NTI_ABOUT) { cmdStr = _T("nti_cmd_about"); }
	else if (cmdId == ID_NULL_ARX_CMD) { cmdStr = _T("nti_cmd_null"); }
	else if (cmdId == ID_NTI_TEST_DX9_MAIN) { cmdStr = _T("nti_cmd_dx9_main"); }
	else if (cmdId == ID_NTIMENU_DOCKBAR) { cmdStr = _T("nti_cmd_dockbar"); }
	if (cmdStr != NULL) {
		str.Format(_T("%s\n"), cmdStr);
	    acDocManager->sendStringToExecute(acDocManager->curDocument(), str, false, false);
	}
}

