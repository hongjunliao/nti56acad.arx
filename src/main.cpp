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
//#include <WinSock2.h>
//#include <stdio.h>
//#include <process.h>
//#include <time.h>

#include <locale.h>
#include "hp/hp_iocp.h"
#include "hp/hp_log.h"	 /* hp_log */
#include "hp/hp_test.h" /* hp_test */
#include "hp/hp_opt.h" /* hp_opt_argv */
#include "hp/hp_err.h"
#include "hp/sdsinc.h"	//sds
#define OPTPARSE_IMPLEMENTATION
#define OPTPARSE_API static
#include "optparse/optparse.h"		/* option */
//#include "getopt.h"			/* getopt_long */

#include <conio.h>			/* _getch() */
#ifdef __cplusplus
extern "C" {
#endif
#include "redis/src/zmalloc.h"
#ifdef __cplusplus
}
#endif

/////////////////////////////////////////////////////////////////////////////////////

HWND g_hwnd;
int is_chld = 0;

int sdl_main(int, char**);
int win32_main(int argc, char *argv[]);
int win32_docking(int, char**);
int win32_docking2(int, char**);
int test_nti_imgui_main(int, char**);
int win32_docking_gl3(int, char**);
int win32_dx9_main(int, char**);

static void outOfMemoryHandler(size_t allocation_size) {}

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	int i, rc;
	setlocale(LC_COLLATE, "");
	/* init COM */
	HRESULT Hr = CoInitialize(NULL);
	if (FAILED(Hr)) return 0;

	/* init winsock */
	WSADATA wsd;
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0) {
		fprintf(stdout, "%s: error WSAStartup, err=%d\n", __FUNCTION__, WSAGetLastError());
		return 1;
	}

	/* alloc console for debug if needed */
#ifndef NDEBUG
	//AllocConsole();
	//FILE * fp = NULL;
	//freopen_s(&fp, "CONOUT$", "w+t", stdout);
	//     freopen_s(&fp, "CONOUT$", "w+t", stderr);
	//     fprintf(stdout, "%s: cmdline='%s'\n", __FUNCTION__, lpCmdLine);
#endif /* NDEBUG */

	hp_log(stdout, "%s: build at %s %s\n", __FUNCTION__, __DATE__, __TIME__);

	/* init argc,argv, as WinMain haven't provide them */
	char cmdline[512] = "";
	cmdline[0] = '\0';
	strncpy(cmdline, GetCommandLineA(), sizeof(cmdline));
#ifndef NDEBUG
	/* sample: 'foo -h172.168.1.171 -p9200' */
	fprintf(stdout, "%s: cmdline=%d/'%s'\n", __FUNCTION__, (int)strlen(cmdline), cmdline);
#endif /* NDEBUG */

	int argc = 0;
	char * argv[512] = { 0 };
	argc = hp_opt_argv(cmdline, argv);
#ifndef NDEBUG
	fprintf(stdout, "%s: argc=%d, argv=%p\n", __FUNCTION__, argc, argv);
	for (i = 0; i < argc; ++i)
		fprintf(stdout, "%s: argv[%d]=%s\n", __FUNCTION__, i, argv[i]);
#endif /* NDEBUG */

	/* run test? */
	sds test = 0;

	/* parse argc/argv */
	struct optparse_long longopts[] = {
		{"test", 't', OPTPARSE_REQUIRED},
		{"help", 'h', OPTPARSE_NONE},
		{0}
	};
	struct optparse options;
	optparse_init(&options, argv);

    for (; (rc= optparse_long(&options, longopts, NULL)) != -1; ) {
        switch (rc) {
        case 't':
			test = sdsnew(options.optarg ? options.optarg : "");
            break;
		case 'h':
		case '?':
			fprintf(stdout, "test for nti56acad.arx\n"
				"Usage: %s --test=testname[,testname2]\n"
				, argv[0]);
			break;
		}
	}

	if(test)
		rc = hp_test(test, argc, argv, 0, 1);
	//test_hp_err_main(argc, argv);
	//hp_iocp_test_main(argc, argv);

	sdsfree(test);

	/* clean */
	WSACleanup();
	::CoUninitialize();

	fprintf(stdout, "%s: exited, Press ANY key to close\n", __FUNCTION__);
	_getch();
	return rc;
	//return win32_dx9_main(0, 0);
}
