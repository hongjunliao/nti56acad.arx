/*!
 * This file is PART of nti56acad project
 * @author hongjun.liao <docici@126.com>, @date 2020/09/11
 *
 * imgui
 * */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "stdafx.h"
#include "imgui.h"
#include "imgui_impl_opengl2.h"
#include "imgui_impl_win32.h"
#include <GL/GL.h>
#include "nti_imgui.h"		//nti_imgui_create
#include "nti_arx.h"		//
#include "nti_cmn.h"	//nti_wnddata
#include "nti_render.h"
#include "nti_xlsx.h"
#include "ImGuiFileDialog/ImGuiFileDialog.h"
#ifndef NTI56_WITHOUT_ARX
#include "nti_arx.h"		//
#endif

#ifndef WM_DPICHANGED
#define WM_DPICHANGED 0x02E0 // From Windows SDK 8.1+ headers
#endif

/////////////////////////////////////////////////////////////////////////////////////

//tabs window
void nti_tabswnd_render(nti_imgui_wnddata * wnddata)
{
	if (!wnddata)
		return;
	ImGui::Begin(wnddata->title);
	nti_wnddata_reactor * reactor = (nti_wnddata_reactor *)wnddata;
	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar("tab", tab_bar_flags)) {
		if (ImGui::BeginTabItem(("图块"))) {
			
			// ImGuiComboFlags_PopupAlignLeft
			if(!reactor->curr_block && listFirst(reactor->block_list))
				reactor->curr_block = listFirst(reactor->block_list);
			const char* combo_label = (reactor->curr_block ? (char const *)listNodeValue(reactor->curr_block) : "");

			if (ImGui::BeginCombo("blocks", combo_label, 0)){
				listIter * iter = listGetIterator(reactor->block_list, 0);
				listNode * node;
				for (node = listNext(iter); node; ) {

					char * bname = (char *)listNodeValue(node);
					assert(bname);

					const bool is_selected = (reactor->curr_block == node);
					if(ImGui::Selectable(bname, is_selected))
						reactor->curr_block = node;
					if (is_selected)
						ImGui::SetItemDefaultFocus();

					node = listNext(iter);
				}
				listReleaseIterator(iter);

				ImGui::EndCombo();
			}

			ImGui::InputText("what:", reactor->what, IM_ARRAYSIZE(reactor->what));
			ImGui::InputText("class:", reactor->cls, IM_ARRAYSIZE(reactor->cls));
			ImGui::InputText("object id:", reactor->obj_id, IM_ARRAYSIZE(reactor->obj_id));
			ImGui::InputText("handle:", reactor->handle, IM_ARRAYSIZE(reactor->handle));

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem(("工具"))) {
			ImGui::Text("This is the Cucumber tab!\nblah blah blah blah blah");
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("导入/导出")) {
			static std::string filePathName;

			ImGui::Text("%s", filePathName.c_str());
			if (ImGui::Button("选择文件...")) {
				igfd::ImGuiFileDialog::Instance()->OpenModal("ChooseFileDlgKey", "Choose File", ".xlsx", ".");

				nti_import_from_excel("test/device1.xlsx", 0);
#ifndef NTI56_WITHOUT_ARX
				//int rc = nti_insert_table();
#endif
			}
			// display
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
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem(("打印"))) {
			ImGui::Text("This is the Cucumber tab!\nblah blah blah blah blah");
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem(("设置"))) {
			ImGui::Text("This is the Cucumber tab!\nblah blah blah blah blah");
			ImGui::EndTabItem();
		}
#ifndef NDEBUG
		if (ImGui::BeginTabItem(("Test"))) {

			static bool show_demo_window = 0;
			if (show_demo_window) {
				ImGui::BeginChild("imgui demo window");
				ImGui::ShowDemoWindow(&show_demo_window);
				ImGui::EndChild();
			}
			ImGui::Checkbox("imgui demo window", &show_demo_window);
#if (!NDEBUG && !NTI56_WITHOUT_ARX)
			if (ImGui::Button("test_nti_arx_main")) {
				int rc = test_nti_arx_main(0, 0); 
			}
#endif
			ImGui::EndTabItem();
		}
#endif // !NDEBUG
		ImGui::EndTabBar();
	}
	ImGui::End();
}

//reactor window
void nti_tabswnd_reactor(nti_imgui_wnddata * wnddata)
{
	if (!wnddata)
		return;
	nti_wnddata_reactor * reactor = (nti_wnddata_reactor *)wnddata;

	ImGui::Begin("reactor");                          // Create a window called "Hello, world!" and append into it.
	ImGui::InputText("what:", reactor->what, IM_ARRAYSIZE(reactor->what));
	ImGui::InputText("class:", reactor->cls, IM_ARRAYSIZE(reactor->cls));
	ImGui::InputText("object id:", reactor->obj_id, IM_ARRAYSIZE(reactor->obj_id));
	ImGui::InputText("handle:", reactor->handle, IM_ARRAYSIZE(reactor->handle));
	ImGui::End();
}

// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
void nti_tabswnd_simple(nti_imgui_wnddata * wnddata)
{
	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	static bool show_demo_window = true;
	static bool show_another_window = false;
	static float f = 0.0f;
	static int counter = 0;
	static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

	ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

	ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
	ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
	ImGui::Checkbox("Another Window", &show_another_window);

	ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
	ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

	if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
		counter++;
	ImGui::SameLine();
	ImGui::Text("counter = %d", counter);

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();
}

// 3. Show another simple window.
void nti_tabswnd_another(nti_imgui_wnddata * wnddata)
{
	static bool show_another_window = false;
	ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
	ImGui::Text("Hello from another window!");
	if (ImGui::Button("Close Me"))
		show_another_window = false;
	ImGui::End();
}
