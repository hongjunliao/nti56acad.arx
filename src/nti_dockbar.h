/*!
 * This file is PART of nti56acad project
 * @author hongjun.liao <docici@126.com>, @date 2020/09/11
 *
 * dock ctrl bar
 * */
#ifndef NTI_DOCKBAR_H
#define NTI_DOCKBAR_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "stdafx.h"
#ifndef NTI56_WITHOUT_ARX
#include "acuidock.h" //CAcUiDockControlBar
#endif //NTI56_WITHOUT_ARX

#include "nti_cmn.h"	//nti_wnddata
/////////////////////////////////////////////////////////////////////////////////////

class nti_dockbar : public nti_dockbase{
public:
	nti_dockbar();
	virtual ~nti_dockbar();
public:
	nti_wnddata * wnddata;
public:
	//virtual BOOL Create(CWnd* pParent, LPCTSTR lpszTitle); 
//	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
protected: 
	//{{AFX_MSG(CDockControlBar) 
	//afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//afx_msg void OnPaint();
	//afx_msg void OnTimer(UINT_PTR nIDEvent);
	//afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG 
	DECLARE_MESSAGE_MAP() 

	//virtual void AssertValid() const { } 
};
#endif //NTI_DOCKBAR_H
