/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  drivetrain.cpp
// Date:  11/3/2007
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains class functionality for Drivetrain class.

// Local headers
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
		Debugger::GetInstance() << "ERROR:  Must have at least 1 gear!" << Debugger::Priority::High;
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

	WriteDifferential(file, rearDifferential.get());
	WriteDifferential(file, midDifferential.get());
	WriteDifferential(file, frontDifferential.get());
}

void Drivetrain::WriteDifferential(BinaryWriter& file, const Differential* differential)
{
	if (differential)
	{
		file.Write(true);
		differential->Write(file);
	}
	else
		file.Write(false);
}

void Drivetrain::ReadDifferential(BinaryReader& file, std::unique_ptr<Differential>& differential, const int& fileVersion)
{
	bool hasDifferential;
	file.Read(hasDifferential);
	if (hasDifferential)
	{
		differential = std::make_unique<Differential>();
		differential->Read(file, fileVersion);
	}
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

	if (fileVersion >= 6)// TODO:  Check this # is correct
	{
		file.Read(gearRatios);
		ReadDifferential(file, rearDifferential, fileVersion);
		ReadDifferential(file, midDifferential, fileVersion);
		ReadDifferential(file, frontDifferential, fileVersion);
	}
	else if (fileVersion >= 5)
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
//		vvaseString containing the name of the drive style
//
//==========================================================================
vvaseString Drivetrain::GetDriveWheelsName(const DriveWheels &driveWheels)
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

	return vvaseString();
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

Drivetrain& Drivetrain::operator=(const Drivetrain& d)
{
	if (this == &d)
		return *this;

	if (d.rearDifferential)
		rearDifferential = std::make_unique<Differential>(*d.rearDifferential);
	if (d.midDifferential)
		midDifferential = std::make_unique<Differential>(*d.midDifferential);
	if (d.frontDifferential)
		frontDifferential = std::make_unique<Differential>(*d.frontDifferential);

	gearRatios = d.gearRatios;

	return *this;
}

wxPanel* Drivetrain::GetEditPanel()
{
	// TODO:  Implement
	return nullptr;
}

wxTreeListItem* Drivetrain::GetTreeItem()
{
	// TODO:  Implement
	return nullptr;
}

void Drivetrain::CloneTo(Subsystem* target) const
{
	auto* t(dynamic_cast<Drivetrain*>(target));
	assert(t);

	*t = *this;
}

}// namespace VVASE
