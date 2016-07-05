/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  plane3D.h
// Created:  5/23/2008
// Author:  K. Loux
// Description:  Contains class declaration for the Plane3D class.
// History:
//	4/25/2009	- Changed Update() to include color argument in the form of wxColor, K. Loux.
//	5/17/2009	- Removed VTK dependencies, K. Loux.

#ifndef PLANE3D_H_
#define PLANE3D_H_

// VVASE forward declarations
class Color;
class Quadrilateral;

class Plane3D
{
public:
	Plane3D(RenderWindow &renderer);
	~Plane3D();

	// Updates the object in the scene
	void Update(const double &length, const double &width, const double &xOffset,
		const Color &color, const bool &show);

	// Returns true if the passed reference is to an actor from this object
	bool ContainsThisActor(const Primitive *actor);

private:
	Quadrilateral *plane;
};

#endif// PLANE3D_H_