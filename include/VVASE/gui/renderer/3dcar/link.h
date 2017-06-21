/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  link.h
// Created:  5/23/2008
// Author:  K. Loux
// Description:  Contains class declaration for the Link class.  Links are
//				 comprised of two spheres at the ends and are connected by a cylinder.
// History:
//	4/25/2009	- Changed Update() to include color argument in the form of wxColor and
//				  removed PURPOSE enumeration, K. Loux.
//	5/17/2009	- Removed VTK dependencies, K. Loux.

#ifndef LINK_H_
#define LINK_H_

// VVASE forward declarations
class Vector;
class Color;
class Sphere;
class Cylinder;

class Link
{
public:
	Link(RenderWindow &renderer);
	~Link();

	// Update the position of the link in the render window
	void Update(const Vector &end1, const Vector &end2, const double &diameter, const int &resolution,
		const Color &color, bool show);

	// Returns true if the passed reference is to an actor from this object
	bool ContainsThisActor(const Primitive *actor);

	Vector FindClosestPoint(const Vector& point, const Vector& direction) const;

private:
	Cylinder *member;

	Sphere *endPoint1;
	Sphere *endPoint2;
};

#endif// LINK_H_