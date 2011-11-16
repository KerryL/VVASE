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

#ifndef _AARM_H_
#define _AARM_H_

// CarDesigner forward declarations
class RenderWindow;
class Vector;
class Color;
class SPHERE;
class CYLINDER;
class Primitive;

class AARM
{
public:
	// Constructor
	AARM(RenderWindow &_Renderer);

	// Destructor
	~AARM();

	// Updates the actor with the new position and size for the a-arm
	void Update(const Vector &End1, const Vector &Middle, const Vector &End2, const double &Diameter,
		const int &Resolution, const Color &color, bool Show);

	// Returns true if the passed reference is to an actor from this object
	bool ContainsThisActor(const Primitive *Actor);

private:
	// The members
	CYLINDER *Member1;
	CYLINDER *Member2;

	// The points
	SPHERE *EndPoint1;
	SPHERE *MidPoint;
	SPHERE *EndPoint2;
};

#endif// _AARM_H_