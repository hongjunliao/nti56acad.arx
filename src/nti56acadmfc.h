
// nti56acadmfc.h : nti56acadmfc 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"       // 主符号


// Cnti56acadmfcApp:
// 有关此类的实现，请参阅 nti56acadmfc.cpp
//

class Cnti56acadmfcApp : public CWinApp
{
public:
	Cnti56acadmfcApp();


// 重写
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 实现

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern Cnti56acadmfcApp theApp;
