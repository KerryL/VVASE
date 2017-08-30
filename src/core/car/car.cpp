/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  car.cpp
// Date:  11/3/2007
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Container class for vehicle subsystems.  Defines primary interface for
//        getting and setting vehicle data.

// Local headers
#include "VVASE/core/car/car.h"
#include "VVASE/core/car/subsystems/drivetrain.h"// TODO:  Redesign to eliminate need for including these here
#include "VVASE/core/car/subsystems/brakes.h"
#include "VVASE/core/car/subsystems/suspension.h"
#include "VVASE/core/car/subsystems/tireSet.h"
#include "VVASE/core/car/subsystems/tire.h"
#include "VVASE/core/utilities/wheelSetStructures.h"
#include "VVASE/core/utilities/binaryReader.h"
#include "VVASE/core/utilities/binaryWriter.h"
#include "VVASE/core/utilities/debugger.h"

// Standard C++ headers
#include <fstream>

namespace VVASE
{

//==========================================================================
// Class:			Car
// Function:		Car
//
// Description:		Constructor for the Car class.
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
// Exception Safety:
//
//
//==========================================================================
Car::Car() : subsystems(CreateComponents())
{
}

//==========================================================================
// Class:			Car
// Function:		Car
//
// Description:		Copy Constructor for the Car class.
//
// Input Arguments:
//		car	= const Car& that is to be copied
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Car::Car(const Car& car) : subsystems(CreateComponents())
{
	*this = car;
}

//==========================================================================
// Class:			Car
// Function:		Car
//
// Description:		Move copy Constructor for the Car class.
//
// Input Arguments:
//		car	= Car&& that is to be copied
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Car::Car(Car&& car)
{
	*this = std::move(car);
}

//==========================================================================
// Class:			Car
// Function:		Constant definitions
//
// Description:		Constant definitions for the Car class.
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
ComponentManager<Subsystem> Car::subsystemManager;

//const int Car::currentFileVersion = 0;// OBSOLETE 4/25/2009 - Added AppearanceOptions
//const int Car::currentFileVersion = 1;// OBSOLETE 8/17/2009 - Fixed Engine::Write()
//const int Car::currentFileVersion = 2;// OBSOLETE 7/13/2015 - Added Front/RearBarPivotAxis to Suspension
//const int Car::currentFileVersion = 3;// OBSOLETE 5/27/2016 - Added unsprung mass CGs
//const int Car::currentFileVersion = 4;// OBSOLETE 6/8/2016 - Added tire model information and differentials
//const int Car::currentFileVersion = 5;// OBSOLETE 6/19/2017 - Changed to variable-component car model
const int Car::currentFileVersion = 6;

//==========================================================================
// Class:			Car
// Function:		SaveCarToFile
//
// Description:		Calls all of the sub-system Write() functions.
//
// Input Arguments:
//		fileName	= vvaseString specifying the location to write to
//		outFile		= std::ofstream& used to allow writing of the appearance options in
//					  an external function
//
// Output Arguments:
//		None
//
// Return Value:
//		returns true for successful write, false otherwise
//
//==========================================================================
bool Car::SaveCarToFile(vvaseString fileName, std::ofstream& outFile) const
{
	// Open the specified file
	outFile.open(fileName.c_str(), std::ios::binary);// NOT vvaseOutFileStream - always use narrow char here
	if (!outFile.is_open() || !outFile.good())
		return false;

	BinaryWriter binFile(outFile);

	WriteFileHeader(binFile);

	// Call the write functions for each sub-system class
	for (const auto& subsystem : subsystems)
	{
		binFile.Write(subsystem.first);
		//binFile.Write(sizeof(subsystems[subsystem.first]));
		// TODO:  Need to write size information here?  Or as first part of write method for subsystem derivatives?
		subsystem.second->Write(binFile);
	}

	return true;
}

//==========================================================================
// Class:			Car
// Function:		LoadCarFromFile
//
// Description:		Calls the Read() functions for each sub-system class.
//
// Input Arguments:
//		fileName	= vvaseString specifying the location to read from
//		inFile		= std::ifstream* used to allow reading of the appearance options in&
//					  an external function
//
// Output Arguments:
//		fileVersion	= int& specifying the version of the file we're reading from
//
// Return Value:
//		None
//
//==========================================================================
bool Car::LoadCarFromFile(vvaseString fileName, std::ifstream& inFile, int& fileVersion)
{
	inFile.open(fileName.c_str(), std::ios::binary);// NOT vvaseInFileStream - always use narrow char here
	if (!inFile.is_open() || !inFile.good())
		return false;

	BinaryReader binFile(inFile);
	FileHeaderInfo header = ReadFileHeader(binFile);

	// Check to make sure the version matches
	if (header.fileVersion != currentFileVersion)
		Debugger::GetInstance() << "Warning:  Opening file with out-of-date file format." << Debugger::Priority::High;

	// Call the read function for each sub-system class
	// NOTE:  The order that these Read() calls are made must match the order
	// of the Write() calls in SaveCarToFile().
/*	aerodynamics->Read(binFile, header.fileVersion);
	brakes->Read(binFile, header.fileVersion);
	drivetrain->Read(binFile, header.fileVersion);
	engine->Read(binFile, header.fileVersion);
	massProperties->Read(binFile, header.fileVersion);
	suspension->Read(binFile, header.fileVersion);
	tires->Read(binFile, header.fileVersion);*/
	// TODO:  Re-implement (and also handle old file versions!)

	fileVersion = header.fileVersion;

	return true;
}

//==========================================================================
// Class:			Car
// Function:		WriteFileHeader
//
// Description:		Writes the file header information to the file stream.
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
void Car::WriteFileHeader(BinaryWriter& file) const
{
	FileHeaderInfo header;
	header.fileVersion = currentFileVersion;
	file.Write(header.fileVersion);
}

//==========================================================================
// Class:			Car
// Function:		ReadFileHeader
//
// Description:		Reads the file header information from the file stream.
//
// Input Arguments:
//		file	= BinaryReader&
//
// Output Arguments:
//		None
//
// Return Value:
//		FileHeaderInfo as read from the specified file
//
//==========================================================================
Car::FileHeaderInfo Car::ReadFileHeader(BinaryReader& file) const
{
	FileHeaderInfo header;
	file.Read(header.fileVersion);
	return header;
}

//==========================================================================
// Class:			Car
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
void Car::ComputeWheelCenters()
{
	// Call the compute method with the correct tire diameters
	const auto* tires(GetSubsystem<TireSet>());
	GetSubsystem<Suspension>()->ComputeWheelCenters(tires->rightFront->diameter, tires->leftFront->diameter,
		tires->rightRear->diameter, tires->leftRear->diameter);
}

//==========================================================================
// Class:			Car
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
bool Car::HasFrontHalfShafts() const
{
	// If the car is all wheel drive, front wheel drive, or has inboard front
	// brakes, this car has front half shafts
	const auto* drivetrain(GetSubsystem<Drivetrain>());
	if (drivetrain->driveType == Drivetrain::DriveAllWheel ||
		drivetrain->driveType == Drivetrain::DriveFrontWheel ||
		GetSubsystem<Brakes>()->frontBrakesInboard)
		return true;

	return false;
}

//==========================================================================
// Class:			Car
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
bool Car::HasRearHalfShafts() const
{
	// If the car is all wheel drive, front wheel drive, or has inboard front
	// brakes, this car has front half shafts
	const auto* drivetrain(GetSubsystem<Drivetrain>());
	if (drivetrain->driveType == Drivetrain::DriveAllWheel ||
		drivetrain->driveType == Drivetrain::DriveRearWheel ||
		GetSubsystem<Brakes>()->rearBrakesInboard)
		return true;

	return false;
}

//==========================================================================
// Class:			Car
// Function:		operator=
//
// Description:		Assignment operator for the Car class.
//
// Input Arguments:
//		car	= const Car& to assign to this
//
// Output Arguments:
//		None
//
// Return Value:
//		Car& reference to this
//
//==========================================================================
Car& Car::operator=(const Car& car)
{
	if (this == &car)
		return *this;

	assert(subsystems.size() == car.subsystems.size());
	for (const auto& subsystem : car.subsystems)
		subsystem.second->CloneTo(subsystems[subsystem.first].get());

	return *this;
}

//==========================================================================
// Class:			Car
// Function:		operator=
//
// Description:		Move assignment operator for the Car class.
//
// Input Arguments:
//		car	= Car&& to assign to this
//
// Output Arguments:
//		None
//
// Return Value:
//		Car& reference to this
//
//==========================================================================
Car& Car::operator=(Car&& car)
{
	if (this == &car)
		return *this;

	subsystems.clear();
	subsystems = std::move(car.subsystems);
	return *this;
}

//==========================================================================
// Class:			Car
// Function:		CreateComponents
//
// Description:		Generates instances of the subsystems currently registered.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		Car::SubsystemsMap
//
//==========================================================================
Car::SubsystemsMap Car::CreateComponents()
{
	SubsystemsMap subsystems;
	for (const auto& subsystem : subsystemManager.GetInfo())
		subsystems[subsystem.name] = subsystem.create();

	return subsystems;
}

}// namespace VVASE

