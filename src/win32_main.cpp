// =============================================================================
//                                  INCLUDES
// =============================================================================
#include "stdafx.h"
#include <Windows.h>
#include <random>
#include <stdio.h>
#include <glad/glad.h>
#include <imgui.h>
#include <imgui_impl_win32.h>
#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <application.h>
#include <tchar.h>
IApplication::~IApplication() = default;
HGLRC g_GLRenderContext;
HDC g_HDCDeviceContext;
extern HWND g_hwnd;
extern int is_chld;
int g_display_w = 800;
int g_display_h = 600;
ImVec4 clear_color;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void CreateGlContext()
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
            0, 0, 0, 0};
    g_HDCDeviceContext = GetDC(g_hwnd);
    int pixelFormat = ChoosePixelFormat(g_HDCDeviceContext, &pfd);

    SetPixelFormat(g_HDCDeviceContext, pixelFormat, &pfd);

    g_GLRenderContext = wglCreateContext(g_HDCDeviceContext);

    wglMakeCurrent(g_HDCDeviceContext, g_GLRenderContext);
    gladLoadGL();
}

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;
    switch (msg) {
    case WM_SIZE:  {
        if (wParam != SIZE_MINIMIZED)  {
            g_display_w = (UINT)LOWORD(lParam);
            g_display_h = (UINT)HIWORD(lParam);
        }
        return 0;
    }
    case WM_SYSCOMMAND:  {
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    }
    case WM_DESTROY:  {
        PostQuitMessage(0);
        return 0;
    }
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

bool Init(HINSTANCE hInstance)
{
    ImGui_ImplWin32_EnableDpiAwareness();

    int wstyle = (is_chld ? (WS_OVERLAPPEDWINDOW | WS_CHILD | WS_VISIBLE | WS_POPUP)
                         : (WS_OVERLAPPEDWINDOW | WS_VISIBLE));
    static WNDCLASSEX wc = {sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("nti56acad"), NULL};
    ::RegisterClassEx(&wc);
    g_hwnd = ::CreateWindowEx(WS_EX_TOPMOST, wc.lpszClassName, _T("nti56acad"), wstyle, 100, 100, 200, 100, 0, NULL, wc.hInstance, NULL);

    // Show the window
    ShowWindow(g_hwnd, SW_SHOWDEFAULT);
    UpdateWindow(g_hwnd);

    //Prepare OpenGlContext
    CreateGlContext();
    // Setup Dear ImGui binding
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   // Enable Multi-Viewport / Platform Windows

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    //Init Win32
    ImGui_ImplWin32_Init(g_hwnd);

    //Init OpenGL Imgui Implementation
    // GL 3.0 + GLSL 130
    const char *glsl_version = "#version 130";
    ImGui_ImplOpenGL3_Init(glsl_version);
    //Set Window bg color
    clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    return true;
}

void Cleanup(HINSTANCE hInstance)
{
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();

    wglDeleteContext(g_GLRenderContext);
    ImGui_ImplWin32_Shutdown();

    ImGui::DestroyContext();

    DestroyWindow(g_hwnd);

    UnregisterClass(L"IMGUI", hInstance);
}

int win32_main(int argc, char *argv[])
{
    HINSTANCE hInstance = GetModuleHandle(NULL);
    
    if (!Init(hInstance))
        return 1;

    ImGuiIO &io = ImGui::GetIO();
    // Main loop
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));

    while (msg.message != WM_QUIT)  {
        // Poll and handle messages (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            continue;
        }
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ImGui::ShowDemoWindow(0);

		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
		{
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

			ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)

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

		wglMakeCurrent( g_HDCDeviceContext,g_GLRenderContext);

		glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Update and Render additional Platform Windows
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			HGLRC GLRenderContext = wglGetCurrentContext();

			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();

			wglMakeCurrent(g_HDCDeviceContext, GLRenderContext);
		}

		SwapBuffers(g_HDCDeviceContext);
	}

    Cleanup(hInstance);
    return 0;
}
