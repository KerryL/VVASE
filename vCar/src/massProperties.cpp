/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  massProperties.cpp
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
#include "vCar/massProperties.h"
#include "vMath/matrix.h"
#include "vMath/complex.h"
#include "vUtilities/debugger.h"
#include "vUtilities/machineDefinitions.h"

//==========================================================================
// Class:			MassProperties
// Function:		MassProperties
//
// Description:		Constructor for the MassProperties class.
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
MassProperties::MassProperties()
{
	// Initialize the mass properties
	mass = 0.0;
	ixx = 0.0;
	iyy = 0.0;
	izz = 0.0;
	ixy = 0.0;
	ixz = 0.0;
	iyz = 0.0;
	centerOfGravity.x = 0.0;
	centerOfGravity.y = 0.0;
	centerOfGravity.z = 0.0;
}

//==========================================================================
// Class:			MassProperties
// Function:		MassProperties
//
// Description:		Copy constructor for the MassProperties class.
//
// Input Arguments:
//		massProperties	= const MassProperties& to copy to this object
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
MassProperties::MassProperties(const MassProperties &massProperties)
{
	// Do the copy
	*this = massProperties;
}

//==========================================================================
// Class:			MassProperties
// Function:		~MassProperties
//
// Description:		Destructor for the MassProperties class.
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
MassProperties::~MassProperties()
{
}

//==========================================================================
// Class:			MassProperties
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
bool MassProperties::IsValidInertiaTensor(void) const
{
	Vector principleInertias;
	/*Vector ixxDirection;
	Vector iyyDirection;
	Vector izzDirection;*/// FIXME:  Can these be removed?

	if (!GetPrincipleInertias(&principleInertias))
	{
		Debugger::GetInstance().Print(_T("Warning (IsValidInertiaTensor):  Failed to compute principle moments of inertia"),
			Debugger::PriorityHigh);
		return false;
	}

	// To be physically possible, the sum of any two principle moments of inertia
	// needs to be greater than the third (only valid when inertias are given with
	// respect to the CG).  Proof for this is the Generalized Perpendicular Axis Theorem
	if (principleInertias.x + principleInertias.y <= principleInertias.z ||
		principleInertias.x + principleInertias.z <= principleInertias.y ||
		principleInertias.y + principleInertias.z <= principleInertias.x)
		return false;

	return true;
}

//==========================================================================
// Class:			MassProperties
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
//		principleInertias	= Vector* specifying the principle moments of
//							  inertia for this object
//		ixxDirection		= Vector* specifying the first principle axis
//							  of rotation
//		iyyDirection		= Vector* specifying the second principle axis
//							  of rotation
//		izzDirection		= Vector* specifying the third principle axis
//							  of rotation
//
// Return Value:
//		bool, true for successful completion of calculations, false for
//		errors
//
//==========================================================================
bool MassProperties::GetPrincipleInertias(Vector *principleInertias, Vector *ixxDirection,
										   Vector *iyyDirection, Vector *izzDirection) const
{
	// Lets initialize PrincipleInertias to zero in case we return false
	principleInertias->Set(0.0, 0.0, 0.0);

	// The principle moments of inertia are the eigenvalues, and the associated
	// eigenvectors give the directions.
	double a, b, c, d;

	a = -1.0;
	b = ixx + iyy + izz;
	c = ixy * ixy + ixz * ixz + iyz * iyz - ixx * iyy - ixx * izz - iyy * izz;
	d = ixx * iyy * izz - ixx * iyz * iyz - iyy * ixz * ixz - izz * ixy * ixy
		+ 2.0 * ixy * ixz * iyz;

	// Helper variables
	double q, r;
	q = (3.0 * a * c - b * b) / (9.0 * a  * a);
	r = (9.0 * a * b * c - 27.0 * a * a * d - 2 * b * b *b) / (54.0 * a * a * a);

	// Check the descriminant to make sure we're going to get three real roots
	if (q * q * q + r * r > 0)
		return false;

	Complex i = Complex::I;
	Complex s(r, sqrt(-q * q * q - r * r));
	Complex t = s.GetConjugate();
	s = s.ToPower(1.0/3.0);
	t = t.ToPower(1.0/3.0);

	// We can ignore the imaginary parts here because we know they're zero.  We checked the
	// descriminant above to make sure.
	principleInertias->x = (s + t - b / (3.0 * a)).real;
	principleInertias->y = ((s + t) * -0.5 - b / (3.0 * a) +
		(s - t) * i * sqrt(3.0) / 2.0).real;
	principleInertias->z = ((s + t) * -0.5 - b / (3.0 * a) -
		(s - t) * i * sqrt(3.0) / 2.0).real;

	// Only calculate the directions if given valid pointers
	if (ixxDirection != NULL && iyyDirection != NULL && izzDirection != NULL)
	{
		// Calculating the eigenvectors is simply solving the following matrix equation:
		//  (Inertia - lambda * I) * x = 0.  Here, lambda is an eigenvalue (principle moment
		//  of inertia, and I is the 3x3 identity matrix.
		Matrix inertia(3, 3, ixx, ixy, ixz, ixy, iyy, iyz, ixz, iyz, izz);
		Matrix inertiaMinusILambda(3, 3);

		// Create an identity matrix
		Matrix identity(3, 3);
		identity.MakeIdentity();

		// Initialize the direction vectors
		ixxDirection->Set(0.0, 0.0, 0.0);
		iyyDirection->Set(0.0, 0.0, 0.0);
		izzDirection->Set(0.0, 0.0, 0.0);

		// Assigns Inertia - lambda * I to InertiaMinusILambda
		inertiaMinusILambda = inertia - (identity * principleInertias->x);
		inertiaMinusILambda = inertiaMinusILambda.GetRowReduced();

		// Check to see if we have any free variables and assign them 1 if we do
		// Usually, free variables are zero, here we'll use anything smaller
		// than 5.0e-8.
		double tolerance = 5.0e-8;
		if (fabs(inertiaMinusILambda.GetElement(3, 1)) < tolerance &&
			fabs(inertiaMinusILambda.GetElement(3, 2)) < tolerance &&
			fabs(inertiaMinusILambda.GetElement(3, 3)) < tolerance)
			ixxDirection->z = 1.0;
		if (fabs(inertiaMinusILambda.GetElement(2, 1)) < tolerance &&
			fabs(inertiaMinusILambda.GetElement(2, 2)) < tolerance &&
			fabs(inertiaMinusILambda.GetElement(2, 3)) < tolerance)
			ixxDirection->y = 1.0;
		if (fabs(inertiaMinusILambda.GetElement(1, 1)) < tolerance &&
			fabs(inertiaMinusILambda.GetElement(1, 2)) < tolerance &&
			fabs(inertiaMinusILambda.GetElement(1, 3)) < tolerance)
			ixxDirection->x = 1.0;

		if (ixxDirection->z != 1.0)
		{
			// No free pivots, null vector is the only solution
			Debugger::GetInstance().Print(_T("Error (GetPrincipleInertias): NULL principle direction"));
			return false;
		}
		else if (ixxDirection->y != 1.0)
		{
			// One free pivot, solve for .x and .y
			ixxDirection->y = -inertiaMinusILambda.GetElement(2, 3) /
				inertiaMinusILambda.GetElement(2, 2);
			ixxDirection->x = -(inertiaMinusILambda.GetElement(1, 2) * ixxDirection->y +
				inertiaMinusILambda.GetElement(1, 3)) / inertiaMinusILambda.GetElement(1, 1);
		}
		else if (ixxDirection->x != 1.0)
		{
			// Two free pivots, solve for .x
			ixxDirection->x =
				(inertiaMinusILambda.GetElement(1, 2) + inertiaMinusILambda.GetElement(1, 3)) /
				inertiaMinusILambda.GetElement(1, 1);
		}
		else
		{
			// No fixed pivots, all vectors are solutions
			Debugger::GetInstance().Print(_T("Error (GetPrincipleInertias): No limits on principle direction"));
			return false;
		}

		// Assigns Inertia - lambda * I to InertiaMinusILambda
		inertiaMinusILambda = inertia - (identity * principleInertias->y);
		inertiaMinusILambda = inertiaMinusILambda.GetRowReduced();

		if (fabs(inertiaMinusILambda.GetElement(3, 1)) < tolerance &&
			fabs(inertiaMinusILambda.GetElement(3, 2)) < tolerance &&
			fabs(inertiaMinusILambda.GetElement(3, 3)) < tolerance)
			iyyDirection->z = 1.0;
		if (fabs(inertiaMinusILambda.GetElement(2, 1)) < tolerance &&
			fabs(inertiaMinusILambda.GetElement(2, 2)) < tolerance &&
			fabs(inertiaMinusILambda.GetElement(2, 3)) < tolerance)
			iyyDirection->y = 1.0;
		if (fabs(inertiaMinusILambda.GetElement(1, 1)) < tolerance &&
			fabs(inertiaMinusILambda.GetElement(1, 2)) < tolerance &&
			fabs(inertiaMinusILambda.GetElement(1, 3)) < tolerance)
			iyyDirection->x = 1.0;

		if (iyyDirection->z != 1.0)
		{
			// No free pivots, null vector is the only solution
			Debugger::GetInstance().Print(_T("Error (GetPrincipleInertias): NULL principle direction"));
			return false;
		}
		else if (iyyDirection->y != 1.0)
		{
			// One free pivot, solve for .x and .y
			iyyDirection->y = -inertiaMinusILambda.GetElement(2, 3) /
				inertiaMinusILambda.GetElement(2, 2);
			iyyDirection->x = -(inertiaMinusILambda.GetElement(1, 2) * iyyDirection->y +
				inertiaMinusILambda.GetElement(1, 3)) / inertiaMinusILambda.GetElement(1, 1);
		}
		else if (iyyDirection->x != 1.0)
		{
			// Two free pivots, solve for .x
			iyyDirection->x =
				(inertiaMinusILambda.GetElement(1, 2) + inertiaMinusILambda.GetElement(1, 3)) /
				inertiaMinusILambda.GetElement(1, 1);
		}
		else
		{
			// No fixed pivots, all vectors are solutions
			Debugger::GetInstance().Print(_T("Error (GetPrincipleInertias): No limits on principle direction"));
			return false;
		}

		// Assigns Inertia - lambda * I to inertiaMinusILambda
		inertiaMinusILambda = inertia - (identity * principleInertias->z);
		inertiaMinusILambda = inertiaMinusILambda.GetRowReduced();

		if (fabs(inertiaMinusILambda.GetElement(3, 1)) < tolerance &&
			fabs(inertiaMinusILambda.GetElement(3, 2)) < tolerance &&
			fabs(inertiaMinusILambda.GetElement(3, 3)) < tolerance)
			izzDirection->z = 1.0;
		if (fabs(inertiaMinusILambda.GetElement(2, 1)) < tolerance &&
			fabs(inertiaMinusILambda.GetElement(2, 2)) < tolerance &&
			fabs(inertiaMinusILambda.GetElement(2, 3)) < tolerance)
			izzDirection->y = 1.0;
		if (fabs(inertiaMinusILambda.GetElement(1, 1)) < tolerance &&
			fabs(inertiaMinusILambda.GetElement(1, 2)) < tolerance &&
			fabs(inertiaMinusILambda.GetElement(1, 3)) < tolerance)
			izzDirection->x = 1.0;

		if (izzDirection->z != 1.0)
		{
			// No free pivots, null vector is the only solution
			Debugger::GetInstance().Print(_T("Error (GetPrincipleInertias): NULL principle direction"));
			return false;
		}
		else if (izzDirection->y != 1.0)
		{
			// One free pivot, solve for .x and .y
			izzDirection->y = -inertiaMinusILambda.GetElement(2, 3) /
				inertiaMinusILambda.GetElement(2, 2);
			izzDirection->x = -(inertiaMinusILambda.GetElement(1, 2) * izzDirection->y +
				inertiaMinusILambda.GetElement(1, 3)) / inertiaMinusILambda.GetElement(1, 1);
		}
		else if (izzDirection->x != 1.0)
		{
			// Two free pivots, solve for .x
			izzDirection->x =
				(inertiaMinusILambda.GetElement(1, 2) + inertiaMinusILambda.GetElement(1, 3)) /
				inertiaMinusILambda.GetElement(1, 1);
		}
		else
		{
			// No fixed pivots, all vectors are solutions
			Debugger::GetInstance().Print(_T("Error (GetPrincipleInertias): No limits on principle direction"));
			return false;
		}

		// Return unit vectors.
		*ixxDirection = ixxDirection->Normalize();
		*iyyDirection = iyyDirection->Normalize();
		*izzDirection = izzDirection->Normalize();
	}

	return true;
}

//==========================================================================
// Class:			MassProperties
// Function:		Write
//
// Description:		Writes these mass properties to file.
//
// Input Arguments:
//		outFile	= std::ofstream* pointing to the file stream to write to
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MassProperties::Write(std::ofstream *outFile) const
{
	// Write this object to file
	outFile->write((char*)&mass, sizeof(double));
	outFile->write((char*)&ixx, sizeof(double));
	outFile->write((char*)&iyy, sizeof(double));
	outFile->write((char*)&izz, sizeof(double));
	outFile->write((char*)&ixy, sizeof(double));
	outFile->write((char*)&ixz, sizeof(double));
	outFile->write((char*)&iyz, sizeof(double));
	outFile->write((char*)&centerOfGravity, sizeof(Vector));
	outFile->write((char*)&unsprungMass, sizeof(WheelSet));
	outFile->write((char*)&wheelInertias, sizeof(VectorSet));
}

//==========================================================================
// Class:			MassProperties
// Function:		Read
//
// Description:		Read from file to fill these mass properties.
//
// Input Arguments:
//		inFile		= std::ifstream* pointing to the file stream to read from
//		fileVersion	= int specifying which file version we're reading from
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MassProperties::Read(std::ifstream *inFile, int fileVersion)
{
	// Read this object from file accoring to the file version we're using
	if (fileVersion >= 0)// All versions
	{
		inFile->read((char*)&mass, sizeof(double));
		inFile->read((char*)&ixx, sizeof(double));
		inFile->read((char*)&iyy, sizeof(double));
		inFile->read((char*)&izz, sizeof(double));
		inFile->read((char*)&ixy, sizeof(double));
		inFile->read((char*)&ixz, sizeof(double));
		inFile->read((char*)&iyz, sizeof(double));
		inFile->read((char*)&centerOfGravity, sizeof(Vector));
		inFile->read((char*)&unsprungMass, sizeof(WheelSet));
		inFile->read((char*)&wheelInertias, sizeof(VectorSet));
	}
	else
		assert(false);
}

//==========================================================================
// Class:			MassProperties
// Function:		operator =
//
// Description:		Assignment operator for MassProperties class.
//
// Input Arguments:
//		massProperties	= const MassProperties& to assign to this object
//
// Output Arguments:
//		None
//
// Return Value:
//		MassProperties&, reference to this object
//
//==========================================================================
MassProperties& MassProperties::operator = (const MassProperties &massProperties)
{
	// Check for self-assignment
	if (this == &massProperties)
		return *this;

	// Perform the assignment
	mass			= massProperties.mass;
	ixx				= massProperties.ixx;
	iyy				= massProperties.iyy;
	izz				= massProperties.izz;
	ixy				= massProperties.ixy;
	ixz				= massProperties.ixz;
	iyz				= massProperties.iyz;
	centerOfGravity	= massProperties.centerOfGravity;
	unsprungMass	= massProperties.unsprungMass;
	wheelInertias	= massProperties.wheelInertias;

	return *this;
}