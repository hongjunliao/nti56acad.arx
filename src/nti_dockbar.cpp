/*!
 * This file is PART of nti56acad project
 * @author hongjun.liao <docici@126.com>, @date 2020/09/11
 *
 * dock ctrl bar
 * */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "stdafx.h"
#include "nti_dockbar.h" //nti_dockbar
#include "nti_imgui.h"		//nti_imgui_create

/////////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(nti_dockbar, nti_dockbase)
	//{{AFX_MSG_MAP(nti_dockbar)
	ON_WM_PAINT()   
	//}}AFX_MSG_MAP   
END_MESSAGE_MAP()

BOOL nti_dockbar::Create(CWnd* pParent, LPCTSTR lpszTitle)   
{   
#ifndef NTI56_WITHOUT_ARX
	CString strWndClass;
	strWndClass = AfxRegisterWndClass(CS_DBLCLKS, LoadCursor(NULL, IDC_ARROW));
	CRect rect(0, 0, 250, 250);

	if (!nti_dockbase::Create(strWndClass, lpszTitle, WS_VISIBLE|WS_CHILD|WS_CLIPCHILDREN,rect,pParent,0))
	{   
		return (FALSE);   
	}
#else
	if (!CToolBar::Create(pParent))
	{
		return (FALSE);
	}
#endif //NTI56_WITHOUT_ARX		

	if(nti_imgui_create(m_hWnd) != 0)
		return FALSE;

	return (TRUE);   
}   

void nti_dockbar::OnPaint()
{
	nti_imgui_paint();
	nti_dockbase::OnPaint();

	//Invalidate(0);
}