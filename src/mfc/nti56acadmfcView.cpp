
// nti56acadmfcView.cpp : Cnti56acadmfcView ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
#ifndef SHARED_HANDLERS
#include "nti56acadmfc.h"
#endif

#include "nti56acadmfcDoc.h"
#include "nti56acadmfcView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Cnti56acadmfcView

IMPLEMENT_DYNCREATE(Cnti56acadmfcView, CView)

BEGIN_MESSAGE_MAP(Cnti56acadmfcView, CView)
	// ��׼��ӡ����
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &Cnti56acadmfcView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// Cnti56acadmfcView ����/����

Cnti56acadmfcView::Cnti56acadmfcView()
{
	// TODO: �ڴ˴���ӹ������

}

Cnti56acadmfcView::~Cnti56acadmfcView()
{
}

BOOL Cnti56acadmfcView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return CView::PreCreateWindow(cs);
}

// Cnti56acadmfcView ����

void Cnti56acadmfcView::OnDraw(CDC* /*pDC*/)
{
	Cnti56acadmfcDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: �ڴ˴�Ϊ����������ӻ��ƴ���
}


// Cnti56acadmfcView ��ӡ


void Cnti56acadmfcView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL Cnti56acadmfcView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Ĭ��׼��
	return DoPreparePrinting(pInfo);
}

void Cnti56acadmfcView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��Ӷ���Ĵ�ӡǰ���еĳ�ʼ������
}

void Cnti56acadmfcView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��Ӵ�ӡ����е��������
}

void Cnti56acadmfcView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void Cnti56acadmfcView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// Cnti56acadmfcView ���

#ifdef _DEBUG
void Cnti56acadmfcView::AssertValid() const
{
	CView::AssertValid();
}

void Cnti56acadmfcView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

Cnti56acadmfcDoc* Cnti56acadmfcView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(Cnti56acadmfcDoc)));
	return (Cnti56acadmfcDoc*)m_pDocument;
}
#endif //_DEBUG


// Cnti56acadmfcView ��Ϣ�������
