/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  text_class.h
// Created:  11/17/2010
// Author:  K. Loux
// Description:  Derived from PRIMITVE, this class is used to draw text.
// History:

#ifndef _TEXT_CLASS_H_
#define _TEXT_CLASS_H_

// wxWidgets headers
#include <wx/wx.h>

// VVASE headers
#include "vRenderer/primitives/primitive.h"

// FTGL forward declarations
class FTFont;

class TEXT_RENDERING : public PRIMITIVE
{
public:
	// Constructor
	TEXT_RENDERING(RENDER_WINDOW &_RenderWindow);

	// Destructor
	~TEXT_RENDERING();

	// Mandatory overloads from PRIMITIVE - for creating geometry and testing the
	// validity of this object's parameters
	void GenerateGeometry(void);
	bool HasValidParameters(void);

	// Set option methods
	void SetAngle(double _Angle) { Angle = _Angle; Modified = true; };
	void SetFont(FTFont *_Font) { Font = _Font; Modified = true; };
	void SetText(wxString _Text) { Text = _Text; Modified = true; };
	void SetPosition(double _X, double _Y) { X = _X; Y = _Y; Modified = true; };
	void SetCentered(bool _Centered) { Centered = _Centered; Modified = true; };

	double GetTextHeight(void) const;
	double GetTextWidth(void) const;

private:
	// The angle at which this text is inclined
	double Angle;// 0 is horizontal, angle builds counter-clockwise about an axis out of the screen

	// The actual text content and font
	wxString Text;
	FTFont *Font;

	// Flag indicating whether the text is centered at (X, Y) or if, if false,
	// (X, Y) represents the lower left corner of the text bounding box
	bool Centered;

	// Position of this object in the render window
	double X, Y;
};

#endif// _TEXT_CLASS_H_