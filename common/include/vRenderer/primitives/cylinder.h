/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  cylinder.h
// Created:  5/14/2009
// Author:  K. Loux
// Description:  Derived from Primitive for creating cylindrical objects.
// History:

#ifndef _CYLINDER_H_
#define _CYLINDER_H_

// Local headers
#include "vRenderer/primitives/primitive.h"
#include "vMath/vector.h"

class Cylinder : public Primitive
{
public:
	// Constructor
	Cylinder(RenderWindow &_renderWindow);

	// Destructor
	~Cylinder();

	// Mandatory overloads from Primitive - for creating geometry and testing the
	// validity of this object's parameters
	void GenerateGeometry(void);
	bool HasValidParameters(void);

	// Private data accessors
	void SetResolution(const int &_resolution);
	void SetCapping(const bool &_drawCaps);
	void SetEndPoint1(const Vector &_endPoint1);
	void SetEndPoint2(const Vector &_endPoint2);
	void SetRadius(const double &_radius);

private:
	// Capping On/Off
	bool drawCaps;

	// The resolution
	int resolution;

	// The locations of the cylinder ends
	Vector endPoint1;
	Vector endPoint2;

	// The cylinder's radius
	double radius;
};

#endif// _CYLINDER_H_