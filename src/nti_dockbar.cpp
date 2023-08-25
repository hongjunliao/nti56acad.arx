
#include "stdafx.h"
#include "nti_dockbar.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_win32.h"
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <GL/GL.h>
#include <tchar.h>

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

static int imgui_render(HDC hdc, RECT * rect)
{
	wglMakeCurrent(hdc, g_hRC);
	static bool show_demo_window = false;
	static bool show_another_window = false;
	static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
	{
		static float f = 0.0f;
		static int counter = 0;

		ImVec2 size(rect->right - rect->left, rect->bottom - rect->top);
		ImVec2 pos(rect->left, rect->top);

		ImGui::SetNextWindowPos(pos);
		ImGui::SetNextWindowSize(size);

		ImGui::Begin("Hello, world!", 0
			, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
		ImGui::Checkbox("Another Window", &show_another_window);

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
		ImGui::End();
	}

	// 3. Show another simple window.
	if (show_another_window)
	{
		ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			show_another_window = false;
		ImGui::End();
	}

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

//BEGIN_MESSAGE_MAP(nti_dockwnd, CWnd)
//	//	ON_WM_CREATE()
//	//	ON_WM_SIZE()
//	//	ON_WM_PAINT()
//	//    ON_WM_DESTROY()
//	ON_WM_ERASEBKGND()
//END_MESSAGE_MAP()

void nti_dockwnd::OnDraw(CDC* pDC)
{
	RECT rectobj, *rect = &rectobj;
	GetClientRect(rect);
	imgui_render(pDC->m_hDC, rect);
}

LRESULT nti_dockwnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(GetSafeHwnd(), message, wParam, lParam))
		return true;

	switch (message)
	{
	case WM_CREATE: {
		HWND hWnd = GetSafeHwnd();
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

		SetTimer((UINT_PTR)this, 16, MyTimerProc);
		break;
	}
	case WM_PAINT: {
		RECT rectobj, *rect = &rectobj;
		GetClientRect(rect);

		PAINTSTRUCT ps;
		CDC * pDC = BeginPaint(&ps);
		imgui_render(pDC->m_hDC, rect);
		EndPaint(&ps);

		break; 
	}
	//case WM_SIZE:
	//	if (wParam != SIZE_MINIMIZED)
	//	{
	//		g_Width = LOWORD(lParam);
	//		g_Height = HIWORD(lParam);
	//	}
	//	return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();

		HWND hWnd = GetSafeHwnd();
		CleanupDeviceWGL(hWnd, &g_MainWindow);
		wglDeleteContext(g_hRC);
		return 0;
	}
	return CView::WindowProc(message, wParam, lParam);
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
//	ON_WM_CREATE()
//	ON_WM_SIZE()
//	ON_WM_PAINT()
//    ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// nti_dockbar 消息处理程序

//BOOL nti_dockbar::PreCreateWindow(CREATESTRUCT& cs)
//{
//	// TODO: 在此处通过修改
//	//  CREATESTRUCT cs 来修改窗口类或样式
//	cs.style |= (WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
//	return nti_dockbase::PreCreateWindow(cs);
//}

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
    
	//m_wnd.Create(_T("STATIC"), _T("Hi"), WS_CHILD | WS_VISIBLE,
	//		CRect(0, 0, 20, 20), this, 1534);
	return TRUE;
}

//bool nti_dockbar::OnClosing()
//{
//    ImGui_ImplOpenGL3_Shutdown();
//    ImGui_ImplWin32_Shutdown();
//    ImGui::DestroyContext();
//
//    HWND hWnd = GetSafeHwnd();
//    CleanupDeviceWGL(hWnd, &g_MainWindow);
//    wglDeleteContext(g_hRC);
//
//	return true;
//}

void nti_dockbar::SizeChanged(CRect * lpRect, BOOL /*bFloating*/, int /*flags*/)
{
    g_Width = lpRect->Width();
    g_Height = lpRect->Height();
	if(!m_hWnd)
		m_wnd.MoveWindow(0, 0, g_Width, g_Height);
	else
		::MoveWindow(m_hWnd, 0, 0, g_Width, g_Height, TRUE);
}

//BOOL nti_dockbar::PreTranslateMessage(MSG* pMsg)
//{
//	return FALSE;
//	ImGui_ImplWin32_WndProcHandler(GetSafeHwnd(), pMsg->message, pMsg->wParam, pMsg->lParam);
//	return nti_dockbase::PreTranslateMessage(pMsg);
//}

//LRESULT nti_dockbar::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
//{
//
//    //	nti_imgui_msghdl(GetSafeHwnd(), message, wParam, lParam);
//    if(ImGui_ImplWin32_WndProcHandler(GetSafeHwnd(), message, wParam, lParam))
//        return TRUE;
//    return nti_dockbase::WindowProc(message, wParam, lParam);
//}

//void nti_dockbar::PaintControlBar(CDC *pDC)
//{
//	RECT rectobj, *rect = &rectobj;
//	GetClientRect(rect);
//	imgui_render(pDC->m_hDC, rect);
//}

BOOL nti_dockbar::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}
