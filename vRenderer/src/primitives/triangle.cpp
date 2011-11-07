/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  triangle.cpp
// Created:  5/14/2009
// Author:  K. Loux
// Description:  Derived from PRIMITIVE for creating triangular objects.
// History:

// Local headers
#include "vRenderer/primitives/triangle.h"
#include "vRenderer/render_window_class.h"
#include "vMath/car_math.h"

//==========================================================================
// Class:			TRIANGLE
// Function:		TRIANGLE
//
// Description:		Constructor for the TRIANGLE class.
//
// Input Arguments:
//		_RenderWindow	= RENDER_WINDOW& pointing to the object that owns this
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
TRIANGLE::TRIANGLE(RENDER_WINDOW &_RenderWindow) : PRIMITIVE(_RenderWindow)
{
	// Initialize private data
	Corner1.Set(0.0, 0.0, 0.0);
	Corner2.Set(0.0, 0.0, 0.0);
	Corner3.Set(0.0, 0.0, 0.0);
}

//==========================================================================
// Class:			TRIANGLE
// Function:		~TRIANGLE
//
// Description:		Destructor for the TRIANGLE class.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
TRIANGLE::~TRIANGLE()
{
}

//==========================================================================
// Class:			TRIANGLE
// Function:		GenerateGeometry
//
// Description:		Creates the OpenGL instructions to create this object in
//					the scene.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void TRIANGLE::GenerateGeometry(void)
{
	// Used as each triangle is created
	VECTOR Normal = (Corner2 - Corner1).Cross(Corner3 - Corner1).Normalize();

	// Set the normal for the triangle
	glNormal3d(Normal.X, Normal.Y, Normal.Z);

	// This is just one triangle
	glBegin(GL_TRIANGLES);

	// Add the three vertecies
	glVertex3d(Corner1.X, Corner1.Y, Corner1.Z);
	glVertex3d(Corner2.X, Corner2.Y, Corner2.Z);
	glVertex3d(Corner3.X, Corner3.Y, Corner3.Z);

	// Complete the triangle
	glEnd();

	return;
}

//==========================================================================
// Class:			TRIANGLE
// Function:		HasValidParameters
//
// Description:		Checks to see if the information about this object is
//					valid and complete (gives permission to create the object).
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for OK to draw, false otherwise
//
//==========================================================================
bool TRIANGLE::HasValidParameters(void)
{
	// Triangles must have non-zero edge lengths
	if (!VVASEMath::IsZero(Corner1.Distance(Corner2)) && !VVASEMath::IsZero(Corner1.Distance(Corner3))
		&& !VVASEMath::IsZero(Corner2.Distance(Corner3)))
		return true;

	// Otherwise return false
	return false;
}

//==========================================================================
// Class:			TRIANGLE
// Function:		SetCorner1
//
// Description:		Sets the location of the first corner of the triangle.
//
// Input Arguments:
//		_Corner1	= const VECTOR&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void TRIANGLE::SetCorner1(const VECTOR &_Corner1)
{
	// Set the corner location to the argument
	Corner1 = _Corner1;
	
	// Reset the modified flag
	Modified = true;

	return;
}

//==========================================================================
// Class:			TRIANGLE
// Function:		SetCorner2
//
// Description:		Sets the location of the second corner of the triangle.
//
// Input Arguments:
//		_Corner2	= const VECTOR&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void TRIANGLE::SetCorner2(const VECTOR &_Corner2)
{
	// Set the corner location to the argument
	Corner2 = _Corner2;
	
	// Reset the modified flag
	Modified = true;

	return;
}

//==========================================================================
// Class:			TRIANGLE
// Function:		SetCorner3
//
// Description:		Sets the location of the third corner of the triangle.
//
// Input Arguments:
//		_Corner3	= const VECTOR&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void TRIANGLE::SetCorner3(const VECTOR &_Corner3)
{
	// Set the corner location to the argument
	Corner3 = _Corner3;
	
	// Reset the modified flag
	Modified = true;

	return;
}