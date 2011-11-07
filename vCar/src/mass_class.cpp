/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  mass_class.cpp
// Created:  11/3/2007
// Author:  K. Loux
// Description:  Contains class functionality for mass class.
// History:
//	3/9/2008	- Changed the structure of the DEBUGGER class, K. Loux.
//  3/29/2008	- Added IsValidInertiaTensor function, K. Loux.
//	11/22/2009	- Moved to vCar.lib, K. Loux.

// Standard C++ headers
#include <fstream>

// wxWidgets headers
#include <wx/wx.h>

// VVASE headers
#include "vCar/mass_class.h"
#include "vMath/matrix_class.h"
#include "vMath/complex_class.h"
#include "vUtilities/debug_class.h"
#include "vUtilities/machine_defs.h"

//==========================================================================
// Class:			MASS_PROPERTIES
// Function:		MASS_PROPERTIES
//
// Description:		Constructor for the MASS_PROPERTIES class.
//
// Input Arguments:
//		_Debugger	= const DEBUGGER& reference to applications debug printing utility
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
MASS_PROPERTIES::MASS_PROPERTIES(const DEBUGGER &_Debugger) : Debugger(_Debugger)
{
	// Initialize the mass properties
	Mass = 0.0;
	Ixx = 0.0;
	Iyy = 0.0;
	Izz = 0.0;
	Ixy = 0.0;
	Ixz = 0.0;
	Iyz = 0.0;
	CenterOfGravity.X = 0.0;
	CenterOfGravity.Y = 0.0;
	CenterOfGravity.Z = 0.0;
}

//==========================================================================
// Class:			MASS_PROPERTIES
// Function:		MASS_PROPERTIES
//
// Description:		Copy constructor for the MASS_PROPERTIES class.
//
// Input Arguments:
//		MassProperties	= const MASS_PROPERTIES& to copy to this object
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
MASS_PROPERTIES::MASS_PROPERTIES(const MASS_PROPERTIES &MassProperties) : Debugger(MassProperties.Debugger)
{
	// Do the copy
	*this = MassProperties;
}

//==========================================================================
// Class:			MASS_PROPERTIES
// Function:		~MASS_PROPERTIES
//
// Description:		Destructor for the MASS_PROPERTIES class.
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
MASS_PROPERTIES::~MASS_PROPERTIES()
{
}

//==========================================================================
// Class:			MASS_PROPERTIES
// Function:		IsValidInertiaTensor
//
// Description:		Checks to see if the inertias specified by the user are
//					physically possible
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for valid properties, false for impossible properties
//
//==========================================================================
bool MASS_PROPERTIES::IsValidInertiaTensor(void) const
{
	VECTOR PrincipleInertias;
	VECTOR IxxDirection;
	VECTOR IyyDirection;
	VECTOR IzzDirection;

	if (!GetPrincipleInertias(&PrincipleInertias))
	{
		Debugger.Print(_T("Warning (IsValidInertiaTensor):  Failed to compute principle moments of inertia"),
			DEBUGGER::PriorityHigh);
		return false;
	}

	// To be physically possible, the sum of any two principle moments of inertia
	// needs to be greater than the third (only valid when inertias are given with
	// respect to the CG).  Proof for this is the Generalized Perpendicular Axis Theorem
	if (PrincipleInertias.X + PrincipleInertias.Y <= PrincipleInertias.Z ||
		PrincipleInertias.X + PrincipleInertias.Z <= PrincipleInertias.Y ||
		PrincipleInertias.Y + PrincipleInertias.Z <= PrincipleInertias.X)
		return false;

	return true;
}

//==========================================================================
// Class:			MASS_PROPERTIES
// Function:		GetPrincipleInertias
//
// Description:		Returns true for successful completion.  Puts the
//					principle moments of inertia in the PrincipleInertias
//					vector where the .X value is in the IxxDirection, etc.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		PrincipleInertias	= *VECTOR specifying the principle moments of
//							  inertia for this object
//		IxxDirection		= *VECTOR specifying the first principle axis
//							  of rotation
//		IyyDirection		= *VECTOR specifying the second principle axis
//							  of rotation
//		IzzDirection		= *VECTOR specifying the third principle axis
//							  of rotation
//
// Return Value:
//		bool, true for successful completion of calculations, false for
//		errors
//
//==========================================================================
bool MASS_PROPERTIES::GetPrincipleInertias(VECTOR *PrincipleInertias, VECTOR *IxxDirection,
										   VECTOR *IyyDirection, VECTOR *IzzDirection) const
{
	// Lets initialize PrincipleInertias to zero in case we return false
	PrincipleInertias->Set(0.0, 0.0, 0.0);

	// The principle moments of inertia are the eigenvalues, and the associated
	// eigenvectors give the directions.
	double a, b, c, d;

	a = -1.0;
	b = Ixx + Iyy + Izz;
	c = Ixy * Ixy + Ixz * Ixz + Iyz * Iyz - Ixx * Iyy - Ixx * Izz - Iyy * Izz;
	d = Ixx * Iyy * Izz - Ixx * Iyz * Iyz - Iyy * Ixz * Ixz - Izz * Ixy * Ixy
		+ 2.0 * Ixy * Ixz * Iyz;

	// Helper variables
	double q, r;
	q = (3.0 * a * c - b * b) / (9.0 * a  * a);
	r = (9.0 * a * b * c - 27.0 * a * a * d - 2 * b * b *b) / (54.0 * a * a * a);

	// Check the descriminant to make sure we're going to get three real roots
	if (q * q * q + r * r > 0)
		return false;

	COMPLEX I = COMPLEX::I;
	COMPLEX s(r, sqrt(-q * q * q - r * r));
	COMPLEX t = s.GetConjugate();
	s = s.ToPower(1.0/3.0);
	t = t.ToPower(1.0/3.0);

	// We can ignore the imaginary parts here because we know they're zero.  We checked the
	// descriminant above to make sure.
	PrincipleInertias->X = (s + t - b / (3.0 * a)).Real;
	PrincipleInertias->Y = ((s + t) * -0.5 - b / (3.0 * a) +
		(s - t) * I * sqrt(3.0) / 2.0).Real;
	PrincipleInertias->Z = ((s + t) * -0.5 - b / (3.0 * a) -
		(s - t) * I * sqrt(3.0) / 2.0).Real;

	// Only calculate the directions if given valid pointers
	if (IxxDirection != NULL && IyyDirection != NULL && IzzDirection != NULL)
	{
		// Calculating the eigenvectors is simply solving the following matrix equation:
		//  (Inertia - lambda * I) * x = 0.  Here, lambda is an eigenvalue (principle moment
		//  of inertia, and I is the 3x3 identity matrix.
		MATRIX Inertia(3, 3, Ixx, Ixy, Ixz, Ixy, Iyy, Iyz, Ixz, Iyz, Izz);
		MATRIX InertiaMinusILambda(3, 3);

		// Create an identity matrix
		MATRIX Identity(3, 3);
		Identity.MakeIdentity();

		// Initialize the direction vectors
		IxxDirection->Set(0.0, 0.0, 0.0);
		IyyDirection->Set(0.0, 0.0, 0.0);
		IzzDirection->Set(0.0, 0.0, 0.0);

		// Assigns Inertia - lambda * I to InertiaMinusILambda
		InertiaMinusILambda = Inertia - (Identity * PrincipleInertias->X);
		InertiaMinusILambda.RowReduce();

		// Check to see if we have any free variables and assign them 1 if we do
		// Usually, free variables are zero, here we'll use anything smaller
		// than 5.0e-8.
		double Tolerance = 5.0e-8;
		if (fabs(InertiaMinusILambda.GetElement(3, 1)) < Tolerance &&
			fabs(InertiaMinusILambda.GetElement(3, 2)) < Tolerance &&
			fabs(InertiaMinusILambda.GetElement(3, 3)) < Tolerance)
			IxxDirection->Z = 1.0;
		if (fabs(InertiaMinusILambda.GetElement(2, 1)) < Tolerance &&
			fabs(InertiaMinusILambda.GetElement(2, 2)) < Tolerance &&
			fabs(InertiaMinusILambda.GetElement(2, 3)) < Tolerance)
			IxxDirection->Y = 1.0;
		if (fabs(InertiaMinusILambda.GetElement(1, 1)) < Tolerance &&
			fabs(InertiaMinusILambda.GetElement(1, 2)) < Tolerance &&
			fabs(InertiaMinusILambda.GetElement(1, 3)) < Tolerance)
			IxxDirection->X = 1.0;

		if (IxxDirection->Z != 1.0)
		{
			// No free pivots, null vector is the only solution
			Debugger.Print(_T("Error (GetPrincipleInertias): NULL principle direction"));
			return false;
		}
		else if (IxxDirection->Y != 1.0)
		{
			// One free pivot, solve for .X and .Y
			IxxDirection->Y = -InertiaMinusILambda.GetElement(2, 3) /
				InertiaMinusILambda.GetElement(2, 2);
			IxxDirection->X = -(InertiaMinusILambda.GetElement(1, 2) * IxxDirection->Y +
				InertiaMinusILambda.GetElement(1, 3)) / InertiaMinusILambda.GetElement(1, 1);
		}
		else if (IxxDirection->X != 1.0)
		{
			// Two free pivots, solve for .X
			IxxDirection->X =
				(InertiaMinusILambda.GetElement(1, 2) + InertiaMinusILambda.GetElement(1, 3)) /
				InertiaMinusILambda.GetElement(1, 1);
		}
		else
		{
			// No fixed pivots, all vectors are solutions
			Debugger.Print(_T("Error (GetPrincipleInertias): No limits on principle direction"));
			return false;
		}

		// Assigns Inertia - lambda * I to InertiaMinusILambda
		InertiaMinusILambda = Inertia - (Identity * PrincipleInertias->Y);
		InertiaMinusILambda.RowReduce();

		if (fabs(InertiaMinusILambda.GetElement(3, 1)) < Tolerance &&
			fabs(InertiaMinusILambda.GetElement(3, 2)) < Tolerance &&
			fabs(InertiaMinusILambda.GetElement(3, 3)) < Tolerance)
			IyyDirection->Z = 1.0;
		if (fabs(InertiaMinusILambda.GetElement(2, 1)) < Tolerance &&
			fabs(InertiaMinusILambda.GetElement(2, 2)) < Tolerance &&
			fabs(InertiaMinusILambda.GetElement(2, 3)) < Tolerance)
			IyyDirection->Y = 1.0;
		if (fabs(InertiaMinusILambda.GetElement(1, 1)) < Tolerance &&
			fabs(InertiaMinusILambda.GetElement(1, 2)) < Tolerance &&
			fabs(InertiaMinusILambda.GetElement(1, 3)) < Tolerance)
			IyyDirection->X = 1.0;

		if (IyyDirection->Z != 1.0)
		{
			// No free pivots, null vector is the only solution
			Debugger.Print(_T("Error (GetPrincipleInertias): NULL principle direction"));
			return false;
		}
		else if (IyyDirection->Y != 1.0)
		{
			// One free pivot, solve for .X and .Y
			IyyDirection->Y = -InertiaMinusILambda.GetElement(2, 3) /
				InertiaMinusILambda.GetElement(2, 2);
			IyyDirection->X = -(InertiaMinusILambda.GetElement(1, 2) * IyyDirection->Y +
				InertiaMinusILambda.GetElement(1, 3)) / InertiaMinusILambda.GetElement(1, 1);
		}
		else if (IyyDirection->X != 1.0)
		{
			// Two free pivots, solve for .X
			IyyDirection->X =
				(InertiaMinusILambda.GetElement(1, 2) + InertiaMinusILambda.GetElement(1, 3)) /
				InertiaMinusILambda.GetElement(1, 1);
		}
		else
		{
			// No fixed pivots, all vectors are solutions
			Debugger.Print(_T("Error (GetPrincipleInertias): No limits on principle direction"));
			return false;
		}

		// Assigns Inertia - lambda * I to InertiaMinusILambda
		InertiaMinusILambda = Inertia - (Identity * PrincipleInertias->Z);
		InertiaMinusILambda.RowReduce();

		if (fabs(InertiaMinusILambda.GetElement(3, 1)) < Tolerance &&
			fabs(InertiaMinusILambda.GetElement(3, 2)) < Tolerance &&
			fabs(InertiaMinusILambda.GetElement(3, 3)) < Tolerance)
			IzzDirection->Z = 1.0;
		if (fabs(InertiaMinusILambda.GetElement(2, 1)) < Tolerance &&
			fabs(InertiaMinusILambda.GetElement(2, 2)) < Tolerance &&
			fabs(InertiaMinusILambda.GetElement(2, 3)) < Tolerance)
			IzzDirection->Y = 1.0;
		if (fabs(InertiaMinusILambda.GetElement(1, 1)) < Tolerance &&
			fabs(InertiaMinusILambda.GetElement(1, 2)) < Tolerance &&
			fabs(InertiaMinusILambda.GetElement(1, 3)) < Tolerance)
			IzzDirection->X = 1.0;

		if (IzzDirection->Z != 1.0)
		{
			// No free pivots, null vector is the only solution
			Debugger.Print(_T("Error (GetPrincipleInertias): NULL principle direction"));
			return false;
		}
		else if (IzzDirection->Y != 1.0)
		{
			// One free pivot, solve for .X and .Y
			IzzDirection->Y = -InertiaMinusILambda.GetElement(2, 3) /
				InertiaMinusILambda.GetElement(2, 2);
			IzzDirection->X = -(InertiaMinusILambda.GetElement(1, 2) * IzzDirection->Y +
				InertiaMinusILambda.GetElement(1, 3)) / InertiaMinusILambda.GetElement(1, 1);
		}
		else if (IzzDirection->X != 1.0)
		{
			// Two free pivots, solve for .X
			IzzDirection->X =
				(InertiaMinusILambda.GetElement(1, 2) + InertiaMinusILambda.GetElement(1, 3)) /
				InertiaMinusILambda.GetElement(1, 1);
		}
		else
		{
			// No fixed pivots, all vectors are solutions
			Debugger.Print(_T("Error (GetPrincipleInertias): No limits on principle direction"));
			return false;
		}

		// Return unit vectors.
		*IxxDirection = IxxDirection->Normalize();
		*IyyDirection = IyyDirection->Normalize();
		*IzzDirection = IzzDirection->Normalize();
	}

	return true;
}

//==========================================================================
// Class:			MASS_PROPERTIES
// Function:		Write
//
// Description:		Writes these mass properties to file.
//
// Input Arguments:
//		OutFile	= std::ofstream* pointing to the file stream to write to
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MASS_PROPERTIES::Write(std::ofstream *OutFile) const
{
	// Write this object to file
	OutFile->write((char*)&Mass, sizeof(double));
	OutFile->write((char*)&Ixx, sizeof(double));
	OutFile->write((char*)&Iyy, sizeof(double));
	OutFile->write((char*)&Izz, sizeof(double));
	OutFile->write((char*)&Ixy, sizeof(double));
	OutFile->write((char*)&Ixz, sizeof(double));
	OutFile->write((char*)&Iyz, sizeof(double));
	OutFile->write((char*)&CenterOfGravity, sizeof(VECTOR));
	OutFile->write((char*)&UnsprungMass, sizeof(WHEEL_SET));
	OutFile->write((char*)&WheelInertias, sizeof(VECTOR_SET));

	return;
}

//==========================================================================
// Class:			MASS_PROPERTIES
// Function:		Read
//
// Description:		Read from file to fill these mass properties.
//
// Input Arguments:
//		InFile		= std::ifstream* pointing to the file stream to read from
//		FileVersion	= int specifying which file version we're reading from
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MASS_PROPERTIES::Read(std::ifstream *InFile, int FileVersion)
{
	// Read this object from file accoring to the file version we're using
	if (FileVersion >= 0)// All versions
	{
		InFile->read((char*)&Mass, sizeof(double));
		InFile->read((char*)&Ixx, sizeof(double));
		InFile->read((char*)&Iyy, sizeof(double));
		InFile->read((char*)&Izz, sizeof(double));
		InFile->read((char*)&Ixy, sizeof(double));
		InFile->read((char*)&Ixz, sizeof(double));
		InFile->read((char*)&Iyz, sizeof(double));
		InFile->read((char*)&CenterOfGravity, sizeof(VECTOR));
		InFile->read((char*)&UnsprungMass, sizeof(WHEEL_SET));
		InFile->read((char*)&WheelInertias, sizeof(VECTOR_SET));
	}
	else
		assert(0);

	return;
}

//==========================================================================
// Class:			MASS_PROPERTIES
// Function:		operator =
//
// Description:		Assignment operator for MASS_PROPERTIES class.
//
// Input Arguments:
//		MassProperties	= const MASS_PROPERTIES& to assign to this object
//
// Output Arguments:
//		None
//
// Return Value:
//		MASS_PROPERTIES&, reference to this object
//
//==========================================================================
MASS_PROPERTIES& MASS_PROPERTIES::operator = (const MASS_PROPERTIES &MassProperties)
{
	// Check for self-assignment
	if (this == &MassProperties)
		return *this;

	// Perform the assignment
	Mass			= MassProperties.Mass;
	Ixx				= MassProperties.Ixx;
	Iyy				= MassProperties.Iyy;
	Izz				= MassProperties.Izz;
	Ixy				= MassProperties.Ixy;
	Ixz				= MassProperties.Ixz;
	Iyz				= MassProperties.Iyz;
	CenterOfGravity	= MassProperties.CenterOfGravity;
	UnsprungMass	= MassProperties.UnsprungMass;
	WheelInertias	= MassProperties.WheelInertias;

	return *this;
}