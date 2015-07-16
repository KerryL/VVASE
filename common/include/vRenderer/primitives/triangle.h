/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  triangle.h
// Created:  5/14/2009
// Author:  K. Loux
// Description:  Derived from Primitve for creating triangular objects.
// History:

#ifndef TRIANGLE_H_
#define TRIANGLE_H_

// Local headers
#include "vRenderer/primitives/primitive.h"
#include "vMath/vector.h"

class Triangle : public Primitive
{
public:
	Triangle(RenderWindow &renderWindow);
	~Triangle();

	// Mandatory overloads from Primitive - for creating geometry and testing the
	// validity of this object's parameters
	void GenerateGeometry();
	bool HasValidParameters();

	// Private data accessors
	void SetCorner1(const Vector &corner1);
	void SetCorner2(const Vector &corner2);
	void SetCorner3(const Vector &corner3);

private:
	Vector corner1;
	Vector corner2;
	Vector corner3;
};

#endif// TRIANGLE_H_