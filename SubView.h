// SubView.h : interface of the CSubView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SUBVIEW_H__D55C436B_FDD2_11D2_8800_00A0C9062EBA__INCLUDED_)
#define AFX_SUBVIEW_H__D55C436B_FDD2_11D2_8800_00A0C9062EBA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "view.hh"
#include "pole.hh"
#include "glrender.hh"

class CControl;
class Cell;
class Vertex;

class CSubView : public CView
{
protected: // create from serialization only
	CSubView();
	DECLARE_DYNCREATE(CSubView)

// Attributes
public:
	CSubDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSubView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSubView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

private:
	HDC m_hDC;

	CControl *m_Control;

	ButtonStack *button_stack;
	View *view;
	GerbilPole *pole;
	
	bool wireframe;

	static const double radius;
	static const double znear;	// near and far clipping planes
	static const double zfar;

	void Render();

	void OnButtonDown(int button, UINT nFlags, CPoint point);
	void OnButtonUp(UINT nFlags, CPoint point);

	int height;
	Vertex *pickVertex;

	// model display list caching
	bool modelCached;
	int modelDisplayList;
	int lastDrawState;
protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CSubView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnFileSaveImage();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in SubView.cpp
inline CSubDoc* CSubView::GetDocument()
   { return (CSubDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SUBVIEW_H__D55C436B_FDD2_11D2_8800_00A0C9062EBA__INCLUDED_)
