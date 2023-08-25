
#pragma once

#include "ViewTree.h"
#include "ClassView.h"

class CImguiPane : public CDockablePane
{
public:
	CImguiPane() noexcept;
	virtual ~CImguiPane();

protected:

// 重写
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
//    virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
};
