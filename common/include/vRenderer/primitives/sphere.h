/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  sphere.h
// Created:  5/14/2009
// Author:  K. Loux
// Description:  Derived from Primitive for creating spherical objects.
// History:

#ifndef _SPHERE_H_
#define _SPHERE_H_

// Local headers
#include "vRenderer/primitives/primitive.h"
#include "vMath/vector.h"

class Sphere : public Primitive
{
public:
	// Constructor
	Sphere(RenderWindow &_renderWindow);

	// Destructor
	~Sphere();

	// Mandatory overloads from Primitive - for creating geometry and testing the
	// validity of this object's parameters
	void GenerateGeometry(void);
	bool HasValidParameters(void);

	// Private data accessors
	void SetResolution(const int &_resolution);
	void SetCenter(const Vector &_center);
	void SetRadius(const double &_radius);

private:
	// The resolution
	int resolution;

	// The sphere's center point
	Vector center;

	// The sphere's radius
	double radius;

	// Performs subdivision of the triangles to achieve a "rounder" sphere
	void RecursiveSubdivision(const Vector &corner1, const Vector &corner2,
		const Vector &corner3, int level);

	// Adds a vertex to the glList
	void AddVertex(const Vector &vertex);
};

#endif// _SPHERE_H_