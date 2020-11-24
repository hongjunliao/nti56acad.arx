/*!
 * This file is PART of nti56acad project
 * @author hongjun.liao <docici@126.com>, @date 2020/11/24
 *
 * blocks wnd
 * */
#ifndef NTI_DATALINKSBAR_H
#define NTI_DATALINKSBAR_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "stdafx.h"
#include "sds/win32_sds.h"
/////////////////////////////////////////////////////////////////////////////////////

class nti_datalinksdlg : public CDialogEx {
public:
	nti_datalinksdlg();
	virtual ~nti_datalinksdlg();
public:
	bool m_open;
	sds m_name;
public:
	void render();
protected:
	//virtual BOOL Create(CWnd* pParent, LPCTSTR lpszTitle); 
	//virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnInitDialog();
	virtual void PostNcDestroy();
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
#endif //NTI_DATALINKSBAR_H
