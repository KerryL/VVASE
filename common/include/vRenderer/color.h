/*===================================================================================
                                    CarDesigner
                           Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  color.h
// Created:  5/2/2011
// Author:  K. Loux
// Description:  Contains class declaration for the Color class.  This class contains
//				 RGBA color definitions in the form of doubles for use with wxWidgets
//				 as well as OpenGL.
// History:

#ifndef COLOR_H_
#define COLOR_H_

// wxWidgets forward declarations
class wxString;
class wxColor;

class Color
{
public:
	Color();
	Color(const double &red, const double &green, const double &blue, const double & alpha = 1.0);
	~Color();

	inline double GetRed() const { return red; }
	inline double GetGreen() const { return green; }
	inline double GetBlue() const { return blue; }
	inline double GetAlpha()  const { return alpha; }

	// Constant colors
	static const Color ColorRed;
	static const Color ColorGreen;
	static const Color ColorBlue;
	static const Color ColorWhite;
	static const Color ColorBlack;
	static const Color ColorYellow;
	static const Color ColorCyan;
	static const Color ColorMagenta;
	static const Color ColorGray;

	void Set(const double &red, const double &green, const double &blue, const double & alpha = 1.0);
	void Set(const wxColor &color);
	void SetAlpha(const double &alpha);

	wxColor ToWxColor() const;

private:
	// The class data
	double red;
	double green;
	double blue;
	double alpha;

	// Checks to make sure all values are between 0 and 1 (forces this to be true)
	void ValidateColor();
};

#endif// COLOR_H_