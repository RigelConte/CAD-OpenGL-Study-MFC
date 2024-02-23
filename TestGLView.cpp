// TestGLView.cpp : implementation of the CTestGLView class
//

#include "stdafx.h"
#include "TestGL.h"

#include "TestGLDoc.h"
#include "gl/gl.h"
#include "gl/glu.h"
#include "GLEnabledView.h"
#include "TestGLView.h"
#include "OGLInfoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Vertices DataSets (convex polygons to tessellate)
// NOTE: the polygons will be automatically closed
		GLdouble star[5][3] = { 
				{0.2, 0.0, 0.0},
				{0.5, 0.9, 0.0},
				{0.8, 0.0, 0.0},
				{0.0, 0.6, 0.0},
				{1.0, 0.6, 0.0},
		};

		GLdouble Quad[4][3] = { 
				{0.0, 0.0, 0.0},
				{1.0, 0.0, 0.0},
				{1.0, 1.0, 0.0},
				{0.0, 1.0, 0.0},
		};
		GLdouble Triang[3][3] = { 
				{0.3, 0.3, 0.0},
				{0.7, 0.3, 0.0},
				{0.5, 0.7, 0.0},
		};

		GLdouble Triang2[3][3] = { 
				{0.5, 0.5, 0.0},
				{1.5, 1.0, 0.0},
				{1.5, 0.5, 0.0},
		};

/////////////////////////////////////////////////////////////////////////////
// CTestGLView

IMPLEMENT_DYNCREATE(CTestGLView, CGLEnabledView)

BEGIN_MESSAGE_MAP(CTestGLView, CGLEnabledView)
	
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_HELP_OGLINFO, OnHelpOglinfo)
	ON_COMMAND(ID_TESSELATOR_FILL, OnTesselatorFill)
	ON_UPDATE_COMMAND_UI(ID_TESSELATOR_FILL, OnUpdateTesselatorFill)
	ON_COMMAND(ID_TESSELATOR_NEG, OnTesselatorNeg)
	ON_UPDATE_COMMAND_UI(ID_TESSELATOR_NEG, OnUpdateTesselatorNeg)
	ON_COMMAND(ID_TESSELATOR_NONZERO, OnTesselatorNonzero)
	ON_UPDATE_COMMAND_UI(ID_TESSELATOR_NONZERO, OnUpdateTesselatorNonzero)
	ON_COMMAND(ID_TESSELATOR_ODDRULE, OnTesselatorOddrule)
	ON_UPDATE_COMMAND_UI(ID_TESSELATOR_ODDRULE, OnUpdateTesselatorOddrule)
	ON_COMMAND(ID_TESSELATOR_POS, OnTesselatorPos)
	ON_UPDATE_COMMAND_UI(ID_TESSELATOR_POS, OnUpdateTesselatorPos)
	ON_COMMAND(ID_CHANGE_SCENE, OnChangeScene)
	ON_COMMAND(ID_QUADRIC_FILL, OnQuadricFill)
	ON_COMMAND(ID_QUADRIC_FLAT, OnQuadricFlat)
	ON_COMMAND(ID_QUADRIC_INSIDE, OnQuadricInside)
	ON_COMMAND(ID_QUADRIC_LINE, OnQuadricLine)
	ON_COMMAND(ID_QUADRIC_NONE, OnQuadricNone)
	ON_COMMAND(ID_QUADRIC_OUTSIDE, OnQuadricOutside)
	ON_COMMAND(ID_QUADRIC_POINT, OnQuadricPoint)
	ON_COMMAND(ID_QUADRIC_SILHOUETTE, OnQuadricSilhouette)
	ON_COMMAND(ID_QUADRIC_SMOOTH, OnQuadricSmooth)
	ON_COMMAND(ID_ENVIR_FLAT, OnEnvirFlat)
	ON_COMMAND(ID_ENVIR_LIGHTING, OnEnvirLighting)
	ON_COMMAND(ID_ENVIR_SMOOTH, OnEnvirSmooth)
	ON_UPDATE_COMMAND_UI(ID_ENVIR_LIGHTING, OnUpdateEnvirLighting)
	ON_UPDATE_COMMAND_UI(ID_QUADRIC_FILL, OnUpdateQuadricFill)
	ON_UPDATE_COMMAND_UI(ID_QUADRIC_FLAT, OnUpdateQuadricFlat)
	ON_UPDATE_COMMAND_UI(ID_QUADRIC_INSIDE, OnUpdateQuadricInside)
	ON_UPDATE_COMMAND_UI(ID_QUADRIC_LINE, OnUpdateQuadricLine)
	ON_UPDATE_COMMAND_UI(ID_QUADRIC_NONE, OnUpdateQuadricNone)
	ON_UPDATE_COMMAND_UI(ID_QUADRIC_OUTSIDE, OnUpdateQuadricOutside)
	ON_UPDATE_COMMAND_UI(ID_QUADRIC_POINT, OnUpdateQuadricPoint)
	ON_UPDATE_COMMAND_UI(ID_QUADRIC_SILHOUETTE, OnUpdateQuadricSilhouette)
	ON_UPDATE_COMMAND_UI(ID_QUADRIC_SMOOTH, OnUpdateQuadricSmooth)
	ON_UPDATE_COMMAND_UI(ID_ENVIR_FLAT, OnUpdateEnvirFlat)
	ON_UPDATE_COMMAND_UI(ID_ENVIR_SMOOTH, OnUpdateEnvirSmooth)
	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestGLView construction/destruction

CTestGLView::CTestGLView():
	X_Angle(0.0), Y_Angle(0.0),
	TessWindRule(GLU_TESS_WINDING_ODD), TessFilling(TRUE),
	sceneselect(0),	quadricNormals(GLU_NONE),
	quadricDwStyle(GLU_LINE), quadricOrientation(GLU_OUTSIDE)

{
	// TODO: add construction code here

}

CTestGLView::~CTestGLView()
{
}

/////////////////////////////////////////////////////////////////////////////
// CTestGLView diagnostics

#ifdef _DEBUG
void CTestGLView::AssertValid() const
{
	CGLEnabledView::AssertValid();
}

void CTestGLView::Dump(CDumpContext& dc) const
{
	CGLEnabledView::Dump(dc);
}

CTestGLDoc* CTestGLView::GetDocument() 
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTestGLDoc)));
	return (CTestGLDoc*)m_pDocument;
}
#endif 

/////////////////////////////////////////////////////////////////////////////
// CTestGLView Overridables from CGLEnabledView

void CTestGLView::VideoMode(ColorsNumber & c, ZAccuracy & z, BOOL & dbuf)
{
	c=THOUSANDS;
	z=NORMAL;
	dbuf=TRUE;
}

void CTestGLView::OnCreateGL()
{

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.f,0.f,0.f,1.0f );
	glClearDepth(1.0f);

	float  color[] = {1.f, 1.f, 1.f, 1.f};		// RGBA color spec
	glLightfv(GL_LIGHT0, GL_DIFFUSE, color);

	float  ambient[] = {.3f, .3f, .3f, 1.f};	// RGBA color spec
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

	float pos[] = {1.f, 1.f, 1.f, 0.f};
	glLightfv(GL_LIGHT0, GL_POSITION, pos);
	glEnable(GL_LIGHT0);

	StartStockDListDef();
	glBegin(GL_LINES);
		// yellow x axis arrow
		glColor3f(1.f, 1.f, 0.f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(1.0f, 0.0f, 0.0f);
		glVertex3f(1.0f, 0.0f, 0.0f);
		glVertex3f(0.9f, 0.1f, 0.0f);
		glVertex3f(1.0f, 0.0f, 0.0f);
		glVertex3f(0.9f, -0.1f,0.0f);

		// cyan y axis arrow
		glColor3f(0.f, 1.f, 1.f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 1.0f, 0.0f);
		glVertex3f(0.0f, 1.0f, 0.0f);
		glVertex3f(0.1f, 0.9f, 0.0f);
		glVertex3f(0.0f, 1.0f, 0.0f);
		glVertex3f(-0.1f, 0.9f,0.0f);

		// magenta z axis arrow
		glColor3f(1.f, 0.f, 1.f);
		glVertex3f(0.0f,  0.0f, 0.0f);
		glVertex3f(0.0f,  0.0f, 1.0f);
		glVertex3f(0.0f,  0.0f, 1.0f);
		glVertex3f(0.0f,  0.1f, 0.9f);
		glVertex3f(0.0f,  0.0f, 1.0f);
		glVertex3f(0.0f, -0.1f, 0.9f);
	glEnd();
	EndStockListDef();

	anothercube.StartDef(); // <- Note, do not execute list immediately
		glBegin(GL_QUADS);
			glColor3f(0.4f,  .2f,  .7f);
			glVertex3f(0.1f, 0.1f, .1f);
			glVertex3f(0.6f, .1f,  .1f);
			glVertex3f(0.6f, 0.6f, .1f);
			glVertex3f(0.1f, 0.6f, .1f);
			glColor3f(.8f,   1.f,  .2f);
			glVertex3f(0.1f, 0.1f, .6f);
			glVertex3f(0.6f, .1f,  .6f);
			glVertex3f(0.6f, 0.6f, .6f);
			glVertex3f(0.1f, 0.6f, .6f);
		glEnd();
		glBegin(GL_QUAD_STRIP);
			glColor3f(0.4f,  .2f,  .7f);
			glVertex3f(0.1f, 0.1f, .1f);
			glVertex3f(0.1f, 0.1f, .6f);
			glColor3f(.8f,   1.f,  .2f);
			glVertex3f(0.6f, .1f,  .1f);
			glVertex3f(0.6f, .1f,  .6f);
			glColor3f(0.4f,  .2f,  .7f);
			glVertex3f(0.6f, 0.6f, .1f);
			glVertex3f(0.6f, 0.6f, .6f);
			glColor3f(.8f,   1.f,  .2f);
			glVertex3f(0.1f, 0.6f, .1f);
			glVertex3f(0.1f, 0.6f, .6f);
			glColor3f(0.4f,  .2f,  .7f);
			glVertex3f(0.1f, 0.1f, .1f);
			glVertex3f(0.1f, 0.1f, .6f);
		glEnd();
		anothercube.EndDef();
		BuildTessDispList();
		BuildQuadrDispList();
}

void CTestGLView::BuildTessDispList()
{
		CGLTesselator t;
		tessPolygon.StartDef();
		t.SetWindingRule(TessWindRule);
		t.SetFilling(TessFilling);

		// first polygon: a quadrilateral with a triangular hole
		t.StartDef();

		// define exterior
		t.AddVertexArray(Quad, 4);
		t.ContourSeparator();
		t.AddVertexArray(Triang, 3);
		t.EndDef();

		glTranslatef(1.f, 0.f, 0.f);
		t.StartDef();
		for (int c=0; c < 5; c++)
		{
			t.AddVertex(star[c]);
		};
		t.EndDef();

		// move "the 3d cursor" right again
		glTranslatef(1.f, 0.f, 0.f);

		// third polygon: a quadrilateral and an intersecting triangle
		// NOTE: the intersecting triangle has vertices in CCW order
		//       this has effect on positive and negative winding rule
		t.StartDef();
		t.AddVertexArray(Quad, 4);
		t.ContourSeparator();
		t.AddVertexArray(Triang2, 3);
		t.EndDef();
		// close disp list definition
		tessPolygon.EndDef();
}

void CTestGLView::BuildQuadrDispList()
{
	const int SECTIONS=16;
	const double RADIUS=.5;
	// construct a quadric object
	CGLQuadric q(quadricDwStyle, quadricNormals, quadricOrientation);

	// open disp list definition
	quadric.StartDef();
	
	// draw an azure sphere
	glColor3f(.2f, .5f, .8f);
	q.DrawSphere(RADIUS, SECTIONS, SECTIONS);
	glTranslatef(1.2f, 0.0f, 0.0f);
	// draw a maroon cylinder
	glColor3f(.8f, .5f, .2f);
	q.DrawCylinder(RADIUS,RADIUS, 1.0, SECTIONS, 2);
	glTranslatef(-2.4f, 0.0f, 0.0f);
	
	// draw a green disk
	glColor3f(.5f, .8f, .2f);
	q.DrawDisk(RADIUS - .3, RADIUS, SECTIONS, 2);
	// close disp list definition
	quadric.EndDef();
}

void CTestGLView::OnDrawGL()
{
	glPushMatrix();
	
	// rotate the objects of the given angle
	glRotated(X_Angle, 1.0, 0.0, 0.0);
	glRotated(Y_Angle, 0.0, 1.0, 0.0);
	
	// this should be self explanatory
	DrawStockDispLists();
	switch(sceneselect)
	{
	case 0:// simple scene
		glBegin(GL_QUAD_STRIP);
			glColor3f(1.0f, 0.0f, 1.0f);
			glVertex3f(-0.3f, 0.3f, 0.3f);
			glColor3f(1.0f, 0.0f, 0.0f);
			glVertex3f(-0.3f, -0.3f, 0.3f);
			glColor3f(1.0f, 1.0f, 1.0f);
			glVertex3f(0.3f, 0.3f, 0.3f);
			glColor3f(1.0f, 1.0f, 0.0f);
			glVertex3f(0.3f, -0.3f, 0.3f);
			glColor3f(0.0f, 1.0f, 1.0f);
			glVertex3f(0.3f, 0.3f, -0.3f);
			glColor3f(0.0f, 1.0f, 0.0f);
			glVertex3f(0.3f, -0.3f, -0.3f);
			glColor3f(0.0f, 0.0f, 1.0f);
			glVertex3f(-0.3f, 0.3f, -0.3f);
			glColor3f(0.0f, 0.0f, 0.0f);
			glVertex3f(-0.3f, -0.3f,  -0.3f);
			glColor3f(1.0f, 0.0f, 1.0f);
			glVertex3f(-0.3f, 0.3f, 0.3f);
			glColor3f(1.0f, 0.0f, 0.0f);
			glVertex3f(-0.3f, -0.3f, 0.3f);
		glEnd();
		glBegin(GL_QUADS);
			glColor3f(1.0f, 0.0f, 1.0f);
			glVertex3f(-0.3f, 0.3f, 0.3f);
			glColor3f(1.0f, 1.0f, 1.0f);
			glVertex3f(0.3f, 0.3f, 0.3f);
			glColor3f(0.0f, 1.0f, 1.0f);
			glVertex3f(0.3f, 0.3f, -0.3f);
			glColor3f(0.0f, 0.0f, 1.0f);
			glVertex3f(-0.3f, 0.3f, -0.3f);
			glColor3f(1.0f, 0.0f, 0.0f);
			glVertex3f(-0.3f, -0.3f, 0.3f);
			glColor3f(1.0f, 1.0f, 0.0f);
			glVertex3f(0.3f, -0.3f, 0.3f);
			glColor3f(0.0f, 1.0f, 0.0f);
			glVertex3f(0.3f, -0.3f, -0.3f);
			glColor3f(0.0f, 0.0f, 0.0f);
			glVertex3f(-0.3f, -0.3f, -0.3f);
		glEnd();
		
		// execute previously prepared displ. list 2 times
		// Note:All translations are relative (imagine to deal with a "3D cursor")
		glTranslatef(-1.35f, -0.2f, -0.2f);
		anothercube.Draw();
		glTranslatef(2.f, 0.f, 0.f);
		anothercube.Draw();
		break;
	case 1: // tesselators scene
		// move left to almost center the tesselators display list
		glTranslatef(-1.5f, 0.f, 0.1f);
		// choose the color and draw
		glColor3f(1.f, 1.f, 1.f);
		tessPolygon.Draw();
		break;
	case 2: // quadric scene
		quadric.Draw();
		break;
	};
	glPopMatrix();
}

/////////////////////////////////////////////////////////////////////////////
// CTestGLView message handlers

void CTestGLView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// remember where we clicked
	MouseDownPoint = point;
	SetCapture();
}

void CTestGLView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// forget where we clicked
	MouseDownPoint = CPoint(0, 0);
	// release mouse capture
	ReleaseCapture();
}

void CTestGLView::OnMouseMove(UINT nFlags, CPoint point) 
{
	// check if we have captured the mouse
	if (GetCapture() == this)
	{
		// increment the object rotation angles
		X_Angle += double(point.y - MouseDownPoint.y) / 3.6;
		Y_Angle += double(point.x - MouseDownPoint.x) / 3.6;

		// redraw the view
		Invalidate(TRUE);

		// remember the mouse point
		MouseDownPoint=point;
	};
}

void CTestGLView::OnHelpOglinfo() 
{
	COGLInfoDlg dlg;
	int pos = 0;
	dlg.m_accel      = GetInformation(ACCELERATION);
	dlg.m_rend       = GetInformation(RENDERER);
	dlg.m_vendor     = GetInformation(VENDOR);
	dlg.m_version    = GetInformation(VERSION);
	dlg.m_extensions = GetInformation(EXTENSIONS);

	while ( (pos = dlg.m_extensions.Find(" ")) != -1 )
		dlg.m_extensions.SetAt(pos,'\n');
	dlg.DoModal();	
}

void CTestGLView::OnTesselatorFill() 
{
	TessFilling ^= 1;
	BeginGLCommands();
	BuildTessDispList();
	EndGLCommands();
	Invalidate(TRUE);
}

void CTestGLView::OnUpdateTesselatorFill(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(sceneselect == 1);
	pCmdUI->SetCheck(TessFilling);
}

void CTestGLView::OnTesselatorNeg() 
{
	TessWindRule = GLU_TESS_WINDING_NEGATIVE;

	BeginGLCommands();
	BuildTessDispList();
	EndGLCommands();

	Invalidate(TRUE);
}

void CTestGLView::OnUpdateTesselatorNeg(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(sceneselect == 1);
	pCmdUI->SetCheck(TessWindRule == GLU_TESS_WINDING_NEGATIVE);
}

void CTestGLView::OnTesselatorNonzero() 
{
	TessWindRule=GLU_TESS_WINDING_NONZERO;

	BeginGLCommands();
	BuildTessDispList();
	EndGLCommands();

	Invalidate(TRUE);
}

void CTestGLView::OnUpdateTesselatorNonzero(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(sceneselect == 1);
	pCmdUI->SetCheck(TessWindRule == GLU_TESS_WINDING_NONZERO);
}

void CTestGLView::OnTesselatorOddrule() 
{	
	TessWindRule = GLU_TESS_WINDING_ODD;
	BeginGLCommands();
	BuildTessDispList();
	EndGLCommands();
	Invalidate(TRUE);
}

void CTestGLView::OnUpdateTesselatorOddrule(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(sceneselect == 1);
	pCmdUI->SetCheck(TessWindRule == GLU_TESS_WINDING_ODD);
}

void CTestGLView::OnTesselatorPos() 
{

	TessWindRule = GLU_TESS_WINDING_POSITIVE;

	BeginGLCommands();
	BuildTessDispList();
	EndGLCommands();

	Invalidate(TRUE);
}

void CTestGLView::OnUpdateTesselatorPos(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(sceneselect == 1);
	pCmdUI->SetCheck(TessWindRule == GLU_TESS_WINDING_POSITIVE);
}

void CTestGLView::OnChangeScene() 
{
	++sceneselect %= 3;
	Invalidate(TRUE);
}

void CTestGLView::OnQuadricFill() 
{
	quadricDwStyle = GLU_FILL;
	BeginGLCommands();
	BuildQuadrDispList();
	EndGLCommands();
	Invalidate(TRUE);
}

void CTestGLView::OnQuadricFlat() 
{
	quadricNormals = GLU_FLAT;
	BeginGLCommands();
	BuildQuadrDispList();
	EndGLCommands();
	Invalidate(TRUE);
}

void CTestGLView::OnQuadricInside() 
{
	quadricOrientation = GLU_INSIDE;
	BeginGLCommands();
	BuildQuadrDispList();
	EndGLCommands();
	Invalidate(TRUE);
}

void CTestGLView::OnQuadricLine() 
{
	quadricDwStyle = GLU_LINE;
	BeginGLCommands();
	BuildQuadrDispList();
	EndGLCommands();
	Invalidate(TRUE);
}

void CTestGLView::OnQuadricNone() 
{
	quadricNormals = GLU_NONE;
	BeginGLCommands();
	BuildQuadrDispList();
	EndGLCommands();
	Invalidate(TRUE);
}

void CTestGLView::OnQuadricOutside() 
{
	quadricOrientation = GLU_OUTSIDE;
	BeginGLCommands();
	BuildQuadrDispList();
	EndGLCommands();
	Invalidate(TRUE);
}

void CTestGLView::OnQuadricPoint() 
{
	quadricDwStyle = GLU_POINT;
	BeginGLCommands();
	BuildQuadrDispList();
	EndGLCommands();
	Invalidate(TRUE);
}

void CTestGLView::OnQuadricSilhouette() 
{
	quadricDwStyle = GLU_SILHOUETTE;
	BeginGLCommands();
	BuildQuadrDispList();
	EndGLCommands();
	Invalidate(TRUE);
}

void CTestGLView::OnQuadricSmooth() 
{
	quadricNormals = GLU_SMOOTH;
	BeginGLCommands();
	BuildQuadrDispList();
	EndGLCommands();
	Invalidate(TRUE);
}

void CTestGLView::OnUpdateQuadricFill(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(sceneselect == 2);
	pCmdUI->SetCheck(quadricDwStyle == GLU_FILL);
}

void CTestGLView::OnUpdateQuadricFlat(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(sceneselect == 2);
	pCmdUI->SetCheck(quadricNormals == GLU_FLAT);
}

void CTestGLView::OnUpdateQuadricInside(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(sceneselect == 2);
	pCmdUI->SetCheck(quadricOrientation == GLU_INSIDE);
}

void CTestGLView::OnUpdateQuadricLine(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(sceneselect == 2);
	pCmdUI->SetCheck(quadricDwStyle == GLU_LINE);
}

void CTestGLView::OnUpdateQuadricNone(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(sceneselect == 2);
	pCmdUI->SetCheck(quadricNormals == GLU_NONE);
}

void CTestGLView::OnUpdateQuadricOutside(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(sceneselect == 2);
	pCmdUI->SetCheck(quadricOrientation == GLU_OUTSIDE);
}

void CTestGLView::OnUpdateQuadricPoint(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(sceneselect == 2);
	pCmdUI->SetCheck(quadricDwStyle == GLU_POINT);
}

void CTestGLView::OnUpdateQuadricSilhouette(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(sceneselect == 2);
	pCmdUI->SetCheck(quadricDwStyle == GLU_SILHOUETTE);
}

void CTestGLView::OnUpdateQuadricSmooth(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(sceneselect == 2);
	pCmdUI->SetCheck(quadricNormals == GLU_SMOOTH);
}

void CTestGLView::OnEnvirFlat() 
{
	BeginGLCommands();
	glShadeModel(GL_FLAT);
	EndGLCommands();
	Invalidate(TRUE);
}

void CTestGLView::OnEnvirLighting() 
{
	BeginGLCommands();

	if (glIsEnabled(GL_LIGHTING)) 
		glDisable(GL_LIGHTING);
	else 
		glEnable(GL_LIGHTING);

	EndGLCommands();
	Invalidate(TRUE);
}

void CTestGLView::OnEnvirSmooth() 
{
	BeginGLCommands();
	glShadeModel(GL_SMOOTH);
	EndGLCommands();
	Invalidate(TRUE);
}

void CTestGLView::OnUpdateEnvirLighting(CCmdUI* pCmdUI) 
{
	BeginGLCommands();
	pCmdUI->SetCheck(glIsEnabled(GL_LIGHTING));
	EndGLCommands();
}

void CTestGLView::OnUpdateEnvirFlat(CCmdUI* pCmdUI) 
{
	BeginGLCommands();
	
	GLint val;
	glGetIntegerv(GL_SHADE_MODEL, &val);
	pCmdUI->SetCheck(val == GL_FLAT);

	EndGLCommands();
}

void CTestGLView::OnUpdateEnvirSmooth(CCmdUI* pCmdUI) 
{
	BeginGLCommands();

	GLint val;
	glGetIntegerv(GL_SHADE_MODEL, &val);
	pCmdUI->SetCheck(val == GL_SMOOTH);

	EndGLCommands();
}
