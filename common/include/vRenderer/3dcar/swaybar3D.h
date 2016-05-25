/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  swaybar3D.h
// Created:  1/11/2009
// Author:  K. Loux
// Description:  Contains class declaration for the Swaybar3D class.
// History:
//	4/25/2009	- Changed Update() to include color argument in the form of wxColor, K. Loux.
//	5/17/2009	- Removed VTK dependencies, K. Loux.

#ifndef SWAYBAR3D_H_
#define SWAYBAR3D_H_

// VVASE headers
#include "vCar/suspension.h"

// VVASE forward declarations
class Vector;
class Color;
class Cylinder;

class Swaybar3D
{
public:
	Swaybar3D(RenderWindow &renderer);
	~Swaybar3D();

	// Updates the 3D representation of the sway bar on the screen
	void Update(const Vector &rightLink, const Vector &leftLink, const Vector &torsionMemberTopRight,
		const Vector &torsionMemberBottomLeft, const Vector &midPoint, const Vector &axisPivot,
		const Suspension::BarStyle &barStyle, const double &dimension,
		const int &resolution, const Color &color, bool show);

	// Returns true if the passed reference is to an actor from this object
	bool ContainsThisActor(const Primitive *actor);

private:
	Cylinder *torqueArm1;
	Cylinder *torqueArm2;

	Cylinder *torsionMember;
};

#endif// SWAYBAR3D_H_