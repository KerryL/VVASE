/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  corner_class.cpp
// Created:  3/23/2008
// Author:  K. Loux
// Description:  Contains class functionality for corner class.  This class conatins
//				 the suspension information for one corner of the car (four instances
//				 are required to describe the entire suspension).
// History:
//	2/16/2009	- Changed to use enumeration style array of points instead of having each
//				  point declared individually, K. Loux.
//	11/22/2009	- Moved to vCar.lib, K. Loux.

// Standard C++ headers
#include <fstream>

// wxWidgets headers
#include <wx/wx.h>

// VVASE headers
#include "vCar/corner_class.h"
#include "vUtilities/debug_class.h"
#include "vUtilities/machine_defs.h"

//==========================================================================
// Class:			CORNER
// Function:		CORNER
//
// Description:		Constructor for the CORNER class.
//
// Input Arguments:
//		_Location	= const LOCATION& describing which corner this object
//					  represents
//		_Debugger	= const DEBUGGER& reference to the application's debug
//					  printing utility
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
CORNER::CORNER(const LOCATION &_Location, const DEBUGGER &_Debugger)
			   : Location(_Location), Debugger(_Debugger)
{
	// Initialize the hardpoint locations
	int i;
	for (i = 0; i < NumberOfHardpoints; i++)
		Hardpoints[i].Set(0.0, 0.0, 0.0);

	// Initialize the other parameters, too
	StaticCamber = 0.0;
	StaticToe = 0.0;
	ActuationAttachment = AttachmentLowerAArm;
	ActuationType = ActuationPushPullrod;
	// FIXME!!! Spring and damper!
}

//==========================================================================
// Class:			CORNER
// Function:		~CORNER
//
// Description:		Destructor for the CORNER class.
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
CORNER::~CORNER()
{
}

//==========================================================================
// Class:			CORNER
// Function:		GetActuationAttachmentName
//
// Description:		Returns the name of the specified actuation attachment method.
//
// Input Arguments:
//		_Attachment	= const ACTUATION_ATTACHMENT& of interest
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containg the name of the specified attachment method
//
//==========================================================================
wxString CORNER::GetActuationAttachmentName(const ACTUATION_ATTACHMENT &_Attachment)
{
	switch (_Attachment)
	{
	case AttachmentLowerAArm:
		return _T("Lower A-Arm");
		break;

	case AttachmentUpperAArm:
		return _T("Upper A-Arm");
		break;

	case AttachmentUpright:
		return _T("Upright");
		break;

	default:
		assert(0);
		break;
	}

	return wxEmptyString;
}

//==========================================================================
// Class:			CORNER
// Function:		GetActuationTypeName
//
// Description:		Returns the name of the specified actuation type.
//
// Input Arguments:
//		_Type	= const ACTUATION_TYPE& of interest
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containg the name of the specified actuation type
//
//==========================================================================
wxString CORNER::GetActuationTypeName(const ACTUATION_TYPE &_Type)
{
	switch (_Type)
	{
	case ActuationPushPullrod:
		return _T("Push/Pullrod");
		break;

	case ActuationOutboard:
		return _T("Outboard");
		break;

	default:
		assert(0);
		break;
	}

	return wxEmptyString;
}

//==========================================================================
// Class:			CORNER
// Function:		GetHardpointName
//
// Description:		Returns the name of the point.
//
// Input Arguments:
//		Point	= const HARDPOINT&, specifying the point to retrieve
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the name of the point
//
//==========================================================================
wxString CORNER::GetHardpointName(const HARDPOINTS &Point)
{
	switch (Point)
	{
	case LowerFrontTubMount:
		return _T("Lower Front Tub Mount");
		break;

	case LowerRearTubMount:
		return _T("Lower Rear Tub Mount");
		break;

	case UpperFrontTubMount:
		return _T("Upper Front Tub Mount");
		break;

	case UpperRearTubMount:
		return _T("Upper Rear Tub Mount");
		break;

	case OutboardTieRod:
		return _T("Outboard Tie Rod");
		break;

	case InboardTieRod:
		return _T("Inboard Tie Rod");
		break;

	case WheelCenter:
		return _T("Wheel Center");
		break;

	case LowerBallJoint:
		return _T("Lower Ball Joint");
		break;

	case UpperBallJoint:
		return _T("Upper Ball Joint");
		break;

	case OutboardPushrod:
		return _T("Outboard Pushrod");
		break;

	case InboardPushrod:
		return _T("Inboard Pushrod");
		break;

	case BellCrankPivot1:
		return _T("Bell Crank Pivot 1");
		break;

	case BellCrankPivot2:
		return _T("Bell Crank Pivot 2");
		break;

	case OutboardSpring:
		return _T("Outboard Spring");
		break;

	case InboardSpring:
		return _T("Inboard Spring");
		break;

	case OutboardShock:
		return _T("Outboard Shock");
		break;

	case InboardShock:
		return _T("Inboard Shock");
		break;

	case ContactPatch:
		return _T("Contact Patch");
		break;

	case OutboardBarLink:
		return _T("Outboard Bar Link");
		break;

	case InboardBarLink:
		return _T("Inboard Bar Link");
		break;

	case BarArmAtPivot:
		return _T("Bar Arm At Pivot");
		break;

	case GearEndBarShaft:
		return _T("Gear End Bar Shaft");
		break;

	case OutboardHalfShaft:
		return _T("Outboard Half Shaft");
		break;

	case InboardHalfShaft:
		return _T("Inboard Half Shaft");
		break;

	default:
		assert(0);
		break;
	}

	return wxEmptyString;
}

//==========================================================================
// Class:			CORNER
// Function:		GetLocationName
//
// Description:		Returns the name of the specified location.
//
// Input Arguments:
//		_Location	= const LOCATION& of interest
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containg the name of the specified actuation type
//
//==========================================================================
wxString CORNER::GetLocationName(const LOCATION &_Location)
{
	switch (_Location)
	{
	case LocationLeftFront:
		return _T("Left Front");
		break;

	case LocationRightFront:
		return _T("Right Front");
		break;

	case LocationLeftRear:
		return _T("Left Rear");
		break;

	case LocationRightRear:
		return _T("Right Rear");
		break;

	default:
		assert(0);
		break;
	}

	return wxEmptyString;
}

//==========================================================================
// Class:			CORNER
// Function:		ComputeWheelCenter
//
// Description:		Computes the location of the wheel center based on the
//					static toe and camber.
//
// Input Arguments:
//		TireDiameter	= const double& specifying the tire diameter at this corner
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void CORNER::ComputeWheelCenter(const double &TireDiameter)
{
	// Get the "unperturbed" wheel center from the diameter and the contact patch location
	Hardpoints[WheelCenter] = Hardpoints[ContactPatch];
	Hardpoints[WheelCenter].Z = TireDiameter / 2.0;

	// These next two operations have sign changes depending on which
	// side of the car this corner is on:
	//  Camber is always positive for the top leaning out away from the car
	//  Toe is always positive for toe out

	// Rotate the wheel center about the X-axis for camber effects
	double RotationAngle = StaticCamber;
	if (Location == LocationRightFront || Location == LocationRightRear)
		RotationAngle *= -1.0;

	Hardpoints[WheelCenter].Rotate(Hardpoints[ContactPatch], RotationAngle, VECTOR::AxisX);

	// Rotate the wheel center about the Z axis for toe effects
	RotationAngle = StaticToe;
	if (Location == LocationRightFront || Location == LocationRightRear)
		RotationAngle *= -1.0;

	Hardpoints[WheelCenter].Rotate(Hardpoints[ContactPatch], RotationAngle, VECTOR::AxisZ);

	return;
}

//==========================================================================
// Class:			CORNER
// Function:		Write
//
// Description:		Writes this corner to file.
//
// Input Arguments:
//		OutFile	= std::ofstream* pointing to the output stream
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void CORNER::Write(std::ofstream *OutFile) const
{
	// Write the components that make up this object to file
	OutFile->write((char*)&StaticCamber, sizeof(double));
	OutFile->write((char*)&StaticToe, sizeof(double));

	// NOT YET USED!!!
	/*SPRING Spring;
	DAMPER Damper;*/

	OutFile->write((char*)&ActuationAttachment, sizeof(ACTUATION_ATTACHMENT));
	OutFile->write((char*)&ActuationType, sizeof(ACTUATION_TYPE));
	OutFile->write((char*)&Location, sizeof(LOCATION));
	OutFile->write((char*)Hardpoints, sizeof(VECTOR) * NumberOfHardpoints);

	return;
}

//==========================================================================
// Class:			CORNER
// Function:		Read
//
// Description:		Read from file to fill this corner.
//
// Input Arguments:
//		InFile		= std::ifstream* pointing to the input stream
//		FileVersion	= int specifying which file version we're reading from
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void CORNER::Read(std::ifstream *InFile, int FileVersion)
{
	// Read this object from file accoring to the file version we're using
	if (FileVersion >= 0)// All versions
	{
		// Read the components that make up this object from file
		InFile->read((char*)&StaticCamber, sizeof(double));
		InFile->read((char*)&StaticToe, sizeof(double));

		// NOT YET USED!!!
		/*SPRING Spring;
		DAMPER Damper;*/

		InFile->read((char*)&ActuationAttachment, sizeof(ACTUATION_ATTACHMENT));
		InFile->read((char*)&ActuationType, sizeof(ACTUATION_TYPE));
		InFile->read((char*)&Location, sizeof(LOCATION));
		InFile->read((char*)Hardpoints, sizeof(VECTOR) * NumberOfHardpoints);
	}
	else
		assert(0);

	return;
}

//==========================================================================
// Class:			CORNER
// Function:		operator=
//
// Description:		Assignment operator.  This is necessary due to the references
//					contained within this object.
//
// Input Arguments:
//		Corner	= const CORNER& to be copied
//
// Output Arguments:
//		None
//
// Return Value:
//		CORNER&, reference to this
//
//==========================================================================
CORNER& CORNER::operator=(const CORNER& Corner)
{
	// Check for self assignment
	if (this == &Corner)
		return *this;

	// Copy settings
	StaticCamber = Corner.StaticCamber;
	StaticToe = Corner.StaticToe;
	Spring = Corner.Spring;
	Damper = Corner.Damper;

	ActuationAttachment = Corner.ActuationAttachment;
	ActuationType = Corner.ActuationType;

	// Copy the hardpoints
	int i;
	for (i = 0; i < NumberOfHardpoints; i++)
		Hardpoints[i] = Corner.Hardpoints[i];

	return *this;
}