/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

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

#ifndef TIRE3D_H_
#define TIRE3D_H_

// VVASE forward declarations
class Vector;
class Color;
class Cylinder;
class Disk;

class Tire3D
{
public:
	Tire3D(RenderWindow &renderer);
	~Tire3D();

	// Updates the 3D representation of the tire on the screen
	void Update(const Vector &contactPatch, const Vector &center, Vector originalNormal,
		Vector targetNormal, const double &width, const double &insideDiameter,
		const int &resolution, const Color &color, bool show);

	bool ContainsThisActor(const Primitive *actor);

private:
	Cylinder *outerSurface;
	Cylinder *innerSurface;

	Disk *sidewall1;
	Disk *sidewall2;
};

#endif// TIRE3D_H_