// dear imgui: wrappers for C++ standard library (STL) types (sds, etc.)
// This is also an example of how you may wrap your own similar types.

// Compatibility:
// - sds support is only guaranteed to work from C++11.
//   If you try to use it pre-C++11, please share your findings (w/ info about compiler/architecture)

// Changelog:
// - v0.10: Initial version. Added InputText() / InputTextMultiline() calls with sds

#include "imgui.h"
#include "imgui_sds.h"

struct InputTextCallback_UserData
{
    sds*            Str;
    ImGuiInputTextCallback  ChainCallback;
    void*                   ChainCallbackUserData;
};

static int InputTextCallback(ImGuiInputTextCallbackData* data)
{
    InputTextCallback_UserData* user_data = (InputTextCallback_UserData*)data->UserData;
    if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
    {
        // Resize string callback
        // If for some reason we refuse the new length (BufTextLen) and/or capacity (BufSize) we need to set them back to what we want.
        IM_ASSERT(data->Buf == *user_data->Str);
        *user_data->Str = sdsMakeRoomFor(*user_data->Str, data->BufTextLen);
        sdsIncrLen(*user_data->Str, data->BufTextLen - sdslen(*user_data->Str));
        data->Buf = (char*)*user_data->Str;
    }
    else if (user_data->ChainCallback)
    {
        // Forward to user callback, if any
        data->UserData = user_data->ChainCallbackUserData;
        return user_data->ChainCallback(data);
    }
    return 0;
}

bool ImGui::InputText(const char* label, sds* str, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data)
{
    IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
    flags |= ImGuiInputTextFlags_CallbackResize;

    InputTextCallback_UserData cb_user_data;
    cb_user_data.Str = str;
    cb_user_data.ChainCallback = callback;
    cb_user_data.ChainCallbackUserData = user_data;
    return InputText(label, (char*)*str, sdsavail(*str) + 1, flags, InputTextCallback, &cb_user_data);
}

bool ImGui::InputTextMultiline(const char* label, sds* str, const ImVec2& size, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data)
{
    IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
    flags |= ImGuiInputTextFlags_CallbackResize;

    InputTextCallback_UserData cb_user_data;
    cb_user_data.Str = str;
    cb_user_data.ChainCallback = callback;
    cb_user_data.ChainCallbackUserData = user_data;
    return InputTextMultiline(label, (char*)*str, sdsavail(*str) + 1, size, flags, InputTextCallback, &cb_user_data);
}

bool ImGui::InputTextWithHint(const char* label, const char* hint, sds* str, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data)
{
    IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
    flags |= ImGuiInputTextFlags_CallbackResize;

    InputTextCallback_UserData cb_user_data;
    cb_user_data.Str = str;
    cb_user_data.ChainCallback = callback;
    cb_user_data.ChainCallbackUserData = user_data;
    return InputTextWithHint(label, hint, (char*)*str, sdsavail(*str) + 1, flags, InputTextCallback, &cb_user_data);
}

/////////////////////////////////////////////////////////////////////////////////////

#ifndef NDEBUG
#include <assert.h>
#include "nti_imgui.h"
#include "nti_render.h"
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
    ImGui::InputText("name", &wnddata->name);

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

    rc = nti_imgui_create(g_hwnd);
    assert(rc == 0);

    test_wnddata wnddataobj = { {"hello"} }, *wnddata = &wnddataobj;
    wnddata->name = sdsempty();

    nti_imgui_add_render(test_render, (nti_imgui_wnddata *)wnddata);
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

            nti_imgui_paint();
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
