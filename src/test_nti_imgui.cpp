// dear imgui: standalone example application for OpenGL2 with Winapi
// If you are new to dear imgui, see examples/README.txt and documentation at the top of imgui.cpp.

#include "stdafx.h"
#include "imgui.h"
#include "imgui_impl_opengl2.h"
#include "imgui_impl_win32.h"
#include <windows.h>
#include <GL/GL.h>
#include <tchar.h>
#include "nti_imgui.h"		//nti_imgui_create

extern HWND g_hwnd;
extern int is_chld;

// Win32 message handler
static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	nti_imgui_WndProc(hWnd, msg, wParam, lParam);
	static int flag = 0;
	
	if(flag)
		InvalidateRect(g_hwnd, NULL, TRUE);
	
	switch (msg) {
	case WM_CREATE:
		break;
	case WM_PAINT:
		flag = 1;
		nti_imgui_paint();
		break;
	case WM_DESTROY: {
		//::UnregisterClass(wc.lpszClassName, wc.hInstance); 
		//nti_imgui_destroy(hWnd);
		::PostQuitMessage(0);
		break;
	}
	default:
		break;
	}
	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

// Main code
int test_nti_imgui_main(int, char**)
{
	int rc;
	// Create application window
	int wstyle = (is_chld ? (WS_OVERLAPPEDWINDOW | WS_CHILD | WS_VISIBLE | WS_POPUP)
		: (WS_OVERLAPPEDWINDOW | WS_VISIBLE));
	static WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("nti56acad"), NULL };
	::RegisterClassEx(&wc);
	g_hwnd = ::CreateWindowEx(WS_EX_TOPMOST, wc.lpszClassName, _T("nti56acad"), wstyle, 100, 100, 200, 100, 0, NULL, wc.hInstance, NULL);

	rc = nti_imgui_create(g_hwnd);
	if(rc != 0)
		return -1;

	// Show the window
	::ShowWindow(g_hwnd, SW_SHOWDEFAULT);
	::UpdateWindow(g_hwnd);

	if (!is_chld) {
		// Main loop
		MSG msg;
		ZeroMemory(&msg, sizeof(msg));
		while (msg.message != WM_QUIT)
		{
			// Poll and handle messages (inputs, window resize, etc.)
			// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
			// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
			// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
			// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
			if (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
				continue;
			}
		}
	}

	return 0;
}
