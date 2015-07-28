/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  zoomBox.cpp
// Created:  5/5/2011
// Author:  K. Loux
// Description:  Logic for drawing the zoom box as the user moves the mouse.
// History:

// Local headers
#include "vRenderer/primitives/zoomBox.h"
#include "vRenderer/renderWindow.h"
#include "vRenderer/line.h"

//==========================================================================
// Class:			ZoomBox
// Function:		ZoomBox
//
// Description:		Constructor for ZoomBox class.
//
// Input Arguments:
//		renderWindow	= RenderWindow&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
ZoomBox::ZoomBox(RenderWindow &renderWindow) : Primitive(renderWindow)
{
	// Initially, we don't want to draw this
	isVisible = false;

	xAnchor = 0;
	yAnchor = 0;

	xFloat = 0;
	yFloat = 0;

	color = Color::ColorBlack;
}

//==========================================================================
// Class:			ZoomBox
// Function:		GenerateGeometry
//
// Description:		Generates OpenGL commands to draw the box.
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
void ZoomBox::GenerateGeometry()
{
	Line box;
	std::vector<std::pair<double, double> > points;
	points.push_back(std::make_pair(xAnchor, yAnchor));
	points.push_back(std::make_pair(xFloat, yAnchor));
	points.push_back(std::make_pair(xFloat, yFloat));
	points.push_back(std::make_pair(xAnchor, yFloat));
	points.push_back(std::make_pair(xAnchor, yAnchor));
	box.Draw(points);
}

//==========================================================================
// Class:			ZoomBox
// Function:		HasValidParameters
//
// Description:		Checks to see if we're OK to draw the box.
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
bool ZoomBox::HasValidParameters()
{
	// Just make sure the box actually exists
	// It is up to the render window to ensure the coordinates are valid
	if (abs(int(xAnchor - xFloat)) > 0 && abs(int(yAnchor - yFloat)) > 0)
		return true;

	return false;
}

//==========================================================================
// Class:			ZoomBox
// Function:		SetAnchorCorner
//
// Description:		Sets the location of the box's fixed corner.
//
// Input Arguments:
//		xAnchor	= const unsigned int& (pixels)
//		yAnchor	= const unsigned int& (pixels)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void ZoomBox::SetAnchorCorner(const unsigned int &xAnchor, const unsigned int &yAnchor)
{
	this->xAnchor = xAnchor;
	this->yAnchor = yAnchor;

	modified = true;
}

//==========================================================================
// Class:			ZoomBox
// Function:		SetFloatingCorner
//
// Description:		Sets the location of the box's floating corner.
//
// Input Arguments:
//		xFloat	= const unsigned int& (pixels)
//		yFloat	= const unsigned int& (pixels)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void ZoomBox::SetFloatingCorner(const unsigned int &xFloat, const unsigned int &yFloat)
{
	this->xFloat = xFloat;
	this->yFloat = yFloat;

	modified = true;
}