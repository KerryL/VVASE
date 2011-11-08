/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  quadrilateral.h
// Created:  5/14/2009
// Author:  K. Loux
// Description:  Derived from PRIMITIVE for creating rectangular objects.
// History:

#ifndef _QUADRILATERAL_H_
#define _QUADRILATERAL_H_

// Local headers
#include "vRenderer/primitives/primitive.h"
#include "vMath/vector_class.h"

class QUADRILATERAL : public Primitive
{
public:
	// Constructor
	QUADRILATERAL(RenderWindow &_RenderWindow);

	// Destructor
	~QUADRILATERAL();

	// Mandatory overloads from PRIMITIVE - for creating geometry and testing the
	// validity of this object's parameters
	void GenerateGeometry(void);
	bool HasValidParameters(void);

	// Private data accessors
	void SetNormal(const Vector &_Normal);
	void SetCenter(const Vector &_Center);
	void SetAxis(const Vector &_Axis);
	void SetWidth(const double &_Width);
	void SetLength(const double &_Length);

private:
	// The normal direction for this object
	Vector Normal;

	// The axis direction (specifies the direction for the length)
	Vector Axis;

	// The location of the center of this object
	Vector Center;

	// The dimensions of the quad
	double Width;
	double Length;
};

#endif// _QUADRILATERAL_H_