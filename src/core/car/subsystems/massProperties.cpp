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

// Eigen headers
#include <Eigen/Eigenvalues>

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
MassProperties::MassProperties() : Subsystem()
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

	wheelInertias.leftFront.x() = 0.0;
	wheelInertias.leftFront.y() = 0.0;
	wheelInertias.leftFront.z() = 0.0;

	wheelInertias.rightFront.x() = 0.0;
	wheelInertias.rightFront.y() = 0.0;
	wheelInertias.rightFront.z() = 0.0;

	wheelInertias.leftRear.x() = 0.0;
	wheelInertias.leftRear.y() = 0.0;
	wheelInertias.leftRear.z() = 0.0;

	wheelInertias.rightRear.x() = 0.0;
	wheelInertias.rightRear.y() = 0.0;
	wheelInertias.rightRear.z() = 0.0;

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
	GetPrincipleInertias(principleInertias);

	// To be physically possible, the sum of any two principle moments of inertia
	// needs to be greater than the third (only valid when inertias are given with
	// respect to the CG).  Proof for this is the Generalized Perpendicular Axis Theorem
	if (principleInertias.x() + principleInertias.y() <= principleInertias.z() ||
		principleInertias.x() + principleInertias.z() <= principleInertias.y() ||
		principleInertias.y() + principleInertias.z() <= principleInertias.x())
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
//		principleInertias	= Eigen::Vector3d& specifying the principle moments of
//							  inertia for this object
//		direction1			= Eigen::Vector3d* specifying the first principle axis
//							  of rotation
//		direction2			= Eigen::Vector3d* specifying the second principle axis
//							  of rotation
//		direction3			= Eigen::Vector3d* specifying the third principle axis
//							  of rotation
//
// Return Value:
//		None
//
//==========================================================================
void MassProperties::GetPrincipleInertias(Eigen::Vector3d& principleInertias,
	Eigen::Vector3d *direction1, Eigen::Vector3d *direction2, Eigen::Vector3d *direction3) const
{
	Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> solver(inertia);
	principleInertias = solver.eigenvalues();
	if (direction1)
		*direction1 = solver.eigenvectors().col(0);
	if (direction2)
		*direction2 = solver.eigenvectors().col(1);
	if (direction3)
		*direction3 = solver.eigenvectors().col(2);
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
	cg.x() = (s->leftFront.hardpoints[Corner::ContactPatch].x() * (cornerWeights.leftFront - unsprungMass.leftFront)
		+ s->rightFront.hardpoints[Corner::ContactPatch].x() * (cornerWeights.rightFront - unsprungMass.rightFront)
		+ s->leftRear.hardpoints[Corner::ContactPatch].x() * (cornerWeights.leftRear - unsprungMass.leftRear)
		+ s->rightRear.hardpoints[Corner::ContactPatch].x() * (cornerWeights.rightRear - unsprungMass.rightRear)) / sprungMass;

	cg.y() = (s->leftFront.hardpoints[Corner::ContactPatch].y() * (cornerWeights.leftFront - unsprungMass.leftFront)
		+ s->rightFront.hardpoints[Corner::ContactPatch].y() * (cornerWeights.rightFront - unsprungMass.rightFront)
		+ s->leftRear.hardpoints[Corner::ContactPatch].y() * (cornerWeights.leftRear - unsprungMass.leftRear)
		+ s->rightRear.hardpoints[Corner::ContactPatch].y() * (cornerWeights.rightRear - unsprungMass.rightRear)) / sprungMass;

	cg.z() = (totalCGHeight * GetTotalMass()
		- unsprungCGHeights.leftFront * unsprungMass.leftFront
		- unsprungCGHeights.rightFront * unsprungMass.rightFront
		- unsprungCGHeights.leftRear * unsprungMass.leftRear
		- unsprungCGHeights.rightRear * unsprungMass.rightRear) / sprungMass;

	return cg;
}

}// namespace VVASE
