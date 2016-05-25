/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  suspension.cpp
// Created:  11/3/2007
// Author:  K. Loux
// Description:  Contains class functionality for Suspension class.
// History:
//	2/17/2008	- Made SolveForPoint and SolveForXY use newly found analytical solution
//				  for intersection of three spheres problem, K. Loux.
//	3/9/2008	- Changed structure of Debugger class, K. Loux.
//	2/16/2009	- Changed to use enumeration style array of points instead of having each
//				  point declared individually, K. Loux.
//	4/11/2009	- Changed structure of SolveForPoint and SolveForXY to take original
//				  and new centers and locations as first step toward making point
//				  solving more robust, K. Loux.
//	4/19/2009	- Made all of the Solve...() functions static, and made Debugger static, K. Loux.
//	11/22/2009	- Moved to vCar.lib, K. Loux.
//	1/8/2010	- Modified to correct potential divide by zero in SolveForXY, K. Loux.

// Standard C++ headers
#include <fstream>

// wxWidgets headers
#include <wx/wx.h>

// VVASE headers
#include "vCar/suspension.h"
#include "vUtilities/machineDefinitions.h"
#include "vUtilities/debugger.h"

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
	leftRear(Corner::LocationLeftRear)
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
	// FIXME:  Third springs and dampers!
}

//==========================================================================
// Class:			Suspension
// Function:		Write
//
// Description:		Writes this suspension to file.
//
// Input Arguments:
//		outFile	= std::ofstream* pointing to the output stream
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Suspension::Write(std::ofstream *outFile) const
{
	// Write the components that make up this object to file
	rightFront.Write(outFile);
	leftFront.Write(outFile);
	rightRear.Write(outFile);
	leftRear.Write(outFile);

	outFile->write((char*)hardpoints, sizeof(Vector) * NumberOfHardpoints);

	outFile->write((char*)&barRate, sizeof(FrontRearDouble));
	outFile->write((char*)&rackRatio, sizeof(double));

	// Flags and styles
	outFile->write((char*)&isSymmetric, sizeof(bool));
	outFile->write((char*)&frontBarStyle, sizeof(BarStyle));
	outFile->write((char*)&rearBarStyle, sizeof(BarStyle));
	outFile->write((char*)&frontBarAttachment, sizeof(BarAttachment));
	outFile->write((char*)&rearBarAttachment, sizeof(BarAttachment));
	outFile->write((char*)&frontHasThirdSpring, sizeof(bool));
	outFile->write((char*)&rearHasThirdSpring, sizeof(bool));
}

//==========================================================================
// Class:			Suspension
// Function:		Read
//
// Description:		Read from file to fill this suspension.
//
// Input Arguments:
//		inFile		= std::ifstream* pointing to the input stream
//		fileVersion	= int specifying which file version we're reading from
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Suspension::Read(std::ifstream *inFile, int fileVersion)
{
	// Read the components that make up this object from file
	rightFront.Read(inFile, fileVersion);
	leftFront.Read(inFile, fileVersion);
	rightRear.Read(inFile, fileVersion);
	leftRear.Read(inFile, fileVersion);

	if (fileVersion < 3)
	{
		inFile->read((char*)hardpoints, sizeof(Vector) * FrontBarPivotAxis);
		hardpoints[FrontBarPivotAxis] = Vector(0.0, 0.0, 0.0);
		inFile->read((char*)hardpoints + sizeof(Vector) * (FrontBarPivotAxis + 1),
			sizeof(Vector) * (RearBarPivotAxis - FrontBarPivotAxis - 1));
		hardpoints[RearBarPivotAxis] = Vector(0.0, 0.0, 0.0);
		inFile->read((char*)hardpoints + sizeof(Vector) * (RearBarPivotAxis + 1),
			sizeof(Vector) * (NumberOfHardpoints - RearBarPivotAxis - 1));
	}
	else
		inFile->read((char*)hardpoints, sizeof(Vector) * NumberOfHardpoints);

	inFile->read((char*)&barRate, sizeof(FrontRearDouble));
	inFile->read((char*)&rackRatio, sizeof(double));

	// Flags and styles
	inFile->read((char*)&isSymmetric, sizeof(bool));
	inFile->read((char*)&frontBarStyle, sizeof(BarStyle));
	inFile->read((char*)&rearBarStyle, sizeof(BarStyle));
	inFile->read((char*)&frontBarAttachment, sizeof(BarAttachment));
	inFile->read((char*)&rearBarAttachment, sizeof(BarAttachment));
	inFile->read((char*)&frontHasThirdSpring, sizeof(bool));
	inFile->read((char*)&rearHasThirdSpring, sizeof(bool));

	// Third spring and damper objects
	// NOT YET USED!!!
	/*Spring FrontThirdSpring;
	Spring RearThirdSpring;
	Damper FrontThirdDamper;
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

	// Third spring and damper objects
	frontThirdSpring = suspension.frontThirdSpring;
	rearThirdSpring = suspension.rearThirdSpring;
	frontThirdDamper = suspension.frontThirdDamper;
	rearThirdDamper = suspension.rearThirdDamper;

	return *this;
}