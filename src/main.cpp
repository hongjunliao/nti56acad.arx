/*
 * 
 */
#include "stdafx.h"
#include <SDL.h>

HWND g_hwnd;
int is_chld = 0;

extern int sdl_main(int, char**);
int win32_main(int argc, char *argv[]);
int win32_docking(int, char**);

int main(int argc, char ** argv)
{
	return win32_docking(argc, argv);
}