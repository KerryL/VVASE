/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  quadrilateral.h
// Created:  5/14/2009
// Author:  K. Loux
// Description:  Derived from Primitive for creating rectangular objects.
// History:

#ifndef _QUADRILATERAL_H_
#define _QUADRILATERAL_H_

// Local headers
#include "vRenderer/primitives/primitive.h"
#include "vMath/vector.h"

class Quadrilateral : public Primitive
{
public:
	// Constructor
	Quadrilateral(RenderWindow &_renderWindow);

	// Destructor
	~Quadrilateral();

	// Mandatory overloads from Primitive - for creating geometry and testing the
	// validity of this object's parameters
	void GenerateGeometry(void);
	bool HasValidParameters(void);

	// Private data accessors
	void SetNormal(const Vector &_normal);
	void SetCenter(const Vector &_center);
	void SetAxis(const Vector &_axis);
	void SetWidth(const double &_width);
	void SetLength(const double &_length);

private:
	// The normal direction for this object
	Vector normal;

	// The axis direction (specifies the direction for the length)
	Vector axis;

	// The location of the center of this object
	Vector center;

	// The dimensions of the quad
	double width;
	double length;
};

#endif// _QUADRILATERAL_H_