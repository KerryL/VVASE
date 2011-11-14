/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  complex.cpp
// Created:  3/30/2008
// Author:  K. Loux
// Description:  Contains class functionality for complex number class.
// History:
//	4/11/2009	- Changed all functions to take addresses of and use const, K. Loux.
//	6/15/2009	- Corrected function signatures for overloaded operators, K. Loux.
//	11/22/2009	- Moved to vMath.lib, K. Loux.
//	11/7/2011	- Corrected camelCase, K. Loux.

// Standard C++ headers
#include <iostream>

// wxWidgets headers
#include <wx/wx.h>

// VVASE headers
#include "vMath/complex.h"

//==========================================================================
// Class:			Complex
// Function:		Complex
//
// Description:		Constructor for the Complex class.
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
Complex::Complex()
{
}

//==========================================================================
// Class:			Complex
// Function:		Complex
//
// Description:		Constructor for the Complex class.
//
// Input Arguments:
//		_real		= const double& Real component of the complex number
//		_imaginary	= const double& imaginary component of the complex number
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Complex::Complex(const double &_real, const double &_imaginary)
{
	// Assign the arguments to the class members
	real = _real;
	imaginary = _imaginary;
}

//==========================================================================
// Class:			Complex
// Function:		~Complex
//
// Description:		Destructor for the Complex class.
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
Complex::~Complex()
{
}

//==========================================================================
// Class:			Complex
// Function:		Constant Definitions
//
// Description:		Defines class level constants for the Complex class.
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
const Complex Complex::I(0.0, 1.0);

//==========================================================================
// Class:			friend of Complex
// Function:		operator <<
//
// Description:		Writes the value of Target to a stream.
//
// Input Arguments:
//		writeOut	= ostream& to write out to
//		c		= const Complex& value to be written to the stream
//
// Output Arguments:
//		None
//
// Return Value:
//		&ostream containing the formatted value
//
//==========================================================================
ostream &operator << (ostream &writeOut, const Complex &c)
{
	writeOut << c.Print();

	return writeOut;
}

//==========================================================================
// Class:			Complex
// Function:		Print
//
// Description:		Prints this object to a string.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the formatted value of this object
//
//==========================================================================
wxString Complex::Print(void) const
{
	wxString temp;

	if (imaginary >= 0)
		temp.Printf("%0.3f + %0.3f i", real, imaginary);
	else
		temp.Printf("%0.3f - %0.3f i", real, -imaginary);

	return temp;
}

//==========================================================================
// Class:			Complex
// Function:		operator +
//
// Description:		Addition operator for the Complex class.
//
// Input Arguments:
//		c	= const Complex& to add to this
//
// Output Arguments:
//		None
//
// Return Value:
//		const Complex result of the addition
//
//==========================================================================
const Complex Complex::operator + (const Complex &c) const
{
	// Make a copy of this object
	Complex temp = *this;

	// Do the addition
	temp += c;

	return temp;
}

//==========================================================================
// Class:			Complex
// Function:		operator -
//
// Description:		Subraction operator for the Complex class.
//
// Input Arguments:
//		c	= const Complex& to subtract from this
//
// Output Arguments:
//		None
//
// Return Value:
//		const Complex result of the subtraction
//
//==========================================================================
const Complex Complex::operator - (const Complex &c) const
{
	// Make a copy of this object
	Complex temp = *this;

	// Do the subtraction
	temp -= c;

	return temp;
}

//==========================================================================
// Class:			Complex
// Function:		operator *
//
// Description:		Multiplication operator for the Complex class.
//
// Input Arguments:
//		c	= const Complex& to multiply with this
//
// Output Arguments:
//		None
//
// Return Value:
//		const Complex result of the multiplication
//
//==========================================================================
const Complex Complex::operator * (const Complex &c) const
{
	// Make a copy of this object
	Complex temp = *this;

	// Do the multiplication
	temp *= c;

	return temp;
}

//==========================================================================
// Class:			Complex
// Function:		operator /
//
// Description:		Division operator for the Complex class.
//
// Input Arguments:
//		c	= const Complex& to divide by this
//
// Output Arguments:
//		None
//
// Return Value:
//		const Complex result of the division
//
//==========================================================================
const Complex Complex::operator / (const Complex &c) const
{
	// Make a copy of this object
	Complex temp = *this;

	// Do the division
	temp /= c;

	return temp;
}

//==========================================================================
// Class:			Complex
// Function:		ToPower
//
// Description:		Raises this object to the specified power.
//
// Input Arguments:
//		n	= const double& specifiying the power to which this will be raised
//
// Output Arguments:
//		None
//
// Return Value:
//		Complex& result of the power
//
//==========================================================================
Complex& Complex::ToPower(const double &n)
{
	// Convert this from Cartesian to polar form
	double r = sqrt(real * real + imaginary * imaginary);
	double theta = atan2(imaginary, real);

	// Use De Moivre's theorem to raise this to a power
	r = pow(r, n);
	theta *= n;

	// Convert back to Cartesian form
	real = r * cos(theta);
	imaginary = r * sin(theta);

	return *this;
}

//==========================================================================
// Class:			Complex
// Function:		operator +=
//
// Description:		Addition assignment operator for the Complex class.
//
// Input Arguments:
//		c	= const Complex& to add to this
//
// Output Arguments:
//		None
//
// Return Value:
//		Complex& result of the addition
//
//==========================================================================
Complex& Complex::operator += (const Complex &c)
{
	// Add the real components
	real += c.real;

	// Add the imaginary components
	imaginary += c.imaginary;

	return *this;
}

//==========================================================================
// Class:			Complex
// Function:		operator -=
//
// Description:		Subraction assignment operator for the Complex class.
//
// Input Arguments:
//		c	= const Complex& to subtract from this
//
// Output Arguments:
//		None
//
// Return Value:
//		Complex& result of the subtraction
//
//==========================================================================
Complex& Complex::operator -= (const Complex &c)
{
	// Subtract the real components
	real -= c.real;

	// Subtract the imaginary components
	imaginary -= c.imaginary;

	return *this;
}

//==========================================================================
// Class:			Complex
// Function:		operator *=
//
// Description:		Multiplication assignment operator for the Complex class.
//
// Input Arguments:
//		c	= const Complex& to multiply by this
//
// Output Arguments:
//		None
//
// Return Value:
//		Complex& result of the multiplication
//
//==========================================================================
Complex& Complex::operator *= (const Complex &c)
{
	double temp = real;

	// Similar to a dot product, the real component of the result
	// is the sum of the products of the like components
	real = real * c.real - imaginary * c.imaginary;

	// Similar to a cross product, the imaginary component of the
	// result is the sum of the products of the opposite components
	imaginary = temp * c.imaginary + imaginary * c.real;

	return *this;
}

//==========================================================================
// Class:			Complex
// Function:		operator /=
//
// Description:		Division assignment operator for the Complex class.
//
// Input Arguments:
//		c	= const Complex& to divide by this
//
// Output Arguments:
//		None
//
// Return Value:
//		Complex& result of the division
//
//==========================================================================
Complex& Complex::operator /= (const Complex &c)
{
	double temp = real;

	// Compute the real portion of the result
	real = (real * c.real + imaginary * c.imaginary) /
		(c.real * c.real + c.imaginary * c.imaginary);

	// Compute the imaginary portion of the result
	imaginary = (imaginary * c.real - temp * c.imaginary) /
		(c.real * c.real + c.imaginary * c.imaginary);

	return *this;
}

//==========================================================================
// Class:			Complex
// Function:		operator ==
//
// Description:		Equal to comparison operator for the Complex class.
//
// Input Arguments:
//		c	= const Complex& to compare to this
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for equal to, false for not equal to
//
//==========================================================================
bool Complex::operator == (const Complex &c) const
{
	// Check to see if both the real and imaginary components are equal
	if (real == c.real && imaginary == c.imaginary)
		return true;
	else
		return false;
}

//==========================================================================
// Class:			Complex
// Function:		operator !=
//
// Description:		Not equal to comparison operator for the Complex class.
//
// Input Arguments:
//		c	= const Complex& to compare to this
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, false for equal to, true for not equal to
//
//==========================================================================
bool Complex::operator != (const Complex &c) const
{
	return !(*this == c);
}

//==========================================================================
// Class:			Complex
// Function:		GetConjugate
//
// Description:		Returns the complex conjugate of this object.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		const Complex with the same real part as this and an imaginary component
//		with opposite magnitude as this
//
//==========================================================================
const Complex Complex::GetConjugate(void) const
{
	Complex temp;

	// Direct assignment of the real component
	temp.real = real;

	// The imaginary part is the opposite of this
	temp.imaginary = -imaginary;

	return temp;
}

//==========================================================================
// Class:			Complex
// Function:		operator +
//
// Description:		Addition operator for the Complex class.
//
// Input Arguments:
//		n	= const double& to add to this
//
// Output Arguments:
//		None
//
// Return Value:
//		const Complex result of the addition
//
//==========================================================================
const Complex Complex::operator + (const double &n) const
{
	Complex temp;

	// Add the real component
	temp.real = real + n;

	// Direct assignment of the imaginary componet
	temp.imaginary = imaginary;

	return temp;
}

//==========================================================================
// Class:			Complex
// Function:		operator -
//
// Description:		Subtraction operator for the Complex class.
//
// Input Arguments:
//		n	= const double& to subtract from this
//
// Output Arguments:
//		None
//
// Return Value:
//		const Complex result of the subtraction
//
//==========================================================================
const Complex Complex::operator - (const double &n) const
{
	Complex temp;

	// Subtract the real component
	temp.real = real - n;

	// Direct assignment of the imaginary component
	temp.imaginary = imaginary;

	return temp;
}

//==========================================================================
// Class:			Complex
// Function:		operator *
//
// Description:		Multiplication operator for the Complex class.
//
// Input Arguments:
//		n	= const double& to multiply by this
//
// Output Arguments:
//		None
//
// Return Value:
//		const Complex result of the multiplication
//
//==========================================================================
const Complex Complex::operator * (const double &n) const
{
	Complex temp;

	// Perform the multiplication on both components
	temp.real = real * n;
	temp.imaginary = imaginary * n;

	return temp;
}

//==========================================================================
// Class:			Complex
// Function:		operator /
//
// Description:		Division operator for the Complex class.
//
// Input Arguments:
//		n	= const double& to divide by this
//
// Output Arguments:
//		None
//
// Return Value:
//		const Complex result of the division
//
//==========================================================================
const Complex Complex::operator / (const double &n) const
{
	Complex temp;

	// Perform the division on both components
	temp.real = real / n;
	temp.imaginary = imaginary / n;

	return temp;
}