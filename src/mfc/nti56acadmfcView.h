
// nti56acadmfcView.h : Cnti56acadmfcView ��Ľӿ�
//

#pragma once


class Cnti56acadmfcView : public CView
{
protected: // �������л�����
	Cnti56acadmfcView();
	DECLARE_DYNCREATE(Cnti56acadmfcView)

// ����
public:
	Cnti56acadmfcDoc* GetDocument() const;

// ����
public:

// ��д
public:
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// ʵ��
public:
	virtual ~Cnti56acadmfcView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // nti56acadmfcView.cpp �еĵ��԰汾
inline Cnti56acadmfcDoc* Cnti56acadmfcView::GetDocument() const
   { return reinterpret_cast<Cnti56acadmfcDoc*>(m_pDocument); }
#endif

