/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  suspension.cpp
// Date:  11/3/2007
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains class functionality for Suspension class.

// wxWidgets headers
#include <wx/wx.h>

// Local headers
#include "vCar/suspension.h"
#include "VVASE/core/utilities/debugger.h"
#include "VVASE/core/utilities/binaryReader.h"
#include "VVASE/core/utilities/binaryWriter.h"

namespace VVASE
{

//==========================================================================
// Class:			Suspension
// Function:		Suspension
//
// Description:		Constructor for the Suspension class.
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
Suspension::Suspension() : rightFront(Corner::LocationRightFront),
	leftFront(Corner::LocationLeftFront), rightRear(Corner::LocationRightRear),
	leftRear(Corner::LocationLeftRear), hardpoints(NumberOfHardpoints)
{
	// Initialize the hardpoints within this object as well
	int i;
	for (i = 0; i < NumberOfHardpoints; i++)
		hardpoints[i].Set(0.0, 0.0, 0.0);

	// Initialize the other suspension parameters
	barRate.front = 0.0;
	barRate.rear = 0.0;
	rackRatio = 1.0;
	isSymmetric = false;
	frontBarStyle = SwayBarNone;
	rearBarStyle = SwayBarNone;
	frontBarAttachment = BarAttachmentBellcrank;
	rearBarAttachment = BarAttachmentBellcrank;
	frontHasThirdSpring = false;
	rearHasThirdSpring = false;

	frontThirdSpring.rate = 0.0;
	rearThirdSpring.rate = 0.0;
	// FIXME:  Third dampers!
}

//==========================================================================
// Class:			Suspension
// Function:		Write
//
// Description:		Writes this suspension to file.
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
void Suspension::Write(BinaryWriter& file) const
{
	// Write the components that make up this object to file
	rightFront.Write(file);
	leftFront.Write(file);
	rightRear.Write(file);
	leftRear.Write(file);

	file.Write(hardpoints);

	file.Write(barRate);
	file.Write(rackRatio);

	// Flags and styles
	file.Write(isSymmetric);
	file.Write((unsigned int)frontBarStyle);
	file.Write((unsigned int)rearBarStyle);
	file.Write((unsigned int)frontBarAttachment);
	file.Write((unsigned int)rearBarAttachment);
	file.Write(frontHasThirdSpring);
	file.Write(rearHasThirdSpring);

	file.Write(frontThirdSpring.rate);
	file.Write(rearThirdSpring.rate);
}

//==========================================================================
// Class:			Suspension
// Function:		Read
//
// Description:		Read from file to fill this suspension.
//
// Input Arguments:
//		file		= BinaryReader&
//		fileVersion	= const int& specifying which file version we're reading from
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Suspension::Read(BinaryReader& file, const int& fileVersion)
{
	// Read the components that make up this object from file
	rightFront.Read(file, fileVersion);
	leftFront.Read(file, fileVersion);
	rightRear.Read(file, fileVersion);
	leftRear.Read(file, fileVersion);

	if (fileVersion >= 5)
		file.Read(hardpoints);
	else if (fileVersion >= 4)
	{
		unsigned int i;
		for (i = 0; i < NumberOfHardpoints; i++)
			file.Read(hardpoints[i]);
	}
	else// if (fileVersion < 3)
	{
		assert(false);
		// TODO:  Fix this so old files will open
		/*inFile->read((char*)hardpoints, sizeof(Vector) * FrontBarPivotAxis);
		hardpoints[FrontBarPivotAxis] = Vector(0.0, 0.0, 0.0);
		inFile->read((char*)hardpoints + sizeof(Vector) * (FrontBarPivotAxis + 1),
			sizeof(Vector) * (RearBarPivotAxis - FrontBarPivotAxis - 1));
		hardpoints[RearBarPivotAxis] = Vector(0.0, 0.0, 0.0);
		inFile->read((char*)hardpoints + sizeof(Vector) * (RearBarPivotAxis + 1),
			sizeof(Vector) * (NumberOfHardpoints - RearBarPivotAxis - 1));*/
	}

	file.Read(barRate);
	file.Read(rackRatio);

	// Flags and styles
	file.Read(isSymmetric);

	unsigned int temp;
	file.Read(temp);
	frontBarStyle = static_cast<BarStyle>(temp);

	file.Read(temp);
	rearBarStyle = static_cast<BarStyle>(temp);

	file.Read(temp);
	frontBarAttachment = static_cast<BarAttachment>(temp);

	file.Read(temp);
	rearBarAttachment = static_cast<BarAttachment>(temp);

	file.Read(frontHasThirdSpring);
	file.Read(rearHasThirdSpring);

	// Third spring and damper objects
	if (fileVersion >= 4)
	{
		file.Read(frontThirdSpring.rate);
		file.Read(rearThirdSpring.rate);
	}
	// NOT YET USED!!!
	/*Damper FrontThirdDamper;
	Damper RearThirdDamper;*/
}

//==========================================================================
// Class:			Suspension
// Function:		GetBarStyleName
//
// Description:		Returns the name of the sway bar style.
//
// Input Arguments:
//		barStyle	= const BarStyle& of interest
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the name of the bar style
//
//==========================================================================
wxString Suspension::GetBarStyleName(const BarStyle &barStyle)
{
	switch (barStyle)
	{
	case SwayBarNone:
		return _T("None");
		break;

	case SwayBarUBar:
		return _T("U-Bar");
		break;

	case SwayBarTBar:
		return _T("T-Bar");
		break;

	case SwayBarGeared:
		return _T("Geared");
		break;

	default:
		assert(0);
		break;
	}

	return wxEmptyString;
}

//==========================================================================
// Class:			Suspension
// Function:		GetHardpointName
//
// Description:		Returns the name of the point.
//
// Input Arguments:
//		point	= const Hardpoints&, specifying the point to retrieve
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the name of the point
//
//==========================================================================
wxString Suspension::GetHardpointName(const Hardpoints& point)
{
	switch (point)
	{
	case FrontBarMidPoint:
		return _T("Front Bar Pivot");
		break;

	case FrontBarPivotAxis:
		return _T("Front Bar Pivot Axis");
		break;

	case FrontThirdSpringInboard:
		return _T("Front Third Spring Inboard");
		break;

	case FrontThirdSpringOutboard:
		return _T("Front Third Spring Outboard");
		break;

	case FrontThirdDamperInboard:
		return _T("Front Third Damper Inboard");
		break;

	case FrontThirdDamperOutboard:
		return _T("Front Third Damper Outboard");
		break;

	case RearBarMidPoint:
		return _T("Rear Bar Pivot");
		break;

	case RearBarPivotAxis:
		return _T("Rear Bar Pivot Axis");
		break;

	case RearThirdSpringInboard:
		return _T("Rear Third Spring Inboard");
		break;

	case RearThirdSpringOutboard:
		return _T("Rear Third Spring Outboard");
		break;

	case RearThirdDamperInboard:
		return _T("Rear Third Damper Inboard");
		break;

	case RearThirdDamperOutboard:
		return _T("Rear Third Damper Outboard");
		break;

	default:
		assert(0);
		break;
	}

	return wxEmptyString;
}

//==========================================================================
// Class:			Suspension
// Function:		GetBarAttachmentname
//
// Description:		Returns the name of the bar attachment method.
//
// Input Arguments:
//		barAttachment	= const BarAttachment&, specifying the type of attachment
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the name of the attachment method
//
//==========================================================================
wxString Suspension::GetBarAttachmentname(const BarAttachment &barAttachment)
{
	switch (barAttachment)
	{
	case BarAttachmentBellcrank:
		return _T("Bellcrank");
		break;

	case BarAttachmentLowerAArm:
		return _T("Lower A-Arm");
		break;

	case BarAttachmentUpperAArm:
		return _T("Upper A-Arm");
		break;

	case BarAttachmentUpright:
		return _T("Upright");
		break;

	default:
		assert(0);
		break;
	}

	return wxEmptyString;
}

//==========================================================================
// Class:			Suspension
// Function:		ComputeWheelCenters
//
// Description:		Calls the methods at each corner that compute the wheel
//					center location.
//
// Input Arguments:
//		rfTireDiameter	= const double& specifying the diameter of the right front tire
//		lfTireDiameter	= const double& specifying the diameter of the left front tire
//		rrTireDiameter	= const double& specifying the diameter of the right rear tire
//		lrTireDiameter	= const double& specifying the diameter of the left rear tire
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Suspension::ComputeWheelCenters(const double &rfTireDiameter, const double &lfTireDiameter,
									 const double &rrTireDiameter, const double &lrTireDiameter)
{
	// Call the methods for each corner
	rightFront.ComputeWheelCenter(rfTireDiameter);
	leftFront.ComputeWheelCenter(lfTireDiameter);
	rightRear.ComputeWheelCenter(rrTireDiameter);
	leftRear.ComputeWheelCenter(lrTireDiameter);
}

//==========================================================================
// Class:			Suspension
// Function:		operator=
//
// Description:		Overloaded assignment operator.  This is necessary due to
//					the references that this class contains.
//
// Input Arguments:
//		suspension	= const Suspension& to be assigned to this
//
// Output Arguments:
//		None
//
// Return Value:
//		Suspension&, reference to this
//
//==========================================================================
Suspension& Suspension::operator=(const Suspension& suspension)
{
	// Check for self assignment
	if (this == &suspension)
		return *this;

	// Copy the corners
	rightFront = suspension.rightFront;
	leftFront = suspension.leftFront;
	rightRear = suspension.rightRear;
	leftRear = suspension.leftRear;

	// Copy the hardpoints
	int i;
	for (i = 0; i < NumberOfHardpoints; i++)
		hardpoints[i] = suspension.hardpoints[i];

	// Suspension parameters
	barRate = suspension.barRate;
	rackRatio = suspension.rackRatio;

	// Flags and styles
	isSymmetric = suspension.isSymmetric;
	frontBarStyle = suspension.frontBarStyle;
	rearBarStyle = suspension.rearBarStyle;
	frontBarAttachment = suspension.frontBarAttachment;
	rearBarAttachment = suspension.rearBarAttachment;
	frontHasThirdSpring = suspension.frontHasThirdSpring;
	rearHasThirdSpring = suspension.rearHasThirdSpring;

	frontBarSignGreaterThan = suspension.frontBarSignGreaterThan;
	rearBarSignGreaterThan = suspension.rearBarSignGreaterThan;

	// Third spring and damper objects
	frontThirdSpring = suspension.frontThirdSpring;
	rearThirdSpring = suspension.rearThirdSpring;
	frontThirdDamper = suspension.frontThirdDamper;
	rearThirdDamper = suspension.rearThirdDamper;

	return *this;
}

//==========================================================================
// Class:			Suspension
// Function:		UpdateSymmetry
//
// Description:		Goes through all of the corner point and ensures symmetry
//					from right to left (updates left to match right).
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
void Suspension::UpdateSymmetry()
{
	// Check to make sure this car is symmetric
	if (!isSymmetric)
		return;

	unsigned int i;

	// Copy the front points
	for (i = 0; i < Corner::NumberOfHardpoints; i++)
	{
		// Copy the point
		leftFront.hardpoints[i] = rightFront.hardpoints[i];

		// Flip the sign on the Y componenet
		leftFront.hardpoints[i].y *= -1.0;
	}

	// Copy the rear points
	for (i = 0; i < Corner::NumberOfHardpoints; i++)
	{
		// Copy the point
		leftRear.hardpoints[i] = rightRear.hardpoints[i];

		// Flip the sign on the Y componenet
		leftRear.hardpoints[i].y *= -1.0;
	}

	// Copy the other information in the front
	leftFront.actuationAttachment = rightFront.actuationAttachment;
	leftFront.actuationType = rightFront.actuationType;
	leftFront.damper = rightFront.damper;
	leftFront.spring = rightFront.spring;
	leftFront.staticCamber = rightFront.staticCamber;
	leftFront.staticToe = rightFront.staticToe;

	// Copy the other information in the rear
	leftRear.actuationAttachment = rightRear.actuationAttachment;
	leftRear.actuationType = rightRear.actuationType;
	leftRear.damper = rightRear.damper;
	leftRear.spring = rightRear.spring;
	leftRear.staticCamber = rightRear.staticCamber;
	leftRear.staticToe = rightRear.staticToe;
}

}// namespace VVASE
