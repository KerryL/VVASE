/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  aarm.h
// Date:  5/23/2008
// Auth:  K. Loux
// Licn:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains class declaration for the AARM class.  A-arms are
//        comprised of three spheres at the hardpoints and are connected by
//        cylinders.

#ifndef AARM_H_
#define AARM_H_

namespace VVASE
{

// Local forward declarations
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

	Vector FindClosestPoint(const Vector& point, const Vector& direction) const;

private:
	// The members
	Cylinder *member1;
	Cylinder *member2;

	// The points
	Sphere *endPoint1;
	Sphere *midPoint;
	Sphere *endPoint2;
};

}// namespace VVASE

#endif// AARM_H_
