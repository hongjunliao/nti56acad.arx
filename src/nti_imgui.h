/*!
 * This file is PART of nti56acad project
 * @author hongjun.liao <docici@126.com>, @date 2020/09/11
 *
 * imgui
 * */

#ifndef NTI_IMGUI_H
#define NTI_IMGUI_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#ifdef __cplusplus
extern "C" {
#endif
/////////////////////////////////////////////////////////////////////////////////////

struct nti_imgui_wnddata {
	char title[128];
	bool is_open;
};

int nti_imgui_create(HWND hwnd);
int nti_imgui_add_render(void(*render)(nti_imgui_wnddata * wnddata), nti_imgui_wnddata * wnddata);
int nti_imgui_paint();
int nti_imgui_destroy(HWND hwnd);
LRESULT WINAPI nti_imgui_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


/////////////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif
#endif //NTI_IMGUI_H