/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  quadrilateral.h
// Date:  5/14/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Derived from Primitive for creating rectangular objects.

#ifndef QUADRILATERAL_H_
#define QUADRILATERAL_H_

// Local headers
#include "vRenderer/primitives/primitive.h"
#include "vMath/vector.h"

namespace VVASE
{

class Quadrilateral : public Primitive
{
public:
	Quadrilateral(RenderWindow &renderWindow);
	~Quadrilateral();

	// Mandatory overloads from Primitive - for creating geometry and testing the
	// validity of this object's parameters
	void GenerateGeometry();
	bool HasValidParameters();
	bool IsIntersectedBy(const Vector& point, const Vector& direction) const;

	// Private data accessors
	void SetNormal(const Vector &normal);
	void SetCenter(const Vector &center);
	void SetAxis(const Vector &axis);
	void SetWidth(const double &width);
	void SetLength(const double &length);

private:
	Vector normal;
	Vector axis;
	Vector center;

	double width;
	double length;
};

}// namespace VVASE

#endif// QUADRILATERAL_H_
