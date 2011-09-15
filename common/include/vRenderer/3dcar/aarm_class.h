/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  aarm_class.h
// Created:  5/23/2008
// Author:  K. Loux
// Description:  Contains class declaration for the AARM class.  A-arms are
//				 comprised of three spheres at the hardpoints and are connected by
//				 cylinders.
// History:
//	4/25/2009	- Changed Update() to include color argument in the form of wxColor, K. Loux.
//	5/17/2009	- Removed VTK dependencies, K. Loux.

#ifndef _AARM_CLASS_H_
#define _AARM_CLASS_H_

// CarDesigner forward declarations
class RENDER_WINDOW;
class VECTOR;
class COLOR;
class SPHERE;
class CYLINDER;
class PRIMITIVE;

class AARM
{
public:
	// Constructor
	AARM(RENDER_WINDOW &_Renderer);

	// Destructor
	~AARM();

	// Updates the actor with the new position and size for the a-arm
	void Update(const VECTOR &End1, const VECTOR &Middle, const VECTOR &End2, const double &Diameter,
		const int &Resolution, const COLOR &Color, bool Show);

	// Returns true if the passed reference is to an actor from this object
	bool ContainsThisActor(const PRIMITIVE *Actor);

private:
	// The members
	CYLINDER *Member1;
	CYLINDER *Member2;

	// The points
	SPHERE *EndPoint1;
	SPHERE *MidPoint;
	SPHERE *EndPoint2;
};

#endif// _AARM_CLASS_H_