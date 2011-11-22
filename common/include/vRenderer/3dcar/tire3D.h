/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  tire3D.h
// Created:  5/23/2008
// Author:  K. Loux
// Description:  Contains class declaration for the Tire3D class.
// History:
//	4/25/2009	- Changed Update() to include color argument in the form of wxColor, K. Loux.
//	5/17/2009	- Removed VTK dependencies, K. Loux.

#ifndef _TIRE3D_H_
#define _TIRE3D_H_

// VVASE forward declarations
class Vector;
class Color;
class Cylinder;
class Disk;

class Tire3D
{
public:
	// Constructor
	Tire3D(RenderWindow &_renderer);

	// Destructor
	~Tire3D();

	// Updates the 3D representation of the tire on the screen
	void Update(const Vector &contactPatch, const Vector &center, Vector originalNormal,
		Vector targetNormal, const double &width, const double &insideDiameter,
		const int &resolution, const Color &color, bool show);

	// Returns true if the passed reference is to an actor from this object
	bool ContainsThisActor(const Primitive *actor);

private:
	// The outer surface of the tire
	Cylinder *outerSurface;

	// The inner surface of the tire
	Cylinder *innerSurface;

	// The sidewalls
	Disk *sidewall1;
	Disk *sidewall2;
};

#endif// _TIRE3D_H_