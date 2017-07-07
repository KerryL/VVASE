/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  massProperties.cpp
// Date:  11/3/2007
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains class functionality for mass class.

// wxWidgets headers
#include <wx/wx.h>

// Local headers
#include "VVASE/core/car/subsystems/massProperties.h"
#include "VVASE/core/car/subsystems/suspension.h"
#include "VVASE/core/utilities/debugger.h"
#include "VVASE/core/utilities/binaryReader.h"
#include "VVASE/core/utilities/binaryWriter.h"

namespace VVASE
{

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

	totalCGHeight = 0.0;

	cornerWeights.leftFront = 0.0;
	cornerWeights.rightFront = 0.0;
	cornerWeights.leftRear = 0.0;
	cornerWeights.rightRear = 0.0;

	unsprungMass.leftFront = 0.0;
	unsprungMass.rightFront = 0.0;
	unsprungMass.leftRear = 0.0;
	unsprungMass.rightRear = 0.0;

	wheelInertias.leftFront.x = 0.0;
	wheelInertias.leftFront.y = 0.0;
	wheelInertias.leftFront.z = 0.0;

	wheelInertias.rightFront.x = 0.0;
	wheelInertias.rightFront.y = 0.0;
	wheelInertias.rightFront.z = 0.0;

	wheelInertias.leftRear.x = 0.0;
	wheelInertias.leftRear.y = 0.0;
	wheelInertias.leftRear.z = 0.0;

	wheelInertias.rightRear.x = 0.0;
	wheelInertias.rightRear.y = 0.0;
	wheelInertias.rightRear.z = 0.0;

	unsprungCGHeights.leftFront = 0.0;
	unsprungCGHeights.rightFront = 0.0;
	unsprungCGHeights.leftRear = 0.0;
	unsprungCGHeights.rightRear = 0.0;
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
bool MassProperties::IsValidInertiaTensor() const
{
	Eigen::Vector3d principleInertias;
	/*Eigen::Vector3d ixxDirection;
	Eigen::Vector3d iyyDirection;
	Eigen::Vector3d izzDirection;*/// FIXME:  Can these be removed?

	if (!GetPrincipleInertias(&principleInertias))
	{
		Debugger::GetInstance() << "Warning (IsValidInertiaTensor):  Failed to compute principle moments of inertia"
			<< Debugger::PriorityHigh;
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
//		principleInertias	= Eigen::Vector3d* specifying the principle moments of
//							  inertia for this object
//		ixxDirection		= Eigen::Vector3d* specifying the first principle axis
//							  of rotation
//		iyyDirection		= Eigen::Vector3d* specifying the second principle axis
//							  of rotation
//		izzDirection		= Eigen::Vector3d* specifying the third principle axis
//							  of rotation
//
// Return Value:
//		bool, true for successful completion of calculations, false for
//		errors
//
//==========================================================================
bool MassProperties::GetPrincipleInertias(Eigen::Vector3d *principleInertias, Eigen::Vector3d *ixxDirection,
										   Eigen::Vector3d *iyyDirection, Eigen::Vector3d *izzDirection) const
{
	// TODO:  Take references instead?
	// TODO:  Update with Eigen methods
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
			Debugger::GetInstance() << "Error (GetPrincipleInertias): NULL principle direction" << Debugger::PriorityHigh;
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
			Debugger::GetInstance() << "Error (GetPrincipleInertias): No limits on principle direction" << Debugger::PriorityHigh;
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
			Debugger::GetInstance() << "Error (GetPrincipleInertias): NULL principle direction" << Debugger::PriorityHigh;
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
			Debugger::GetInstance() << "Error (GetPrincipleInertias): No limits on principle direction" << Debugger::PriorityHigh;
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
			Debugger::GetInstance() << "Error (GetPrincipleInertias): NULL principle direction" << Debugger::PriorityHigh;
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
			Debugger::GetInstance() << "Error (GetPrincipleInertias): No limits on principle direction" << Debugger::PriorityHigh;
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
//		file	= BinaryWriter&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MassProperties::Write(BinaryWriter& file) const
{
	// Write this object to file
	file.Write(mass);
	file.Write(ixx);
	file.Write(iyy);
	file.Write(izz);
	file.Write(ixy);
	file.Write(ixz);
	file.Write(iyz);
	file.Write(totalCGHeight);
	file.Write(cornerWeights);
	file.Write(unsprungMass);
	file.Write(wheelInertias);
	file.Write(unsprungCGHeights);
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
void MassProperties::Read(BinaryReader& file, const int& fileVersion)
{
	// Read this object from file accoring to the file version we're using
	if (fileVersion >= 4)
	{
		file.Read(mass);
		file.Read(ixx);
		file.Read(iyy);
		file.Read(izz);
		file.Read(ixy);
		file.Read(ixz);
		file.Read(iyz);
		file.Read(totalCGHeight);
		file.Read(cornerWeights);
		file.Read(unsprungMass);
		file.Read(wheelInertias);
		file.Read(unsprungCGHeights);
	}
	else if (fileVersion >= 0)
	{
		file.Read(mass);
		file.Read(ixx);
		file.Read(iyy);
		file.Read(izz);
		file.Read(ixy);
		file.Read(ixz);
		file.Read(iyz);

		double dummy;
		file.Read(dummy);
		file.Read(dummy);

		file.Read(totalCGHeight);
		file.Read(unsprungMass);
		file.Read(wheelInertias);

		cornerWeights.leftFront = 600.0 / 32.174;
		cornerWeights.rightFront = 600.0 / 32.174;
		cornerWeights.leftRear = 600.0 / 32.174;
		cornerWeights.rightRear = 600.0 / 32.174;

		unsprungCGHeights.leftFront = 10.0;
		unsprungCGHeights.rightFront = 10.0;
		unsprungCGHeights.leftRear = 10.0;
		unsprungCGHeights.rightRear = 10.0;
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
	mass					= massProperties.mass;
	ixx						= massProperties.ixx;
	iyy						= massProperties.iyy;
	izz						= massProperties.izz;
	ixy						= massProperties.ixy;
	ixz						= massProperties.ixz;
	iyz						= massProperties.iyz;
	totalCGHeight			= massProperties.totalCGHeight;
	cornerWeights			= massProperties.cornerWeights;
	unsprungMass			= massProperties.unsprungMass;
	wheelInertias			= massProperties.wheelInertias;
	unsprungCGHeights		= massProperties.unsprungCGHeights;

	return *this;
}

//==========================================================================
// Class:			MassProperties
// Function:		GetTotalMass
//
// Description:		Returns calculated total mass.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		double [slug]
//
//==========================================================================
double MassProperties::GetTotalMass() const
{
	return cornerWeights.leftFront + cornerWeights.rightFront
		+ cornerWeights.leftRear + cornerWeights.rightRear;
}

//==========================================================================
// Class:			MassProperties
// Function:		GetSprungMass
//
// Description:		Returns calculated sprung mass.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		double [slug]
//
//==========================================================================
double MassProperties::GetSprungMass() const
{
	return GetTotalMass() - unsprungMass.leftFront - unsprungMass.rightFront
		- unsprungMass.leftRear - unsprungMass.rightRear;
}

//==========================================================================
// Class:			MassProperties
// Function:		GetSprungMassCG
//
// Description:		Returns calculated sprung mass CG.
//
// Input Arguments:
//		s	= const Suspension*
//
// Output Arguments:
//		None
//
// Return Value:
//		Eigen::Vector3d [in]
//
//==========================================================================
Eigen::Vector3d MassProperties::GetSprungMassCG(const Suspension* s) const
{
	const double sprungMass(GetSprungMass());
	Eigen::Vector3d cg;
	cg.x = (s->leftFront.hardpoints[Corner::ContactPatch].x * (cornerWeights.leftFront - unsprungMass.leftFront)
		+ s->rightFront.hardpoints[Corner::ContactPatch].x * (cornerWeights.rightFront - unsprungMass.rightFront)
		+ s->leftRear.hardpoints[Corner::ContactPatch].x * (cornerWeights.leftRear - unsprungMass.leftRear)
		+ s->rightRear.hardpoints[Corner::ContactPatch].x * (cornerWeights.rightRear - unsprungMass.rightRear)) / sprungMass;

	cg.y = (s->leftFront.hardpoints[Corner::ContactPatch].y * (cornerWeights.leftFront - unsprungMass.leftFront)
		+ s->rightFront.hardpoints[Corner::ContactPatch].y * (cornerWeights.rightFront - unsprungMass.rightFront)
		+ s->leftRear.hardpoints[Corner::ContactPatch].y * (cornerWeights.leftRear - unsprungMass.leftRear)
		+ s->rightRear.hardpoints[Corner::ContactPatch].y * (cornerWeights.rightRear - unsprungMass.rightRear)) / sprungMass;

	cg.z = (totalCGHeight * GetTotalMass()
		- unsprungCGHeights.leftFront * unsprungMass.leftFront
		- unsprungCGHeights.rightFront * unsprungMass.rightFront
		- unsprungCGHeights.leftRear * unsprungMass.leftRear
		- unsprungCGHeights.rightRear * unsprungMass.rightRear) / sprungMass;

	return cg;
}

}// namespace VVASE
