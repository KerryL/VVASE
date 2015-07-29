/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  text.cpp
// Created:  5/2/2011
// Author:  K. Loux
// Description:  Derived from Primitive for creating text objects on a plot.
// History:

// Local headers
#include "vRenderer/primitives/text.h"
#include "vRenderer/renderWindow.h"
#include "vMath/carMath.h"

// FTGL headers
#include <FTGL/ftgl.h>

//==========================================================================
// Class:			TextRendering
// Function:		TextRendering
//
// Description:		Constructor for the TextRendering class.
//
// Input Arguments:
//		renderWindow	= RenderWindow& reference to the object that owns this
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
TextRendering::TextRendering(RenderWindow &renderWindow) : Primitive(renderWindow)
{
	color.Set(0.0, 0.0, 0.0, 1.0);

	angle = 0.0;
	x = 0;
	y = 0;
	text = wxEmptyString;

	centered = false;
	font = NULL;
}

//==========================================================================
// Class:			TextRendering
// Function:		~TextRendering
//
// Description:		Destructor for the TextRendering class.
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
TextRendering::~TextRendering()
{
}

//==========================================================================
// Class:			TextRendering
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
void TextRendering::GenerateGeometry()
{
	if (font && !text.IsEmpty())
	{
		glPushMatrix();
			glLoadIdentity();

			// Position the text
			if (centered)
				glTranslated(x - GetTextWidth() / 2.0 * cos(angle * VVASEMath::Pi / 180.0)
					+ GetTextHeight() / 2.0 * sin(angle * VVASEMath::Pi / 180.0),
					y - GetTextWidth() / 2.0 * sin(angle * VVASEMath::Pi / 180.0)
					- GetTextHeight() / 2.0 * cos(angle * VVASEMath::Pi / 180.0), 0.0);
			else
				glTranslated(x, y, 0.0);
			glRotated(angle, 0.0, 0.0, 1.0);

			font->Render(text.mb_str());
		glPopMatrix();
	}
}

//==========================================================================
// Class:			TextRendering
// Function:		SetFont
//
// Description:		Sets the font pointer.
//
// Input Arguments:
//		font	= FTFont*
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void TextRendering::SetFont(FTFont *font)
{
	this->font = font;
	font->UseDisplayList(false);// Because we're using our own lists
	modified = true;
}

//==========================================================================
// Class:			TextRendering
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
bool TextRendering::HasValidParameters()
{
	if (VVASEMath::IsNaN(angle))
		return false;

	return true;
}

//==========================================================================
// Class:			TextRendering
// Function:		GetTextHeight
//
// Description:		Returns the height of the bounding box for the current
//					text.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		double, height in pixels of the current text
//
//==========================================================================
double TextRendering::GetTextHeight() const
{
	if (!font)
		return 0.0;

	FTBBox boundingBox = font->BBox(text.mb_str());

	return boundingBox.Upper().Y() - boundingBox.Lower().Y();
}

//==========================================================================
// Class:			TextRendering
// Function:		GetTextWidth
//
// Description:		Returns the width of the bounding box for the current
//					text.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		double, width in pixels of the current text
//
//==========================================================================
double TextRendering::GetTextWidth() const
{
	if (!font)
		return 0.0;

	FTBBox boundingBox = font->BBox(text.mb_str());

	return boundingBox.Upper().X() - boundingBox.Lower().X();
}
