// stdafx.h: 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 项目特定的包含文件
//

#pragma once

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头文件中排除极少使用的内容

//////////////////////////////////////////////////////////////////////////////
//
// Check if the build is DEBUG version and it's intended
// to be used with Non-DEBUG AutoCAD.
// In this case, for MFC header files, we need to undefine
// _DEBUG symbol
// Read this project readme.txt for more detail
//#define AC_FULL_DEBUG
#if defined( _DEBUG) && !defined(AC_FULL_DEBUG)
#define _DEBUG_THIS_ONLY
#undef _DEBUG
#endif

#ifndef WINVER
#define WINVER 0x501
#endif

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxcmn.h>
#include <afxframewndex.h>
#include <afxcontrolbars.h>

// For compilers that support precompilation, includes "wx/wx.h".
//#include "wx/wxprec.h"

// Turn on the _DEBUG symbol if it was defined, before including
// non-MFC header files.
//
#ifdef _DEBUG_THIS_ONLY
#define _DEBUG
#undef _DEBUG_THIS_ONLY
#endif

#ifdef NTI56_ARX

#include <dbapserv.h>
#include <rxregsvc.h>
#include <AcExtensionModule.h>
#include <stdio.h>
#include <stdarg.h>
#include <aced.h>
#include <adslib.h>
#include <dbapserv.h>
#include <rxmfcapi.h>
#include <adeskabb.h>
#include <axlock.h>
#include <acdocman.h>
#include "aced.h"
#include "dbsymtb.h"
#include "adslib.h"
#include "dbents.h"
#include "rxregsvc.h"

#if defined(_DEBUG) && !defined(AC_FULL_DEBUG)
#define _DEBUG_WAS_DEFINED
#undef _DEBUG
#endif
#include <aced.h>
#ifdef _DEBUG_WAS_DEFINED
#define _DEBUG
#endif

#include <dbsymtb.h>
#include <dbapserv.h>
#include <adslib.h>
#include <adui.h>
#include <acui.h>

#include <arxHeaders.h>

#endif //NTI56_WITHOUT_ARX

#include <assert.h> //assert
#include "framework.h"
#include <afxdisp.h>        // MFC 自动化类

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // 功能区和控件条的 MFC 支持

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


#ifdef _DEBUG_WAS_DEFINED
#define _DEBUG
#endif
