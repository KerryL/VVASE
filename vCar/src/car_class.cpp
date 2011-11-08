/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  car_class.cpp
// Created:  11/3/2007
// Author:  K. Loux
// Description:  Contains class functionaliy for car class.
// History:
//	2/24/2008	- Changed half shaft points from DRIVETRAIN to SUSPENSION objects, K. Loux.
//	3/9/2008	- Changed the structure of the Debugger class, K. Loux.
//	4/25/2009	- Added appearance options to write/read methods, incrementented file version
//				  to version 1, K. Loux.
//	5/3/2009	- Added file version arguement to SubSystem->Read() calls for backwards compatability
//				  with file versions, K. Loux.
//	11/22/2009	- Moved to vCar.lib, K. Loux.
//	12/12/2010	- Removed calls to lock CarMutex within the car object - this is done to prevent
//				  race conditions resulting from the need to repeatedly lock and unlock in
//				  an owner object to prevent a dead lock, K. Loux.

// wxWidgets headers
#include <wx/wx.h>

// VVASE headers
#include "vCar/aerodynamics_class.h"
#include "vCar/brakes_class.h"
#include "vCar/car_class.h"
#include "vCar/drivetrain_class.h"
#include "vCar/engine_class.h"
#include "vCar/mass_class.h"
#include "vCar/suspension_class.h"
#include "vCar/tire_class.h"
#include "vCar/tireset_class.h"
#include "vUtilities/debug_class.h"
#include "vUtilities/wheelset_structs.h"
#include "vUtilities/machine_defs.h"
#include "vMath/vector_class.h"

// Standard C++ headers
#include <fstream>

//==========================================================================
// Class:			CAR
// Function:		CAR
//
// Description:		Constructor for the CAR class.  Creates a set of default
//					parameters for the object.
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
CAR::CAR(const Debugger &_debugger) : debugger(_debugger)
{
	// Dynamically allocate memory for these classes
	Suspension = new SUSPENSION(debugger);
	Drivetrain = new DRIVETRAIN(debugger);
	Brakes = new BRAKES(debugger);
	Aerodynamics = new AERODYNAMICS(debugger);
	Engine = new ENGINE(debugger);
	MassProperties = new MASS_PROPERTIES(debugger);
	Tires = new TIRE_SET(debugger);

	// Test suspension
	Suspension->FrontBarStyle = SUSPENSION::SwayBarUBar;
	Suspension->IsSymmetric = true;

	Suspension->RightFront.Hardpoints[CORNER::ContactPatch].Set(0.0, 21.875, 0.0);
	Suspension->LeftFront.Hardpoints[CORNER::ContactPatch].Set(0.0, -21.875, 0.0);
	Suspension->RightRear.Hardpoints[CORNER::ContactPatch].Set(66.5, 20.0, 0.0);
	Suspension->LeftRear.Hardpoints[CORNER::ContactPatch].Set(66.5, -20.0, 0.0);

	Suspension->RightFront.Hardpoints[CORNER::LowerFrontTubMount].Set(-6.893, 3.0, 5.0);
	Suspension->LeftFront.Hardpoints[CORNER::LowerFrontTubMount].Set(-6.893, -3.0, 5.0);
	Suspension->RightRear.Hardpoints[CORNER::LowerFrontTubMount].Set(62.5, 3.2, 5.0);
	Suspension->LeftRear.Hardpoints[CORNER::LowerFrontTubMount].Set(62.5, -3.2, 5.0);

	Suspension->RightFront.Hardpoints[CORNER::LowerRearTubMount].Set(5.107, 3.0, 5.0);
	Suspension->LeftFront.Hardpoints[CORNER::LowerRearTubMount].Set(5.107, -3.0, 5.0);
	Suspension->RightRear.Hardpoints[CORNER::LowerRearTubMount].Set(72.5, 3.2, 5.0);
	Suspension->LeftRear.Hardpoints[CORNER::LowerRearTubMount].Set(72.5, -3.2, 5.0);

	Suspension->RightFront.Hardpoints[CORNER::LowerBallJoint].Set(0.168, 19.8, 5.0);
	Suspension->LeftFront.Hardpoints[CORNER::LowerBallJoint].Set(0.168, -19.8, 5.0);
	Suspension->RightRear.Hardpoints[CORNER::LowerBallJoint].Set(66.213, 20.35, 5.125);
	Suspension->LeftRear.Hardpoints[CORNER::LowerBallJoint].Set(66.213, -20.35, 5.125);

	Suspension->RightFront.Hardpoints[CORNER::UpperFrontTubMount].Set(0.893, 7.0, 13.75);
	Suspension->LeftFront.Hardpoints[CORNER::UpperFrontTubMount].Set(0.893, -7.0, 13.75);
	Suspension->RightRear.Hardpoints[CORNER::UpperFrontTubMount].Set(59.159, 5.0, 12.67);
	Suspension->LeftRear.Hardpoints[CORNER::UpperFrontTubMount].Set(59.159, -5.0, 12.67);

	Suspension->RightFront.Hardpoints[CORNER::UpperRearTubMount].Set(9.393, 7.0, 13.75);
	Suspension->LeftFront.Hardpoints[CORNER::UpperRearTubMount].Set(9.393, -7.0, 13.75);
	Suspension->RightRear.Hardpoints[CORNER::UpperRearTubMount].Set(73.659, 5.0, 12.67);
	Suspension->LeftRear.Hardpoints[CORNER::UpperRearTubMount].Set(73.659, -5.0, 12.67);

	Suspension->RightFront.Hardpoints[CORNER::UpperBallJoint].Set(0.893, 19.0, 15.25);
	Suspension->LeftFront.Hardpoints[CORNER::UpperBallJoint].Set(0.893, -19.0, 15.25);
	Suspension->RightRear.Hardpoints[CORNER::UpperBallJoint].Set(63.5, 18.025, 14.75);
	Suspension->LeftRear.Hardpoints[CORNER::UpperBallJoint].Set(63.5, -18.025, 14.75);

	Suspension->RightFront.Hardpoints[CORNER::InboardTieRod].Set(-0.482, 7.0, 13.75);
	Suspension->LeftFront.Hardpoints[CORNER::InboardTieRod].Set(-0.482, -7.0, 13.75);
	Suspension->RightRear.Hardpoints[CORNER::InboardTieRod].Set(74.925, 5.0, 12.67);
	Suspension->LeftRear.Hardpoints[CORNER::InboardTieRod].Set(74.925, -5.0, 12.67);

	Suspension->RightFront.Hardpoints[CORNER::OutboardTieRod].Set(-0.896, 19.893, 15.36);
	Suspension->LeftFront.Hardpoints[CORNER::OutboardTieRod].Set(-0.896, -19.893, 15.36);
	Suspension->RightRear.Hardpoints[CORNER::OutboardTieRod].Set(69.5, 18.025, 14.75);
	Suspension->LeftRear.Hardpoints[CORNER::OutboardTieRod].Set(69.5, -18.025, 14.75);

	Suspension->RightFront.Hardpoints[CORNER::BellCrankPivot1].Set(-0.482, 8.063, 17.625);
	Suspension->LeftFront.Hardpoints[CORNER::BellCrankPivot1].Set(-0.482, -8.063, 17.625);
	Suspension->RightRear.Hardpoints[CORNER::BellCrankPivot1].Set(62.75, 3.986, 13.255);
	Suspension->LeftRear.Hardpoints[CORNER::BellCrankPivot1].Set(62.75, -3.986, 13.255);

	Suspension->RightFront.Hardpoints[CORNER::BellCrankPivot2].Set(-0.482, 6.438, 16.161);
	Suspension->LeftFront.Hardpoints[CORNER::BellCrankPivot2].Set(-0.482, -6.438, 16.161);
	Suspension->RightRear.Hardpoints[CORNER::BellCrankPivot2].Set(62.75, 4.986, 14.987);
	Suspension->LeftRear.Hardpoints[CORNER::BellCrankPivot2].Set(62.75, -4.986, 14.987);

	Suspension->RightFront.Hardpoints[CORNER::InboardPushrod].Set(2.768, 7.0, 16.75);
	Suspension->LeftFront.Hardpoints[CORNER::InboardPushrod].Set(2.768, -7.0, 16.75);
	Suspension->RightRear.Hardpoints[CORNER::InboardPushrod].Set(61.31, 4.849, 13.918);
	Suspension->LeftRear.Hardpoints[CORNER::InboardPushrod].Set(61.31, -4.849, 13.918);

	Suspension->RightFront.Hardpoints[CORNER::OutboardPushrod].Set(0.107, 17.813, 5.625);
	Suspension->LeftFront.Hardpoints[CORNER::OutboardPushrod].Set(0.107, -17.813, 5.625);
	Suspension->RightRear.Hardpoints[CORNER::OutboardPushrod].Set(65.0, 17.86, 6.1);
	Suspension->LeftRear.Hardpoints[CORNER::OutboardPushrod].Set(65.0, -17.86, 6.1);

	Suspension->RightFront.Hardpoints[CORNER::InboardShock].Set(-9.232, 4.75, 18.75);
	Suspension->LeftFront.Hardpoints[CORNER::InboardShock].Set(-9.232, -4.75, 18.75);
	Suspension->RightRear.Hardpoints[CORNER::InboardShock].Set(72.25, 1.455, 15.871);
	Suspension->LeftRear.Hardpoints[CORNER::InboardShock].Set(72.25, -1.455, 15.871);

	Suspension->RightFront.Hardpoints[CORNER::InboardSpring].Set(-9.232, 4.75, 18.75);
	Suspension->LeftFront.Hardpoints[CORNER::InboardSpring].Set(-9.232, -4.75, 18.75);
	Suspension->RightRear.Hardpoints[CORNER::InboardSpring].Set(72.25, 1.455, 15.871);
	Suspension->LeftRear.Hardpoints[CORNER::InboardSpring].Set(72.25, -1.455, 15.871);

	Suspension->RightFront.Hardpoints[CORNER::OutboardShock].Set(-1.232, 3.375, 19.5);
	Suspension->LeftFront.Hardpoints[CORNER::OutboardShock].Set(-1.232, -3.375, 19.5);
	Suspension->RightRear.Hardpoints[CORNER::OutboardShock].Set(62.75, 1.455, 15.871);
	Suspension->LeftRear.Hardpoints[CORNER::OutboardShock].Set(62.75, -1.455, 15.871);

	Suspension->RightFront.Hardpoints[CORNER::OutboardSpring].Set(-1.232, 3.375, 19.5);
	Suspension->LeftFront.Hardpoints[CORNER::OutboardSpring].Set(-1.232, -3.375, 19.5);
	Suspension->RightRear.Hardpoints[CORNER::OutboardSpring].Set(62.75, 1.455, 15.871);
	Suspension->LeftRear.Hardpoints[CORNER::OutboardSpring].Set(62.75, -1.455, 15.871);

	Suspension->RightFront.Hardpoints[CORNER::BarArmAtPivot].Set(4.518, 5.0, 15.0);
	Suspension->LeftFront.Hardpoints[CORNER::BarArmAtPivot].Set(4.518, -5.0, 15.0);
	Suspension->RightRear.Hardpoints[CORNER::BarArmAtPivot].Set(0.0, 0.0, 0.0);
	Suspension->LeftRear.Hardpoints[CORNER::BarArmAtPivot].Set(0.0, 0.0, 0.0);

	Suspension->Hardpoints[SUSPENSION::FrontBarMidPoint].Set(4.518, 0.0, 15.0);
	Suspension->Hardpoints[SUSPENSION::RearBarMidPoint].Set(0.0, 0.0, 0.0);

	Suspension->RightFront.Hardpoints[CORNER::InboardBarLink].Set(4.518, 5.0, 19.0);
	Suspension->LeftFront.Hardpoints[CORNER::InboardBarLink].Set(4.518, -5.0, 19.0);
	Suspension->RightRear.Hardpoints[CORNER::InboardBarLink].Set(0.0, 0.0, 0.0);
	Suspension->LeftRear.Hardpoints[CORNER::InboardBarLink].Set(0.0, 0.0, 0.0);

	Suspension->RightFront.Hardpoints[CORNER::OutboardBarLink].Set(-1.232, 5.0, 19.0);
	Suspension->LeftFront.Hardpoints[CORNER::OutboardBarLink].Set(-1.232, -5.0, 19.0);
	Suspension->RightRear.Hardpoints[CORNER::OutboardBarLink].Set(0.0, 0.0, 0.0);
	Suspension->LeftRear.Hardpoints[CORNER::OutboardBarLink].Set(0.0, 0.0, 0.0);

	Suspension->RightRear.Hardpoints[CORNER::InboardHalfShaft].Set(66.5, 7.0, 9.75);
	Suspension->LeftRear.Hardpoints[CORNER::InboardHalfShaft].Set(66.5, -7.0, 9.75);
	Suspension->RightRear.Hardpoints[CORNER::OutboardHalfShaft].Set(66.5, 20.0, 10.25);
	Suspension->LeftRear.Hardpoints[CORNER::OutboardHalfShaft].Set(66.5, -20.0, 10.25);

	Suspension->RackRatio = 0.8 * 2.0 / 3.14159;// [in/rad]

	// Test tires
	Tires->RightFront->Diameter = 20.5;
	Tires->RightFront->Width = 7.0;

	Tires->LeftFront->Diameter = 20.5;
	Tires->LeftFront->Width = 7.0;

	Tires->RightRear->Diameter = 20.5;
	Tires->RightRear->Width = 7.0;

	Tires->LeftRear->Diameter = 20.5;
	Tires->LeftRear->Width = 7.0;

	// Test brakes
	Brakes->RearBrakesInboard = true;
	Brakes->PercentFrontBraking = 0.667;

	// Test mass properties
	MassProperties->CenterOfGravity.Set(34.0, 0.0, 8.75);
	MassProperties->Mass = 520.0 / 32.174;
}

//==========================================================================
// Class:			CAR
// Function:		CAR
//
// Description:		Copy Constructor for the CAR class.  This call the copy
//					constructors for all of the subsystems as well.  This
//					DOES NOT allocate memory for this car.  Correct usage is
//					something like:
//						CAR NewCar = new CAR(OldCar);
//
// Input Arguments:
//		Car	= const &CAR that is to be copied
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
CAR::CAR(const CAR &Car) : debugger(Car.debugger)
{
	// Initialize the pointers
	Suspension		= NULL;
	Drivetrain		= NULL;
	Brakes			= NULL;
	Aerodynamics	= NULL;
	Engine			= NULL;
	MassProperties	= NULL;
	Tires			= NULL;

	// Do the copy
	*this = Car;
}

//==========================================================================
// Class:			CAR
// Function:		~CAR
//
// Description:		Destructor for the CAR class.
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
CAR::~CAR()
{
	// Delete our dynamically allocated variables
	delete Suspension;
	Suspension = NULL;

	delete Drivetrain;
	Drivetrain = NULL;

	delete Brakes;
	Brakes = NULL;

	delete Aerodynamics;
	Aerodynamics = NULL;

	delete Engine;
	Engine = NULL;

	delete MassProperties;
	MassProperties = NULL;

	delete Tires;
	Tires = NULL;
}

//==========================================================================
// Class:			CAR
// Function:		Constant definitions
//
// Description:		Constant definitions for the CAR class.
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
//const int CAR::CurrentFileVersion = 0;// OBSOLETE 4/25/2009 - Added APPEARANCE_OPTIONS
//const int CAR::CurrentFileVersion = 1;// OBSOLETE 8/17/2009 - Fixed ENGINE::Write()
const int CAR::CurrentFileVersion = 2;

//==========================================================================
// Class:			CAR
// Function:		SaveCarToFile
//
// Description:		Calls all of the sub-system Write() functions.
//
// Input Arguments:
//		FileName	= wxString specifying the location to write to
//		_OutFile	= std::ofstream* used to allow writing of the appearance options in
//					  an external function
//
// Output Arguments:
//		None
//
// Return Value:
//		returns true for successful write, false otherwise
//
//==========================================================================
bool CAR::SaveCarToFile(wxString FileName, std::ofstream *_OutFile) const
{
	// Open the specified file
	std::ofstream OutFile(FileName.c_str(), ios::out | ios::binary);

	// Make sure the file was opened OK
	if (!OutFile.is_open() || !OutFile.good())
		return false;

	// Write the file header information
	WriteFileHeader(&OutFile);

	// Call the write functions for each sub-system class
	// NOTE:  The order that these calls are made must be identical to the
	// order of the Read() calls in LoadCarFromFile().
	Aerodynamics->Write(&OutFile);
	Brakes->Write(&OutFile);
	Drivetrain->Write(&OutFile);
	Engine->Write(&OutFile);
	MassProperties->Write(&OutFile);
	Suspension->Write(&OutFile);
	Tires->Write(&OutFile);

	// If we're saving the options (done elsewhere), open the additional file
	if (_OutFile != NULL)
	{
		_OutFile->open(FileName.c_str(), ios::out | ios::binary);
		_OutFile->seekp(OutFile.tellp());
	}

	// Close the local version of the file
	OutFile.close();

	return true;
}

//==========================================================================
// Class:			CAR
// Function:		LoadCarFromFile
//
// Description:		Calls the Read() functions for each sub-system class.
//
// Input Arguments:
//		FileName	= wxString specifying the location to read from
//		_InFile		= std::ifstream* used to allow reading of the appearance options in
//					  an external function
//
// Output Arguments:
//		FileVersion	= int* specifying the version of the file we're reading from
//
// Return Value:
//		None
//
//==========================================================================
bool CAR::LoadCarFromFile(wxString FileName, std::ifstream *_InFile, int *FileVersion)
{
	// Open the specified file
	std::ifstream InFile(FileName.c_str(), ios::in | ios::binary);

	// Make sure the file was opened OK
	if (!InFile.is_open() || !InFile.good())
		return false;

	// Read the file header information
	FILE_HEADER_INFO Header = ReadFileHeader(&InFile);

	// Check to make sure the version matches
	if (Header.FileVersion != CurrentFileVersion)
		debugger.Print(_T("Warning:  Opening file with out-of-date file format."),
			Debugger::PriorityHigh);

	// Call the read function for each sub-system class
	// NOTE:  The order that these Read() calls are made must match the order
	// of the Write() calls in SaveCarToFile().
	Aerodynamics->Read(&InFile, Header.FileVersion);
	Brakes->Read(&InFile, Header.FileVersion);
	Drivetrain->Read(&InFile, Header.FileVersion);
	Engine->Read(&InFile, Header.FileVersion);
	MassProperties->Read(&InFile, Header.FileVersion);
	Suspension->Read(&InFile, Header.FileVersion);
	Tires->Read(&InFile, Header.FileVersion);

	// If we're reading the options (done elsewhere), open the additional file
	if (InFile != NULL)
	{
		_InFile->open(FileName.c_str(), ios::in | ios::binary);
		_InFile->seekg(InFile.tellg());
		*FileVersion = Header.FileVersion;
	}
	else
		InFile.close();

	return true;
}

//==========================================================================
// Class:			CAR
// Function:		WriteFileHeader
//
// Description:		Writes the file header information to the file stream.
//
// Input Arguments:
//		OutFile	= ofstream* to write to
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void CAR::WriteFileHeader(std::ofstream *OutFile) const
{
	// Set up the header information
	FILE_HEADER_INFO Header;
	Header.FileVersion = CurrentFileVersion;

	// Set the write pointer to the start of the file
	OutFile->seekp(0);

	// Write the header
	OutFile->write((char*)&Header, sizeof(FILE_HEADER_INFO));

	return;
}

//==========================================================================
// Class:			CAR
// Function:		ReadFileHeader
//
// Description:		Reads the file header information from the file stream.
//
// Input Arguments:
//		InFile	= ifstream* to read from
//
// Output Arguments:
//		None
//
// Return Value:
//		FILE_HEADER_INFO as read from the specified file
//
//==========================================================================
CAR::FILE_HEADER_INFO CAR::ReadFileHeader(std::ifstream *InFile) const
{
	// Set get pointer to the start of the file
	InFile->seekg(0);

	// Read the header struct
	char Buffer[sizeof(FILE_HEADER_INFO)];
	InFile->read((char*)Buffer, sizeof(FILE_HEADER_INFO));

	return *reinterpret_cast<FILE_HEADER_INFO*>(Buffer);
}

//==========================================================================
// Class:			CAR
// Function:		ComputeWheelCenters
//
// Description:		Calls the suspension method that computes the wheel center
//					locations based on static toe and camber.
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
void CAR::ComputeWheelCenters(void)
{
	// Call the compute method with the correct tire diameters
	Suspension->ComputeWheelCenters(Tires->RightFront->Diameter, Tires->LeftFront->Diameter,
		Tires->RightRear->Diameter, Tires->LeftRear->Diameter);

	return;
}

//==========================================================================
// Class:			CAR
// Function:		HasFrontHalfShafts
//
// Description:		Determines whether or not this car has front half shafts.
//					(Constant)
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if the car is AWD, FWD, or has inboard front brakes
//
//==========================================================================
bool CAR::HasFrontHalfShafts(void) const
{
	// If the car is all wheel drive, front wheel drive, or has inboard front
	// brakes, this car has front half shafts
	if (Drivetrain->DriveType == DRIVETRAIN::DRIVE_ALL_WHEEL ||
		Drivetrain->DriveType == DRIVETRAIN::DRIVE_FRONT_WHEEL ||
		Brakes->FrontBrakesInboard)
		return true;

	return false;
}

//==========================================================================
// Class:			CAR
// Function:		HasRearHalfShafts
//
// Description:		Determines whether or not this car has rear half shafts.
//					(Constant)
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if the car is AWD, RWD, or has inboard rear brakes
//
//==========================================================================
bool CAR::HasRearHalfShafts(void) const
{
	// If the car is all wheel drive, front wheel drive, or has inboard front
	// brakes, this car has front half shafts
	if (Drivetrain->DriveType == DRIVETRAIN::DRIVE_ALL_WHEEL ||
		Drivetrain->DriveType == DRIVETRAIN::DRIVE_REAR_WHEEL ||
		Brakes->RearBrakesInboard)
		return true;

	return false;
}

//==========================================================================
// Class:			CAR
// Function:		operator=
//
// Description:		Assignment operator for the CAR class.
//
// Input Arguments:
//		Car	= const CAR& to assign to this
//
// Output Arguments:
//		None
//
// Return Value:
//		CAR& reference to this
//
//==========================================================================
CAR& CAR::operator=(const CAR &Car)
{
	// Check for self-assignment
	if (this == &Car)
		return *this;

	// Delete existing class members
	delete Suspension;
	delete Drivetrain;
	delete Brakes;
	delete Aerodynamics;
	delete Engine;
	delete MassProperties;
	delete Tires;

	Suspension		= NULL;
	Drivetrain		= NULL;
	Brakes			= NULL;
	Aerodynamics	= NULL;
	Engine			= NULL;
	MassProperties	= NULL;
	Tires			= NULL;

	// Do the copy
	Suspension		= new SUSPENSION(*Car.Suspension);
	Drivetrain		= new DRIVETRAIN(*Car.Drivetrain);
	Brakes			= new BRAKES(*Car.Brakes);
	Aerodynamics	= new AERODYNAMICS(*Car.Aerodynamics);
	Engine			= new ENGINE(*Car.Engine);
	MassProperties	= new MASS_PROPERTIES(*Car.MassProperties);
	Tires			= new TIRE_SET(*Car.Tires);

	return *this;
}