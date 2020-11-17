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
#include "nti_str.h"		//
#include "nti_cmn.h"		//nti_new

extern "C" {
#include "adlist.h"	//list
}

#ifndef WM_DPICHANGED
#define WM_DPICHANGED 0x02E0 // From Windows SDK 8.1+ headers
#endif

static list * g_renderlist = 0;
/////////////////////////////////////////////////////////////////////////////////////

struct nti_imgui_render {
	nti_imgui_wnddata * wnddata;
	void(*render)(nti_imgui_wnddata * wnddata);
};

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

struct RendererData
{
	HDC hDC;
};

// Data
static HGLRC g_hRC;
static RendererData g_MainWindow;
static int g_Width;
static int g_Height;

/////////////////////////////////////////////////////////////////////////////////////

// Forward declarations of helper functions
static bool CreateDeviceOpenGL2(HWND hWnd, RendererData* data);
static void CleanupDeviceOpenGL2(HWND hWnd, RendererData* data);
static void ResetDevice();

static void Win32_CreateWindow(ImGuiViewport* viewport)
{
	assert(viewport->RendererUserData == NULL);

	RendererData* data = IM_NEW(RendererData);
	CreateDeviceOpenGL2((HWND)viewport->PlatformHandle, data);
	viewport->RendererUserData = data;
}

static void Win32_DestroyWindow(ImGuiViewport* viewport)
{
	if (viewport->RendererUserData != NULL)
	{
		RendererData* data = (RendererData*)viewport->RendererUserData;
		CleanupDeviceOpenGL2((HWND)viewport->PlatformHandle, data);
		IM_DELETE(data);
		viewport->RendererUserData = NULL;
	}
}

static void Win32_RenderWindow(ImGuiViewport* viewport, void*)
{
	RendererData* data = (RendererData*)viewport->RendererUserData;

	if (data)
	{
		wglMakeCurrent(data->hDC, g_hRC);
	}
}

static void Win32_SwapBuffers(ImGuiViewport* viewport, void*)
{
	RendererData* data = (RendererData*)viewport->RendererUserData;

	if (data)
	{
		SwapBuffers(data->hDC);
	}
}

static bool ActivateOpenGL2(HWND hWnd)
{
	HDC hDc = GetDC(hWnd);

	PIXELFORMATDESCRIPTOR pfd = { 0 };

	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;

	int pf = ChoosePixelFormat(hDc, &pfd);
	if (pf == 0)
	{
		return false;
	}

	if (SetPixelFormat(hDc, pf, &pfd) == FALSE)
	{
		return false;
	}

	ReleaseDC(hWnd, hDc);
	return true;
}

// Helper functions

static bool CreateDeviceOpenGL2(HWND hWnd, RendererData* data)
{
	if (!ActivateOpenGL2(hWnd))
		return false;

	data->hDC = GetDC(hWnd);

	if (!g_hRC)
	{
		g_hRC = wglCreateContext(data->hDC);
	}

	return true;
}

static void CleanupDeviceOpenGL2(HWND hWnd, RendererData* data)
{
	wglMakeCurrent(NULL, NULL);
	ReleaseDC(hWnd, data->hDC);
}

static void renderlist_free(void *ptr)
{
	nti_imgui_render * msg = (nti_imgui_render *)ptr;
	assert(ptr);
	free(ptr);
}


int nti_imgui_create(HWND hwnd)
{
	if (!g_renderlist) {
		g_renderlist = listCreate();
		listSetFreeMethod(g_renderlist, renderlist_free);
	}

	ImGui_ImplWin32_EnableDpiAwareness();

	// Initialize Direct3D
	if (!CreateDeviceOpenGL2(hwnd, &g_MainWindow))
	{
		CleanupDeviceOpenGL2(hwnd, &g_MainWindow);
		return 1;
	}

	wglMakeCurrent(g_MainWindow.hDC, g_hRC);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
																//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
																//io.ConfigViewportsNoAutoMerge = true;
																//io.ConfigViewportsNoTaskBarIcon = true;

																
	#ifndef NTI56_WITHOUT_ARX																
		ImGui::StyleColorsDark();								// Setup Dear ImGui style
	#else
		ImGui::StyleColorsLight();
	#endif
	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// Setup Platform/Renderer bindings
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplOpenGL2_Init();

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();

		// Store the hdc for this new window
		assert(platform_io.Renderer_CreateWindow == NULL);
		platform_io.Renderer_CreateWindow = Win32_CreateWindow;
		assert(platform_io.Renderer_DestroyWindow == NULL);
		platform_io.Renderer_DestroyWindow = Win32_DestroyWindow;
		assert(platform_io.Renderer_SwapBuffers == NULL);
		platform_io.Renderer_SwapBuffers = Win32_SwapBuffers;

		// We need to activate the context before drawing
		assert(platform_io.Platform_RenderWindow == NULL);
		platform_io.Platform_RenderWindow = Win32_RenderWindow;
	}
	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Read 'docs/FONTS.txt' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
	ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\simsun.ttc", 13.0f, NULL, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
	IM_ASSERT(font != NULL);

	// Show the window
	// ::ShowWindow(hwnd, SW_SHOWDEFAULT);
	// ::UpdateWindow(hwnd);

	return 0;
}

int nti_imgui_add_render(void(* render)(nti_imgui_wnddata * wnddata), nti_imgui_wnddata * wnddata)
{
	if(!(render))
		return -1;

	nti_imgui_render * ir = nti_new(nti_imgui_render);
	ir->render = render;
	ir->wnddata = wnddata;

	listAddNodeTail(g_renderlist, ir);

	return 0;
}

int nti_imgui_paint()
{
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Our state
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	// Start the Dear ImGui frame
	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	/* render window */
	if (g_renderlist) {
		listIter * iter = listGetIterator(g_renderlist, 0);
		for (listNode * node = listNext(iter); node; ) {
			
			nti_imgui_render * ir = (nti_imgui_render *)listNodeValue(node);
			assert(ir && ir->render);

			if(!ir->wnddata || !ir->wnddata->is_hide)
				ir->render(ir->wnddata);

			node = listNext(iter);
		}
		listReleaseIterator(iter);
	}

	// Rendering
	ImGui::Render();
	glViewport(0, 0, g_Width, g_Height);
	glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
	glClear(GL_COLOR_BUFFER_BIT);

	// If you are using this code with non-legacy OpenGL header/contexts (which you should not, prefer using imgui_impl_opengl3.cpp!!),
	// you may need to backup/reset/restore current shader using the commented lines below.
	//GLint last_program;
	//glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
	//glUseProgram(0);
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
	//glUseProgram(last_program);

	// Update and Render additional Platform Windows
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		HGLRC backup_current_context = wglGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		wglMakeCurrent(g_MainWindow.hDC, backup_current_context);
	}

	return 0;
}

int nti_imgui_destroy(HWND hwnd)
{
	SwapBuffers(g_MainWindow.hDC);
	ImGui_ImplOpenGL2_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupDeviceOpenGL2(hwnd, &g_MainWindow);
	wglDeleteContext(g_hRC);

	listRelease(g_renderlist);
	g_renderlist = 0;

	return 0;
}

// Win32 message handler
LRESULT WINAPI nti_imgui_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg) {
	case WM_CREATE:
		break;
	case WM_SIZE:
		if (wParam != SIZE_MINIMIZED)
		{
			g_Width = LOWORD(lParam);
			g_Height = HIWORD(lParam);
		}
		break;
	case WM_DPICHANGED:
		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports)
		{
			//const int dpi = HIWORD(wParam);
			//printf("WM_DPICHANGED to %d (%.0f%%)\n", dpi, (float)dpi / 96.0f * 100.0f);
			const RECT* suggested_rect = (RECT*)lParam;
			::SetWindowPos(hWnd, NULL, suggested_rect->left, suggested_rect->top, suggested_rect->right - suggested_rect->left, suggested_rect->bottom - suggested_rect->top, SWP_NOZORDER | SWP_NOACTIVATE);
		}
		break;
	default:
		break;
	}
	return 0;
}
