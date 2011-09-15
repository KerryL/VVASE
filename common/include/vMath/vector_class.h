/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  vector_class.h
// Created:  11/3/2007
// Author:  K. Loux
// Description:  Contains class declaration for VECTOR class.  Vector class is for three element vector.
// History:
//	2/25/2008	- Changed RAD_TO_DEG and RAD_TO_DEG from constants to inline functions, K. Loux.
//	2/26/2008	- Made order of rotations an argurment in the Rotate member function, K. Loux.
//	3/3/2008	- Changed function arguments to addresses where applicable to reduce overhead, K. Loux.
//	3/9/2008	- Removed dependency on MFC class CString by switching to wxString, K. Loux.
//	3/14/2008	- Added AnglesTo function, K. Loux.
//	3/23/2008	- Moved RAD_TO_DEG and DEG_TO_RAD and constants to CONVERT class and changed
//				  arguments for class functions from degrees to radians, K. Loux.
//	4/11/2009	- Changed all functions to take addresses of and use const, K. Loux.
//	4/17/2009	- Renamed ROTATION_AXIS enumeration to AXIS, K. Loux
//	11/22/2009	- Moved to vMath.lib, K. Loux.
//	11/1/2010	- Removed non-const Normalize(), K. Loux.

#ifndef _VECTOR_CLASSS_H_
#define _VECTOR_CLASSS_H_

// Standard C++ headers
#include <cmath>
#include <iostream>

// wxWidgets forward declarations
class wxString;

// VVASE forward declarations
class MATRIX;

using namespace std;

class VECTOR
{
public:
	// Constructor
	VECTOR();
	VECTOR(const double &_X, const double &_Y, const double &_Z);

	// Destructor
	~VECTOR();

	// Main class data
	double	X, Y, Z;

	// Enumerations
	enum AXIS
	{
		AxisX,
		AxisY,
		AxisZ
	};

	// Vector norm
	double Length(void) const { return sqrt(X*X + Y*Y + Z*Z); };

	// Calculates the distance between this and Target
	double Distance(const VECTOR &Vector) const { return sqrt((X - Vector.X)*(X - Vector.X)
		+ (Y - Vector.Y)*(Y - Vector.Y) + (Z - Vector.Z)*(Z - Vector.Z)); };

	// Rotation functions
	void Rotate(const VECTOR &CoR, const VECTOR &Rotations, AXIS First = AxisX,
		AXIS Second = AxisY, AXIS Third = AxisZ);
	void Rotate(const VECTOR &CoR, const double &Angle, const AXIS &About);
	void Rotate(const double &Angle, const AXIS &About);
	void Rotate(const double &Angle, const VECTOR &RotationAxis);

	// Returns a string with the name of the specified axis
	static wxString GetAxisName(AXIS Axis);

	// Return the angles between this and Target
	VECTOR AnglesTo(const VECTOR &Target) const;

	// Prints the object to a string
	wxString Print(void) const;

	// Set this object to the specified values
	void Set(const double &_X, const double &_Y, const double &_Z);

	// Force magnitude == 1 while preserving the direction
	VECTOR Normalize(void) const;

	// Multiplication that results in a 3x3 matrix
	MATRIX OuterProduct(const VECTOR &Vector) const;

	// Operators
	VECTOR operator + (const VECTOR &Vector) const;
	VECTOR operator - (const VECTOR &Vector) const;
	double operator * (const VECTOR &Vector) const { return X * Vector.X + Y * Vector.Y + Z * Vector.Z; };// Dot product
	VECTOR operator * (const double &Double) const;// Scalar multiplication
	VECTOR operator / (const double &Double) const;// Scalar division
	VECTOR Cross(const VECTOR &Vector) const;
	VECTOR& operator += (const VECTOR &Vector);
	VECTOR& operator -= (const VECTOR &Vector);
	VECTOR& operator *= (const double &Double) { X *= Double; Y *= Double; Z *= Double; return *this; };
	VECTOR& operator /= (const double &Double) { X /= Double; Y /= Double; Z /= Double; return *this; };
	bool operator == (const VECTOR &Vector) const;
	bool operator != (const VECTOR &Vector) const;
	friend ostream& operator << (ostream &WriteOut, const VECTOR &Vector);
};

#endif// _VECTOR_CLASSS_H_