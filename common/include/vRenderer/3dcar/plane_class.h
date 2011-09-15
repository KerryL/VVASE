/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  plane_class.h
// Created:  5/23/2008
// Author:  K. Loux
// Description:  Contains class declaration for the PLANE class.
// History:
//	4/25/2009	- Changed Update() to include color argument in the form of wxColor, K. Loux.
//	5/17/2009	- Removed VTK dependencies, K. Loux.

#ifndef _PLANE_CLASS_H_
#define _PLANE_CLASS_H_

// VVASE forward declarations
class COLOR;
class QUADRILATERAL;

class PLANE
{
public:
	// Constructor
	PLANE(RENDER_WINDOW &_Renderer);

	// Destructor
	~PLANE();

	// Updates the object in the scene
	void Update(const double &Length, const double &Width, const double &XOffset,
		const COLOR &Color, const bool &Show);

private:
	// The plane actor
	QUADRILATERAL *Plane;
};

#endif// _PLANE_CLASS_H_