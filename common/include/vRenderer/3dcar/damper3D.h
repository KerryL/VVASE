/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  damper3D.h
// Created:  1/5/2009
// Author:  K. Loux
// Description:  Contains class declaration for the Damper3D class.
// History:
//	4/25/2009	- Changed Update() to include color argument in the form of wxColor, K. Loux.
//	5/17/2009	- Removed VTK dependencies, K. Loux.

#ifndef _DAMPER3D_H_
#define _DAMPER3D_H_

// VVASE forward declarations
class Vector;
class Color;
class Cylinder;
class Sphere;

class Damper3D
{
public:
	// Constructor
	Damper3D(RenderWindow &_renderer);

	// Destructor
	~Damper3D();

	// Updates the 3D representation of the damper on the screen
	void Update(const Vector &inboardEnd, const Vector &outboardEnd, const double &bodyDiameter,
		const double &shaftDiameter, const double &bodyLength, const int &resolution,
		const Color &bodyColor, const Color &shaftColor, bool show);

	// Returns true if the passed reference is to an actor from this object
	bool ContainsThisActor(const Primitive *actor);

private:
	// The damper body
	Cylinder *body;

	// The shaft
	Cylinder *shaft;

	// The end points
	Sphere *inboardEndPoint;
	Sphere *outboardEndPoint;
};

#endif// _DAMPER3D_H_