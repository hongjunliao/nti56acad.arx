/*!
 * This file is PART of nti56acad.arx project
 * @author hongjun.liao <docici@126.com>, @date 2020/09/11
 *
 * dock ctrl bar
 *
 * @date 2023/08/25 updated using CAcUiDockControlBar on objectarx and CDockablePane on MFC
 * */

#include "stdafx.h"
#include "nti_dockbar.h"
#include "nti_str.h" //nti_wcstr2a
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_win32.h"
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <GL/GL.h>
#include <tchar.h>

#ifdef NTI56_ARX
#include "nti_arx.h" //nti_foreach_symtbl
#include "ArxDbgUiTdcSymTbl.h"
#include "ArxDbgUtils.h"
#include "ArxDbgUiTdmEntities.h"
#endif //#ifdef NTI56_ARX
 /////////////////////////////////////////////////////////////////////////////////////

// Data stored per platform window
struct WGL_WindowData { HDC hDC; };

// Data
static HGLRC            g_hRC;
static WGL_WindowData   g_MainWindow;
static int              g_Width;
static int              g_Height;

// Helper functions
static bool CreateDeviceWGL(HWND hWnd, WGL_WindowData* data)
{
    HDC hDc = ::GetDC(hWnd);
    PIXELFORMATDESCRIPTOR pfd = { 0 };
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;

    const int pf = ::ChoosePixelFormat(hDc, &pfd);
    if (pf == 0)
        return false;
    if (::SetPixelFormat(hDc, pf, &pfd) == FALSE)
        return false;
    ::ReleaseDC(hWnd, hDc);

    data->hDC = ::GetDC(hWnd);
    if (!g_hRC)
        g_hRC = wglCreateContext(data->hDC);
    return true;
}

static void CleanupDeviceWGL(HWND hWnd, WGL_WindowData* data)
{
    wglMakeCurrent(NULL, NULL);
    ::ReleaseDC(hWnd, data->hDC);
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static void CALLBACK MyTimerProc(

	HWND hWnd,        // handle to window for timer messages 
	UINT message,     // WM_TIMER message 
	UINT_PTR idTimer,     // timer identifier 
	DWORD dwTime)     // current system time 
{
	nti_dockbar * d = (nti_dockbar *)(idTimer);
	if(d) d->InvalidateRect(0);
	else  InvalidateRect(hWnd, 0, TRUE);
	//   frame->m_imguipane.GetWindowRect();
}


struct MyItem
{
    const char* field;
    const char* value;
};


static void
imgui_render_symtbl(ImGuiTreeNodeFlags node_flags)
{
	//for selection state
#ifdef NTI56_ARX
	static AcDbObjectId id;
#else
	static int i,t = -1 ,c = -1;
#endif //#ifdef NTI56_ARX
	//Tree:
	ImGui::BeginChild("left pane", ImVec2(ImGui::GetContentRegionAvail().x * 0.392f, 400),
			true, ImGuiWindowFlags_HorizontalScrollbar);
	if (ImGui::TreeNode("Block table")) {
		ImGui::Unindent(ImGui::GetTreeNodeToLabelSpacing());
#ifdef NTI56_ARX
		//AcDbObjectIdArray  m_dictObjIdList;
		nti_foreach_symtbl([node_flags](AcDbDatabase*& db, AcDbSymbolTable*& symTbl)->int {
			AcDbBlockTable* tbl;
			Acad::ErrorStatus  es = db->getBlockTable(tbl, AcDb::kForRead);
			if (es != Acad::eOk) {
				ArxDbgUtils::rxErrorMsg(es);
				return -1;
			}
			symTbl = tbl;
			return 0; 
		}, [/*&m_dictObjIdList, */node_flags](AcDbSymbolTableRecord*& tblRec)->int {
			const TCHAR* symName;
			tblRec->getName(symName);
			//m_dictObjIdList.append(tblRec->objectId());    // keep track of the objectId for each entry
			ImGui::TreeNodeEx(nti_wcstr2a(symName), 
					(id == tblRec->objectId() ? ImGuiTreeNodeFlags_Selected | node_flags : node_flags));
			if (ImGui::IsItemClicked())
				{ id = tblRec->objectId();}
			tblRec->close();
			return 0;
		});
#else 
		char const * data[] = { u8"NTI-光电开关示例", u8"NTI-物流开关示例" };
		for (i = 0; i < 2; ++i) {
		ImGui::TreeNodeEx((void*)(intptr_t)&i, 
			(t == 0 && c == i ? ImGuiTreeNodeFlags_Selected | node_flags : node_flags), data[i]);
		if (ImGui::IsItemClicked())
			{ t = 0; c = i;}
	}
#endif //#ifdef NTI56_ARX
		ImGui::Indent(ImGui::GetTreeNodeToLabelSpacing());
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Dimension Style Table")) {
#ifdef NTI56_ARX
		//AcDbObjectIdArray  m_dictObjIdList;
		nti_foreach_symtbl([node_flags](AcDbDatabase*& db, AcDbSymbolTable*& symTbl)->int {
			AcDbDimStyleTable* tbl;
			Acad::ErrorStatus  es = db->getDimStyleTable(tbl, AcDb::kForRead);
			if (es != Acad::eOk) {
				ArxDbgUtils::rxErrorMsg(es);
				return -1;
			}
			symTbl = tbl;
			return 0;
		}, [/*&m_dictObjIdList, */node_flags](AcDbSymbolTableRecord*& tblRec)->int {
			const TCHAR* symName;
			tblRec->getName(symName);
			//m_dictObjIdList.append(tblRec->objectId());    // keep track of the objectId for each entry
			ImGui::TreeNodeEx(nti_wcstr2a(symName),
					(id == tblRec->objectId() ? ImGuiTreeNodeFlags_Selected | node_flags : node_flags));
			if (ImGui::IsItemClicked())
				{ id = tblRec->objectId();}
			tblRec->close();
			return 0;
		});
#else
		char const * data[] = { u8"Dimension Style-1", u8"Dimension Style-2" };
		for (int i = 0; i < 2; ++i) {
			ImGui::TreeNodeEx((void*)(intptr_t)&i, 
					(t == 1 && c == i ? ImGuiTreeNodeFlags_Selected | node_flags : node_flags), data[i]);
			if (ImGui::IsItemClicked())
				{ t = 1;c = i;}
		}
#endif //#ifdef NTI56_ARX
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Layer Table")) {
#ifdef NTI56_ARX
		//AcDbObjectIdArray  m_dictObjIdList;
		nti_foreach_symtbl([node_flags](AcDbDatabase*& db, AcDbSymbolTable*& symTbl)->int {
			AcDbLayerTable* tbl;
			Acad::ErrorStatus  es = db->getLayerTable(tbl, AcDb::kForRead);
			if (es != Acad::eOk) {
				ArxDbgUtils::rxErrorMsg(es);
				return -1;
			}
			symTbl = tbl;
			return 0;
		}, [/*&m_dictObjIdList, */node_flags](AcDbSymbolTableRecord*& tblRec)->int {
			const TCHAR* symName;
			tblRec->getName(symName);
			//m_dictObjIdList.append(tblRec->objectId());    // keep track of the objectId for each entry
			ImGui::TreeNodeEx(nti_wcstr2a(symName),
					(id == tblRec->objectId() ? ImGuiTreeNodeFlags_Selected | node_flags : node_flags));
			if (ImGui::IsItemClicked())
				{ id = tblRec->objectId();}
			tblRec->close();
			return 0;
		});
#else
		char const * data[] = { u8"Layer-1", u8"Layer-2" };
		for (int i = 0; i < 2; ++i) {
			ImGui::TreeNodeEx((void*)(intptr_t)&i,
					(t == 2 && c == i ? ImGuiTreeNodeFlags_Selected | node_flags : node_flags), data[i]);
			if (ImGui::IsItemClicked())
				{ t = 2; c = i;}
		}
#endif //#ifdef NTI56_ARX
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Linetype Table")) {
#ifdef NTI56_ARX
		//AcDbObjectIdArray  m_dictObjIdList;
		nti_foreach_symtbl([node_flags](AcDbDatabase*& db, AcDbSymbolTable*& symTbl)->int {
			AcDbLinetypeTable* tbl;
			Acad::ErrorStatus  es = db->getLinetypeTable(tbl, AcDb::kForRead);
			if (es != Acad::eOk) {
				ArxDbgUtils::rxErrorMsg(es);
				return -1;
			}
			symTbl = tbl;
			return 0;
		}, [/*&m_dictObjIdList, */node_flags](AcDbSymbolTableRecord*& tblRec)->int {
			const TCHAR* symName;
			tblRec->getName(symName);
			//m_dictObjIdList.append(tblRec->objectId());    // keep track of the objectId for each entry
			ImGui::TreeNodeEx(nti_wcstr2a(symName),
					(id == tblRec->objectId() ? ImGuiTreeNodeFlags_Selected | node_flags : node_flags));
			if (ImGui::IsItemClicked())
				{ id = tblRec->objectId();}
			tblRec->close();
			return 0;
		});
#else
		char const * data[] = { u8"Layer-1", u8"Layer-2" };
		for (int i = 0; i < 2; ++i) {
			ImGui::TreeNodeEx((void*)(intptr_t)&i,
					(t == 3 && c == i ? ImGuiTreeNodeFlags_Selected | node_flags : node_flags), data[i]);
			if (ImGui::IsItemClicked()/* && !ImGui::IsItemToggledOpen()*/)
				{t = 3;c = i;}
		}
#endif //#ifdef NTI56_ARX
		ImGui::TreePop();
	}
	ImGui::EndChild();
	ImGui::SameLine();
	//table: Field/Value
	ImGui::BeginChild("item view", ImVec2(0, 400/*-ImGui::GetFrameHeightWithSpacing()*/));
	static ImGuiTableFlags flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_RowBg;
	if (ImGui::BeginTable("table1", 2, flags)) {
		ImGui::TableSetupColumn("Field");
		ImGui::TableSetupColumn("Value");
		ImGui::TableHeadersRow();

#ifdef NTI56_ARX
		if (id) {
			AcDbObject* obj = NULL;
			Acad::ErrorStatus es = acdbOpenObject(obj, id, AcDb::kForRead);
			//setExtensionButtons(obj);
			//setBlockInfoButtons(obj);

			if (es == Acad::eOk) {
				nti_prop_t p;
				nti_getprop(obj, p);
				POSITION posk, posv;
				for (posk = p._1.GetHeadPosition(), posv = p._2.GetHeadPosition(); posk && posv;) {
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::TextUnformatted(nti_wcstr2a((LPCTSTR)p._1.GetNext(posk)));
					ImGui::TableSetColumnIndex(1);
					ImGui::TextUnformatted(nti_wcstr2a((LPCTSTR)p._2.GetNext(posv)));
				}
				obj->close();
			}
		}
#else
		for (int row = 0; row < 5; row++)
		{
			ImGui::TableNextRow();
			for (int column = 0; column < 2; column++)
			{
				ImGui::TableSetColumnIndex(column);
				char buf[32];
				sprintf(buf, "%s %d,%d", column == 0 ? "key" : "value", column, row);
				ImGui::TextUnformatted(buf);
			}
		}
#endif //#ifdef NTI56_ARX

		ImGui::EndTable();
	}
	ImGui::EndChild();
}

static void
imgui_render_dictionaries(ImGuiTreeNodeFlags node_flags)
{
	////Tree:
	//ImGui::BeginChild("left pane", ImVec2(300, 600), true);
	//static int node_clicked = 0;
	//char const * data[] = { u8"ACAD_1", u8"ACAD_2" };
	//ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow
	//	| ImGuiTreeNodeFlags_OpenOnDoubleClick
	//	/*|ImGuiTreeNodeFlags_SpanAvailWidth*/;
	//node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet
	//if (ImGui::TreeNode("<Root Directory>")) {
	//	for (int i = 0; i < 2; ++i) {
	//		ImGui::TreeNodeEx((void*)(intptr_t)&i, node_flags, data[i]);
	//		if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
	//			node_clicked = i;
	//	}
	//	ImGui::TreePop();
	//}
	//ImGui::EndChild();
	//ImGui::SameLine();
	////table: Field/Value
	//ImGui::BeginChild("item view", ImVec2(0, 600/*-ImGui::GetFrameHeightWithSpacing()*/)); // Leave room for 1 line below us
	//static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;
	//if (ImGui::BeginTable("table1", 2, flags)) {
	//	ImGui::TableSetupColumn("Field");
	//	ImGui::TableSetupColumn("Value");
	//	ImGui::TableHeadersRow();

	//	for (int row = 0; row < 5; row++)
	//	{
	//		ImGui::TableNextRow();
	//		for (int column = 0; column < 2; column++)
	//		{
	//			ImGui::TableSetColumnIndex(column);
	//			char buf[32];
	//			sprintf(buf, "%s %d,%d", column == 0 ? "key" : "value", column, row);
	//			ImGui::TextUnformatted(buf);
	//		}
	//	}
	//	ImGui::EndTable();
	//}
	//ImGui::EndChild();
}

static void imgui_render_database()
{
	////Image:
	//ImGui::BeginChild("left pane", ImVec2(300, 600), true);
	//ImTextureID my_tex_id = io.Fonts->TexID;
	//float my_tex_w = (float)io.Fonts->TexWidth;
	//float my_tex_h = (float)io.Fonts->TexHeight;
	//{
	//	static bool use_text_color_for_tint = false;
	//	ImGui::Text("Preview Image");
	//	ImVec2 pos = ImGui::GetCursorScreenPos();
	//	ImVec2 uv_min = ImVec2(0.0f, 0.0f);                 // Top-left
	//	ImVec2 uv_max = ImVec2(1.0f, 1.0f);                 // Lower-right
	//	ImVec4 tint_col = use_text_color_for_tint ? ImGui::GetStyleColorVec4(ImGuiCol_Text) : ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
	//	ImVec4 border_col = ImGui::GetStyleColorVec4(ImGuiCol_Border);
	//	ImGui::Image(my_tex_id, ImVec2(my_tex_w, my_tex_h), uv_min, uv_max, tint_col, border_col);
	//}

	//ImGui::EndChild();
	//ImGui::SameLine();
	////table: Field/Value
	//ImGui::BeginChild("item view", ImVec2(0, 600/*-ImGui::GetFrameHeightWithSpacing()*/)); // Leave room for 1 line below us
	//static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;
	//if (ImGui::BeginTable("table1", 2, flags)) {
	//	ImGui::TableSetupColumn("Field");
	//	ImGui::TableSetupColumn("Value");
	//	ImGui::TableHeadersRow();

	//	for (int row = 0; row < 5; row++)
	//	{
	//		ImGui::TableNextRow();
	//		for (int column = 0; column < 2; column++)
	//		{
	//			ImGui::TableSetColumnIndex(column);
	//			char buf[32];
	//			sprintf(buf, "%s %d,%d", column == 0 ? "key" : "value", column, row);
	//			ImGui::TextUnformatted(buf);
	//		}
	//	}
	//	ImGui::EndTable();
	//}
	//ImGui::EndChild();

}

static int imgui_do_render(ImGuiIO& io)
{
	static  bool show_demo_window = false;
	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

	if (ImGui::CollapsingHeader(u8"数据库信息")) {
		ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow
			| ImGuiTreeNodeFlags_OpenOnDoubleClick
			| ImGuiTreeNodeFlags_SpanFullWidth;
		node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet
		if (ImGui::BeginTabBar("database_info", ImGuiTabBarFlags_None)) {
			if (ImGui::BeginTabItem(u8"符号表")) {
				imgui_render_symtbl(node_flags);
			    ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Dictionaries")) {
				imgui_render_dictionaries(node_flags);
			    ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Database")) {
				imgui_render_database();
			    ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
	}
	if (ImGui::CollapsingHeader(u8"设置")) {
		// Setup Dear ImGui style
        ImGui::SeparatorText(u8"主题/风格");
        static int e = 0;
        ImGui::RadioButton(u8"深色", &e, 0); ImGui::SameLine();
        ImGui::RadioButton(u8"浅色", &e, 1); ImGui::SameLine();
        ImGui::RadioButton(u8"经典", &e, 2);

		if(e == 0 )       ImGui::StyleColorsDark();
		else if(e == 1 )  ImGui::StyleColorsLight();
		else              ImGui::StyleColorsClassic();
	}
	if (ImGui::CollapsingHeader(u8"关于/帮助")) {
		ImGui::Text("git@gitee.com:jun/nti56acad.arx");
#ifdef NTI56_ARX
		ImGui::Text("a simple AotuCAD ObjectARX app");
		ImGui::Text("build at %s %s for AotuCAD %d", __DATE__, __TIME__, NTI56_ARX);
#else
		ImGui::Text("test program in MFC");
		ImGui::Text("build at %s %s", __DATE__, __TIME__);
#endif //#ifdef NTI56_ARX
		ImGui::Text("version %s-%s, %s", "master", "2f2a843", u8"检测到新版本!");
	}

	ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

	return 0;
}

static int imgui_render(HDC hdc, RECT * rect)
{
	wglMakeCurrent(hdc, g_hRC);
	static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImVec2 size(rect->right - rect->left, rect->bottom - rect->top);
	ImVec2 pos(rect->left, rect->top);
	ImGui::SetNextWindowPos(pos);
	ImGui::SetNextWindowSize(size);

	ImGui::Begin("ntidockbar", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);  // Create a window called "Hello, world!" and append into it.
	imgui_do_render(io);
	ImGui::End();
	// Rendering
	ImGui::Render();
	glViewport(0, 0, g_Width, g_Height);
	glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	// Present
	::SwapBuffers(g_MainWindow.hDC);

	return 0;
}

static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_CREATE: {
		// Initialize OpenGL
		if (!CreateDeviceWGL(hWnd, &g_MainWindow))
		{
			CleanupDeviceWGL(hWnd, &g_MainWindow);
			::DestroyWindow(hWnd);
			// ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
			return 1;
		}
		wglMakeCurrent(g_MainWindow.hDC, g_hRC);

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;   // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;    // Enable Gamepad Controls

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();

		// Setup Platform/Renderer backends
		ImGui_ImplWin32_InitForOpenGL(hWnd);
		ImGui_ImplOpenGL3_Init();
		// Load Fonts
		// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
		// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
		// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
		// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
		// - Read 'docs/FONTS.md' for more instructions and details.
		// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
		//io.Fonts->AddFontDefault();
		//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
		//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
		//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
		//io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
		ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\simsun.ttc", 13.0f, NULL, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
		IM_ASSERT(font != NULL);

		SetTimer(hWnd, (UINT_PTR)0, 16, MyTimerProc);
		break;
	}
	case WM_PAINT: {
		RECT rectobj, *rect = &rectobj;
		GetClientRect(hWnd, rect);

		PAINTSTRUCT ps;
		HDC hDC = BeginPaint(hWnd, &ps);
		imgui_render(hDC, rect);
		EndPaint(hWnd, &ps);

		break;
	}
	case WM_SIZE:
		if (wParam != SIZE_MINIMIZED)
		{
			g_Width = LOWORD(lParam);
			g_Height = HIWORD(lParam);
		}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();

		CleanupDeviceWGL(hWnd, &g_MainWindow);
		wglDeleteContext(g_hRC);
		return 0;
	}
	return ::DefWindowProcW(hWnd, msg, wParam, lParam);

}

//////////////////////////////////////////////////////////////////////
// 构造/析构

nti_dockbar::nti_dockbar() noexcept
{
	m_hWnd = 0;
}

nti_dockbar::~nti_dockbar()
{
}

BEGIN_MESSAGE_MAP(nti_dockbar, nti_dockbase)
	ON_WM_DESTROY()
#ifdef NTI56_ARX
	ON_WM_ERASEBKGND()
#else
	ON_WM_CREATE()
	ON_WM_SIZE()
	//ON_WM_PAINT()
#endif //#ifdef NTI56_ARX
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// nti_dockbar 消息处理程序

#ifdef NTI56_ARX
BOOL nti_dockbar::CreateControlBar(LPCREATESTRUCT lpCreateStruct)
{
	if (!nti_dockbase::CreateControlBar(lpCreateStruct))
		return FALSE;

    //Create application window
    ImGui_ImplWin32_EnableDpiAwareness();
    WNDCLASSEXW wc = { sizeof(wc), CS_OWNDC, WndProc, 0L, 0L, GetModuleHandle(NULL), 
				NULL, NULL, NULL, NULL, L"ntidockbar", NULL };
    ::RegisterClassExW(&wc);
    m_hWnd = ::CreateWindow(wc.lpszClassName, L"ntidockbar", WS_CHILD | WS_VISIBLE
					, 0, 0, 100, 100, GetSafeHwnd(), NULL, wc.hInstance, NULL);
	assert(m_hWnd);
	return TRUE;
}

void nti_dockbar::SizeChanged(CRect * lpRect, BOOL /*bFloating*/, int /*flags*/)
{
    g_Width = lpRect->Width();
    g_Height = lpRect->Height();

	int x = lpRect->left, y = lpRect->top;
	::MoveWindow(m_hWnd, x, y, g_Width, g_Height, TRUE);
}

BOOL nti_dockbar::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}
#else
// nti_dockbar 消息处理程序

int nti_dockbar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (nti_dockbase::OnCreate(lpCreateStruct) == -1)
		return -1;

	//Create application window
	ImGui_ImplWin32_EnableDpiAwareness();
	WNDCLASSEXW wc = { sizeof(wc), CS_OWNDC, WndProc, 0L, 0L, GetModuleHandle(NULL),
				NULL, NULL, NULL, NULL, L"ntidockbar", NULL };
	::RegisterClassExW(&wc);
	m_hWnd = ::CreateWindow(wc.lpszClassName, L"ntidockbar", WS_CHILD | WS_VISIBLE
		, 0, 0, 100, 100, GetSafeHwnd(), NULL, wc.hInstance, NULL);
	assert(m_hWnd);

	return 0;
}

void nti_dockbar::OnSize(UINT nType, int cx, int cy)
{
	g_Width = cx;
	g_Height = cy;
	int x = 0, y = 0;
	::MoveWindow(m_hWnd, x, y, g_Width, g_Height, TRUE);

	nti_dockbase::OnSize(nType, cx, cy);
}

#endif //#ifdef NTI56_ARX

void nti_dockbar::OnDestroy()
{
	::DestroyWindow(m_hWnd);
}
