/*!
* This file is PART of nti56acad project
* @author hongjun.liao <docici@126.com>, @date 2020/11/24
*
* blocks wnd
* */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "stdafx.h"
#include <assert.h>
#include "nti_datalinksdlg.h" //nti_datalinksdlg
#include "nti_imgui.h"		//nti_imgui_create
#include "nti_render.h"		//
#include "nti_cmn.h"	//nti_wnddata
#include "imgui_sds.h"
#include "resource.h"
/////////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(nti_datalinksdlg, CDialogEx)
END_MESSAGE_MAP()

nti_datalinksdlg::nti_datalinksdlg():CDialogEx(IDD_ABOUTBOX1)
{
	m_name = sdsempty();
	m_open = true;
}

nti_datalinksdlg::~nti_datalinksdlg()
{
	sdsfree(m_name);
}

BOOL nti_datalinksdlg::OnInitDialog()
{
	nti_imgui_add(std::bind(&nti_datalinksdlg::render, this), this->GetSafeHwnd());
	return TRUE;
}

void nti_datalinksdlg::PostNcDestroy()
{
	CDialogEx::PostNcDestroy();
	delete this;
}

void nti_datalinksdlg::render()
{
	HWND hwnd = GetSafeHwnd();
	RECT rectobj = { 0 }, *rect = &rectobj;
	::GetWindowRect(hwnd, rect);

	ImVec2 size(rect->right - rect->left, rect->bottom - rect->top - 20);
	ImVec2 pos(rect->left, rect->top + 20);
	ImGui::SetNextWindowPos(pos);
	ImGui::SetNextWindowSize(size);

	if (ImGui::Begin("nti_datalinksdlg", &m_open
			, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize)) {
		ImGui::InputText("name", &m_name);
		ImGui::End();
	}
}

//BOOL nti_datalinksdlg::Create(CWnd* pParent, LPCTSTR lpszTitle)
//{
//	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, nti_imgui_WndProc, 0L, 0L
//		, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("ImGui Example"), NULL };
//	::RegisterClassEx(&wc);
//
//	RECT rect = { 0 };
//
//#ifndef NTI56_WITHOUT_ARX																
//	if (!nti_dockbase::Create(wc.lpszClassName, lpszTitle, WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN, rect, pParent, 2342)) {
//	//if (!nti_dockbase::Create(pParent, lpszTitle, 2342)){
//#else
//	if (CToolBar::Create(pParent)) {
//#endif
//		return (FALSE);
//	}
//	return (TRUE);
//}

//int nti_datalinksdlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
//{
//	int rc = nti_dockbase::OnCreate(lpCreateStruct);
//
//	HWND hwnd = GetSafeHwnd();
//
//	nti_imgui_create(hwnd, GetParent()->GetSafeHwnd());
//	rc = nti_imgui_add(nti_tabswnd_render, (nti_imgui_wnddata *)&wnddata->reactor);
//	assert(rc == 0);
//
//	//nti_imgui_add_render(nti_tabswnd_render, 0);
//
//	//nti_imgui_add_render(nti_tabswnd_simple, 0);
//	//nti_imgui_add_render(nti_tabswnd_another, 0);
//
//
////SetTimer(1, 16, NULL);
//	return rc;
//}

//void nti_datalinksdlg::OnTimer(UINT_PTR nIDEvent)
//{
//	//nti_imgui_render();
//}

//void nti_datalinksdlg::OnSize(UINT nType, int cx, int cy)
//{
//	CDockablePane::OnSize(nType, cx, cy);
//}
//
//LRESULT nti_datalinksdlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
//{
//	nti_imgui_msghdl(GetSafeHwnd(), message, wParam, lParam);
//	return nti_datalinksdlg::WindowProc(message, wParam, lParam);
//}