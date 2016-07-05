/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  sphere.h
// Created:  5/14/2009
// Author:  K. Loux
// Description:  Derived from Primitive for creating spherical objects.
// History:

#ifndef SPHERE_H_
#define SPHERE_H_

// Local headers
#include "vRenderer/primitives/primitive.h"
#include "vMath/vector.h"

class Sphere : public Primitive
{
public:
	Sphere(RenderWindow &renderWindow);
	~Sphere();

	// Mandatory overloads from Primitive - for creating geometry and testing the
	// validity of this object's parameters
	void GenerateGeometry();
	bool HasValidParameters();
	bool IsIntersectedBy(const Vector& point, const Vector& direction) const;

	// Private data accessors
	void SetResolution(const int &resolution);
	void SetCenter(const Vector &center);
	void SetRadius(const double &radius);

	Vector GetCenter() const { return center; }

private:
	int resolution;

	Vector center;

	double radius;

	// Performs subdivision of the triangles to achieve a "rounder" sphere
	void RecursiveSubdivision(const Vector &corner1, const Vector &corner2,
		const Vector &corner3, int level);

	void AddVertex(const Vector &vertex);
};

#endif// SPHERE_H_