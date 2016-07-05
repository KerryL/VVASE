/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  quadrilateral.h
// Created:  5/14/2009
// Author:  K. Loux
// Description:  Derived from Primitive for creating rectangular objects.
// History:

#ifndef QUADRILATERAL_H_
#define QUADRILATERAL_H_

// Local headers
#include "vRenderer/primitives/primitive.h"
#include "vMath/vector.h"

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

#endif// QUADRILATERAL_H_