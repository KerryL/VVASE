/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  color_class.h
// Created:  4/25/2009
// Author:  K. Loux
// Description:  Contains class declaration for the COLOR class.  This class contains
//				 RGBA color definitions in the form of doubles for use with wxWidgets
//				 as well as OpenGL.
// History:

#ifndef _COLOR_CLASS_H_
#define _COLOR_CLASS_H_

// wxWidgets forward declarations
class wxString;
class wxColor;

class COLOR
{
public:
	// Constructors
	COLOR();
	COLOR(const double &_Red, const double &_Green, const double &_Blue, double _Alpha = 1.0);

	// Destructor
	~COLOR();

	// The color accessors
	inline double GetRed(void) const { return Red; };
	inline double GetGreen(void) const { return Green; };
	inline double GetBlue(void) const { return Blue; };
	inline double GetAlpha(void)  const { return Alpha; };

	// Constant colors
	static const COLOR ColorRed;
	static const COLOR ColorGreen;
	static const COLOR ColorBlue;
	static const COLOR ColorWhite;
	static const COLOR ColorBlack;
	static const COLOR ColorYellow;
	static const COLOR ColorCyan;
	static const COLOR ColorMagenta;
	static const COLOR ColorGray;

	// For setting the value
	void Set(const double &_Red, const double &_Green, const double &_Blue, double _Alpha = 1.0);
	void Set(wxColor &Color);
	void SetAlpha(const double &_Alpha);

	// Conversion to a wxColor
	wxColor ToWxColor(void);

private:
	// The class data
	double Red;
	double Green;
	double Blue;
	double Alpha;

	// Checks to make sure all values are between 0 and 1 (forces this to be true)
	void ValidateColor(void);
};

#endif// _COLOR_CLASS_H_