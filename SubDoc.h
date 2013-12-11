// SubDoc.h : interface of the CSubDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SUBDOC_H__D55C4369_FDD2_11D2_8800_00A0C9062EBA__INCLUDED_)
#define AFX_SUBDOC_H__D55C4369_FDD2_11D2_8800_00A0C9062EBA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Cell;
class Average;

class CSubDoc : public CDocument
{
protected: // create from serialization only
	CSubDoc();
	DECLARE_DYNCREATE(CSubDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSubDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

// Implementation
private:
	int level;
public:
	Cell *editCell, *saveCell;
	Average *average;

	void Resubdivide();
	void SetLevel( int level );
	void SetAverage( Average *a );
public:
	virtual ~CSubDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CSubDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SUBDOC_H__D55C4369_FDD2_11D2_8800_00A0C9062EBA__INCLUDED_)
