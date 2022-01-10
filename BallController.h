// this file uses Doxygen comment blocks for automatic extraction of source code documentation.

/*!\file
 Declaration of the CBallController class.
 This class implements a virtual trackball rotation controller suitable for
 use in 3D graphical applications.
 This implementation "takes inspiration" and merges features found in
 K.Shoemake's Arcball controller (see Graphic Gems vol.4) and J.Childs' GLcube rotation
 controller which is in turn based on M.J.Kilgard's GLUT code.
 The interaction method has been tailored on a Document-View-Controller paradigm
 such as that in Microsoft MFC framework but it's applicable to other
 contexts as well.
 \version 0.2
 \date 15/10/99
 \author Alessandro Falappa
*/

//		Notes added by RJM
//
//		To use the CBallController class:
//			Add a CBallController to the CWnd derived class
//			Add handlers for left mouse up and down and connect to MouseUp() and MouseDown()
//				example for mouse down
//					m_TrackBall.MouseDown(point);
//					SetCapture();
//
//				example for mouse up
//					m_TrackBall.MouseUp(point);
//					ReleaseCapture();
//					Invalidate();
//			Add handler for mouse motion and conect to MouseMove()
//				example for mouse move
//					m_TrackBall.MouseMove(point);
//					Invalidate();
//			Apply rotations to a active context using IssueGLRotation()
//
//		you can also hook the keyboard using OnKeyDown
//			example for key press
//				m_TrackBall.Key(nChar);
//
//		if you are drawing lots of stuff, it works well to have a flag indicating you are 
//		repositioning using the trackball in your drawing class.  this way you can reduce
//		quality/detail/#objects while rotating.  you set the flag when mouse button is
//		pressed, then clear it when buton is released
//
//-----------------------------------------------------------------------------
// BallController.h: interface for the CBallController class.

#if !defined(AFX_BALLCONTROLLER_H__196CBD82_3858_11D2_80B1_A7800FACFE7F__INCLUDED_)
#define AFX_BALLCONTROLLER_H__196CBD82_3858_11D2_80B1_A7800FACFE7F__INCLUDED_

#include "3dmath.h"	// Added by ClassView
#include "gl\gl.h"
#include "gl\glu.h"

//MSVC compiler specifics
#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// Auxiliary Type Definitions
/*!
Defines some constant values used to set the axes to which
constrain rotations about.
*/
enum AxisSet
{
	NO_AXES,
	CAMERA_AXES,
	BODY_AXES,
	OTHER_AXES
};

/*!
This is the controller object
*/
class CBallController  
{
private:
	unitquaternion m_lastQuat;
	bool bDrawConstraints;
	tmatrix bodyorientation;
	int angleKeyIncrement;
	void DrawConstraints();
	vector* GetUsedAxisSet();
	vector BallColor;
	bool bProjectionMethod2;
	bool bDrawBallArea;
	int GLdisplayList;
	unitquaternion currentQuat;
	unitquaternion previousQuat;
	real radius;
	real winWidth;
	real winHeight;
	real xprev;
	real yprev;
	CPoint center;
	bool mouseButtonDown;
	AxisSet whichConstraints;
	int currentAxisIndex;
	vector cameraAxes[3];
	vector bodyAxes[3];
	vector* otherAxes;
	int otherAxesNum;

	void InitDisplayLists();
	void initVars(void);
	void ProjectOnSphere(vector& v) const;
	unitquaternion RotationFromMove(const vector& vfrom,const vector& vto);
	vector ConstrainToAxis(const vector& loose,const vector& axis);
	int NearestConstraintAxis(const vector& loose);
	void DrawBallLimit();
public:
	void ClearLastRotation();
	void ApplyLastRotation();
	void SetBallOrientation(unitquaternion quat);
	unitquaternion GetBallOrientation();
	void RestoreOrientation();
	void SaveOrientation();
	void ResetOrientation(double angle = 90.0);
	void Oscillate(real x, real y, BOOL reset = TRUE);
	bool GetDrawConstraints();
	void SetDrawConstraints(bool flag=true);
	void DrawBall();
	int GetAngleKeyIncrement();
	void SetAngleKeyIncrement(int ang);
	void UseConstraints(AxisSet constraints);
	void ToggleMethod();
	void SetAlternateMethod(bool flag=true);
	vector GetColorV();
	COLORREF GetColor();
	void SetColor(COLORREF col);
	void SetColorV(vector colvec);
	CBallController();
	CBallController(const real& rad);
	CBallController(const real& rad,const unitquaternion& initialOrient);
	CBallController(const CBallController& other);
	virtual ~CBallController();
	CBallController& operator=(const CBallController& other);
	void Resize(const real& newRadius);
	void ClientAreaResize(const CRect& newSize);
	void MouseDown(const CPoint& location);
	void MouseUp(const CPoint& location);
	void MouseMove(const CPoint& location);
	void IssueGLrotation();
	BOOL Key(UINT nChar);
};

//---------------------------------------------------------------------------
// inlines

inline CBallController::~CBallController()
{
//	if(otherAxes) delete[] otherAxes;
}

inline CBallController::CBallController(const CBallController& other)
{
	*this=other;
}

inline void CBallController::Resize(const real& newRadius)
{
	radius=newRadius;
}

inline void CBallController::ClientAreaResize(const CRect& newSize)
{
	winWidth=real(newSize.Width());
	winHeight=real(newSize.Height());
}

inline CBallController::CBallController()
{
	initVars();
	currentQuat=unitquaternion(DegToRad(70),X_AXIS);
//	currentQuat=unitquaternion(DegToRad(90),X_AXIS);
}

inline COLORREF CBallController::GetColor()
{
	return RGB(int(BallColor.x()*255),int(BallColor.y()*255),int(BallColor.z()*255));
}

inline vector CBallController::GetColorV()
{
	return BallColor;
}

inline void CBallController::SetAlternateMethod(bool flag)
{
	bProjectionMethod2=flag;
}

inline void CBallController::ToggleMethod()
{
	if(bProjectionMethod2) bProjectionMethod2=false;
	else bProjectionMethod2=true;
}

inline void CBallController::UseConstraints(AxisSet constraints)
{
	whichConstraints=constraints;
}

inline int CBallController::GetAngleKeyIncrement()
{
	return angleKeyIncrement;
}

inline void CBallController::SetAngleKeyIncrement(int ang)
{
	angleKeyIncrement=abs(ang)%360;
}

inline bool CBallController::GetDrawConstraints()
{
	return bDrawConstraints;
}

inline void CBallController::SetDrawConstraints(bool flag)
{
	bDrawConstraints=flag;
}

#endif
