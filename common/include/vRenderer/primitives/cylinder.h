/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  cylinder.h
// Created:  5/14/2009
// Author:  K. Loux
// Description:  Derived from PRIMITIVE for creating cylindrical objects.
// History:

#ifndef _CYLINDER_H_
#define _CYLINDER_H_

// Local headers
#include "vRenderer/primitives/primitive.h"
#include "vMath/vector_class.h"

class CYLINDER : public PRIMITIVE
{
public:
	// Constructor
	CYLINDER(RENDER_WINDOW &_RenderWindow);

	// Destructor
	~CYLINDER();

	// Mandatory overloads from PRIMITIVE - for creating geometry and testing the
	// validity of this object's parameters
	void GenerateGeometry(void);
	bool HasValidParameters(void);

	// Private data accessors
	void SetResolution(const int &_Resolution);
	void SetCapping(const bool &_DrawCaps);
	void SetEndPoint1(const VECTOR &_EndPoint1);
	void SetEndPoint2(const VECTOR &_EndPoint2);
	void SetRadius(const double &_Radius);

private:
	// Capping On/Off
	bool DrawCaps;

	// The resolution
	int Resolution;

	// The locations of the cylinder ends
	VECTOR EndPoint1;
	VECTOR EndPoint2;

	// The cylinder's radius
	double Radius;
};

#endif// _CYLINDER_H_