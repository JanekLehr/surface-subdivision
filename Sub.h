// Sub.h : main header file for the SUB application
//

#if !defined(AFX_SUB_H__D55C4363_FDD2_11D2_8800_00A0C9062EBA__INCLUDED_)
#define AFX_SUB_H__D55C4363_FDD2_11D2_8800_00A0C9062EBA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CSubApp:
// See Sub.cpp for the implementation of this class
//

class CSubApp : public CWinApp
{
public:
	CSubApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSubApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CSubApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SUB_H__D55C4363_FDD2_11D2_8800_00A0C9062EBA__INCLUDED_)
