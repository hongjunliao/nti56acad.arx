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
#include "imgui.h"

extern "C" {
#include "adlist.h"	//list
}

#ifdef __cplusplus
extern "C" {
#endif
/////////////////////////////////////////////////////////////////////////////////////

struct nti_imgui {
	HWND hwnd;
	HWND phwnd;
	ImVec4 clear_color;
	list * renderlist;
};

struct nti_imgui_wnddata {
	char title[128];
	bool is_open;
	nti_imgui * imgui;
};

int nti_imgui_create(HWND hwnd, HWND phwnd, int flags = 0);
int nti_imgui_add(void(*render)(nti_imgui_wnddata * wnddata), nti_imgui_wnddata * wnddata);
int nti_imgui_render();
RECT nti_imgui_size();
int nti_imgui_modal(void(*render)(nti_imgui_wnddata * wnddata), nti_imgui_wnddata * wnddata);
int nti_imgui_destroy(HWND hwnd);
LRESULT nti_imgui_msghdl(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI nti_imgui_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
nti_imgui * nti_imgui_();
/////////////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif
#endif //NTI_IMGUI_H