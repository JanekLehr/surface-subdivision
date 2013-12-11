// SubView.cpp : implementation of the CSubView class
//

#include "stdafx.h"
#include "Sub.h"

#include "SubDoc.h"
#include "SubView.h"
#include "Control.h"

#include "gl/gl.h"
#include "gl/glu.h"

#include "view.hh"
#include "pole.hh"
#include "glrender.hh"
#include "subsurf.hh"

void setNormals(Cell *c);

#include "cell/cell.hh"

const double CSubView::radius = 0.05;
const double CSubView::znear = 0.5;	// near and far clipping planes
const double CSubView::zfar = 60.0;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSubView

IMPLEMENT_DYNCREATE(CSubView, CView)

BEGIN_MESSAGE_MAP(CSubView, CView)
	//{{AFX_MSG_MAP(CSubView)
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_FILE_SAVE_IMAGE, OnFileSaveImage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSubView construction/destruction

CSubView::CSubView() : wireframe( false )
{
	m_Control = 0;

	button_stack = 0;
	view = 0;
	pole = 0;

	modelDisplayList = 0;
	modelCached = false;
}

CSubView::~CSubView()
{
	delete m_Control;

	delete button_stack;
	delete view;
	delete pole;
}

BOOL CSubView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	m_Control = new CControl( this );
	if( m_Control->GetSafeHwnd() == 0 )
		m_Control->Create( IDD_CONTROL );
	
	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CSubView drawing

void
CSubView::Render()
{
	CSubDoc* doc = GetDocument();

	GLfloat mat_ambient[] = {0.1f,0.1f,0.1f,1.0f};
	GLfloat mat_diffuse[] = {0.7f,0.7f,0.7f,1.0f};

	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);

	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_FOG);

	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0);
   
	if( m_Control->m_Wireframe )
	{
		glDisable( GL_LIGHTING );
		glShadeModel( GL_FLAT );
	}
	else
	{
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glShadeModel( GL_SMOOTH );
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_COLOR_MATERIAL);
	view->glSetup();		// sets light position & other stuff
	pole->glRender(NULL);
	view->glRender();

	CellVertexIterator vertices( doc->saveCell );
	Vertex *v;
	while( (v = vertices.next()) != 0 )
		if( v == pickVertex )
			sphere( 0,1,1, v->pos, radius );
		else
			sphere( 1,1,1, v->pos, radius );

	int drawState = 0; // indicates whether we are drawing the lores or hires, wire or fill
	Cell *drawCell;
	if( view->active() && m_Control->m_MoveLoRes || pole->active() && m_Control->m_EditLoRes )
	{
		drawCell = doc->saveCell;
		drawState |= 1;
	}
	else
	{
		drawCell = doc->editCell;
	}

    if( m_Control->m_Wireframe )
		drawState |= 2;
	
	if( !modelDisplayList )
		modelDisplayList = glGenLists(1);

	if( modelCached && drawState == lastDrawState )
	{
		// hit
		glCallList( modelDisplayList );
	}
	else
	{
		// miss
		glNewList( modelDisplayList, GL_COMPILE_AND_EXECUTE );
		if( m_Control->m_Wireframe )
			drawWireframe( drawCell );
		else
			drawFilled( drawCell );
		glEndList();
		modelCached = true;
		lastDrawState = drawState;
	}

	glDisable(GL_COLOR_MATERIAL);
	glFinish();
}

void CSubView::OnDraw(CDC* pDC)
{
	Render();
	::SwapBuffers( m_hDC );
}

/////////////////////////////////////////////////////////////////////////////
// CSubView diagnostics

#ifdef _DEBUG
void CSubView::AssertValid() const
{
	CView::AssertValid();
}

void CSubView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSubDoc* CSubView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSubDoc)));
	return (CSubDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSubView message handlers


void CSubView::OnInitialUpdate() 
{
	m_Control->m_LevelSlider.SetPos( 0 );
	CView::OnInitialUpdate();
}

void CSubView::OnSize(UINT nType, int cx, int cy) 
{
	if( view )
		view->resize( cx, cy );
	height = cy;
	Invalidate( FALSE );
}

void CSubView::OnButtonDown( int button, UINT nFlags, CPoint point )
{
	// If another button is already down, then just ignore this.
	if( !button_stack->count() )
	{
		point.y = height - point.y; // Use lower-left hand origin, like normal people.
		if( nFlags & MK_SHIFT )
		{
			button_stack->push( button, MShift );
			pickVertex = control_point_pick( GetDocument()->saveCell, view, point.x, point.y );
			if( pickVertex )
				pole->begin( point.x,point.y,pickVertex->pos );
		}
		else if( nFlags & MK_CONTROL )
		{
			button_stack->push( button, MControl );
			pickVertex = control_point_pick( GetDocument()->saveCell, view, point.x, point.y );
			if( pickVertex )
				pole->begin( point.x,point.y,pickVertex->pos );
		}
		else
		{
			button_stack->push( button, 0 );
			view->begin( point.x,point.y );
			Invalidate( FALSE );
		}
		SetCapture();
	}
}

void CSubView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	OnButtonDown( 0, nFlags, point );
}

void CSubView::OnMButtonDown(UINT nFlags, CPoint point) 
{
	OnButtonDown( 1, nFlags, point );
}

void CSubView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	OnButtonDown( 2, nFlags, point );
}

void CSubView::OnMouseMove(UINT nFlags, CPoint point) 
{
	if( view->active() )
	{
		point.y = height - point.y; // Use lower-left hand origin, like normal people.
		view->update(point.x,point.y);
		Invalidate(FALSE);
	}
	else if( pole->active() )
	{
		point.y = height - point.y; // Use lower-left hand origin, like normal people.
		ASSERT( pickVertex );
		pole->update( point.x,point.y,pickVertex->pos );
		if( !m_Control->m_EditLoRes )
			GetDocument()->Resubdivide();
		else
			GetDocument()->UpdateAllViews(0); // the document was just modified by pole->update.
		Invalidate(FALSE);
	}
}

void CSubView::OnButtonUp(UINT nFlags, CPoint point)
{
	// Once we have a button down, only call it a button up if each button goes
	// up.
	if( !(nFlags & ( MK_LBUTTON | MK_MBUTTON | MK_RBUTTON )) )
	{
		if( view->active() )
		{
			point.y = height - point.y; // Use lower-left hand origin, like normal people.
			view->end(point.x,point.y);
		}
		else if( pole->active() )
		{
			point.y = height - point.y; // Use lower-left hand origin, like normal people.
			pole->end(point.x,point.y);
			GetDocument()->Resubdivide();
			pickVertex = 0;
		}

		button_stack->reset();
		ReleaseCapture();
		Invalidate(FALSE);
	}
}

void CSubView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	OnButtonUp(nFlags, point);
}

void CSubView::OnMButtonUp(UINT nFlags, CPoint point) 
{
	OnButtonUp(nFlags, point);;
}

void CSubView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	OnButtonUp(nFlags, point);
}

BOOL CSubView::OnEraseBkgnd(CDC* pDC) 
{
	// How about we not erase jack?
	// return CView::OnEraseBkgnd(pDC);
	return TRUE;
}

BOOL CSubView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	if( ! CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext) )
		return 0;
		
	// Initialize OpenGL

	PIXELFORMATDESCRIPTOR pfd = {
		sizeof( PIXELFORMATDESCRIPTOR ),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		24,
		0, 0, 0, 0, 0, 0,
		0, 0, 0, 
		0, 0, 0, 0, 
		16,
		0, 0, 
		PFD_MAIN_PLANE,
		0, 0, 0, 0 };
	int pixelformat;
	
	HGLRC m_hRC;

	m_hDC = ::GetDC(this->m_hWnd);

	if ( 0 == ( pixelformat = ::ChoosePixelFormat( m_hDC, &pfd ) ) ) {
		TRACE( "ChoosePixelFormat failed\n" );
		return 0;
	}

	if ( FALSE == ::SetPixelFormat( m_hDC, pixelformat, &pfd ) ) {
		TRACE( "SetPixelFormat failed\n" );
		return 0;
	}

	m_hRC = ::wglCreateContext( m_hDC );

	::wglMakeCurrent( m_hDC, m_hRC );
	glEnable( GL_DEPTH_TEST );
	glDrawBuffer( GL_BACK );
	glReadBuffer( GL_BACK );
	glClearColor( 0, 0, 0, 1 );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	button_stack = new ButtonStack();
	view = new View(button_stack);
	int width = rect.right-rect.left;
	int height = rect.bottom-rect.top;
    view->perspective( width, height, 40, double(width)/height, znear, zfar );
    view->lookAt( 1,2,8, 0,0,0, 0 );

    pole = new GerbilPole(view);

	return TRUE;
}

void CSubView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	modelCached = false;
	Invalidate( FALSE );
}

void CSubView::OnFileSaveImage() 
{
	TRACE( "save image\n" );
	CFileDialog filedialog( FALSE, "bmp" );
	if( filedialog.DoModal() )
	{
		CRect wrect;
		GetWindowRect( wrect );
		int width = wrect.Width();
		int height = wrect.Height();
		unsigned char *buffer = new unsigned char[ width * height * 3 ];
		Render(); // into back buffer w/o swap
		glPixelStorei( GL_PACK_ALIGNMENT, 1 );
		glReadPixels( 0,0,width,height,GL_RGB,GL_UNSIGNED_BYTE,buffer );

		BITMAPFILEHEADER bmfh;
		BITMAPINFOHEADER bmih;

		int bytes, pad;
		bytes = width * 3;
		pad = (bytes%4) ? 4-(bytes%4) : 0;
		bytes += pad;
		bytes *= height;

		bmfh.bfType = 0x4d42;    // "BM"
		bmfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + bytes;
		bmfh.bfReserved1 = 0;
		bmfh.bfReserved2 = 0;
		bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

		bmih.biSize = sizeof(BITMAPINFOHEADER);
		bmih.biWidth = width;
		bmih.biHeight = height;
		bmih.biPlanes = 1;
		bmih.biBitCount = 24;
		bmih.biCompression = BI_RGB;
		bmih.biSizeImage = 0;
		bmih.biXPelsPerMeter = (int)(100 / 2.54 * 72);
		bmih.biYPelsPerMeter = (int)(100 / 2.54 * 72);
		bmih.biClrUsed = 0;
		bmih.biClrImportant = 0;

		CFile file( filedialog.GetPathName(), CFile::modeWrite | CFile::modeCreate );
		file.Write( &bmfh, sizeof(BITMAPFILEHEADER) );
		file.Write( &bmih, sizeof(BITMAPINFOHEADER) );

		bytes /= height;
		unsigned char* scanline = new unsigned char[ bytes ];
		for ( int j = 0; j < height; ++j )
		{
			memcpy( scanline, buffer + j*3*width, bytes );
			for ( int i = 0; i < width; ++i )
			{
				unsigned char temp = scanline[i*3];
				scanline[i*3] = scanline[i*3+2];
				scanline[i*3+2] = temp;
			}
			file.Write( scanline, bytes );
		}
		delete [] scanline;
		delete [] buffer;
		file.Close();
	}
}
