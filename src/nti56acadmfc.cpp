
// nti56acadmfc.cpp : ??????��???????????
//

#include "stdafx.h"
#include <assert.h>
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "nti56acadmfc.h"
#include "MainFrm.h"
#include "nti_imgui.h" /*nti_wnddata*/
#include "nti_cmn.h" /*nti_wnddata*/
#include "nti_render.h" /*nti_wnddata*/

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static nti_wnddata g_wnddataobj = { 0 };
nti_wnddata * g_wnddata = &g_wnddataobj;

HWND g_hwnd = 0;
int is_chld = 0;

extern "C" {
	int test_libxlsxio_main(int argc, char ** argv);
}

// Cnti56acadmfcApp

BEGIN_MESSAGE_MAP(Cnti56acadmfcApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &Cnti56acadmfcApp::OnAppAbout)
END_MESSAGE_MAP()


// Cnti56acadmfcApp ????

Cnti56acadmfcApp::Cnti56acadmfcApp()
{
	// TODO: ????????��??? ID ??????�I?��??? ID ????????????????????
	//? CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("nti56acadmfc.AppID.NoVersion"));

	// TODO: ??????????????
	// ????????????????????? InitInstance ??
}

// ��?????? Cnti56acadmfcApp ????

Cnti56acadmfcApp theApp;


static VOID CALLBACK MyTimerProc(
	HWND hwnd,        // handle to window for timer messages 
	UINT message,     // WM_TIMER message 
	UINT idTimer,     // timer identifier 
	DWORD dwTime)     // current system time 
{
	nti_imgui_paint();
}

// Cnti56acadmfcApp ?????

BOOL Cnti56acadmfcApp::InitInstance()
{
	int i, rc;
#ifndef NDEBUG
	test_libxlsxio_main(0, 0);
#endif

	rc = nti_wnddata_init(g_wnddata);

	for(i = 0; i < 50; ++i){
		char * bname = nti_newn(128, char);
		sprintf(bname, u8"block��%d", i);
		listAddNodeTail(g_wnddata->reactor.block_list, bname);
	}

	CWinApp::InitInstance();

	EnableTaskbarInteraction(FALSE);

	// ??? RichEdit ?????? AfxInitRichEdit2()	
	// AfxInitRichEdit2();

	// ????????
	// ???��?????��??????????��
	// ??????????????��????????????
	// ???????????????????
	// ????????��???????????
	// TODO: ???????????????
	// ??????????????????
	SetRegistryKey(_T("??��???????????????��???"));


	// ???????????????????????????????
	// ????????????????��?????????????
	CMainFrame* pFrame = new CMainFrame;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;
	// ??????????????????
	pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
		NULL);

	// ��??????????????????????????????????��???
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();

	g_hwnd = pFrame->GetMessageBar()->GetSafeHwnd();
	rc = nti_imgui_create(g_hwnd);

	rc = nti_imgui_add_render(nti_tabswnd_render, (nti_imgui_wnddata *)&g_wnddata->reactor);
	assert(rc == 0);

	SetTimer(g_hwnd, 12323, 16, (TIMERPROC)MyTimerProc);

	//nti_imgui_add_render(nti_tabswnd_simple, 0);
	//nti_imgui_add_render(nti_tabswnd_another, 0);

	return TRUE;
}

int Cnti56acadmfcApp::ExitInstance()
{
	//TODO: ?????????????????????
	return CWinApp::ExitInstance();

	nti_wnddata_uninit(g_wnddata);
}

// Cnti56acadmfcApp ???????????


// ??????��????????????? CAboutDlg ?????

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ?????????
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ???

// ???
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// ???????��???????��???????
void Cnti56acadmfcApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// Cnti56acadmfcApp ???????????



