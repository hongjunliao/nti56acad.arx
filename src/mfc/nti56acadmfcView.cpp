
// nti56acadmfcView.cpp : Cnti56acadmfcView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
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
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &Cnti56acadmfcView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// Cnti56acadmfcView 构造/析构

Cnti56acadmfcView::Cnti56acadmfcView()
{
	// TODO: 在此处添加构造代码

}

Cnti56acadmfcView::~Cnti56acadmfcView()
{
}

BOOL Cnti56acadmfcView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// Cnti56acadmfcView 绘制

void Cnti56acadmfcView::OnDraw(CDC* /*pDC*/)
{
	Cnti56acadmfcDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
}


// Cnti56acadmfcView 打印


void Cnti56acadmfcView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL Cnti56acadmfcView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void Cnti56acadmfcView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void Cnti56acadmfcView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
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


// Cnti56acadmfcView 诊断

#ifdef _DEBUG
void Cnti56acadmfcView::AssertValid() const
{
	CView::AssertValid();
}

void Cnti56acadmfcView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

Cnti56acadmfcDoc* Cnti56acadmfcView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(Cnti56acadmfcDoc)));
	return (Cnti56acadmfcDoc*)m_pDocument;
}
#endif //_DEBUG


// Cnti56acadmfcView 消息处理程序
