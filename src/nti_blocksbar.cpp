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
#include <tchar.h>
#include "resource.h"
#include <assert.h>
#include "nti_blocksbar.h" //nti_blocksbar
#include "nti_imgui.h"		//nti_imgui_create
#include "nti_render.h"		//
#include "nti_cmn.h"	//nti_wnddata
#include "nti_imgui.h"		//nti_imgui_create
#include "nti_arx.h"		//
#include "nti_xlsx.h"
#include "ImGuiFileDialog/ImGuiFileDialog.h"
#include "nti_str.h"
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
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_NTI_ADD_DATALINK, OnNtiAddDatalinks)
	//}}AFX_MSG_MAP   
END_MESSAGE_MAP()

nti_blocksbar::nti_blocksbar()
{
	m_name = sdsempty();
	m_open = true;

	m_block_list = listCreate();
	listSetDupMethod(m_block_list, list_dup);
	listSetFreeMethod(m_block_list, list_free);
	listSetMatchMethod(m_block_list, list_match);

	m_datalink_list = listCreate();
	listSetFreeMethod(m_block_list, list_free);

	m_curr_block = 0;
	m_curr_datalink = 0;
}

nti_blocksbar::~nti_blocksbar()
{
	sdsfree(m_name);
	listRelease(m_block_list);
	listRelease(m_datalink_list);
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

	if (ImGui::CollapsingHeader("Blocks")) {
		// ImGuiComboFlags_PopupAlignLeft
		if (!m_curr_block && listFirst(m_block_list))
			m_curr_block = listFirst(m_block_list);
		const char* combo_label = (m_curr_block ? (char const *)listNodeValue(m_curr_block) : "");

		if (ImGui::BeginCombo("blocks", combo_label, ImGuiComboFlags_HeightLarge)) {

			listIter * iter = listGetIterator(m_block_list, 0);
			listNode * node;
			for (node = listNext(iter); node; ) {

				char * bname = (char *)listNodeValue(node);
				assert(bname);

				const bool is_selected = (m_curr_block == node);
				if (ImGui::Selectable(bname, is_selected))
					m_curr_block = node;
				if (is_selected)
					ImGui::SetItemDefaultFocus();

				node = listNext(iter);
			}
			listReleaseIterator(iter);

			ImGui::EndCombo();
		}
	}
#ifndef NTI56_WITHOUT_ARX
	if (ImGui::CollapsingHeader("Datalinks")) {
		if (!m_curr_datalink)
			m_curr_datalink = listFirst(m_datalink_list);

		ImGui::Columns(3, "datalinks"); // 4-ways, with border
		ImGui::Separator();
		ImGui::Text("Name"); ImGui::NextColumn();
		ImGui::Text("File"); ImGui::NextColumn();
		ImGui::Text("Desc"); ImGui::NextColumn();
		ImGui::Separator();
		listIter * iter = listGetIterator(m_datalink_list, 0);
		listNode * node;
		for (node = listNext(iter); node; ) {

			nti_datalink * link = (nti_datalink *)listNodeValue(node);
			assert(link);

			if (ImGui::Selectable(link->name.utf8Ptr(), m_curr_datalink == node, ImGuiSelectableFlags_SpanAllColumns))
				m_curr_datalink = node;
			bool hovered = ImGui::IsItemHovered();
			ImGui::NextColumn();
			ImGui::Text(link->conn.utf8Ptr()); ImGui::NextColumn();
			ImGui::Text(link->desc.utf8Ptr()); ImGui::NextColumn();

			node = listNext(iter);
		}
		listReleaseIterator(iter);
		ImGui::Columns(1);
		ImGui::Separator();

		if (ImGui::Button("Add")) {
			//ImGui::OpenPopup("Delete?");
			//if (ImGui::BeginPopupModal("Delete?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
			//{
			//	ImGui::Text("All those beautiful files will be deleted.\nThis operation cannot be undone!\n\n");
			//	ImGui::Separator();

			//	//static int unused_i = 0;
			//	//ImGui::Combo("Combo", &unused_i, "Delete\0Delete harder\0");

			//	static bool dont_ask_me_next_time = false;
			//	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
			//	ImGui::Checkbox("Don't ask me next time", &dont_ask_me_next_time);
			//	ImGui::PopStyleVar();

			//	if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
			//	ImGui::SetItemDefaultFocus();
			//	ImGui::SameLine();
			//	if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
			//	ImGui::EndPopup();
			//}

			PostMessage(WM_COMMAND, ID_NTI_ADD_DATALINK);
		}
	}
#endif
	if (ImGui::CollapsingHeader("Test")) {
		ImGui::InputText("name", &m_name);

		// test: imgui dmeo window
		static bool show_demo_window = 0;
		if (show_demo_window) {
			ImGui::ShowDemoWindow(&show_demo_window);
		}
		ImGui::Checkbox("imgui demo window", &show_demo_window);

		// test: table
		ImGui::Columns(4, "mycolumns"); // 4-ways, with border
		ImGui::Separator();
		ImGui::Text("ID"); ImGui::NextColumn();
		ImGui::Text("Name"); ImGui::NextColumn();
		ImGui::Text("Path"); ImGui::NextColumn();
		ImGui::Text("Hovered"); ImGui::NextColumn();
		ImGui::Separator();
		const char* names[3] = { "One", "Two", "Three" };
		const char* paths[3] = { "/path/one", "/path/two", "/path/three" };
		static int selected = -1;
		for (int i = 0; i < 3; i++)
		{
			char label[32];
			sprintf(label, "%04d", i);
			if (ImGui::Selectable(label, selected == i, ImGuiSelectableFlags_SpanAllColumns))
				selected = i;
			bool hovered = ImGui::IsItemHovered();
			ImGui::NextColumn();
			ImGui::Text(names[i]); ImGui::NextColumn();
			ImGui::Text(paths[i]); ImGui::NextColumn();
			ImGui::Text("%d", hovered); ImGui::NextColumn();
		}
		ImGui::Columns(1);
		ImGui::Separator();

		// test: ImGuiFileDialog
		static std::string filePathName;

		ImGui::Text("%s", filePathName.c_str());
		if (ImGui::Button("File...")) {
			igfd::ImGuiFileDialog::Instance()->OpenModal("ChooseFileDlgKey", "Choose File", ".xlsx", ".");

			nti_import_from_excel("test/device1.xlsx", 0);
#if (!NDEBUG && !NTI56_WITHOUT_ARX)
			int rc = nti_insert_table();
#endif
		}
		if (igfd::ImGuiFileDialog::Instance()->FileDialog("ChooseFileDlgKey"))
		{
			// action if OK
			if (igfd::ImGuiFileDialog::Instance()->IsOk == true)
			{
				filePathName = igfd::ImGuiFileDialog::Instance()->GetFilePathName();
				std::string filePath = igfd::ImGuiFileDialog::Instance()->GetCurrentPath();
				// action
			}
			// close
			igfd::ImGuiFileDialog::Instance()->CloseDialog("ChooseFileDlgKey");
		}

		//test: modal dialog
		if (ImGui::Button("about...")) {
			PostMessage(WM_COMMAND, ID_APP_ABOUT);
		}

		//test: modal dialog with imgui
		if (ImGui::Button("imgui about...")) {
			PostMessage(WM_COMMAND, ID_NTI_ABOUT);
		}
		//ImGui::TreePop();
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


void nti_blocksbar::OnSetFocus(CWnd* pOldWnd)
{
	m_setfocus = true;
	return nti_dockbase::OnSetFocus(pOldWnd);
}

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


void nti_blocksbar::OnNtiAddDatalinks()
{
#ifndef NTI56_WITHOUT_ARX
	createAndSetDataLink();
#endif
}

#ifdef NTI56_WITHOUT_ARX
LRESULT nti_blocksbar::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	nti_imgui_msghdl(GetSafeHwnd(), message, wParam, lParam);
	return nti_dockbase::WindowProc(message, wParam, lParam);
}

#endif

