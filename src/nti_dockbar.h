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

/////////////////////////////////////////////////////////////////////////////////////

#ifndef NTI56_WITHOUT_ARX
#define nti_dockbase CAcUiDockControlBar
#else
#define nti_dockbase CToolBar
#endif //NTI56_WITHOUT_ARX	

class nti_dockbar : public nti_dockbase{
public:
	virtual BOOL Create(CWnd* pParent, LPCTSTR lpszTitle); 
protected: 
	//{{AFX_MSG(CDockControlBar) 
	afx_msg void OnPaint(); 
	//}}AFX_MSG 
	DECLARE_MESSAGE_MAP() 
};
#endif //NTI_DOCKBAR_H
