/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  cone.h
// Created:  5/14/2009
// Author:  K. Loux
// Description:  Derived from PRIMITIVE for creating conical objects.
// History:

#ifndef _CONE_H_
#define _CONE_H_

// Local headers
#include "vRenderer/primitives/primitive.h"
#include "vMath/vector_class.h"

class CONE : public PRIMITIVE
{
public:
	// Constructor
	CONE(RENDER_WINDOW &_RenderWindow);

	// Destructor
	~CONE();

	// Mandatory overloads from PRIMITIVE - for creating geometry and testing the
	// validity of this object's parameters
	void GenerateGeometry(void);
	bool HasValidParameters(void);

	// Private data accessors
	void SetResolution(const int &_Resolution);
	void SetCapping(const bool &_DrawCaps);
	void SetTip(const VECTOR &_Tip);
	void SetBaseCenter(const VECTOR &_BaseCenter);
	void SetRadius(const double &_Radius);

private:
	// Capping On/Off
	bool DrawCaps;

	// The resolution
	int Resolution;

	// The locations of the cone's tip and base
	VECTOR Tip;
	VECTOR BaseCenter;

	// The cone's radius at the base
	double Radius;
};

#endif// _CONE_H_