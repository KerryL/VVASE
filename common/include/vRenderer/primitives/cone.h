/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  cone.h
// Created:  5/14/2009
// Author:  K. Loux
// Description:  Derived from Primitive for creating conical objects.
// History:

#ifndef CONE_H_
#define CONE_H_

// Local headers
#include "vRenderer/primitives/primitive.h"
#include "vMath/vector.h"

class Cone : public Primitive
{
public:
	Cone(RenderWindow &renderWindow);
	~Cone();

	// Mandatory overloads from Primitive - for creating geometry and testing the
	// validity of this object's parameters
	void GenerateGeometry();
	bool HasValidParameters();
	bool IsIntersectedBy(const Vector& point, const Vector& direction) const;

	// Private data accessors
	void SetResolution(const int &resolution);
	void SetCapping(const bool &drawCaps);
	void SetTip(const Vector &tip);
	void SetBaseCenter(const Vector &baseCenter);
	void SetRadius(const double &radius);

private:
	bool drawCaps;

	int resolution;

	Vector tip;
	Vector baseCenter;

	double radius;
};

#endif// CONE_H_