/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  point3D.h
// Created:  1/5/2009
// Author:  K. Loux
// Description:  Contains class declaration for the POINT3D class.
// History:
//	4/25/2009	- Changed Update() to include color argument in the form of wxColor and
//				  removed PURPOSE enumeration, K. Loux.
//	5/17/2009	- Removed VTK dependencies, K. Loux.

#ifndef _POINT3D_CLASS_H_
#define _POINT3D_CLASS_H_

// VVASE forward declarations
class Vector;
class Color;
class SPHERE;

class POINT3D
{
public:
	// Constructor
	POINT3D(RenderWindow &_Renderer);

	// Destructor
	~POINT3D();

	// Updates the 3D representation of the point on the screen
	void Update(const Vector &Position, const double &Diameter, const int &Resolution,
		const Color &color, bool Show);

private:
	// The point actor
	SPHERE *Point;
};

#endif// _POINT3D_CLASS_H_