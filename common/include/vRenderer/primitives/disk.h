/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  disk.h
// Created:  5/14/2009
// Author:  K. Loux
// Description:  Derived from Primitive for creating disk objects.
// History:

#ifndef _DISK_H_
#define _DISK_H_

// Local headers
#include "vRenderer/primitives/primitive.h"
#include "vMath/vector.h"

class Disk : public Primitive
{
public:
	// Constructor
	Disk(RenderWindow &_renderWindow);

	// Destructor
	~Disk();

	// Mandatory overloads from Primitive - for creating geometry and testing the
	// validity of this object's parameters
	void GenerateGeometry(void);
	bool HasValidParameters(void);

	// Private data accessors
	void SetResolution(const int &_resolution);
	void SetOuterRadius(const double &_outerRadius);
	void SetInnerRadius(const double &_innerRadius);
	void SetCenter(const Vector &_center);
	void SetNormal(const Vector &_normal);

private:
	// The resolution
	int resolution;

	// The location of the disk
	Vector center;

	// The normal direction
	Vector normal;

	// The inner and outer radii
	double outerRadius;
	double innerRadius;
};

#endif// _DISK_H_