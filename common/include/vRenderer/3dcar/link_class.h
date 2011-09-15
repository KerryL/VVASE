/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  link_class.h
// Created:  5/23/2008
// Author:  K. Loux
// Description:  Contains class declaration for the LINK class.  Links are
//				 comprised of two spheres at the ends and are connected by a cylinder.
// History:
//	4/25/2009	- Changed Update() to include color argument in the form of wxColor and
//				  removed PURPOSE enumeration, K. Loux.
//	5/17/2009	- Removed VTK dependencies, K. Loux.

#ifndef _LINK_CLASS_H_
#define _LINK_CLASS_H_

// VVASE forward declarations
class VECTOR;
class COLOR;
class SPHERE;
class CYLINDER;

class LINK
{
public:
	// Constructor
	LINK(RENDER_WINDOW &_Renderer);

	// Destructor
	~LINK();

	// Update the position of the link in the render window
	void Update(const VECTOR &End1, const VECTOR &End2, const double &Diameter, const int &Resolution,
		const COLOR &Color, bool Show);

	// Returns true if the passed reference is to an actor from this object
	bool ContainsThisActor(const PRIMITIVE *Actor);

private:
	// The main body actor
	CYLINDER *Member;

	// The end points
	SPHERE *EndPoint1;
	SPHERE *EndPoint2;
};

#endif// _LINK_CLASS_H_