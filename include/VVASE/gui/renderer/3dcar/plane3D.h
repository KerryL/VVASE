/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  plane3D.h
// Date:  5/23/2008
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains class declaration for the Plane3D class.

#ifndef PLANE3D_H_
#define PLANE3D_H_

namespace VVASE
{

// Local forward declarations
class Color;
class Quadrilateral;

class Plane3D
{
public:
	Plane3D(RenderWindow &renderer);
	~Plane3D();

	// Updates the object in the scene
	void Update(const double &length, const double &width, const double &xOffset,
		const Color &color, const bool &show);

	// Returns true if the passed reference is to an actor from this object
	bool ContainsThisActor(const Primitive *actor);

private:
	Quadrilateral *plane;
};

}// namespace VVASE

#endif// PLANE3D_H_
