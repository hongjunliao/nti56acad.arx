// dear imgui: wrappers for Redis/sds
#pragma once

#include "imgui.h"
#include "sds/win32_sds.h"

namespace ImGui
{
    // ImGui::InputText() with sds
    // Because text input needs dynamic resizing, we need to setup a callback to grow the capacity
    IMGUI_API bool  InputText(const char* label, sds * str, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);
    IMGUI_API bool  InputTextMultiline(const char* label, sds* str, const ImVec2& size = ImVec2(0, 0), ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);
    IMGUI_API bool  InputTextWithHint(const char* label, const char* hint, sds* str, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);
}

#ifndef NDEBUG
int test_imgui_sds_main(int argc, char ** argv);
#endif
