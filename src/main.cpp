/*!
 * This file is PART of nti56acad project
 * @author hongjun.liao <docici@126.com>, @date 2020/09/11
 *
 * main
 * */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "stdafx.h"
#include <SDL.h>

/////////////////////////////////////////////////////////////////////////////////////

HWND g_hwnd;
int is_chld = 0;

#ifndef NTI56_WITHOUT_MFC

int sdl_main(int, char**);
int win32_main(int argc, char *argv[]);
int win32_docking(int, char**);
int win32_docking2(int, char**);
int test_nti_imgui_main(int, char**);
int win32_docking_gl3(int, char**);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	return test_nti_imgui_main(0, 0);
}

#endif