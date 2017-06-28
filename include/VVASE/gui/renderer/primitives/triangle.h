/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  triangle.h
// Date:  5/14/2009
// Auth:  K. Loux
// Licn:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Derived from Primitve for creating triangular objects.

#ifndef TRIANGLE_H_
#define TRIANGLE_H_

// Local headers
#include "vRenderer/primitives/primitive.h"
#include "vMath/vector.h"

namespace VVASE
{

class Triangle : public Primitive
{
public:
	Triangle(RenderWindow &renderWindow);
	~Triangle();

	// Mandatory overloads from Primitive - for creating geometry and testing the
	// validity of this object's parameters
	void GenerateGeometry();
	bool HasValidParameters();
	bool IsIntersectedBy(const Vector& point, const Vector& direction) const;

	// Private data accessors
	void SetCorner1(const Vector &corner1);
	void SetCorner2(const Vector &corner2);
	void SetCorner3(const Vector &corner3);

private:
	Vector corner1;
	Vector corner2;
	Vector corner3;
};

}// namespace VVASE

#endif// TRIANGLE_H_
