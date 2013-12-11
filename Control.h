#if !defined(AFX_CONTROL_H__D55C4372_FDD2_11D2_8800_00A0C9062EBA__INCLUDED_)
#define AFX_CONTROL_H__D55C4372_FDD2_11D2_8800_00A0C9062EBA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Control.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CControl dialog
class CSubView;

class CControl : public CDialog
{
private:
	CSubView *view;
// Construction
public:
	CControl(CSubView* pParent = NULL);   // standard constructor
	virtual void OnOK();
// Dialog Data
	//{{AFX_DATA(CControl)
	enum { IDD = IDD_CONTROL };
	CComboBox	m_Substyle;
	CSliderCtrl	m_LevelSlider;
	BOOL	m_EditLoRes;
	BOOL	m_MoveLoRes;
	BOOL	m_Wireframe;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CControl)
	public:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CControl)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeSubstyle();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONTROL_H__D55C4372_FDD2_11D2_8800_00A0C9062EBA__INCLUDED_)
