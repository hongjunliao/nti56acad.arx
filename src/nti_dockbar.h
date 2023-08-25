/*!
 * This file is PART of nti56acad.arx project
 * @author hongjun.liao <docici@126.com>, @date 2020/09/11
 *
 * dock ctrl bar
 *
 * @date 2023/08/25 updated using CAcUiDockControlBar on objectarx and CDockablePane on MFC
 * */

#pragma once

#ifdef NTI56_ARX
#define nti_dockbase CAcUiDockControlBar
#else
#define nti_dockbase CDockablePane
#endif //#ifdef NTI56_ARX

class nti_dockbar : public nti_dockbase
{
protected:
	HWND	   m_hWnd;
public:
	nti_dockbar() noexcept;
	virtual ~nti_dockbar();
#ifdef NTI56_ARX
	virtual BOOL CreateControlBar(LPCREATESTRUCT lpCreateStruct);
	virtual void SizeChanged(CRect * /*lpRect*/, BOOL /*bFloating*/, int /*flags*/);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
#else
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
#endif //#ifdef NTI56_ARX

	afx_msg void OnDestroy();
	DECLARE_MESSAGE_MAP()
};
