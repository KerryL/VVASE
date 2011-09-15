/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  primitive.cpp
// Created:  5/14/2009
// Author:  K. Loux
// Description:  Abstract base class for creating 3D objects.
// History:

// Local headers
#include "vRenderer/primitives/primitive.h"
#include "vRenderer/render_window_class.h"

//==========================================================================
// Class:			PRIMITIVE
// Function:		PRIMITIVE
//
// Description:		Constructor for the PRIMITIVE class.
//
// Input Argurments:
//		_RenderWindow	= RENDER_WINDOW& pointing to the object that owns this
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
PRIMITIVE::PRIMITIVE(RENDER_WINDOW &_RenderWindow) : RenderWindow(_RenderWindow)
{
	// Initialize private data
	IsVisible = true;
	Modified = true;

	// Initialize the color to black
	Color = COLOR::ColorBlack;

	// Initialize the list index to zero
	ListIndex = 0;

	// Add this object to the renderer
	RenderWindow.AddActor(this);
}

//==========================================================================
// Class:			PRIMITIVE
// Function:		PRIMITIVE
//
// Description:		Copy constructor for the PRIMITIVE class.
//
// Input Argurments:
//		Primitive	= const PRIMITIVE& to copy to this object
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
PRIMITIVE::PRIMITIVE(const PRIMITIVE &Primitive) : RenderWindow(Primitive.RenderWindow)
{
	// Do the copy
	*this = Primitive;
}

//==========================================================================
// Class:			PRIMITIVE
// Function:		~PRIMITIVE
//
// Description:		Destructor for the PRIMITIVE class.
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
PRIMITIVE::~PRIMITIVE()
{
	// Release the glList for this object (if the list exists)
	if (ListIndex != 0)
		glDeleteLists(ListIndex, 1);
}

//==========================================================================
// Class:			PRIMITIVE
// Function:		Draw
//
// Description:		Calls two mandatory overloads that 1) check to see if the
//					information describing this object is valid, and if so, 2)
//					calls the GenerateGeometry() method to create the object.
//					Uses glLists if geometry has already been created and all
//					information is up-to-date.
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PRIMITIVE::Draw(void)
{
	// Check to see if we need to re-create the geometry for this object
	if (Modified || ListIndex == 0)
	{
		// If the list index is zero, try and get a valid index
		if (ListIndex == 0)
			ListIndex = glGenLists(1);

		// Start a new glList
		glNewList(ListIndex, GL_COMPILE);

		// Check to see if this object's parameters allow for it to be drawn correctly
		// or if the visibility flag is false
		if (!HasValidParameters() || !IsVisible)
		{
			// End the list without drawing the object
			glEndList();

			return;
		}

		// Set the color
		glColor4d(Color.GetRed(), Color.GetGreen(), Color.GetBlue(), Color.GetAlpha());

		// If the object is transparent, enable alpha blending
		if (Color.GetAlpha() != 1.0)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			// Also disable the Z-buffer
			glDepthMask(GL_FALSE);
		}

		// Generate the object's geometry
		GenerateGeometry();

		// Turn off alpha blending, if we turned it on to render this object
		if (Color.GetAlpha() != 1.0)
		{
			glDisable(GL_BLEND);

			// And re-enable the z-buffer
			glDepthMask(GL_TRUE);
		}

		// End the glList
		glEndList();

		// Reset the modified flag
		Modified = false;
	}

	// Call the list we created for this object (if it has a valid index)
	if (ListIndex != 0)
		glCallList(ListIndex);

	return;
}

//==========================================================================
// Class:			PRIMITIVE
// Function:		SetVisibility
//
// Description:		Sets the visibility flag for this object.
//
// Input Argurments:
//		_IsVisible	= const bool&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PRIMITIVE::SetVisibility(const bool &_IsVisible)
{
	// Set the visibility flag to the argument
	IsVisible = _IsVisible;
	
	// Reset the modified flag
	Modified = true;

	return;
}

//==========================================================================
// Class:			PRIMITIVE
// Function:		SetColor
//
// Description:		Sets the color of this object.
//
// Input Argurments:
//		_Color	= const COLOR&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PRIMITIVE::SetColor(const COLOR &_Color)
{
	// Set the color to the argument
	Color = _Color;
	
	// Reset the modified flag
	Modified = true;

	return;
}

//==========================================================================
// Class:			PRIMITIVE
// Function:		operator =
//
// Description:		Assignment operator for PRIMITIVE class.
//
// Input Argurments:
//		Primitive	= const PRIMITIVE& to assign to this object
//
// Output Arguments:
//		None
//
// Return Value:
//		PRIMITIVE&, reference to this object
//
//==========================================================================
PRIMITIVE& PRIMITIVE::operator = (const PRIMITIVE &Primitive)
{
	// Check for self-assignment
	if (this == &Primitive)
		return *this;

	// Perform the assignment
	IsVisible	= Primitive.IsVisible;
	Color		= Primitive.Color;
	Modified	= Primitive.Modified;

	// The list index just gets zeroed out
	ListIndex = 0;

	return *this;
}