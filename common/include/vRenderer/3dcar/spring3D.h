/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  spring3D.h
// Created:  1/5/2009
// Author:  K. Loux
// Description:  Contains class declaration for the Spring3D class.
// History:
//	4/25/2009	- Changed Update() to include color argument in the form of wxColor, K. Loux.
//	5/17/2009	- Removed VTK dependencies, K. Loux.

#ifndef _SPRING3D_H_
#define _SPRING3D_H_

// VVASE forward declarations
class Vector;
class Color;
class Cylinder;
class Sphere;

class Spring3D
{
public:
	// Constructor
	Spring3D(RenderWindow &_renderer);

	// Destructor
	~Spring3D();

	// Updates the 3D representation of the spring on the screen
	void Update(const Vector &end1, const Vector &end2, const double &diameter,
		const double &pointDiameter, const int &resolution, const Color &color, bool show);

	// Returns true if the passed reference is to an actor from this object
	bool ContainsThisActor(const Primitive *actor);

private:
	// The main body of the spring
	Cylinder *spring;

	// The end points
	Sphere *endPoint1;
	Sphere *endPoint2;
};

#endif// _SPRING3D_H_