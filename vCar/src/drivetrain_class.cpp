/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  drivetrain_class.cpp
// Created:  11/3/2007
// Author:  K. Loux
// Description:  Contains class functionality for drivetrain class.
// History:
//	2/24/2008	- Fixed SetNumberOfGears and added delete[] in destructor, K. Loux.
//	3/9/2008	- Changed the structure of the DEBUGGER class, K. Loux.
//	11/22/2009	- Moved to vCar.lib, K. Loux.

// wxWidgets headers
#include <wx/wx.h>

// Standard C++ headers
#include <fstream>

// VVASE headers
#include "vCar/differential_class.h"
#include "vCar/drivetrain_class.h"
#include "vUtilities/wheelset_structs.h"
#include "vUtilities/debug_class.h"
#include "vUtilities/machine_defs.h"

//==========================================================================
// Class:			DRIVETRAIN
// Function:		DRIVETRAIN
//
// Description:		Constructor for the DRIVETRAIN class.
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
DRIVETRAIN::DRIVETRAIN(const DEBUGGER &_Debugger) : Debugger(_Debugger)
{
	// Initialize the number of gears to zero
	NumberOfGears = 0;
	GearRatio = NULL;

	// Allocate memory for the differential object
	Differential = new DIFFERENTIAL(Debugger);

	// Initialize this object
	DriveType = DRIVE_REAR_WHEEL;
	TransmissionInertia = 0.0;
}

//==========================================================================
// Class:			DRIVETRAIN
// Function:		DRIVETRAIN
//
// Description:		Copy constructor for the DRIVETRAIN class.
//
// Input Arguments:
//		Drivetrain	= const DRIVETRAIN&, object to be copied
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
DRIVETRAIN::DRIVETRAIN(const DRIVETRAIN &Drivetrain) : Debugger(Drivetrain.Debugger)
{
	// Initialize the pointers to NULL so we don't get an error when we try to
	// delete them in the asignment operator method
	GearRatio = NULL;
	Differential = NULL;

	// Do the copy
	*this = Drivetrain;
}

//==========================================================================
// Class:			DRIVETRAIN
// Function:		~DRIVETRAIN
//
// Description:		Destructor for the DRIVETRAIN class.
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
DRIVETRAIN::~DRIVETRAIN()
{
	// Delete our dynamically allocated variables
	delete [] GearRatio;
	GearRatio = NULL;

	delete Differential;
	Differential = NULL;
}

//==========================================================================
// Class:			DRIVETRAIN
// Function:		SetNumberOfGears
//
// Description:		Sets the number of gears we have available and allocates
//					memory for the gear ratios accordingly.
//
// Input Arguments:
//		NumGears	= const short& describing the number of gears we have
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void DRIVETRAIN::SetNumberOfGears(const short &NumGears)
{
	// Make sure we have at least one gear
	if (NumGears < 1)
	{
		Debugger.Print(_T("ERROR:  Must have at least 1 gear!"));

		return;
	}

	// Set the number of gears
	NumberOfGears = NumGears;

	// Delete the old gear ratio array
	delete [] GearRatio;

	// Dynamically allocate memory for GearRatio according to the number of gears we have
	GearRatio = new double[NumGears];

	return;
}

//==========================================================================
// Class:			DRIVETRAIN
// Function:		Write
//
// Description:		Writes this drivetrain to file.
//
// Input Arguments:
//		OutFile	= std::ofstream* pointing to the files stream to write to
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void DRIVETRAIN::Write(std::ofstream *OutFile) const
{
	// Write this object to file
	// This is done item by item because NumberOfGears needs to be used to
	// determine size of GearRatio.  Also, Differential needs to be handled
	// separately.
	OutFile->write((char*)&DriveType, sizeof(DRIVE_WHEELS));
	OutFile->write((char*)&NumberOfGears, sizeof(short));
	OutFile->write((char*)&TransmissionInertia, sizeof(double));
	if (NumberOfGears > 0)
		OutFile->write((char*)GearRatio, sizeof(double) * NumberOfGears);

	// Write the differential
	Differential->Write(OutFile);

	return;
}

//==========================================================================
// Class:			DRIVETRAIN
// Function:		Read
//
// Description:		Read from file to fill this drivetrain.
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
void DRIVETRAIN::Read(std::ifstream *InFile, int FileVersion)
{
	// Read this object from file - again, this is the same order as found
	// in the Read() function.
	InFile->read((char*)&DriveType, sizeof(DriveType));
	InFile->read((char*)&NumberOfGears, sizeof(NumberOfGears));
	InFile->read((char*)&TransmissionInertia, sizeof(TransmissionInertia));

	// Re-allocate the memory for the NumberOfGears
	if (NumberOfGears > 0)
	{
		SetNumberOfGears(NumberOfGears);
		InFile->read((char*)GearRatio, sizeof(double) * NumberOfGears);
	}

	// Read the differential
	Differential->Read(InFile, FileVersion);

	return;
}

//==========================================================================
// Class:			DRIVETRAIN
// Function:		GetDriveWheelsName
//
// Description:		Returns the name of the drivetrain type.
//
// Input Arguments:
//		_DriveWheels	= const DRIVE_WHEELS& specifying the type of interest
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the name of the drive style
//
//==========================================================================
wxString DRIVETRAIN::GetDriveWheelsName(const DRIVE_WHEELS &_DriveWheels)
{
	switch (_DriveWheels)
	{
	case DRIVE_REAR_WHEEL:
		return _T("Rear Wheel Drive");
		break;

	case DRIVE_FRONT_WHEEL:
		return _T("Front Wheel Drive");
		break;

	case DRIVE_ALL_WHEEL:
		return _T("All Wheel Drive");
		break;

	default:
		assert(0);
		break;
	}

	return wxEmptyString;
}

//==========================================================================
// Class:			DRIVETRAIN
// Function:		operator=
//
// Description:		Assignment operator for DRIVETRAIN class.
//
// Input Arguments:
//		Drivetrain	= const DRIVETRAIN& to copy from
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
DRIVETRAIN& DRIVETRAIN::operator=(const DRIVETRAIN &Drivetrain)
{
	// Check for self-assignment
	if (this == &Drivetrain)
		return *this;

	// Allocate memory for the gear ratios depending on the number of gears
	NumberOfGears = Drivetrain.NumberOfGears;

	delete GearRatio;
	GearRatio = NULL;

	if (NumberOfGears > 0)
	{
		GearRatio = new double[NumberOfGears];
		SetNumberOfGears(NumberOfGears);
		int i;
		for (i = 0; i < NumberOfGears; i++)
			GearRatio[i] = Drivetrain.GearRatio[i];
	}

	// Copy the differential
	delete Differential;
	Differential = new DIFFERENTIAL(*Drivetrain.Differential);

	// Copy the drive type
	DriveType = Drivetrain.DriveType;

	return *this;
}