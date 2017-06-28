/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  link.h
// Date:  5/23/2008
// Auth:  K. Loux
// Licn:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains class declaration for the Link class.  Links are
//				 comprised of two spheres at the ends and are connected by a cylinder.

#ifndef LINK_H_
#define LINK_H_

namespace VVASE
{

// Local forward declarations
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

}// namespace VVASE

#endif// LINK_H_
