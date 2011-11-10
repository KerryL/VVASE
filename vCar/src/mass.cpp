/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  mass.cpp
// Created:  11/3/2007
// Author:  K. Loux
// Description:  Contains class functionality for mass class.
// History:
//	3/9/2008	- Changed the structure of the Debugger class, K. Loux.
//  3/29/2008	- Added IsValidInertiaTensor function, K. Loux.
//	11/22/2009	- Moved to vCar.lib, K. Loux.

// Standard C++ headers
#include <fstream>

// wxWidgets headers
#include <wx/wx.h>

// VVASE headers
#include "vCar/mass.h"
#include "vMath/matrix.h"
#include "vMath/complex.h"
#include "vUtilities/debugger.h"
#include "vUtilities/machineDefinitions.h"

//==========================================================================
// Class:			MASS_PROPERTIES
// Function:		MASS_PROPERTIES
//
// Description:		Constructor for the MASS_PROPERTIES class.
//
// Input Arguments:
//		_debugger	= const Debugger& reference to applications debug printing utility
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
MASS_PROPERTIES::MASS_PROPERTIES(const Debugger &_debugger) : debugger(_debugger)
{
	// Initialize the mass properties
	Mass = 0.0;
	Ixx = 0.0;
	Iyy = 0.0;
	Izz = 0.0;
	Ixy = 0.0;
	Ixz = 0.0;
	Iyz = 0.0;
	CenterOfGravity.x = 0.0;
	CenterOfGravity.y = 0.0;
	CenterOfGravity.z = 0.0;
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
MASS_PROPERTIES::MASS_PROPERTIES(const MASS_PROPERTIES &MassProperties) : debugger(MassProperties.debugger)
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
	Vector PrincipleInertias;
	Vector IxxDirection;
	Vector IyyDirection;
	Vector IzzDirection;

	if (!GetPrincipleInertias(&PrincipleInertias))
	{
		debugger.Print(_T("Warning (IsValidInertiaTensor):  Failed to compute principle moments of inertia"),
			Debugger::PriorityHigh);
		return false;
	}

	// To be physically possible, the sum of any two principle moments of inertia
	// needs to be greater than the third (only valid when inertias are given with
	// respect to the CG).  Proof for this is the Generalized Perpendicular Axis Theorem
	if (PrincipleInertias.x + PrincipleInertias.y <= PrincipleInertias.z ||
		PrincipleInertias.x + PrincipleInertias.z <= PrincipleInertias.y ||
		PrincipleInertias.y + PrincipleInertias.z <= PrincipleInertias.x)
		return false;

	return true;
}

//==========================================================================
// Class:			MASS_PROPERTIES
// Function:		GetPrincipleInertias
//
// Description:		Returns true for successful completion.  Puts the
//					principle moments of inertia in the PrincipleInertias
//					vector where the .x value is in the IxxDirection, etc.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		PrincipleInertias	= *Vector specifying the principle moments of
//							  inertia for this object
//		IxxDirection		= *Vector specifying the first principle axis
//							  of rotation
//		IyyDirection		= *Vector specifying the second principle axis
//							  of rotation
//		IzzDirection		= *Vector specifying the third principle axis
//							  of rotation
//
// Return Value:
//		bool, true for successful completion of calculations, false for
//		errors
//
//==========================================================================
bool MASS_PROPERTIES::GetPrincipleInertias(Vector *PrincipleInertias, Vector *IxxDirection,
										   Vector *IyyDirection, Vector *IzzDirection) const
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

	Complex I = Complex::I;
	Complex s(r, sqrt(-q * q * q - r * r));
	Complex t = s.GetConjugate();
	s = s.ToPower(1.0/3.0);
	t = t.ToPower(1.0/3.0);

	// We can ignore the imaginary parts here because we know they're zero.  We checked the
	// descriminant above to make sure.
	PrincipleInertias->x = (s + t - b / (3.0 * a)).real;
	PrincipleInertias->y = ((s + t) * -0.5 - b / (3.0 * a) +
		(s - t) * I * sqrt(3.0) / 2.0).real;
	PrincipleInertias->z = ((s + t) * -0.5 - b / (3.0 * a) -
		(s - t) * I * sqrt(3.0) / 2.0).real;

	// Only calculate the directions if given valid pointers
	if (IxxDirection != NULL && IyyDirection != NULL && IzzDirection != NULL)
	{
		// Calculating the eigenvectors is simply solving the following matrix equation:
		//  (Inertia - lambda * I) * x = 0.  Here, lambda is an eigenvalue (principle moment
		//  of inertia, and I is the 3x3 identity matrix.
		Matrix Inertia(3, 3, Ixx, Ixy, Ixz, Ixy, Iyy, Iyz, Ixz, Iyz, Izz);
		Matrix InertiaMinusILambda(3, 3);

		// Create an identity matrix
		Matrix Identity(3, 3);
		Identity.MakeIdentity();

		// Initialize the direction vectors
		IxxDirection->Set(0.0, 0.0, 0.0);
		IyyDirection->Set(0.0, 0.0, 0.0);
		IzzDirection->Set(0.0, 0.0, 0.0);

		// Assigns Inertia - lambda * I to InertiaMinusILambda
		InertiaMinusILambda = Inertia - (Identity * PrincipleInertias->x);
		InertiaMinusILambda = InertiaMinusILambda.GetRowReduced();

		// Check to see if we have any free variables and assign them 1 if we do
		// Usually, free variables are zero, here we'll use anything smaller
		// than 5.0e-8.
		double Tolerance = 5.0e-8;
		if (fabs(InertiaMinusILambda.GetElement(3, 1)) < Tolerance &&
			fabs(InertiaMinusILambda.GetElement(3, 2)) < Tolerance &&
			fabs(InertiaMinusILambda.GetElement(3, 3)) < Tolerance)
			IxxDirection->z = 1.0;
		if (fabs(InertiaMinusILambda.GetElement(2, 1)) < Tolerance &&
			fabs(InertiaMinusILambda.GetElement(2, 2)) < Tolerance &&
			fabs(InertiaMinusILambda.GetElement(2, 3)) < Tolerance)
			IxxDirection->y = 1.0;
		if (fabs(InertiaMinusILambda.GetElement(1, 1)) < Tolerance &&
			fabs(InertiaMinusILambda.GetElement(1, 2)) < Tolerance &&
			fabs(InertiaMinusILambda.GetElement(1, 3)) < Tolerance)
			IxxDirection->x = 1.0;

		if (IxxDirection->z != 1.0)
		{
			// No free pivots, null vector is the only solution
			debugger.Print(_T("Error (GetPrincipleInertias): NULL principle direction"));
			return false;
		}
		else if (IxxDirection->y != 1.0)
		{
			// One free pivot, solve for .x and .y
			IxxDirection->y = -InertiaMinusILambda.GetElement(2, 3) /
				InertiaMinusILambda.GetElement(2, 2);
			IxxDirection->x = -(InertiaMinusILambda.GetElement(1, 2) * IxxDirection->y +
				InertiaMinusILambda.GetElement(1, 3)) / InertiaMinusILambda.GetElement(1, 1);
		}
		else if (IxxDirection->x != 1.0)
		{
			// Two free pivots, solve for .x
			IxxDirection->x =
				(InertiaMinusILambda.GetElement(1, 2) + InertiaMinusILambda.GetElement(1, 3)) /
				InertiaMinusILambda.GetElement(1, 1);
		}
		else
		{
			// No fixed pivots, all vectors are solutions
			debugger.Print(_T("Error (GetPrincipleInertias): No limits on principle direction"));
			return false;
		}

		// Assigns Inertia - lambda * I to InertiaMinusILambda
		InertiaMinusILambda = Inertia - (Identity * PrincipleInertias->y);
		InertiaMinusILambda = InertiaMinusILambda.GetRowReduced();

		if (fabs(InertiaMinusILambda.GetElement(3, 1)) < Tolerance &&
			fabs(InertiaMinusILambda.GetElement(3, 2)) < Tolerance &&
			fabs(InertiaMinusILambda.GetElement(3, 3)) < Tolerance)
			IyyDirection->z = 1.0;
		if (fabs(InertiaMinusILambda.GetElement(2, 1)) < Tolerance &&
			fabs(InertiaMinusILambda.GetElement(2, 2)) < Tolerance &&
			fabs(InertiaMinusILambda.GetElement(2, 3)) < Tolerance)
			IyyDirection->y = 1.0;
		if (fabs(InertiaMinusILambda.GetElement(1, 1)) < Tolerance &&
			fabs(InertiaMinusILambda.GetElement(1, 2)) < Tolerance &&
			fabs(InertiaMinusILambda.GetElement(1, 3)) < Tolerance)
			IyyDirection->x = 1.0;

		if (IyyDirection->z != 1.0)
		{
			// No free pivots, null vector is the only solution
			debugger.Print(_T("Error (GetPrincipleInertias): NULL principle direction"));
			return false;
		}
		else if (IyyDirection->y != 1.0)
		{
			// One free pivot, solve for .x and .y
			IyyDirection->y = -InertiaMinusILambda.GetElement(2, 3) /
				InertiaMinusILambda.GetElement(2, 2);
			IyyDirection->x = -(InertiaMinusILambda.GetElement(1, 2) * IyyDirection->y +
				InertiaMinusILambda.GetElement(1, 3)) / InertiaMinusILambda.GetElement(1, 1);
		}
		else if (IyyDirection->x != 1.0)
		{
			// Two free pivots, solve for .x
			IyyDirection->x =
				(InertiaMinusILambda.GetElement(1, 2) + InertiaMinusILambda.GetElement(1, 3)) /
				InertiaMinusILambda.GetElement(1, 1);
		}
		else
		{
			// No fixed pivots, all vectors are solutions
			debugger.Print(_T("Error (GetPrincipleInertias): No limits on principle direction"));
			return false;
		}

		// Assigns Inertia - lambda * I to InertiaMinusILambda
		InertiaMinusILambda = Inertia - (Identity * PrincipleInertias->z);
		InertiaMinusILambda = InertiaMinusILambda.GetRowReduced();

		if (fabs(InertiaMinusILambda.GetElement(3, 1)) < Tolerance &&
			fabs(InertiaMinusILambda.GetElement(3, 2)) < Tolerance &&
			fabs(InertiaMinusILambda.GetElement(3, 3)) < Tolerance)
			IzzDirection->z = 1.0;
		if (fabs(InertiaMinusILambda.GetElement(2, 1)) < Tolerance &&
			fabs(InertiaMinusILambda.GetElement(2, 2)) < Tolerance &&
			fabs(InertiaMinusILambda.GetElement(2, 3)) < Tolerance)
			IzzDirection->y = 1.0;
		if (fabs(InertiaMinusILambda.GetElement(1, 1)) < Tolerance &&
			fabs(InertiaMinusILambda.GetElement(1, 2)) < Tolerance &&
			fabs(InertiaMinusILambda.GetElement(1, 3)) < Tolerance)
			IzzDirection->x = 1.0;

		if (IzzDirection->z != 1.0)
		{
			// No free pivots, null vector is the only solution
			debugger.Print(_T("Error (GetPrincipleInertias): NULL principle direction"));
			return false;
		}
		else if (IzzDirection->y != 1.0)
		{
			// One free pivot, solve for .x and .y
			IzzDirection->y = -InertiaMinusILambda.GetElement(2, 3) /
				InertiaMinusILambda.GetElement(2, 2);
			IzzDirection->x = -(InertiaMinusILambda.GetElement(1, 2) * IzzDirection->y +
				InertiaMinusILambda.GetElement(1, 3)) / InertiaMinusILambda.GetElement(1, 1);
		}
		else if (IzzDirection->x != 1.0)
		{
			// Two free pivots, solve for .x
			IzzDirection->x =
				(InertiaMinusILambda.GetElement(1, 2) + InertiaMinusILambda.GetElement(1, 3)) /
				InertiaMinusILambda.GetElement(1, 1);
		}
		else
		{
			// No fixed pivots, all vectors are solutions
			debugger.Print(_T("Error (GetPrincipleInertias): No limits on principle direction"));
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
	OutFile->write((char*)&CenterOfGravity, sizeof(Vector));
	OutFile->write((char*)&UnsprungMass, sizeof(WheelSet));
	OutFile->write((char*)&WheelInertias, sizeof(VectorSet));

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
		InFile->read((char*)&CenterOfGravity, sizeof(Vector));
		InFile->read((char*)&UnsprungMass, sizeof(WheelSet));
		InFile->read((char*)&WheelInertias, sizeof(VectorSet));
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