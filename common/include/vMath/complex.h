/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  complex.h
// Created:  3/30/2008
// Author:  K. Loux
// Description:  Contains class declaration for complex number class.
// History:
//	4/11/2009	- Changed all functions to take addresses of and use const, K. Loux.
//	6/15/2009	- Corrected function signatures for overloaded operators, K. Loux.
//	11/22/2009	- Moved to vMath.lib, K. Loux.
//	11/7/2011	- Corrected camelCase, K. Loux.

#ifndef COMPLEX_H_
#define COMPLEX_H_
// TODO:  Replace with std::complex
using namespace std;

class Complex
{
public:
	Complex();
	Complex(const double &real, const double &imaginary);
	~Complex();

	// Prints the value to a string
	wxString Print() const;

	const Complex GetConjugate() const;

	// Operators
	const Complex operator + (const Complex &c) const;
	const Complex operator - (const Complex &c) const;
	const Complex operator * (const Complex &c) const;
	const Complex operator / (const Complex &c) const;
	Complex& operator += (const Complex &c);
	Complex& operator -= (const Complex &c);
	Complex& operator *= (const Complex &c);
	Complex& operator /= (const Complex &c);
	bool operator == (const Complex &c) const;
	bool operator != (const Complex &c) const;
	const Complex operator + (const double &n) const;
	const Complex operator - (const double &n) const;
	const Complex operator * (const double &n) const;
	const Complex operator / (const double &n) const;

	// Raises this object to the specified power
	Complex& ToPower(const double &n);

	// For streaming the value
	friend ostream &operator << (ostream &writeOut, const Complex &c);

	// The actual data contents of this class
	double real;
	double imaginary;

	// Defining the square root of negative 1
	static const Complex I;
};

#endif// COMPLEX_H_