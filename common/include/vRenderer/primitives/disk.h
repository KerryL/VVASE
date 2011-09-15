/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  disk.h
// Created:  5/14/2009
// Author:  K. Loux
// Description:  Derived from PRIMITIVE for creating disk objects.
// History:

#ifndef _DISK_H_
#define _DISK_H_

// Local headers
#include "vRenderer/primitives/primitive.h"
#include "vMath/vector_class.h"

class DISK : public PRIMITIVE
{
public:
	// Constructor
	DISK(RENDER_WINDOW &_RenderWindow);

	// Destructor
	~DISK();

	// Mandatory overloads from PRIMITIVE - for creating geometry and testing the
	// validity of this object's parameters
	void GenerateGeometry(void);
	bool HasValidParameters(void);

	// Private data accessors
	void SetResolution(const int &_Resolution);
	void SetOuterRadius(const double &_OuterRadius);
	void SetInnerRadius(const double &_InnerRadius);
	void SetCenter(const VECTOR &_Center);
	void SetNormal(const VECTOR &_Normal);

private:
	// The resolution
	int Resolution;

	// The location of the disk
	VECTOR Center;

	// The normal direction
	VECTOR Normal;

	// The inner and outer radii
	double OuterRadius;
	double InnerRadius;
};

#endif// _DISK_H_