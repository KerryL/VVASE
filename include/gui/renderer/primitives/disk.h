/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  disk.h
// Created:  5/14/2009
// Author:  K. Loux
// Description:  Derived from Primitive for creating disk objects.
// History:

#ifndef DISK_H_
#define DISK_H_

// Local headers
#include "vRenderer/primitives/primitive.h"
#include "vMath/vector.h"

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

#endif// DISK_H_