
// nti56acadmfc.h : nti56acadmfc Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������


// Cnti56acadmfcApp:
// �йش����ʵ�֣������ nti56acadmfc.cpp
//

class Cnti56acadmfcApp : public CWinApp
{
public:
	Cnti56acadmfcApp();


// ��д
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// ʵ��

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern Cnti56acadmfcApp theApp;
