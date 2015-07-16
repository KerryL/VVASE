/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  aarm.h
// Created:  5/23/2008
// Author:  K. Loux
// Description:  Contains class declaration for the AARM class.  A-arms are
//				 comprised of three spheres at the hardpoints and are connected by
//				 cylinders.
// History:
//	4/25/2009	- Changed Update() to include color argument in the form of wxColor, K. Loux.
//	5/17/2009	- Removed VTK dependencies, K. Loux.

#ifndef AARM_H_
#define AARM_H_

// CarDesigner forward declarations
class RenderWindow;
class Vector;
class Color;
class Sphere;
class Cylinder;
class Primitive;

class AArm
{
public:
	AArm(RenderWindow &renderer);
	~AArm();

	// Updates the actor with the new position and size for the a-arm
	void Update(const Vector &end1, const Vector &middle, const Vector &end2, const double &diameter,
		const int &resolution, const Color &color, bool show);

	// Returns true if the passed reference is to an actor from this object
	bool ContainsThisActor(const Primitive *actor);

private:
	// The members
	Cylinder *member1;
	Cylinder *member2;

	// The points
	Sphere *endPoint1;
	Sphere *midPoint;
	Sphere *endPoint2;
};

#endif// AARM_H_