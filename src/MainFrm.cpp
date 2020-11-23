
// MainFrm.cpp : CMainFrame ??????
//

#include "stdafx.h"
#include "nti56acadmfc.h"

#include "MainFrm.h"
#include "resource.h"
#include "nti_test.h"
#include "nti_render.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern nti_wnddata * g_wnddata;
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_NTI_BLOCKS, &CMainFrame::on_ID_NTI_BLOCKS)
	ON_COMMAND(ID_32775, &CMainFrame::On32775)
	ON_COMMAND(ID_NTI_ABOUT, &CMainFrame::On32776)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // ????????
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame ????/????

CMainFrame::CMainFrame()
{
	// TODO: ??????????????????
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// ???????????????????????
	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("¦Ä????????????\n");
		return -1;
	}

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("¦Ä???????????\n");
		return -1;      // ¦Ä?????
	}

	//if (!m_dockbar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
	//	!m_dockbar.LoadToolBar(IDR_MAINFRAME))
	//{
	//	TRACE0("¦Ä???????????\n");
	//	return -1;      // ¦Ä?????
	//}

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("¦Ä?????????\n");
		return -1;      // ¦Ä?????
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: ????????????????????????????????
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	//m_dockbar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
	//DockControlBar(&m_dockbar);


	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: ??????????
	//  CREATESTRUCT cs ??????????????

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}

// CMainFrame ???

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame ???????????

void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
	// ?????????????????
	m_wndView.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// ?????????¦Ã????????
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// ?????????????
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}



void CMainFrame::on_ID_NTI_BLOCKS()
{
	g_wnddata->reactor.base.is_open = 1;
}


void CMainFrame::On32775()
{
	win32_dx9_main(0, 0);
}


void CMainFrame::On32776()
{
	nti_imgui_wnddata obj = { ("NTI About"), true }, *wnddata = &obj;
	nti_imgui_modal(sample_render, wnddata);
}
