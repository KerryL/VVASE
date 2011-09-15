/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  triangle.h
// Created:  5/14/2009
// Author:  K. Loux
// Description:  Derived from PRIMITIVE for creating triangluar objects.
// History:

#ifndef _TRIANGLE_H_
#define _TRIANGLE_H_

// Local headers
#include "vRenderer/primitives/primitive.h"
#include "vMath/vector_class.h"

class TRIANGLE : public PRIMITIVE
{
public:
	// Constructor
	TRIANGLE(RENDER_WINDOW &_RenderWindow);

	// Destructor
	~TRIANGLE();

	// Mandatory overloads from PRIMITIVE - for creating geometry and testing the
	// validity of this object's parameters
	void GenerateGeometry(void);
	bool HasValidParameters(void);

	// Private data accessors
	void SetCorner1(const VECTOR &_Corner1);
	void SetCorner2(const VECTOR &_Corner2);
	void SetCorner3(const VECTOR &_Corner3);

private:
	// The locations of the triangle's corners
	VECTOR Corner1;
	VECTOR Corner2;
	VECTOR Corner3;
};

#endif// _TRIANGLE_H_