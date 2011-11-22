/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  cone.h
// Created:  5/14/2009
// Author:  K. Loux
// Description:  Derived from Primitive for creating conical objects.
// History:

#ifndef _CONE_H_
#define _CONE_H_

// Local headers
#include "vRenderer/primitives/primitive.h"
#include "vMath/vector.h"

class Cone : public Primitive
{
public:
	// Constructor
	Cone(RenderWindow &_renderWindow);

	// Destructor
	~Cone();

	// Mandatory overloads from Primitive - for creating geometry and testing the
	// validity of this object's parameters
	void GenerateGeometry(void);
	bool HasValidParameters(void);

	// Private data accessors
	void SetResolution(const int &_resolution);
	void SetCapping(const bool &_drawCaps);
	void SetTip(const Vector &_tip);
	void SetBaseCenter(const Vector &_baseCenter);
	void SetRadius(const double &_radius);

private:
	// Capping On/Off
	bool drawCaps;

	// The resolution
	int resolution;

	// The locations of the cone's tip and base
	Vector tip;
	Vector baseCenter;

	// The cone's radius at the base
	double radius;
};

#endif// _CONE_H_