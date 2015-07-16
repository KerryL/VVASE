/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  point3D.h
// Created:  1/5/2009
// Author:  K. Loux
// Description:  Contains class declaration for the Point3D class.
// History:
//	4/25/2009	- Changed Update() to include color argument in the form of wxColor and
//				  removed PURPOSE enumeration, K. Loux.
//	5/17/2009	- Removed VTK dependencies, K. Loux.

#ifndef POINT3D_H_
#define POINT3D_H_

// VVASE forward declarations
class Vector;
class Color;
class Sphere;

class Point3D
{
public:
	Point3D(RenderWindow &renderer);
	~Point3D();

	// Updates the 3D representation of the point on the screen
	void Update(const Vector &position, const double &diameter, const int &resolution,
		const Color &color, bool show);

private:
	Sphere *point;
};

#endif// POINT3D_H_