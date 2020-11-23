
// nti56acadmfc.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "nti56acadmfc.h"
#include "MainFrm.h"

#include "ChildFrm.h"
#include "nti56acadmfcDoc.h"
#include "nti56acadmfcView.h"

#include "nti_imgui.h" /*nti_wnddata*/
#include "nti_cmn.h" /*nti_wnddata*/
#include "nti_render.h" /*nti_wnddata*/
#include "imgui/misc/cpp/imgui_sds.h"

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
int test_nti_imgui_main(int, char**);


// Cnti56acadmfcApp

BEGIN_MESSAGE_MAP(Cnti56acadmfcApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &Cnti56acadmfcApp::OnAppAbout)
	// �����ļ��ı�׼�ĵ�����
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	// ��׼��ӡ��������
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
END_MESSAGE_MAP()


// Cnti56acadmfcApp ����

Cnti56acadmfcApp::Cnti56acadmfcApp()
{
	m_bHiColorIcons = TRUE;

	// ֧����������������
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// ���Ӧ�ó��������ù�����������ʱ֧��(/clr)�����ģ���: 
	//     1) �����д˸������ã�������������������֧�ֲ�������������
	//     2) ��������Ŀ�У������밴������˳���� System.Windows.Forms ������á�
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: ������Ӧ�ó��� ID �ַ����滻ΪΨһ�� ID �ַ�����������ַ�����ʽ
	//Ϊ CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("nti56acadmfc.AppID.NoVersion"));

	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}

// Ψһ��һ�� Cnti56acadmfcApp ����

Cnti56acadmfcApp theApp;


// Cnti56acadmfcApp ��ʼ��

BOOL Cnti56acadmfcApp::InitInstance()
{
	int i, rc;
#ifndef NDEBUG
	//test_nti_imgui_main(0, 0);
	//test_imgui_sds_main(0, 0);
	test_libxlsxio_main(0, 0);
#endif

	rc = nti_wnddata_init(g_wnddata);

	for (i = 0; i < 50; ++i) {
		char * bname = nti_newn(128, char);
		sprintf(bname, u8"block��%d", i);
		listAddNodeTail(g_wnddata->reactor.block_list, bname);
	}

	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()��  ���򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();


	// ��ʼ�� OLE ��
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// ʹ�� RichEdit �ؼ���Ҫ AfxInitRichEdit2()	
	// AfxInitRichEdit2();

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));
	LoadStdProfileSettings(4);  // ���ر�׼ INI �ļ�ѡ��(���� MRU)


	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// ע��Ӧ�ó�����ĵ�ģ�塣  �ĵ�ģ��
	// �������ĵ�����ܴ��ں���ͼ֮�������
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(IDR_nti56acadmfcTYPE,
		RUNTIME_CLASS(Cnti56acadmfcDoc),
		RUNTIME_CLASS(CChildFrame), // �Զ��� MDI �ӿ��
		RUNTIME_CLASS(Cnti56acadmfcView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	// ������ MDI ��ܴ���
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;


	// ������׼ shell ���DDE�����ļ�������������
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);



	// ��������������ָ�������  ���
	// �� /RegServer��/Register��/Unregserver �� /Unregister ����Ӧ�ó����򷵻� FALSE��
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// �������ѳ�ʼ���������ʾ����������и���
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	g_hwnd = pMainFrame->GetSafeHwnd();
	rc = nti_imgui_create(/*pMainFrame->m_dockbar.GetSafeHwnd()*/0, pMainFrame->GetSafeHwnd(), 1);

	rc = nti_imgui_add(nti_tabswnd_render, (nti_imgui_wnddata *)&g_wnddata->reactor);
	assert(rc == 0);

	//nti_imgui_add_render(nti_tabswnd_simple, 0);
	//nti_imgui_add_render(nti_tabswnd_another, 0);

	return TRUE;
}

int Cnti56acadmfcApp::ExitInstance()
{
	int rc;
	//TODO: �����������ӵĸ�����Դ
	AfxOleTerm(FALSE);

	rc = CWinAppEx::ExitInstance();

	nti_wnddata_uninit(g_wnddata);
	nti_imgui_destroy(g_hwnd);

	return rc;
}

// Cnti56acadmfcApp ��Ϣ�������


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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

// �������жԻ����Ӧ�ó�������
void Cnti56acadmfcApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// Cnti56acadmfcApp �Զ������/���淽��

void Cnti56acadmfcApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
	bNameValid = strName.LoadString(IDS_EXPLORER);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EXPLORER);
}

void Cnti56acadmfcApp::LoadCustomState()
{
}

void Cnti56acadmfcApp::SaveCustomState()
{
}

// Cnti56acadmfcApp ��Ϣ�������



