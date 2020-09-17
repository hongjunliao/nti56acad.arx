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
#include <string>
#include <random>
#include <stdio.h>
#include <glad/glad.h>
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <application.h>
#include <tchar.h>
#include "gbk-utf8/utf8.h"
#include "sds/win32_sds.h"
#include "nti_str.h"		//U8
#include "nti_imgui.h"		//nti_imgui_create

extern HWND g_hwnd;
extern int is_chld;
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

/////////////////////////////////////////////////////////////////////////////////////////

static HGLRC g_GLRenderContext;
static HDC g_HDCDeviceContext;
static ImVec4 clear_color;
static ImVec2 g_wnd_size(400, 200);

static void CreateGlContext()
{
	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, //Flags
		PFD_TYPE_RGBA,                                              // The kind of framebuffer. RGBA or palette.
		32,                                                         // Colordepth of the framebuffer.
		0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0,
		24, // Number of bits for the depthbuffer
		8,  // Number of bits for the stencilbuffer
		0,  // Number of Aux buffers in the framebuffer.
		PFD_MAIN_PLANE,
		0, 0, 0, 0 };
	g_HDCDeviceContext = GetDC(g_hwnd);
	int pixelFormat = ChoosePixelFormat(g_HDCDeviceContext, &pfd);

	SetPixelFormat(g_HDCDeviceContext, pixelFormat, &pfd);

	g_GLRenderContext = wglCreateContext(g_HDCDeviceContext);

	wglMakeCurrent(g_HDCDeviceContext, g_GLRenderContext);
	gladLoadGL();
}

LRESULT WINAPI nti_imgui_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg) {
	case WM_TIMER:
		nti_imgui_paint();
		break;
	case WM_CLOSE:
		::ShowWindow(g_hwnd, SW_HIDE);
		break;
	case WM_SIZE: {
		if (wParam != SIZE_MINIMIZED) {
			g_wnd_size.x = (UINT)LOWORD(lParam);
			g_wnd_size.y =  (UINT)HIWORD(lParam) + 20;
		}
		return 0;
	}
	}
	return FALSE;
}

int nti_imgui_init()
{
	ImGui_ImplWin32_EnableDpiAwareness();

	//Prepare OpenGlContext
	CreateGlContext();
	// Setup Dear ImGui binding
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
								
	ImGui::StyleColorsDark();	// Setup Dear ImGui style
	//ImGui::StyleColorsClassic();

	//Init Win32
	ImGui_ImplWin32_Init(g_hwnd);

	//Init OpenGL Imgui Implementation
	// GL 3.0 + GLSL 130
	const char *glsl_version = "#version 130";
	ImGui_ImplOpenGL3_Init(glsl_version);
	//Set Window bg color
	clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

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

	return 0;
}

int nti_imgui_paint()
{
	ImGuiIO &io = ImGui::GetIO();
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	//ImGui::ShowDemoWindow(0);

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::SetNextWindowSize(g_wnd_size);
		ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)

		ImGui::Text("中文测试");

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

	// Rendering
	ImGui::Render();

	wglMakeCurrent(g_HDCDeviceContext, g_GLRenderContext);

	glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
	glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	SwapBuffers(g_HDCDeviceContext);

	return 0;
}

void nti_imgui_uninit()
{
	HINSTANCE hInstance = GetModuleHandle(NULL);
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();

	wglDeleteContext(g_GLRenderContext);
	ImGui_ImplWin32_Shutdown();

	ImGui::DestroyContext();

	UnregisterClass(_T("IMGUI"), hInstance);
}