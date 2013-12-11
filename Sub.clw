; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CSubView
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "Sub.h"
LastPage=0

ClassCount=6
Class1=CSubApp
Class2=CSubDoc
Class3=CSubView
Class4=CMainFrame

ResourceCount=3
Resource1=IDR_MAINFRAME
Resource2=IDD_ABOUTBOX
Class5=CAboutDlg
Class6=CControl
Resource3=IDD_CONTROL

[CLS:CSubApp]
Type=0
HeaderFile=Sub.h
ImplementationFile=Sub.cpp
Filter=N

[CLS:CSubDoc]
Type=0
HeaderFile=SubDoc.h
ImplementationFile=SubDoc.cpp
Filter=N
BaseClass=CDocument
VirtualFilter=DC
LastObject=CSubDoc

[CLS:CSubView]
Type=0
HeaderFile=SubView.h
ImplementationFile=SubView.cpp
Filter=C
BaseClass=CView
VirtualFilter=VWC
LastObject=CSubView


[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T
LastObject=ID_FILE_SAVE_IMAGE




[CLS:CAboutDlg]
Type=0
HeaderFile=Sub.cpp
ImplementationFile=Sub.cpp
Filter=D
LastObject=CAboutDlg

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_SAVE_AS
Command5=ID_FILE_SAVE_IMAGE
Command6=ID_FILE_MRU_FILE1
Command7=ID_APP_EXIT
Command8=ID_EDIT_UNDO
Command9=ID_EDIT_CUT
Command10=ID_EDIT_COPY
Command11=ID_EDIT_PASTE
Command12=ID_APP_ABOUT
CommandCount=12

[ACL:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_UNDO
Command5=ID_EDIT_CUT
Command6=ID_EDIT_COPY
Command7=ID_EDIT_PASTE
Command8=ID_EDIT_UNDO
Command9=ID_EDIT_CUT
Command10=ID_EDIT_COPY
Command11=ID_EDIT_PASTE
Command12=ID_NEXT_PANE
Command13=ID_PREV_PANE
CommandCount=13

[DLG:IDD_CONTROL]
Type=1
Class=CControl
ControlCount=6
Control1=IDC_LEVELSLIDER,msctls_trackbar32,1342242821
Control2=IDC_STATIC,static,1342308352
Control3=IDC_SUBSTYLE,combobox,1344339970
Control4=IDC_MOVELORES,button,1342259203
Control5=IDC_EDITLORES,button,1342259203
Control6=IDC_WIREFRAME,button,1342259203

[CLS:CControl]
Type=0
HeaderFile=Control.h
ImplementationFile=Control.cpp
BaseClass=CDialog
Filter=D
LastObject=CControl
VirtualFilter=dWC

