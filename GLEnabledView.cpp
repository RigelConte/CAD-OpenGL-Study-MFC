#include "stdafx.h"

#include "gl\gl.h"
#include "gl\glu.h"
//*/
/* SGI openGL libraries (link with OPENGL.LIB and GLU.LIB)
#include "[path-of-SGI-sdk]\include\gl\gl.h"
#include "[path-of-SGI-sdk]\include\gl\glu.h"
//*/
//#include "afxtempl.h"
#include "GLEnabledView.h"

#ifdef _DEBUG 
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAX_LISTS 20
// used to identify a MCD video driver (partial OGL acceleration)
#define INSTALLABLE_DRIVER_TYPE_MASK  (PFD_GENERIC_ACCELERATED|PFD_GENERIC_FORMAT)

/////////////////////////////////////////////////////////////////////////////
// Global Functions/variables
// These functions are used by CGLTesselator class

struct GarbListItem{
	GLdouble *pvert;
	GarbListItem* next;};

GarbListItem* m_garbagelist=NULL;

void AddGarbage(GLdouble * ptr)
{
	ASSERT(ptr!=NULL);
// allocate mem for new list item
	GarbListItem* temp=new GarbListItem;
// store pointer
	temp->pvert=ptr;
// add at head of list
	temp->next=m_garbagelist;
	m_garbagelist=temp;
}

void DeleteGarbage()
{
	if(m_garbagelist!=NULL)
	{
		GarbListItem* punt=m_garbagelist;
		GarbListItem* temp=m_garbagelist;
// scan the list
		while(punt!=NULL)
		{
// delete vertex
			delete[] punt->pvert;
			punt=punt->next;
// delete list item
			delete temp;
			temp=punt;
		};
		m_garbagelist=NULL;
	};
}

void CALLBACK BeginCallback(GLenum type)
{
// issue corresponding GL call
	glBegin(type);
}

void CALLBACK ErrorCallback(GLenum errorCode)
{
	const GLubyte *estring;
	CString mexstr;
// get the error descritption from OGL
	estring = gluErrorString(errorCode);
// prepare and show a message box
	mexstr.Format("Tessellation/Quadric Error: %s\n", estring);
	AfxMessageBox(mexstr,MB_OK | MB_ICONEXCLAMATION);
// replicate mex to debug trace
	TRACE("Tessellation Error: %s\n", estring);
}

void CALLBACK EndCallback()
{
// issue corresponding GL call
	glEnd();
}

void CALLBACK VertexCallback(GLvoid *vertex)
{
// issue corresponding GL call (double is used to get max precision)
	glVertex3dv( (const double *)vertex );
}

void CALLBACK CombineCallback(GLdouble coords[3], GLdouble *data[4], GLfloat weight[4], GLdouble **dataOut )
{
// allocate memory for a new vertex
	GLdouble *vertex;
	vertex = new GLdouble[3];
// store reported vertex
	vertex[0] = coords[0];
	vertex[1] = coords[1];
	vertex[2] = coords[2];
// return vertex to OGL
	*dataOut = vertex;
// add vertex pointer to garbage collection routines
	AddGarbage(vertex);
}

/////////////////////////////////////////////////////////////////////////////
// CGLEnabledView

IMPLEMENT_DYNCREATE(CGLEnabledView, CView)

CGLEnabledView::CGLEnabledView():
	m_dAspectRatio(1.0),
	m_bInsideDispList(FALSE), m_bExternDispListCall(FALSE),
	m_bExternGLCall(FALSE)
{
// define a default cursor
	m_hMouseCursor=AfxGetApp()->LoadStandardCursor(IDC_SIZEALL);
// set the disp list vector to all zeros
	for (int c=0;c<MAX_LISTS;c++) m_DispListVector[c]=0;
}

CGLEnabledView::~CGLEnabledView()
{
}


BEGIN_MESSAGE_MAP(CGLEnabledView, CView)
	//{{AFX_MSG_MAP(CGLEnabledView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGLEnabledView drawing

void CGLEnabledView::OnDraw(CDC* pDC)
{
// prepare a semaphore
	static BOOL 	bBusy = FALSE;
// use the semaphore to enter this critic section
	if(bBusy) return;
	bBusy = TRUE;

// specify the target DeviceContext of the subsequent OGL calls
	wglMakeCurrent(m_pCDC->GetSafeHdc(), m_hRC);

// clear background
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

// call the virtual drawing procedure (to be overridden by user)
	OnDrawGL();

// execute OGL commands (flush the OGL graphical pipeline)
	glFinish();

// if double buffering is used it's time to swap the buffers
	SwapBuffers(m_pCDC->GetSafeHdc());
	
// turn the semaphore "green"
	bBusy = FALSE;

// free the target DeviceContext (window)
    wglMakeCurrent(NULL,NULL);
}

void CGLEnabledView::OnDrawGL()
{
// draw carthesian axes
	glBegin(GL_LINES);
		// red x axis
		glColor3f(1.f,0.f,0.f);
		glVertex3f(0.0f,0.0f,0.0f);
		glVertex3f(1.0f,0.0f,0.0f);
		glVertex3f(1.0f,0.0f,0.0f);
		glVertex3f(0.9f,0.1f,0.0f);
		glVertex3f(1.0f,0.0f,0.0f);
		glVertex3f(0.9f,-0.1f,0.0f);
		// green y axis
		glColor3f(0.f,1.f,0.f);
		glVertex3f(0.0f,0.0f,0.0f);
		glVertex3f(0.0f,1.0f,0.0f);
		glVertex3f(0.0f,1.0f,0.0f);
		glVertex3f(0.1f,0.9f,0.0f);
		glVertex3f(0.0f,1.0f,0.0f);
		glVertex3f(-0.1f,0.9f,0.0f);
		// blue z axis
		glColor3f(0.f,0.f,1.f);
		glVertex3f(0.0f,0.0f,0.0f);
		glVertex3f(0.0f,0.0f,1.0f);
		glVertex3f(0.0f,0.0f,1.0f);
		glVertex3f(0.0f,0.1f,0.9f);
		glVertex3f(0.0f,0.0f,1.0f);
		glVertex3f(0.0f,-0.1f,0.9f);
	glEnd();
}

/////////////////////////////////////////////////////////////////////////////
// CGLEnabledView diagnostics

#ifdef _DEBUG
void CGLEnabledView::AssertValid() const
{
	CView::AssertValid();
}

void CGLEnabledView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
// dump some infos
	CString str;
	GetWindowText(str);
	afxDump<<"\nView Parameters\n\tClient Rectangle :"<<m_ClientRect<<"\n\tAspect Ratio :"<<m_dAspectRatio<<"\n";
	afxDump<<"\nWindowTitle :"<<str<<"\n";
}
#endif //_DEBUG

/////////////////////////////////////////////////////////
// CGLEnabledView Constants

// these are used to construct an equilibrated 256 color palette
static unsigned char _threeto8[8] = 
{
	0, 0111>>1, 0222>>1, 0333>>1, 0444>>1, 0555>>1, 0666>>1, 0377
};

static unsigned char _twoto8[4] = 
{
	0, 0x55, 0xaa, 0xff
};

static unsigned char _oneto8[2] = 
{
	0, 255
};

static int defaultOverride[13] = 
{
	0, 3, 24, 27, 64, 67, 88, 173, 181, 236, 247, 164, 91
};

// Windows Default Palette
static PALETTEENTRY defaultPalEntry[20] = 
{
	{ 0,   0,   0,    0 },
	{ 0x80,0,   0,    0 },
	{ 0,   0x80,0,    0 },
	{ 0x80,0x80,0,    0 },
	{ 0,   0,   0x80, 0 },
	{ 0x80,0,   0x80, 0 },
	{ 0,   0x80,0x80, 0 },
	{ 0xC0,0xC0,0xC0, 0 },

	{ 192, 220, 192,  0 },
	{ 166, 202, 240,  0 },
	{ 255, 251, 240,  0 },
	{ 160, 160, 164,  0 },

	{ 0x80,0x80,0x80, 0 },
	{ 0xFF,0,   0,    0 },
	{ 0,   0xFF,0,    0 },
	{ 0xFF,0xFF,0,    0 },
	{ 0,   0,   0xFF, 0 },
	{ 0xFF,0,   0xFF, 0 },
	{ 0,   0xFF,0xFF, 0 },
	{ 0xFF,0xFF,0xFF, 0 }
};

/////////////////////////////////////////////////////////////////////////////
// CGLEnabledView initialization and palette helpers

BOOL CGLEnabledView::bSetupPixelFormat()
{
// define default desired video mode (pixel format)
	static PIXELFORMATDESCRIPTOR pfd = 
	{
        sizeof(PIXELFORMATDESCRIPTOR),	// size of this pfd
        1,								// version number
        PFD_DRAW_TO_WINDOW |			// support window
        PFD_SUPPORT_OPENGL |			// support OpenGL
        PFD_DOUBLEBUFFER,				// double buffered
        PFD_TYPE_RGBA,                  // RGBA type
        24,                             // 24-bit color depth
        0, 0, 0, 0, 0, 0,               // color bits ignored
        0,                              // no alpha buffer
        0,                              // shift bit ignored
        0,                              // no accumulation buffer
        0, 0, 0, 0,                     // accum bits ignored
        16,                             // 32-bit z-buffer
        0,                              // no stencil buffer
        0,                              // no auxiliary buffer
        PFD_MAIN_PLANE,                 // main layer
        0,                              // reserved
        0, 0, 0                         // layer masks ignored
    };
// let the user change some parameters if he wants
	BOOL bDoublBuf;
	ColorsNumber cnum;
	ZAccuracy zdepth;
	VideoMode(cnum,zdepth,bDoublBuf);
//set the changes
	if(bDoublBuf) pfd.dwFlags=PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL |PFD_DOUBLEBUFFER;
	else pfd.dwFlags=PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
	switch(cnum)
	{
	case INDEXED: pfd.cColorBits=8;
	case THOUSANDS: pfd.cColorBits=16;
	case MILLIONS: pfd.cColorBits=24;
	case MILLIONS_WITH_TRANSPARENCY: pfd.cColorBits=32;
	};
	switch(zdepth)
	{
	case NORMAL: pfd.cDepthBits=16;
	case ACCURATE: pfd.cDepthBits=32;
	};

// ask the system for such video mode
    ASSERT(m_pCDC != NULL);
    int pixelformat;
	if ( (pixelformat = ChoosePixelFormat(m_pCDC->GetSafeHdc(), &pfd)) == 0 )
    {
        AfxMessageBox("ChoosePixelFormat failed");
        return FALSE;
    }

// try to set this video mode    
	if (SetPixelFormat(m_pCDC->GetSafeHdc(), pixelformat, &pfd) == FALSE)
    {
// the requested video mode is not available so get a default one
        pixelformat = 1;	
		if (DescribePixelFormat(m_pCDC->GetSafeHdc(), pixelformat, sizeof(PIXELFORMATDESCRIPTOR), &pfd)==0)
		{
// neither the requested nor the default are available: fail
			AfxMessageBox("SetPixelFormat failed (no OpenGL compatible video mode)");
			return FALSE;
		}
    }

    return TRUE;
}

void CGLEnabledView::CreateRGBPalette()
{
    PIXELFORMATDESCRIPTOR pfd;
    LOGPALETTE *pPal;
    int n, i;

// get the initially choosen video mode
	n = ::GetPixelFormat(m_pCDC->GetSafeHdc());
    ::DescribePixelFormat(m_pCDC->GetSafeHdc(), n, sizeof(pfd), &pfd);

// if is an indexed one...
    if (pfd.dwFlags & PFD_NEED_PALETTE)
    {
// ... construct an equilibrated palette (3 red bits, 3 green bits, 2 blue bits)
// NOTE: this code is integrally taken from MFC example Cube
		n = 1 << pfd.cColorBits;
        pPal = (PLOGPALETTE) new char[sizeof(LOGPALETTE) + n * sizeof(PALETTEENTRY)];

        ASSERT(pPal != NULL);

        pPal->palVersion = 0x300;
        pPal->palNumEntries = n;
        for (i=0; i<n; i++)
        {
            pPal->palPalEntry[i].peRed=ComponentFromIndex(i, pfd.cRedBits, pfd.cRedShift);
            pPal->palPalEntry[i].peGreen=ComponentFromIndex(i, pfd.cGreenBits, pfd.cGreenShift);
            pPal->palPalEntry[i].peBlue=ComponentFromIndex(i, pfd.cBlueBits, pfd.cBlueShift);
            pPal->palPalEntry[i].peFlags=0;
        }

// fix up the palette to include the default Windows palette
        if ((pfd.cColorBits == 8)                           &&
            (pfd.cRedBits   == 3) && (pfd.cRedShift   == 0) &&
            (pfd.cGreenBits == 3) && (pfd.cGreenShift == 3) &&
            (pfd.cBlueBits  == 2) && (pfd.cBlueShift  == 6)
           )
        {
			for (i = 1 ; i <= 12 ; i++)
                pPal->palPalEntry[defaultOverride[i]] = defaultPalEntry[i];
        }

        m_CurrentPalette.CreatePalette(pPal);
        delete pPal;

// set the palette
        m_pOldPalette=m_pCDC->SelectPalette(&m_CurrentPalette, FALSE);
        m_pCDC->RealizePalette();
    }
}

unsigned char CGLEnabledView::ComponentFromIndex(int i, UINT nbits, UINT shift)
{
    unsigned char val;

    val = (unsigned char) (i >> shift);
    switch (nbits) 
	{

    case 1:
        val &= 0x1;
        return _oneto8[val];
    case 2:
        val &= 0x3;
        return _twoto8[val];
    case 3:
        val &= 0x7;
        return _threeto8[val];

    default:
        return 0;
    }
}

/////////////////////////////////////////////////////////////////////////////
// CGLEnabledView message handlers and overridables

int CGLEnabledView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1) return -1;
	
// OpenGL rendering context creation
	PIXELFORMATDESCRIPTOR pfd;
    int         n;

// initialize the private member
	m_pCDC= new CClientDC(this);

// choose the requested video mode
    if (!bSetupPixelFormat()) return 0;
	
// ask the system if the video mode is supported
    n=::GetPixelFormat(m_pCDC->GetSafeHdc());
    ::DescribePixelFormat(m_pCDC->GetSafeHdc(),n,sizeof(pfd),&pfd);

// create a palette if the requested video mode has 256 colors (indexed mode)
    CreateRGBPalette();

// link the Win Device Context with the OGL Rendering Context
    m_hRC = wglCreateContext(m_pCDC->GetSafeHdc());

// specify the target DeviceContext (window) of the subsequent OGL calls
    wglMakeCurrent(m_pCDC->GetSafeHdc(), m_hRC);

// performs default setting of rendering mode,etc..
	OnCreateGL();

// free the target DeviceContext (window)
    wglMakeCurrent(NULL,NULL);
	
	return 0;
}

void CGLEnabledView::OnCreateGL()
{
// perform hidden line/surface removal (enabling Z-Buffer)
	glEnable(GL_DEPTH_TEST);

// set background color to black
	glClearColor(0.f,0.f,0.f,1.0f );

// set clear Z-Buffer value
	glClearDepth(1.0f);
}

void CGLEnabledView::OnDestroy() 
{
// specify the target DeviceContext (window) of the subsequent OGL calls
    wglMakeCurrent(m_pCDC->GetSafeHdc(), m_hRC);

// remove all display lists
	for (int c=0;c<MAX_LISTS;c++) if(m_DispListVector[c]) glDeleteLists(m_DispListVector[c],1);

// release definitely OGL Rendering Context
	if (m_hRC!=NULL) ::wglDeleteContext(m_hRC);

// Select our palette out of the dc
	CPalette palDefault;
	palDefault.CreateStockObject(DEFAULT_PALETTE);
	m_pCDC->SelectPalette(&palDefault, FALSE);

// destroy Win Device Context
	if(m_pCDC) delete m_pCDC;

// finally call the base function
	CView::OnDestroy();	
}

BOOL CGLEnabledView::PreCreateWindow(CREATESTRUCT& cs) 
{
// these styles are requested by OpenGL
	cs.style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

// these styles are meant for a use of this class in a MDI application
	cs.lpszClass = AfxRegisterWndClass(CS_OWNDC | CS_HREDRAW | CS_VREDRAW);

	return CView::PreCreateWindow(cs);
}


BOOL CGLEnabledView::OnEraseBkgnd(CDC* pDC) 
{
// OGL has his own background erasing so tell Windows to skip
	return TRUE;
}

void CGLEnabledView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
// when called with a nonzero window:
	if ( 0 < cx && 0 < cy )
	{
// update the rect and the aspect ratio
		m_ClientRect.right = cx;
		m_ClientRect.bottom = cy;
		m_dAspectRatio=double(cx)/double(cy);

// specify the target DeviceContext of the subsequent OGL calls
		wglMakeCurrent(m_pCDC->GetSafeHdc(), m_hRC);

// call the virtual sizing procedure (to be overridden by user)
		OnSizeGL(cx,cy);

// free the target DeviceContext (window)
		wglMakeCurrent(NULL,NULL);

// force redraw
		Invalidate(TRUE);
	};
}

void CGLEnabledView::OnSizeGL(int cx, int cy)
{
// set correspondence between window and OGL viewport
		glViewport(0,0,cx,cy);

// update the camera
 		glPushMatrix();
			glMatrixMode(GL_PROJECTION);
				glLoadIdentity();
				gluPerspective(40.0,m_dAspectRatio,0.1f, 10.0f);
				glTranslatef(0.0f,0.0f,-4.f);
			glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
}

BOOL CGLEnabledView::OnSetCursor(CWnd* /*pWnd*/, UINT /*nHitTest*/, UINT /*message*/) 
{
	ASSERT(m_hMouseCursor!=NULL);
	::SetCursor(m_hMouseCursor);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CGLEnabledView public members

void CGLEnabledView::VideoMode(ColorsNumber &c, ZAccuracy &z, BOOL &dbuf)
{
// set default videomode
	c=MILLIONS;
	z=NORMAL;
	dbuf=TRUE;
}

void CGLEnabledView::SetMouseCursor(HCURSOR mcursor)
{
// set the specified cursor (only if it is a valid one)
	if(mcursor!=NULL) m_hMouseCursor=mcursor;
}

const CString CGLEnabledView::GetInformation(InfoField type)
{
	PIXELFORMATDESCRIPTOR pfd;
	CString str("Not Available");

// Get information about the DC's current pixel format 
	::DescribePixelFormat( m_pCDC->GetSafeHdc(), ::GetPixelFormat(m_pCDC->GetSafeHdc()),sizeof(PIXELFORMATDESCRIPTOR), &pfd ); 

// specify the target DeviceContext of the subsequent OGL calls
	wglMakeCurrent(m_pCDC->GetSafeHdc(), m_hRC);

	switch(type)
	{
// Derive driver information
	case ACCELERATION: if( 0==(INSTALLABLE_DRIVER_TYPE_MASK & pfd.dwFlags) ) str="Fully Accelerated (ICD)"; // fully in hardware (fastest)
						else if (INSTALLABLE_DRIVER_TYPE_MASK==(INSTALLABLE_DRIVER_TYPE_MASK & pfd.dwFlags) ) str="Partially Accelerated (MCD)"; // partially in hardware (pretty fast, maybe..)
							 else str="Not Accelerated (Software)";	// software
						break;
// get the company name responsible for this implementation
	case VENDOR:str=(char*)::glGetString(GL_VENDOR);
				if ( ::glGetError()!=GL_NO_ERROR) str.Format("Not Available");// failed!
				break;
// get the renderer name; this is specific of an hardware configuration
	case RENDERER:str=(char*)::glGetString(GL_RENDERER);
					if ( ::glGetError()!=GL_NO_ERROR) str.Format("Not Available");// failed!
					break;
// get the version
	case VERSION:str=(char*)::glGetString(GL_VERSION);
				if ( ::glGetError()!=GL_NO_ERROR) str.Format("Not Available");// failed!
				break;
// return a space separated list of extensions
	case EXTENSIONS: str=(char*)::glGetString(GL_EXTENSIONS);
				if ( ::glGetError()!=GL_NO_ERROR) str.Format("Not Available");// failed!
				break;
	};

// free the target DeviceContext (window) and return the result
	wglMakeCurrent(NULL,NULL);
	return str;
}

void CGLEnabledView::DrawStockDispLists()
{
// check if we are already inside a drawing session
	if(m_hRC==wglGetCurrentContext() && m_pCDC->GetSafeHdc()==wglGetCurrentDC() )
	{
// draw directly all display lists
		for (int c=0;c<MAX_LISTS;c++) if(m_DispListVector[c]) glCallList(m_DispListVector[c]);
	}
	else
	{
// specify the target DeviceContext of the subsequent OGL calls
		wglMakeCurrent(m_pCDC->GetSafeHdc(), m_hRC);
// draw all display lists
		for (int c=0;c<MAX_LISTS;c++) if(m_DispListVector[c]) glCallList(m_DispListVector[c]);
// free the target DeviceContext (window)
		wglMakeCurrent(NULL,NULL);
	};
}

void CGLEnabledView::StartStockDListDef()
{
// check if we aren't inside another couple begin/end
	if(!m_bInsideDispList)
	{
// search a free slot
		for (int c=0;m_DispListVector[c]!=0;c++);
// check if we are inside a drawing session or not....
		if(!( m_hRC==wglGetCurrentContext() && m_pCDC->GetSafeHdc()==wglGetCurrentDC() ))
		{
// ...if not specify the target DeviceContext of the subsequent OGL calls
			wglMakeCurrent(m_pCDC->GetSafeHdc(), m_hRC);
// set a warning for EndDispList
			m_bExternDispListCall=TRUE;
		};
// create a handle to the disp list (actually an integer)
		// trace m_DispListVector[c]=glGenLists(1);
// set a semaphore
		m_bInsideDispList=TRUE;
// start the disp list: all subsequent OGL calls will be redirected to the list
		// trace glNewList(m_DispListVector[c],GL_COMPILE);
	};
}

void CGLEnabledView::EndStockListDef()
{
// close the disp list
	glEndList();
// unset the semaphore
	m_bInsideDispList=FALSE;
// if beginDispList set the warn free the target DeviceContext
	if(m_bExternDispListCall) wglMakeCurrent(NULL,NULL);
}

void CGLEnabledView::ClearStockDispLists()
{
// check if we are referring to the right Rendering Context
	if(m_hRC==wglGetCurrentContext() && m_pCDC->GetSafeHdc()==wglGetCurrentDC() )
	{
// delete active display lists
		for (int c=0;c<MAX_LISTS;c++) if(m_DispListVector[c]) glDeleteLists(m_DispListVector[c],1);
	}
	else
	{
// specify the target Rendering Context of the subsequent OGL calls
		wglMakeCurrent(m_pCDC->GetSafeHdc(), m_hRC);
// delete active display lists
		for (int c=0;c<MAX_LISTS;c++) if(m_DispListVector[c]) glDeleteLists(m_DispListVector[c],1);
// free the target Rendering Context (window)
		wglMakeCurrent(NULL,NULL);
	};
}

void CGLEnabledView::BeginGLCommands()
{
// check if we are inside a drawing session or not....
	if(!( m_hRC==wglGetCurrentContext() && m_pCDC->GetSafeHdc()==wglGetCurrentDC() ))
	{
// ...if not specify the target DeviceContext of the subsequent OGL calls
		wglMakeCurrent(m_pCDC->GetSafeHdc(), m_hRC);
// set a warning for EndGLCommands
		m_bExternGLCall=TRUE;
	};
}

void CGLEnabledView::EndGLCommands()
{
// if BeginGLCommands set the warn free the target DeviceContext
	if(m_bExternGLCall) wglMakeCurrent(NULL,NULL);
}

//////////////////////////////////////////////////////////////////////
//
// Implementation of CGLEnabledView::CGLDispList class.
//
/*** DESCRIPTION

  This is actually a helper class which wraps the
  use of display list in OGL.
  It must be used inside an GLEnabledView cause
  a display list must refer to a Rendering Context.
  At present there is no support for Disp. Lists
  Sharing among multiple RCs (that is multiple MDI
  child windows).

****************************************/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction

CGLEnabledView::CGLDispList::CGLDispList():
	m_glListId(0), m_bIsolated(FALSE)
{
}

CGLEnabledView::CGLDispList::~CGLDispList()
{
// remove display list
	glDeleteLists(m_glListId,1); 
}

//////////////////////////////////////////////////////////////////////
// Member functions

void CGLEnabledView::CGLDispList::Draw()
{
// if the list is not empty...
	if(m_glListId)
	{
		if(m_bIsolated)
		{
// save current transformation matrix
			glPushMatrix();
// save current OGL internal state (lighting, shading, and such)
			glPushAttrib(GL_ALL_ATTRIB_BITS);
		};
// draw the list
		glCallList(m_glListId);
		if(m_bIsolated)
		{
// restore transformation matrix
			glPopMatrix();
// restore OGL internal state
			glPopAttrib();
		};
	};
}

void CGLEnabledView::CGLDispList::StartDef(BOOL bImmediateExec)
{
// set the context for GL calls (if needed)
//	BeginGLCommands();
// check if another list is under construction
	int cur;
	glGetIntegerv(GL_LIST_INDEX,&cur);
	if(cur != 0) {TRACE("Error: Nested display list definition!");ASSERT(FALSE);};
// if the list is empty firstly allocate one
	if(!m_glListId) m_glListId=glGenLists(1);

// start or replace a list definition
	if (bImmediateExec) glNewList(m_glListId,GL_COMPILE_AND_EXECUTE);
	else  glNewList(m_glListId,GL_COMPILE);
}

void CGLEnabledView::CGLDispList::EndDef()
{
// check the coupling with a preceding call to StartDef()
	int cur;
	glGetIntegerv(GL_LIST_INDEX,&cur);
	if(cur != m_glListId) {TRACE("CGLDispList:Missing StartDef() before EndDef()\n");return;};
// close list definition
	glEndList();
// free the context (if needed)
//	EndGLCommands();
}

//////////////////////////////////////////////////////////////////////
//
// Implementation of CGLEnabledView::CGLTesselator class.
//
/*** DESCRIPTION

  This is actually a helper class which wraps the
  use of tessellation objects in OGL (see guide).
  It must be used inside an GLEnabledView cause
  a tesselator object must refer to a Rendering Context.

****************************************/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction

CGLEnabledView::CGLTesselator::CGLTesselator()
{
// create tessellation object
	m_tessObj=gluNewTess();
// set callback functions
	gluTessCallback(m_tessObj,GLU_TESS_BEGIN,(void (CALLBACK*)())&BeginCallback); 
	gluTessCallback(m_tessObj,GLU_TESS_VERTEX,(void (CALLBACK*)())&VertexCallback); 
	gluTessCallback(m_tessObj,GLU_TESS_END,(void (CALLBACK*)())&EndCallback);
	gluTessCallback(m_tessObj,GLU_TESS_COMBINE,(void (CALLBACK*)())&CombineCallback);
	gluTessCallback(m_tessObj,GLU_TESS_ERROR,(void (CALLBACK*)())&ErrorCallback);
}

CGLEnabledView::CGLTesselator::~CGLTesselator()
{
// remove tessellation object
	gluDeleteTess(m_tessObj);	
}

//////////////////////////////////////////////////////////////////////
// Member functions

void CGLEnabledView::CGLTesselator::SetWindingRule(GLdouble which)
{
// issue the equivalent GL call
	gluTessProperty(m_tessObj,GLU_TESS_WINDING_RULE,which); 
}

GLdouble CGLEnabledView::CGLTesselator::GetWindingRule()
{
//retrieve attribute
	GLdouble temp;
	gluTessProperty(m_tessObj,GLU_TESS_WINDING_RULE,temp);
// return value
	return temp;
}

void CGLEnabledView::CGLTesselator::SetFilling(BOOL bFill)
{
// issue the equivalent GL calls
	if(bFill) gluTessProperty(m_tessObj,GLU_TESS_BOUNDARY_ONLY,GL_FALSE);
	else gluTessProperty(m_tessObj,GLU_TESS_BOUNDARY_ONLY,GL_TRUE);
}

BOOL CGLEnabledView::CGLTesselator::GetFilling()
{
//retrieve attribute
	GLdouble temp;
	gluTessProperty(m_tessObj,GLU_TESS_BOUNDARY_ONLY,temp);
// convert to a boolean
	return (temp==GL_TRUE);
}

void CGLEnabledView::CGLTesselator::StartDef()
{
// start a polygon definition
	gluTessBeginPolygon(m_tessObj,NULL);
// start a contour definition
	gluTessBeginContour(m_tessObj);
}

void CGLEnabledView::CGLTesselator::EndDef()
{
// end contour and polygon definition
	gluTessEndContour(m_tessObj);
	gluTessEndPolygon(m_tessObj);
// free new vertices created by tessellation
	::DeleteGarbage();
}

void CGLEnabledView::CGLTesselator::ContourSeparator()
{
// insert a contour separation
	gluTessEndContour(m_tessObj);
	gluTessBeginContour(m_tessObj);
}

void CGLEnabledView::CGLTesselator::AddVertex(GLdouble vertData[3])
{
// IMPORTANT: the 3rd parameter must be given otherwise an access
// violation will occur, moreover every vertex must have it's own memory
// location till the closing of the polygon (that is you can't pass all
// the vertices trough the same variable in a for loop).
	gluTessVertex(m_tessObj,vertData,vertData); 
}

void CGLEnabledView::CGLTesselator::AddVertexArray(GLdouble arr[][3], int size)
{
	ASSERT(arr!=NULL);
// pass the vertices to the tessellation object
	for(int ct=0;ct<size;ct++) gluTessVertex(m_tessObj,arr[ct],arr[ct]);
}

//////////////////////////////////////////////////////////////////////
//
// Implementation of CGLEnabledView::CGLQuadric class.
//
/*** DESCRIPTION

  This is actually a helper class which wraps the
  use of quadric objects in OGL (see guide).
  It must be used inside an GLEnabledView cause
  a quadric object must refer to a Rendering Context.

****************************************/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction

CGLEnabledView::CGLQuadric::CGLQuadric(GLenum drwStyle,GLenum normals,GLenum side,BOOL bGenerateTxtrCoords)
{
// check validity of parameters
	ASSERT(normals==GLU_NONE || normals==GLU_FLAT || normals==GLU_SMOOTH);
	ASSERT(drwStyle==GLU_FILL || drwStyle==GLU_LINE || drwStyle==GLU_SILHOUETTE|| drwStyle==GLU_POINT);
	ASSERT(side==GLU_INSIDE || side==GLU_OUTSIDE);
// create quadric object
	m_quadrObj=gluNewQuadric();
// set error callback function (shared with tesselators)
	gluQuadricCallback(m_quadrObj,GLU_ERROR,(void (CALLBACK*)())&ErrorCallback);
// set normal generation
	gluQuadricNormals(m_quadrObj,normals);
// set Texture Coordinates generation
	if(bGenerateTxtrCoords) gluQuadricTexture(m_quadrObj,GL_TRUE);
	else gluQuadricTexture(m_quadrObj,GL_FALSE);
// set how the qadric will be generated
	gluQuadricDrawStyle(m_quadrObj,drwStyle);
// set which side of the quadric is to be considered inside
	gluQuadricOrientation(m_quadrObj,side);
}

CGLEnabledView::CGLQuadric::~CGLQuadric()
{
// remove quadric object
	gluDeleteQuadric(m_quadrObj);	
}

//////////////////////////////////////////////////////////////////////
// Member functions

void CGLEnabledView::CGLQuadric::SetNormals(GLenum type)
{
// check validity of type parameter it must be one of these:
	ASSERT(type==GLU_NONE || type==GLU_FLAT || type==GLU_SMOOTH);
// issue corresponding GL command
	gluQuadricNormals(m_quadrObj,type);
}

void CGLEnabledView::CGLQuadric::SetTextureCoordsGen(BOOL flag)
{
// issue corresponding GL commands
	if(flag) gluQuadricTexture(m_quadrObj,GL_TRUE);
	else gluQuadricTexture(m_quadrObj,GL_FALSE);
}

void CGLEnabledView::CGLQuadric::SetOrientation(GLenum type)
{
// check validity of type parameter it must be one of these:
	ASSERT(type==GLU_INSIDE || type==GLU_OUTSIDE);
// issue corresponding GL command
	gluQuadricOrientation(m_quadrObj,type);
}

void CGLEnabledView::CGLQuadric::SetDrawStyle(GLenum style)
{
// check validity of type parameter it must be one of these:
	ASSERT(style==GLU_FILL || style==GLU_LINE || style==GLU_SILHOUETTE|| style==GLU_POINT);
// issue corresponding GL command
	gluQuadricDrawStyle(m_quadrObj,style);
}

void CGLEnabledView::CGLQuadric::DrawSphere(GLdouble radius, int longitudeSubdiv, int latitudeSubdiv)
{
// issue corresponding GL command
	gluSphere(m_quadrObj,radius,longitudeSubdiv,latitudeSubdiv);
}

void CGLEnabledView::CGLQuadric::DrawCylinder(GLdouble baseRadius,GLdouble topRadius,GLdouble height,int slices,int stacks)
{
// issue corresponding GL command
	gluCylinder(m_quadrObj,baseRadius,topRadius,height,slices,stacks);
}

void CGLEnabledView::CGLQuadric::DrawDisk(GLdouble innerRadius,GLdouble outerRadius,int slices,int loops)
{
// issue corresponding GL command
	gluDisk(m_quadrObj,innerRadius,outerRadius,slices,loops);
}

void CGLEnabledView::CGLQuadric::DrawPartialDisk(GLdouble innerRadius,GLdouble outerRadius,int slices,int loops,GLdouble startAngle,GLdouble sweepAngle)
{
// issue corresponding GL command
	gluPartialDisk(m_quadrObj,innerRadius,outerRadius,slices,loops,startAngle,sweepAngle);
}
