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

#include <windef.h>

#ifdef __cplusplus
extern "C" {
#endif
/////////////////////////////////////////////////////////////////////////////////////

int nti_imgui_create(HWND hwnd);
int nti_imgui_paint();
int nti_imgui_destroy(HWND hwnd);
LRESULT WINAPI nti_imgui_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

/////////////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif
#endif //NTI_IMGUI_H