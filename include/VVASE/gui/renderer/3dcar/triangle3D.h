/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  triangle3D.h
// Date:  5/23/2008
// Auth:  K. Loux
// Licn:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains class declaration for the Triangle3D class.  This is used for
//        bell cranks, uprights, etc.

#ifndef TRIANGLE3D_H_
#define TRIANGLE3D_H_

namespace VVASE
{

// Local forward declarations
class Vector;
class Color;
class Triangle;

class Triangle3D
{
public:
	Triangle3D(RenderWindow &renderer);
	~Triangle3D();

	// Updates the 3D representation of the tire on the screen
	void Update(const Vector &node1, const Vector &node2, const Vector &node3,
		const Color &color, bool show);

	bool ContainsThisActor(const Primitive *actor);

private:
	Triangle *triangle;
};

}// namespace VVASE

#endif// TRIANGLE3D_H_
