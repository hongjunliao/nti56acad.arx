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

#ifdef _MSC_VER

#include "sds/win32_interop/win32_types.h"

#endif /* _MSC_VER */

//////////////////////////////////////////////////////////////////////////////
//
// Check if the build is DEBUG version and it's intended
// to be used with Non-DEBUG AutoCAD.
// In this case, for MFC header files, we need to undefine
// _DEBUG symbol
// Read this project readme.txt for more detail
//#define AC_FULL_DEBUG
#if defined( _DEBUG) && !defined(AC_FULL_DEBUG)
#pragma message("Building debug modeless.arx to be used with release AutoCAD")
#define _DEBUG_THIS_ONLY
#undef _DEBUG
#endif

#ifndef WINVER
#define WINVER 0x501
#endif

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxcmn.h>

// Turn on the _DEBUG symbol if it was defined, before including
// non-MFC header files.
//
#ifdef _DEBUG_THIS_ONLY
#define _DEBUG
#undef _DEBUG_THIS_ONLY
#endif

#ifndef NTI56_WITHOUT_ARX

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

#if defined(_DEBUG) && !defined(AC_FULL_DEBUG)
#define _DEBUG_WAS_DEFINED
#pragma message ("     Compiling MFC / STL / ATL header files in release mode.")
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