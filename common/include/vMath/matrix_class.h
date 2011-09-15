/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  matrix_class.h
// Created:  12/2/2007
// Author:  K. Loux
// Description:  Contains class declaration for MATRIX classes.
// History:
//	2/26/2008	- Changed the way the multiplaction operator works with other matricies to avoid heap
//				  corruption when trying to pass a MATRIX back as the result of a MATRIX * MATRIX
//				  situation, K. Loux.
//	3/3/2008	- Made functions take addresses instead of new objects to reduce overhead, K. Loux.
//	4/11/2009	- Changed all functions to take addresses of and use const, K. Loux.
//	5/15/2009	- Added GetSubMatrix() and GetFirstElementPointer() methods for use with OpenGL
//				  routines, K. Loux.
//	6/15/2009	- Corrected function signatures for overloaded operators, K. Loux.
//	6/17/2009	- Added copy constructor to eliminate need to DeleteElements flag, K. Loux.
//	11/22/2009	- Moved to vMath.lib, K. Loux.

#ifndef _MATRIX_CLASS_H_
#define _MATRIX_CLASS_H_

// wxWidgets forward declarations
class wxString;

// vMath forward declarations
class VECTOR;

class MATRIX
{
public:
	// Constructors
	MATRIX(const int &_Rows, const int &_Columns);
	MATRIX(const int &_Rows, const int &_Columns, double Element1, ...);
	MATRIX(const MATRIX &Matrix);

	// Destructor
	~MATRIX();

	// Sets the values of all of the elements
	void Set(double Element1, ...);

	// Make all elements zero
	void Zero(void);

	// Makes this matrix an identity matrix
	MATRIX& MakeIdentity(void);

	// Transposition function
	MATRIX& Transpose(void);

	// Returns a matrix containing a sub-set of the contents of this matrix
	MATRIX GetSubMatrix(const int &StartRow, const int &StartColumn,
		const int &SubRows, const int &SubColumns) const;

	// Retrieve properties of this matrix
	inline int GetNumberOfRows(void) const { return Rows; };
	inline int GetNumberOfColumns(void) const { return Columns; };
	double GetElement(const int &Row, const int &Column) const;

	// Returns a pointer to the start of the array
	inline double *GetFirstElementPointer(void) const { return Elements; };

	// Set value of element at the specified location
	void SetElement(const int &Row, const int &Column, const double &Value);

	// Print this object to a string
	wxString Print(void) const;

	// Operators
	MATRIX& operator += (const MATRIX &Matrix);
	MATRIX& operator -= (const MATRIX &Matrix);
	MATRIX& operator *= (const MATRIX &Matrix);
	MATRIX& operator *= (const double &Double);
	MATRIX& operator /= (const double &Double);
	MATRIX& operator = (const MATRIX &Matrix);
	double &operator () (const int &Row, const int &Column);
	const MATRIX operator + (const MATRIX &Matrix) const;
	const MATRIX operator - (const MATRIX &Matrix) const;
	const MATRIX operator * (const MATRIX &Matrix) const;
	const MATRIX operator * (const double &Double) const;
	const VECTOR operator * (const VECTOR &Vector) const;
	const MATRIX operator / (const double &Double) const;
	const double &operator () (const int &Row, const int &Column) const;

	//VECTOR UnderVector (VECTOR Vector);// Left divide in MATLAB -> x = A\b where x and b are VECTORs
	MATRIX& RowReduce(void);

private:
	// The size of this matrix
	int Rows;
	int Columns;

	// The array of elements of this matrix
	double *Elements;
};

#endif// _MATRIX_CLASS_H_