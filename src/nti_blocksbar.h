/*!
 * This file is PART of nti56acad project
 * @author hongjun.liao <docici@126.com>, @date 2020/11/24
 *
 * blocks wnd
 * */
#ifndef NTI_BLOCKSAR_H
#define NTI_BLOCKSAR_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "stdafx.h"
#ifndef NTI56_WITHOUT_ARX
#include "acuidock.h" //CAcUiDockControlBar
#endif //NTI56_WITHOUT_ARX

#include "nti_cmn.h"
#include "sds/win32_sds.h"
/////////////////////////////////////////////////////////////////////////////////////

class nti_blocksbar : public nti_dockbase{
public:
	nti_blocksbar();
	virtual ~nti_blocksbar();
public:
	bool m_open;
	sds m_name;
public:
	void render();
public:
	//virtual BOOL Create(CWnd* pParent, LPCTSTR lpszTitle); 
#ifdef NTI56_WITHOUT_ARX
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
#endif
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
#endif //NTI_BLOCKSAR_H