/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  point3D.h
// Date:  1/5/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains class declaration for the Point3D class.

#ifndef POINT3D_H_
#define POINT3D_H_

namespace VVASE
{

// Local forward declarations
class Vector;
class Color;
class Sphere;

class Point3D
{
public:
	Point3D(RenderWindow &renderer);
	~Point3D();

	// Updates the 3D representation of the point on the screen
	void Update(const Vector &position, const double &diameter, const int &resolution,
		const Color &color, bool show);

	// Returns true if the passed reference is to an actor from this object
	bool ContainsThisActor(const Primitive *actor);

private:
	Sphere *point;
};

}// namespace VVASE

#endif// POINT3D_H_
