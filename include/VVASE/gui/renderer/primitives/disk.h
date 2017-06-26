/*===================================================================================
                                       VVASE
                         Copyright Kerry R. Loux 2007-2017
===================================================================================*/

// File:  disk.h
// Date:  5/14/2009
// Auth:  K. Loux
// Desc:  Derived from Primitive for creating disk objects.

#ifndef DISK_H_
#define DISK_H_

// Local headers
#include "vRenderer/primitives/primitive.h"
#include "vMath/vector.h"

namespace VVASE
{

class Disk : public Primitive
{
public:
	Disk(RenderWindow &renderWindow);
	~Disk();

	// Mandatory overloads from Primitive - for creating geometry and testing the
	// validity of this object's parameters
	void GenerateGeometry();
	bool HasValidParameters();
	bool IsIntersectedBy(const Vector& point, const Vector& direction) const;

	void SetResolution(const int &resolution);
	void SetOuterRadius(const double &outerRadius);
	void SetInnerRadius(const double &innerRadius);
	void SetCenter(const Vector &center);
	void SetNormal(const Vector &normal);

private:
	int resolution;

	Vector center;
	Vector normal;

	double outerRadius;
	double innerRadius;
};

}// namespace VVASE

#endif// DISK_H_
