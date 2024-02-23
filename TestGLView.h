// TestGLView.h : interface of the CTestGLView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_TESTGLVIEW_H__59CED136_E584_11D1_ACB3_E52ED8AC9002__INCLUDED_)
#define AFX_TESTGLVIEW_H__59CED136_E584_11D1_ACB3_E52ED8AC9002__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CTestGLView : public CGLEnabledView
{
protected: // create from serialization only
	CTestGLView();
	DECLARE_DYNCREATE(CTestGLView)
		 
// Attributes
public:
	CTestGLDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestGLView)
	//}}AFX_VIRTUAL

// Implementation
public:
	void OnCreateGL();
	void OnDrawGL();
	virtual ~CTestGLView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	void VideoMode(ColorsNumber &c,ZAccuracy &z,BOOL &dbuf);
	//{{AFX_MSG(CTestGLView)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnHelpOglinfo();
	afx_msg void OnTesselatorFill();
	afx_msg void OnUpdateTesselatorFill(CCmdUI* pCmdUI);
	afx_msg void OnTesselatorNeg();
	afx_msg void OnUpdateTesselatorNeg(CCmdUI* pCmdUI);
	afx_msg void OnTesselatorNonzero();
	afx_msg void OnUpdateTesselatorNonzero(CCmdUI* pCmdUI);
	afx_msg void OnTesselatorOddrule();
	afx_msg void OnUpdateTesselatorOddrule(CCmdUI* pCmdUI);
	afx_msg void OnTesselatorPos();
	afx_msg void OnUpdateTesselatorPos(CCmdUI* pCmdUI);
	afx_msg void OnChangeScene();
	afx_msg void OnQuadricFill();
	afx_msg void OnQuadricFlat();
	afx_msg void OnQuadricInside();
	afx_msg void OnQuadricLine();
	afx_msg void OnQuadricNone();
	afx_msg void OnQuadricOutside();
	afx_msg void OnQuadricPoint();
	afx_msg void OnQuadricSilhouette();
	afx_msg void OnQuadricSmooth();
	afx_msg void OnEnvirFlat();
	afx_msg void OnEnvirLighting();
	afx_msg void OnEnvirSmooth();
	afx_msg void OnUpdateEnvirLighting(CCmdUI* pCmdUI);
	afx_msg void OnUpdateQuadricFill(CCmdUI* pCmdUI);
	afx_msg void OnUpdateQuadricFlat(CCmdUI* pCmdUI);
	afx_msg void OnUpdateQuadricInside(CCmdUI* pCmdUI);
	afx_msg void OnUpdateQuadricLine(CCmdUI* pCmdUI);
	afx_msg void OnUpdateQuadricNone(CCmdUI* pCmdUI);
	afx_msg void OnUpdateQuadricOutside(CCmdUI* pCmdUI);
	afx_msg void OnUpdateQuadricPoint(CCmdUI* pCmdUI);
	afx_msg void OnUpdateQuadricSilhouette(CCmdUI* pCmdUI);
	afx_msg void OnUpdateQuadricSmooth(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEnvirFlat(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEnvirSmooth(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	GLenum quadricNormals;
	GLenum quadricDwStyle;
	GLenum quadricOrientation;
	void BuildQuadrDispList();
	CGLDispList quadric;
	int sceneselect;
	void BuildTessDispList();
	BOOL TessFilling;
	CGLDispList tessPolygon;
	GLdouble TessWindRule;
	CGLDispList anothercube;
	CPoint MouseDownPoint;
	double X_Angle;
	double Y_Angle;
};

#ifndef _DEBUG  // debug version in TestGLView.cpp
inline CTestGLDoc* CTestGLView::GetDocument()
   { return (CTestGLDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTGLVIEW_H__59CED136_E584_11D1_ACB3_E52ED8AC9002__INCLUDED_)
