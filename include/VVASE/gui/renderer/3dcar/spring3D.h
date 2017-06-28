/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  spring3D.h
// Date:  1/5/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains class declaration for the Spring3D class.

#ifndef SPRING3D_H_
#define SPRING3D_H_

namespace VVASE
{

// Local forward declarations
class Vector;
class Color;
class Cylinder;
class Sphere;

class Spring3D
{
public:
	Spring3D(RenderWindow &renderer);
	~Spring3D();

	// Updates the 3D representation of the spring on the screen
	void Update(const Vector &end1, const Vector &end2, const double &diameter,
		const double &pointDiameter, const int &resolution, const Color &color, bool show);

	// Returns true if the passed reference is to an actor from this object
	bool ContainsThisActor(const Primitive *actor);

	Vector FindClosestPoint(const Vector& point, const Vector& direction) const;

private:
	Cylinder *spring;

	Sphere *endPoint1;
	Sphere *endPoint2;
};

}// namespace VVASE

#endif// SPRING3D_H_
