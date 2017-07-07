/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  drivetrain.cpp
// Date:  11/3/2007
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains class functionality for Drivetrain class.

// wxWidgets headers
#include <wx/wx.h>

// Local headers
#include "VVASE/core/car/subsystems/differential.h"
#include "VVASE/core/car/subsystems/drivetrain.h"
#include "VVASE/core/utilities/wheelSetStructures.h"
#include "VVASE/core/utilities/debugger.h"
#include "VVASE/core/utilities/binaryReader.h"
#include "VVASE/core/utilities/binaryWriter.h"

namespace VVASE
{

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
	SetNumberOfGears(1);
	rearDifferential = std::make_unique<Differential>();
	driveType = DriveRearWheel;
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

	gearRatios.resize(numGears);
}

//==========================================================================
// Class:			Drivetrain
// Function:		Write
//
// Description:		Writes this drivetrain to file.
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
void Drivetrain::Write(BinaryWriter& file) const
{
	// Write this object to file
	// This is done item by item because NumberOfGears needs to be used to
	// determine size of GearRatio.  Also, Differential needs to be handled
	// separately.
	file.Write((unsigned int)driveType);
	file.Write(gearRatios);

	// Write the differentials
	unsigned int diffCount(differentials.size());
	file.Write(diffCount);

	unsigned int i;
	for (i = 0; i < differentials.size(); i++)
		differentials[i]->Write(file);
}

//==========================================================================
// Class:			Drivetrain
// Function:		Read
//
// Description:		Read from file to fill this drivetrain.
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
void Drivetrain::Read(BinaryReader& file, const int& fileVersion)
{
	// Read this object from file - again, this is the same order as found
	// in the Read() function.
	unsigned int temp;
	file.Read(temp);
	driveType = static_cast<DriveWheels>(temp);

	temp = sizeof(driveType);

	if (fileVersion >= 5)
	{
		file.Read(gearRatios);
	}
	else if (fileVersion >= 0)
	{
		short tempShort;
		file.Read(tempShort);
		SetNumberOfGears(tempShort);

		double dummy;
		file.Read(dummy);

		unsigned int i;
		for (i = 0; i < gearRatios.size(); i++)
			file.Read(gearRatios[i]);
	}
	else
		assert(false);

	// Read the differentials
	unsigned int diffCount(1);
	if (fileVersion >= 5)
		file.Read(diffCount);

	// TODO:  Handle different file versions
	unsigned int i;
	for (i = 0; i < diffCount; i++)
	{
		differentials.push_back(new Differential);
		differentials.front()->Read(file, fileVersion);
	}
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
wxString Drivetrain::GetDriveWheelsName(const DriveWheels &driveWheels)
{
	switch (driveWheels)
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
// Function:		SetAllWheelDrive
//
// Description:		Sets the drive type to all wheel drive.
//
// Input Arguments:
//		rearBias	= const double&
//		midBias		= const double&
//		frontBias	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Drivetrain::SetAllWheelDrive(const double& rearBias, const double& midBias, const double& frontBias)
{
	driveType = DriveAllWheel;
	rearDifferential = std::make_unique<Differential>(rearBias);
	midDifferential = std::make_unique<Differential>(midBias);
	frontDifferential = std::make_unique<Differential>(frontBias);
}

//==========================================================================
// Class:			Drivetrain
// Function:		SetFrontWheelDrive
//
// Description:		Sets the drive type to front wheel drive.
//
// Input Arguments:
//		bias	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Drivetrain::SetFrontWheelDrive(const double& bias)
{
	driveType = DriveFrontWheel;
	frontDifferential = std::make_unique<Differential>(bias);
	rearDifferential = nullptr;
	midDifferential = nullptr;
}

//==========================================================================
// Class:			Drivetrain
// Function:		SetRearWheelDrive
//
// Description:		Sets the drive type to rear wheel drive.
//
// Input Arguments:
//		bias	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Drivetrain::SetRearWheelDrive(const double& bias)
{
	driveType = DriveRearWheel;
	rearDifferential = std::make_unique<Differential>(bias);
	frontDifferential = nullptr;
	midDifferential = nullptr;
}

}// namespace VVASE
