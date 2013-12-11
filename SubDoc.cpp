// SubDoc.cpp : implementation of the CSubDoc class
//

#include "stdafx.h"
#include "Sub.h"

#include "SubDoc.h"

#include "cell/cell.hh"
#include "cell/obj.hh"
#include "average.hh"

int subdivideOnce( Cell * );
int triangulated( Cell * );

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSubDoc

IMPLEMENT_DYNCREATE(CSubDoc, CDocument)

BEGIN_MESSAGE_MAP(CSubDoc, CDocument)
	//{{AFX_MSG_MAP(CSubDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSubDoc construction/destruction

CSubDoc::CSubDoc()
{
	// TODO: add one-time construction code here
	editCell = saveCell = 0;
	level = 0;

	average = new AvgNOOP();
}

CSubDoc::~CSubDoc()
{
	if( editCell )
		Cell::kill( editCell );
	if( saveCell )
		Cell::kill( saveCell );

	delete average;
}

BOOL CSubDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	if( editCell )
		Cell::kill( editCell );
	if( saveCell )
		Cell::kill( saveCell );

		// default cell
    saveCell = Cell::makeTetrahedron();
    {
		CellVertexIterator vertices(saveCell);
		double sqrt2 = sqrt(2.0);

		Vertex *vertex1 = vertices.next();
		Vertex *vertex2 = vertices.next();
		Vertex *vertex3 = vertices.next();
		Vertex *vertex4 = vertices.next();

		vertex1->pos[0] = -sqrt2;  // probably off ???
		vertex1->pos[1] = -1.0;
		vertex1->pos[2] = -1.0;

		vertex2->pos[0] = sqrt2;
		vertex2->pos[1] = -1.0;
		vertex2->pos[2] = -1.0;

		vertex3->pos[0] = 0.0;
		vertex3->pos[1] = 1.0;
		vertex3->pos[2] = -1.0;

		vertex4->pos[0] = 0.0;
		vertex4->pos[1] = 0.0;
		vertex4->pos[2] = 1.0;
    }

	editCell = objCloneCell(saveCell);

	// just generate some reasonable normals for the base mesh
	AvgNOOP::genNormals( saveCell );
	AvgNOOP::genNormals( editCell );

	return TRUE;
}

void CSubDoc::Resubdivide()
{
	if( editCell )
		Cell::kill( editCell );
	editCell = objCloneCell( saveCell );
	std::cout << "level=" << level << std::endl;
    for( int i=0; i<level; ++i )
	{
		subdivideOnce( editCell );
		(*average)( editCell ); // operator()
	}
	if( !level )
	{
		// just generate some reasonable normals for the base mesh
		AvgNOOP::genNormals( editCell );
	}
	UpdateAllViews( 0 );
}

void CSubDoc::SetLevel( int lvl )
{
	level = lvl;
	Resubdivide();
}

void CSubDoc::SetAverage( Average *a )
{
	delete average;
	average = a;
	Resubdivide();
}

/////////////////////////////////////////////////////////////////////////////
// CSubDoc serialization

void CSubDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSubDoc diagnostics

#ifdef _DEBUG
void CSubDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSubDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSubDoc commands

BOOL CSubDoc::OnOpenDocument(LPCTSTR name) 
{
	Cell *cell = objReadCell(name);

	if( !cell )
		return FALSE;

	if( !triangulated(cell) )
	{
		Cell::kill( cell );
		return FALSE;
	}

	Cell::kill( editCell );
	Cell::kill( saveCell );

	saveCell = cell;
	editCell = objCloneCell( saveCell );
	
	// just generate some reasonable normals for the base mesh
	AvgNOOP::genNormals( saveCell );
	AvgNOOP::genNormals( editCell );

	level = 0;
	
	return TRUE;
}

BOOL CSubDoc::OnSaveDocument(LPCTSTR name) 
{
	objWriteCell( editCell, name );
	return TRUE;
}
