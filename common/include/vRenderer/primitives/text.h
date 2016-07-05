/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  text.h
// Created:  5/2/2011
// Author:  K. Loux
// Description:  Derived from Primitive, this class is used to draw text.
// History:

#ifndef TEXT_H_
#define TEXT_H_

// wxWidgets headers
#include <wx/wx.h>

// Local headers
#include "vRenderer/primitives/primitive.h"

// FTGL forward declarations
class FTFont;

class TextRendering : public Primitive
{
public:
	TextRendering(RenderWindow &renderWindow);
	~TextRendering();

	// Mandatory overloads from Primitive - for creating geometry and testing the
	// validity of this object's parameters
	void GenerateGeometry();
	bool HasValidParameters();
	bool IsIntersectedBy(const Vector& /*point*/, const Vector& /*direction*/) const { return false; }

	// Set option methods
	void SetAngle(const double& angle) { this->angle = angle; modified = true; }
	void SetFont(FTFont *font);
	void SetText(const wxString& text) { this->text = text; modified = true; }
	void SetPosition(const double& x, const double& y) { this->x = x; this->y = y; modified = true; }
	void SetCentered(const bool& centered) { this->centered = centered; modified = true; }

	double GetTextHeight() const;
	double GetTextWidth() const;
	wxString GetText() const { return text; }

private:
	double angle;// 0 is horizontal, angle builds counter-clockwise about an axis out of the screen

	wxString text;
	FTFont *font;

	// Flag indicating whether the text is centered at (X, Y) or if, if false,
	// (0, 0) represents the lower left corner of the text bounding box
	bool centered;
	double x, y;
};

#endif// TEXT_H_