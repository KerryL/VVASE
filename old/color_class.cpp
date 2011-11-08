/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  color_class.cpp
// Created:  4/25/2009
// Author:  K. Loux
// Description:  Contains class definition for the COLOR class.  This class contains
//				 RGBA color definitions in the form of doubles for use with wxWidgets
//				 as well as OpenGL.
// History:

// wxWidgets headers
#include <wx/wx.h>
#include <wx/colour.h>

// VVASE headers
#include "vRenderer/color_class.h"

//==========================================================================
// Class:			COLOR
// Function:		COLOR
//
// Description:		Constructor for the COLOR class (default).
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
COLOR::COLOR()
{
	// Assign default values to the class members (solid black)
	Red = 1.0;
	Green = 1.0;
	Blue = 1.0;
	Alpha = 1.0;
}

//==========================================================================
// Class:			COLOR
// Function:		COLOR
//
// Description:		Constructor for the COLOR class.  Sets the class contents
//					as specified by the arguments.
//
// Input Arguments:
//		_Red	= const double& specifying the amount of red in this color (0.0 - 1.0)
//		_Green	= const double& specifying the amount of green in this color (0.0 - 1.0)
//		_Blue	= const double& specifying the amount of blue in this color (0.0 - 1.0)
//		_Alpha	= double specifying the opacity of this color (0.0 - 1.0)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
COLOR::COLOR(const double &_Red, const double &_Green, const double &_Blue, double _Alpha)
{
	// Assign the arguments to the class members
	Red = _Red;
	Green = _Green;
	Blue =_Blue;
	Alpha = _Alpha;

	ValidateColor();
}

//==========================================================================
// Class:			COLOR
// Function:		COLOR
//
// Description:		Destructor for the COLOR class.
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
COLOR::~COLOR()
{
}

//==========================================================================
// Class:			COLOR
// Function:		Constant Declarations
//
// Description:		Constants for the COLOR class.
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
const COLOR COLOR::ColorRed(1.0, 0.0, 0.0);
const COLOR COLOR::ColorGreen(0.0, 1.0, 0.0);
const COLOR COLOR::ColorBlue(0.0, 0.0, 1.0);
const COLOR COLOR::ColorWhite(1.0, 1.0, 1.0);
const COLOR COLOR::ColorBlack(0.0, 0.0, 0.0);
const COLOR COLOR::ColorYellow(1.0, 1.0, 0.0);
const COLOR COLOR::ColorCyan(0.0, 1.0, 1.0);
const COLOR COLOR::ColorMagenta(1.0, 0.0, 1.0);
const COLOR COLOR::ColorGray(0.5, 0.5, 0.5);

//==========================================================================
// Class:			COLOR
// Function:		Set
//
// Description:		Sets the RGBA values for this color.
//
// Input Arguments:
//		_Red	= const double& specifying the amount of red in this color (0.0 - 1.0)
//		_Green	= const double& specifying the amount of green in this color (0.0 - 1.0)
//		_Blue	= const double& specifying the amount of blue in this color (0.0 - 1.0)
//		_Alpha	= double specifying the opacity of this color (0.0 - 1.0)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void COLOR::Set(const double &_Red, const double &_Green, const double &_Blue, double _Alpha)
{
	// Assign the arguments to the class members
	Red = _Red;
	Green = _Green;
	Blue =_Blue;
	Alpha = _Alpha;

	ValidateColor();

	return;
}

//==========================================================================
// Class:			COLOR
// Function:		Set
//
// Description:		Sets the RGBA values for this color.  Overload taking
//					wxColor argument.
//
// Input Arguments:
//		Color	= wxColor& to match
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void COLOR::Set(wxColor &Color)
{
	// Convert from the wxColor to COLOR
	Red = (double)Color.Red() / 255.0;
	Green = (double)Color.Green() / 255.0;
	Blue = (double)Color.Blue() / 255.0;
	Alpha = (double)Color.Alpha() / 255.0;

	ValidateColor();

	return;
}

//==========================================================================
// Class:			COLOR
// Function:		SetAlpha
//
// Description:		Sets the alpha value for this object.
//
// Input Arguments:
//		_Alpha = const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void COLOR::SetAlpha(const double &_Alpha)
{
	Alpha = _Alpha;

	ValidateColor();

	return;
}

//==========================================================================
// Class:			COLOR
// Function:		ToWxColor
//
// Description:		Returns a wxColor object that matches this object's color.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		wxColor that matches this color
//
//==========================================================================
wxColor COLOR::ToWxColor(void)
{
	// Return object
	wxColor Color;

	// Do the conversion to a wxColor object
	Color.Set(char(Red * 255), char(Green * 255), char(Blue * 255), char(Alpha * 255));

	return Color;
}

//==========================================================================
// Class:			COLOR
// Function:		ValidateColor
//
// Description:		Forces all componenets of the color to be between 0 and 1
//					if they are not already.
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
void COLOR::ValidateColor(void)
{
	// Check red
	if (Red < 0.0)
		Red = 0.0;
	else if (Red > 1.0)
		Red = 1.0;

	// Check green
	if (Green < 0.0)
		Green = 0.0;
	else if (Green > 1.0)
		Green = 1.0;

	// Check blue
	if (Blue < 0.0)
		Blue = 0.0;
	else if (Blue > 1.0)
		Blue = 1.0;

	// Check alpha
	if (Alpha < 0.0)
		Alpha= 0.0;
	else if (Alpha > 1.0)
		Alpha = 1.0;

	return;
}