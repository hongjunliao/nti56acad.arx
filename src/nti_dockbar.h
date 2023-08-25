/*!
 * This file is PART of nti56acad project
 * @author hongjun.liao <docici@126.com>, @date 2020/09/11
 *
 * dock ctrl bar
 * */

#pragma once

#ifndef NTI56_WITHOUT_ARX
#define nti_dockbase CAcUiDockControlBar
#else
#define nti_dockbase CDockablePane
#endif //NTI56_WITHOUT_ARX

class nti_dockwnd : public CView {
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//afx_msg void OnDestroy();
	//afx_msg void OnPaint();
	//DECLARE_MESSAGE_MAP()
	virtual void OnDraw(CDC* pDC);
};

class nti_dockbar : public nti_dockbase
{
protected:
	nti_dockwnd m_wnd;
	HWND	   m_hWnd;
public:
	nti_dockbar() noexcept;
	virtual ~nti_dockbar();

protected:

// 重写
public:
	//virtual BOOL PreTranslateMessage(MSG* pMsg);
//    virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
protected:
//	virtual void PaintControlBar(CDC *pDC);
	virtual BOOL CreateControlBar(LPCREATESTRUCT lpCreateStruct);
	virtual void SizeChanged(CRect * /*lpRect*/, BOOL /*bFloating*/, int /*flags*/);
//	virtual bool OnClosing();
	//virtual void GetFloatingMinSize(long *pnMinWidth, long *pnMinHeight);
	//virtual BOOL AddCustomMenuItems(LPARAM hMenu);
	//virtual void OnUserSizing(UINT /*nSide*/, LPRECT /*pRect*/) {};

protected:
//	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    //afx_msg void OnDestroy();
	//afx_msg void OnSize(UINT nType, int cx, int cy);
	//afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	DECLARE_MESSAGE_MAP()
};
