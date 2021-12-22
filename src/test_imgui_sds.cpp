/*!
* This file is PART of nti56acad project
* @author hongjun.liao <docici@126.com>, @date 2020/09/11
*
* arx main
* */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

/////////////////////////////////////////////////////////////////////////////////////

#ifndef NDEBUG
#include <time.h>
#include <windows.h>
#include <tchar.h>
#include <assert.h>
#include "nti_imgui.h"
#include "nti_render.h"
#include "imgui/misc/cpp/imgui_sds.h"
static HWND g_hwnd = 0;
static int is_chld = 0;

struct test_wnddata {
	nti_imgui_wnddata base;
	sds name;
};
// Win32 message handler
static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	nti_imgui_WndProc(hWnd, msg, wParam, lParam);

	switch (msg) {
	case WM_CREATE:
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

static void test_render(nti_imgui_wnddata * wdata)
{
	test_wnddata * wnddata = (test_wnddata *)wdata;

	ImGui::Begin(wdata->title);                          // Create a window called "Hello, world!" and append into it.

	//ImGui::Text("sample text");
	ImGui::InputText("name", &wnddata->name, 0, 0, 0);

	ImGui::End();
}

int test_imgui_sds_main(int argc, char ** argv)
{
	int rc;
	time_t init = time(0);
	// Create application window
	int wstyle = (is_chld ? (WS_OVERLAPPEDWINDOW | WS_CHILD | WS_VISIBLE | WS_POPUP)
		: (WS_OVERLAPPEDWINDOW | WS_VISIBLE));
	static WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("nti56acad"), NULL };
	::RegisterClassEx(&wc);
	g_hwnd = ::CreateWindowEx(WS_EX_TOPMOST, wc.lpszClassName, _T("nti56acad"), wstyle, 100, 100, 400, 200, 0, NULL, wc.hInstance, NULL);

	rc = nti_imgui_create(g_hwnd, 0);
	assert(rc == 0);

	test_wnddata wnddataobj = { {"hello"} }, *wnddata = &wnddataobj;
	wnddata->name = sdsempty();

	nti_imgui_add(test_render, (nti_imgui_wnddata *)wnddata);
	// Show the window
	::ShowWindow(g_hwnd, SW_SHOWDEFAULT);
	::UpdateWindow(g_hwnd);

	if (!is_chld) {
		// Main loop
		MSG msg;
		ZeroMemory(&msg, sizeof(msg));
		while (msg.message != WM_QUIT) {
			if (difftime(time(0), init) > 5) {
				//wnddata->name = sdscpy(wnddata->name, "timed out");
				//break;
			}

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

			nti_imgui_render();
		}
	}

	nti_imgui_destroy(g_hwnd);
	DestroyWindow(g_hwnd);

	int len = sdslen(wnddata->name);
	assert(sdslen(wnddata->name) > 0);
	sdsfree(wnddata->name);

	return 0;
}
#endif