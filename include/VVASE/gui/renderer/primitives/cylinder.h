/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  cylinder.h
// Date:  5/14/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Derived from Primitive for creating cylindrical objects.

#ifndef CYLINDER_H_
#define CYLINDER_H_

// Local headers
#include "vRenderer/primitives/primitive.h"
#include "vMath/vector.h"

namespace VVASE
{

class Cylinder : public Primitive
{
public:
	Cylinder(RenderWindow &renderWindow);
	~Cylinder();

	// Mandatory overloads from Primitive - for creating geometry and testing the
	// validity of this object's parameters
	void GenerateGeometry();
	bool HasValidParameters();
	bool IsIntersectedBy(const Vector& point, const Vector& direction) const;

	// Private data accessors
	void SetResolution(const int &resolution);
	void SetCapping(const bool &drawCaps);
	void SetEndPoint1(const Vector &endPoint1);
	void SetEndPoint2(const Vector &endPoint2);
	void SetRadius(const double &radius);

private:
	bool drawCaps;

	int resolution;

	Vector endPoint1;
	Vector endPoint2;

	double radius;
};

}// namespace VVASE

#endif// CYLINDER_H_
