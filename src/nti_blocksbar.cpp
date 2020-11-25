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
#include "resource.h"
#include <assert.h>
#include "nti_blocksbar.h" //nti_blocksbar
#include "nti_imgui.h"		//nti_imgui_create
#include "nti_render.h"		//
#include "nti_cmn.h"	//nti_wnddata
#include "imgui_sds.h"

/////////////////////////////////////////////////////////////////////////////////////

/**
*/
static void * list_dup(void *ptr)
{
	return strdup((char *)ptr);
}

static void list_free(void *ptr)
{
	free(ptr);
}

static int list_match(void *ptr, void *key)
{
	return strncmp((char *)ptr, (char *)key, strlen((char *)ptr)) == 0;
}

/////////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(nti_blocksbar, nti_dockbase)
	//{{AFX_MSG_MAP(nti_blocksbar)
	//ON_WM_CREATE()
	//ON_WM_PAINT()   
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP   
END_MESSAGE_MAP()

nti_blocksbar::nti_blocksbar()
{
	m_name = sdsempty();
	m_open = true;

	block_list = listCreate();
	listSetDupMethod(block_list, list_dup);
	listSetFreeMethod(block_list, list_free);
	listSetMatchMethod(block_list, list_match);
	curr_block = 0;
}

nti_blocksbar::~nti_blocksbar()
{
	sdsfree(m_name);
	listRelease(block_list);
}

void nti_blocksbar::show()
{
	m_open = true;
#ifndef NTI56_WITHOUT_ARX
	acedGetAcadFrame()->ShowControlBar(this, TRUE, FALSE);
#endif
}

void nti_blocksbar::render()
{
	if(!m_open)
		return;

	HWND hwnd = GetSafeHwnd();

	TITLEBARINFO tblarobj = { sizeof(TITLEBARINFO) }, *tbar = &tblarobj;
	::GetTitleBarInfo(hwnd, tbar);
	RECT rectobj = { 0 }, *rect = &rectobj;
	::GetWindowRect(hwnd, rect);

#ifndef NTI56_WITHOUT_ARX
	ImVec2 size(rect->right - rect->left, rect->bottom - rect->top - (this->IsFloating() ? 0 : 20));
	ImVec2 pos(rect->left, rect->top + (this->IsFloating()? 0 : 20));
#else
	ImVec2 size(rect->right - rect->left, rect->bottom - rect->top - (IsDocked() ? 20 : 0));
	ImVec2 pos(rect->left, rect->top + (IsDocked() ? 20 : 0));
#endif

	ImGui::SetNextWindowPos(pos);
	ImGui::SetNextWindowSize(size);

	ImGui::Begin("nti_blocksbar", 0
			, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

	if (ImGui::TreeNode("blocks")) {
		// ImGuiComboFlags_PopupAlignLeft
		if (!curr_block && listFirst(block_list))
			curr_block = listFirst(block_list);
		const char* combo_label = (curr_block ? (char const *)listNodeValue(curr_block) : "");

		if (ImGui::BeginCombo("blocks", combo_label, ImGuiComboFlags_HeightLarge)) {

			listIter * iter = listGetIterator(block_list, 0);
			listNode * node;
			for (node = listNext(iter); node; ) {

				char * bname = (char *)listNodeValue(node);
				assert(bname);

				const bool is_selected = (curr_block == node);
				if (ImGui::Selectable(bname, is_selected))
					curr_block = node;
				if (is_selected)
					ImGui::SetItemDefaultFocus();

				node = listNext(iter);
			}
			listReleaseIterator(iter);

			ImGui::EndCombo();
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("test")) {
		ImGui::InputText("name", &m_name);

		static bool show_demo_window = 0;
		if (show_demo_window) {
			ImGui::BeginChild("imgui demo window");
			ImGui::ShowDemoWindow(&show_demo_window);
			ImGui::EndChild();
		}
		ImGui::Checkbox("imgui demo window", &show_demo_window);

		//test OK
		if (ImGui::Button("about...")) {
			PostMessage(WM_COMMAND, ID_APP_ABOUT);
		}
		if (ImGui::Button("imgui about...")) {
			PostMessage(WM_COMMAND, ID_NTI_ABOUT);
		}
		ImGui::TreePop();
	}

	ImGui::End();
}

#ifndef NTI56_WITHOUT_ARX
bool nti_blocksbar::OnClosing()
{
	m_open = false;
	return nti_dockbase::OnClosing();
}
#endif

//BOOL nti_blocksbar::Create(CWnd* pParent, LPCTSTR lpszTitle)
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

//int nti_blocksbar::OnCreate(LPCREATESTRUCT lpCreateStruct)
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

//void nti_blocksbar::OnTimer(UINT_PTR nIDEvent)
//{
//	//nti_imgui_render();
//}

//void nti_blocksbar::OnSize(UINT nType, int cx, int cy)
//{
//	nti_dockbase::OnSize(nType, cx, cy);
//}
//

void nti_blocksbar::OnClose()
{
	m_open = false;
	return nti_dockbase::OnClose();
}

#ifdef NTI56_WITHOUT_ARX
LRESULT nti_blocksbar::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	nti_imgui_msghdl(GetSafeHwnd(), message, wParam, lParam);
	return nti_dockbase::WindowProc(message, wParam, lParam);
}

#endif

