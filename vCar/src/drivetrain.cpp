/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  drivetrain.cpp
// Created:  11/3/2007
// Author:  K. Loux
// Description:  Contains class functionality for Drivetrain class.
// History:
//	2/24/2008	- Fixed SetNumberOfGears and added delete[] in destructor, K. Loux.
//	3/9/2008	- Changed the structure of the Debugger class, K. Loux.
//	11/22/2009	- Moved to vCar.lib, K. Loux.

// wxWidgets headers
#include <wx/wx.h>

// Standard C++ headers
#include <fstream>

// VVASE headers
#include "vCar/differential.h"
#include "vCar/drivetrain.h"
#include "vUtilities/wheelSetStructures.h"
#include "vUtilities/debugger.h"
#include "vUtilities/machineDefinitions.h"

//==========================================================================
// Class:			Drivetrain
// Function:		Drivetrain
//
// Description:		Constructor for the Drivetrain class.
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
Drivetrain::Drivetrain()
{
	gearRatio = NULL;
	SetNumberOfGears(1);

	differential = new Differential();

	driveType = DriveRearWheel;
	transmissionInertia = 0.0;
}

//==========================================================================
// Class:			Drivetrain
// Function:		Drivetrain
//
// Description:		Copy constructor for the Drivetrain class.
//
// Input Arguments:
//		drivetrain	= const Drivetrain&, object to be copied
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Drivetrain::Drivetrain(const Drivetrain &drivetrain)
{
	gearRatio = NULL;
	differential = new Differential();
	*this = drivetrain;
}

//==========================================================================
// Class:			Drivetrain
// Function:		~Drivetrain
//
// Description:		Destructor for the Drivetrain class.
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
Drivetrain::~Drivetrain()
{
	delete [] gearRatio;
	gearRatio = NULL;

	delete differential;
	differential = NULL;
}

//==========================================================================
// Class:			Drivetrain
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
void Drivetrain::SetNumberOfGears(const short &numGears)
{
	// Make sure we have at least one gear
	if (numGears < 1)
	{
		Debugger::GetInstance() << "ERROR:  Must have at least 1 gear!" << Debugger::PriorityHigh;
		return;
	}

	numberOfGears = numGears;
	delete [] gearRatio;
	gearRatio = new double[numGears];
}

//==========================================================================
// Class:			Drivetrain
// Function:		Write
//
// Description:		Writes this drivetrain to file.
//
// Input Arguments:
//		outFile	= std::ofstream* pointing to the files stream to write to
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Drivetrain::Write(std::ofstream *outFile) const
{
	// Write this object to file
	// This is done item by item because NumberOfGears needs to be used to
	// determine size of GearRatio.  Also, Differential needs to be handled
	// separately.
	outFile->write((char*)&driveType, sizeof(DriveWheels));
	outFile->write((char*)&numberOfGears, sizeof(short));
	outFile->write((char*)&transmissionInertia, sizeof(double));
	if (numberOfGears > 0)
		outFile->write((char*)gearRatio, sizeof(double) * numberOfGears);

	// Write the differential
	differential->Write(outFile);
}

//==========================================================================
// Class:			Drivetrain
// Function:		Read
//
// Description:		Read from file to fill this drivetrain.
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
void Drivetrain::Read(std::ifstream *inFile, int fileVersion)
{
	// Read this object from file - again, this is the same order as found
	// in the Read() function.
	inFile->read((char*)&driveType, sizeof(driveType));
	inFile->read((char*)&numberOfGears, sizeof(numberOfGears));
	inFile->read((char*)&transmissionInertia, sizeof(transmissionInertia));

	// Re-allocate the memory for the NumberOfGears
	if (numberOfGears > 0)
	{
		SetNumberOfGears(numberOfGears);
		inFile->read((char*)gearRatio, sizeof(double) * numberOfGears);
	}

	// Read the differential
	differential->Read(inFile, fileVersion);
}

//==========================================================================
// Class:			Drivetrain
// Function:		GetDriveWheelsName
//
// Description:		Returns the name of the drivetrain type.
//
// Input Arguments:
//		_driveWheels	= const DriveWheels& specifying the type of interest
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the name of the drive style
//
//==========================================================================
wxString Drivetrain::GetDriveWheelsName(const DriveWheels &_driveWheels)
{
	switch (_driveWheels)
	{
	case DriveRearWheel:
		return _T("Rear Wheel Drive");
		break;

	case DriveFrontWheel:
		return _T("Front Wheel Drive");
		break;

	case DriveAllWheel:
		return _T("All Wheel Drive");
		break;

	default:
		assert(0);
		break;
	}

	return wxEmptyString;
}

//==========================================================================
// Class:			Drivetrain
// Function:		operator=
//
// Description:		Assignment operator for Drivetrain class.
//
// Input Arguments:
//		drivetrain	= const Drivetrain& to copy from
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Drivetrain& Drivetrain::operator=(const Drivetrain &drivetrain)
{
	// Check for self-assignment
	if (this == &drivetrain)
		return *this;

	// Allocate memory for the gear ratios depending on the number of gears
	SetNumberOfGears(numberOfGears);

	if (numberOfGears > 0)
	{
		int i;
		for (i = 0; i < numberOfGears; i++)
			gearRatio[i] = drivetrain.gearRatio[i];
	}

	*differential = *drivetrain.differential;
	driveType = drivetrain.driveType;

	return *this;
}