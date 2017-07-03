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
#include "car.h"
#include "VVASE/core/utilities/wheelSetStructures.h"
#include "VVASE/core/utilities/binaryReader.h"
#include "VVASE/core/utilities/binaryWriter.h"

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
Car::Car() : components(CreateComponents())
{
	// Test suspension
	suspension->frontBarStyle = Suspension::SwayBarUBar;
	suspension->rearBarStyle = Suspension::SwayBarTBar;
	suspension->isSymmetric = true;

	suspension->rightFront.hardpoints[Corner::ContactPatch].Set(0.0, 21.875, 0.0);
	suspension->leftFront.hardpoints[Corner::ContactPatch].Set(0.0, -21.875, 0.0);
	suspension->rightRear.hardpoints[Corner::ContactPatch].Set(66.5, 20.0, 0.0);
	suspension->leftRear.hardpoints[Corner::ContactPatch].Set(66.5, -20.0, 0.0);

	suspension->rightFront.hardpoints[Corner::LowerFrontTubMount].Set(-6.893, 3.0, 5.0);
	suspension->leftFront.hardpoints[Corner::LowerFrontTubMount].Set(-6.893, -3.0, 5.0);
	suspension->rightRear.hardpoints[Corner::LowerFrontTubMount].Set(62.5, 3.2, 5.0);
	suspension->leftRear.hardpoints[Corner::LowerFrontTubMount].Set(62.5, -3.2, 5.0);

	suspension->rightFront.hardpoints[Corner::LowerRearTubMount].Set(5.107, 3.0, 5.0);
	suspension->leftFront.hardpoints[Corner::LowerRearTubMount].Set(5.107, -3.0, 5.0);
	suspension->rightRear.hardpoints[Corner::LowerRearTubMount].Set(72.5, 3.2, 5.0);
	suspension->leftRear.hardpoints[Corner::LowerRearTubMount].Set(72.5, -3.2, 5.0);

	suspension->rightFront.hardpoints[Corner::LowerBallJoint].Set(0.168, 19.8, 5.0);
	suspension->leftFront.hardpoints[Corner::LowerBallJoint].Set(0.168, -19.8, 5.0);
	suspension->rightRear.hardpoints[Corner::LowerBallJoint].Set(66.213, 20.35, 5.125);
	suspension->leftRear.hardpoints[Corner::LowerBallJoint].Set(66.213, -20.35, 5.125);

	suspension->rightFront.hardpoints[Corner::UpperFrontTubMount].Set(0.893, 7.0, 13.75);
	suspension->leftFront.hardpoints[Corner::UpperFrontTubMount].Set(0.893, -7.0, 13.75);
	suspension->rightRear.hardpoints[Corner::UpperFrontTubMount].Set(59.159, 5.0, 12.67);
	suspension->leftRear.hardpoints[Corner::UpperFrontTubMount].Set(59.159, -5.0, 12.67);

	suspension->rightFront.hardpoints[Corner::UpperRearTubMount].Set(9.393, 7.0, 13.75);
	suspension->leftFront.hardpoints[Corner::UpperRearTubMount].Set(9.393, -7.0, 13.75);
	suspension->rightRear.hardpoints[Corner::UpperRearTubMount].Set(73.659, 5.0, 12.67);
	suspension->leftRear.hardpoints[Corner::UpperRearTubMount].Set(73.659, -5.0, 12.67);

	suspension->rightFront.hardpoints[Corner::UpperBallJoint].Set(0.893, 19.0, 15.25);
	suspension->leftFront.hardpoints[Corner::UpperBallJoint].Set(0.893, -19.0, 15.25);
	suspension->rightRear.hardpoints[Corner::UpperBallJoint].Set(63.5, 18.025, 14.75);
	suspension->leftRear.hardpoints[Corner::UpperBallJoint].Set(63.5, -18.025, 14.75);

	suspension->rightFront.hardpoints[Corner::InboardTieRod].Set(-0.482, 7.0, 13.75);
	suspension->leftFront.hardpoints[Corner::InboardTieRod].Set(-0.482, -7.0, 13.75);
	suspension->rightRear.hardpoints[Corner::InboardTieRod].Set(74.925, 5.0, 12.67);
	suspension->leftRear.hardpoints[Corner::InboardTieRod].Set(74.925, -5.0, 12.67);

	suspension->rightFront.hardpoints[Corner::OutboardTieRod].Set(-0.896, 19.893, 15.36);
	suspension->leftFront.hardpoints[Corner::OutboardTieRod].Set(-0.896, -19.893, 15.36);
	suspension->rightRear.hardpoints[Corner::OutboardTieRod].Set(69.5, 18.025, 14.75);
	suspension->leftRear.hardpoints[Corner::OutboardTieRod].Set(69.5, -18.025, 14.75);

	suspension->rightFront.hardpoints[Corner::BellCrankPivot1].Set(-0.482, 8.063, 17.625);
	suspension->leftFront.hardpoints[Corner::BellCrankPivot1].Set(-0.482, -8.063, 17.625);
	suspension->rightRear.hardpoints[Corner::BellCrankPivot1].Set(62.75, 3.986, 13.255);
	suspension->leftRear.hardpoints[Corner::BellCrankPivot1].Set(62.75, -3.986, 13.255);

	suspension->rightFront.hardpoints[Corner::BellCrankPivot2].Set(-0.482, 6.438, 16.161);
	suspension->leftFront.hardpoints[Corner::BellCrankPivot2].Set(-0.482, -6.438, 16.161);
	suspension->rightRear.hardpoints[Corner::BellCrankPivot2].Set(62.75, 4.986, 14.987);
	suspension->leftRear.hardpoints[Corner::BellCrankPivot2].Set(62.75, -4.986, 14.987);

	suspension->rightFront.hardpoints[Corner::InboardPushrod].Set(2.768, 7.0, 16.75);
	suspension->leftFront.hardpoints[Corner::InboardPushrod].Set(2.768, -7.0, 16.75);
	suspension->rightRear.hardpoints[Corner::InboardPushrod].Set(61.31, 4.849, 13.918);
	suspension->leftRear.hardpoints[Corner::InboardPushrod].Set(61.31, -4.849, 13.918);

	suspension->rightFront.hardpoints[Corner::OutboardPushrod].Set(0.107, 17.813, 5.625);
	suspension->leftFront.hardpoints[Corner::OutboardPushrod].Set(0.107, -17.813, 5.625);
	suspension->rightRear.hardpoints[Corner::OutboardPushrod].Set(65.0, 17.86, 6.1);
	suspension->leftRear.hardpoints[Corner::OutboardPushrod].Set(65.0, -17.86, 6.1);

	suspension->rightFront.hardpoints[Corner::InboardDamper].Set(-9.232, 4.75, 18.75);
	suspension->leftFront.hardpoints[Corner::InboardDamper].Set(-9.232, -4.75, 18.75);
	suspension->rightRear.hardpoints[Corner::InboardDamper].Set(72.25, 1.455, 15.871);
	suspension->leftRear.hardpoints[Corner::InboardDamper].Set(72.25, -1.455, 15.871);

	suspension->rightFront.hardpoints[Corner::InboardSpring].Set(-9.232, 4.75, 18.75);
	suspension->leftFront.hardpoints[Corner::InboardSpring].Set(-9.232, -4.75, 18.75);
	suspension->rightRear.hardpoints[Corner::InboardSpring].Set(72.25, 1.455, 15.871);
	suspension->leftRear.hardpoints[Corner::InboardSpring].Set(72.25, -1.455, 15.871);

	suspension->rightFront.hardpoints[Corner::OutboardDamper].Set(-1.232, 3.375, 19.5);
	suspension->leftFront.hardpoints[Corner::OutboardDamper].Set(-1.232, -3.375, 19.5);
	suspension->rightRear.hardpoints[Corner::OutboardDamper].Set(62.75, 1.455, 15.871);
	suspension->leftRear.hardpoints[Corner::OutboardDamper].Set(62.75, -1.455, 15.871);

	suspension->rightFront.hardpoints[Corner::OutboardSpring].Set(-1.232, 3.375, 19.5);
	suspension->leftFront.hardpoints[Corner::OutboardSpring].Set(-1.232, -3.375, 19.5);
	suspension->rightRear.hardpoints[Corner::OutboardSpring].Set(62.75, 1.455, 15.871);
	suspension->leftRear.hardpoints[Corner::OutboardSpring].Set(62.75, -1.455, 15.871);

	suspension->rightFront.hardpoints[Corner::BarArmAtPivot].Set(4.518, 5.0, 15.0);
	suspension->leftFront.hardpoints[Corner::BarArmAtPivot].Set(4.518, -5.0, 15.0);
	suspension->rightRear.hardpoints[Corner::BarArmAtPivot].Set(0.0, 0.0, 0.0);
	suspension->leftRear.hardpoints[Corner::BarArmAtPivot].Set(0.0, 0.0, 0.0);

	suspension->hardpoints[Suspension::FrontBarMidPoint].Set(4.518, 0.0, 15.0);
	suspension->hardpoints[Suspension::FrontBarPivotAxis].Set(0.0, 0.0, 0.0);
	suspension->hardpoints[Suspension::RearBarMidPoint].Set(78.0, 0.0, 4.0);
	suspension->hardpoints[Suspension::RearBarPivotAxis].Set(78.0, 1.0, 4.0);

	suspension->rightFront.hardpoints[Corner::InboardBarLink].Set(4.518, 5.0, 19.0);
	suspension->leftFront.hardpoints[Corner::InboardBarLink].Set(4.518, -5.0, 19.0);
	suspension->rightRear.hardpoints[Corner::InboardBarLink].Set(78.0, 4.0, 14.5);
	suspension->leftRear.hardpoints[Corner::InboardBarLink].Set(78.0, -4.0, 14.5);

	suspension->rightFront.hardpoints[Corner::OutboardBarLink].Set(-1.232, 5.0, 19.0);
	suspension->leftFront.hardpoints[Corner::OutboardBarLink].Set(-1.232, -5.0, 19.0);
	suspension->rightRear.hardpoints[Corner::OutboardBarLink].Set(63.0, 3.75, 14.5);
	suspension->leftRear.hardpoints[Corner::OutboardBarLink].Set(63.0, -3.75, 14.5);

	suspension->rightRear.hardpoints[Corner::InboardHalfShaft].Set(66.5, 7.0, 9.75);
	suspension->leftRear.hardpoints[Corner::InboardHalfShaft].Set(66.5, -7.0, 9.75);
	suspension->rightRear.hardpoints[Corner::OutboardHalfShaft].Set(66.5, 20.0, 10.25);
	suspension->leftRear.hardpoints[Corner::OutboardHalfShaft].Set(66.5, -20.0, 10.25);

	suspension->rackRatio = 0.8 * 2.0 / 3.14159;// [in/rad]

	// Test tires
	tires->rightFront->diameter = 20.5;
	tires->rightFront->width = 7.0;
	tires->rightFront->stiffness = 1000.0;

	tires->leftFront->diameter = 20.5;
	tires->leftFront->width = 7.0;
	tires->leftFront->stiffness = 1000.0;

	tires->rightRear->diameter = 20.5;
	tires->rightRear->width = 7.0;
	tires->rightRear->stiffness = 1000.0;

	tires->leftRear->diameter = 20.5;
	tires->leftRear->width = 7.0;
	tires->leftRear->stiffness = 1000.0;

	// Test brakes
	brakes->rearBrakesInboard = true;
	brakes->percentFrontBraking = 0.667;

	// Test mass properties
	massProperties->mass = 520.0 / 32.174;
	massProperties->totalCGHeight = 9.0;
	massProperties->cornerWeights.leftFront = 110.0 / 32.174;
	massProperties->cornerWeights.rightFront = 110.0 / 32.174;
	massProperties->cornerWeights.leftRear = 140.0 / 32.174;
	massProperties->cornerWeights.rightRear = 140.0 / 32.174;
	massProperties->unsprungMass.leftFront = 35.0 / 32.174;
	massProperties->unsprungMass.rightFront = 35.0 / 32.174;
	massProperties->unsprungMass.leftRear = 25.0 / 32.174;
	massProperties->unsprungMass.rightRear = 25.0 / 32.174;
	massProperties->unsprungCGHeights.leftFront = 10.0;
	massProperties->unsprungCGHeights.rightFront = 10.0;
	massProperties->unsprungCGHeights.leftRear = 10.0;
	massProperties->unsprungCGHeights.rightRear = 10.0;

	// Test spring rates
	suspension->barRate.front = 2000.0;// [in-lbf/rad]
	suspension->barRate.rear = 100.0;// [in-lbf/rad]
	suspension->leftFront.spring.rate = 50.0;// [lb/in]
	suspension->rightFront.spring.rate = 50.0;// [lb/in]
	suspension->leftRear.spring.rate = 60.0;// [lb/in]
	suspension->rightRear.spring.rate = 60.0;// [lb/in]

	// Test drivetrain
	drivetrain->SetRearWheelDrive(1.0);
}

//==========================================================================
// Class:			Car
// Function:		Car
//
// Description:		Copy Constructor for the Car class.  This call the copy
//					constructors for all of the subsystems as well.  This
//					DOES NOT allocate memory for this car.  Correct usage is
//					something like:
//						Car NewCar = new Car(OldCar);
//
// Input Arguments:
//		car	= const &Car that is to be copied
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Car::Car(const Car& car) : mComponents(CreateComponents())
{
	*this = car;
}

Car::Car(Car&& car) : mComponents(std::move(car.mComponents))
{
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
//		fileName	= wxString specifying the location to write to
//		poutFile	= std::ofstream* used to allow writing of the appearance options in
//					  an external function
//
// Output Arguments:
//		None
//
// Return Value:
//		returns true for successful write, false otherwise
//
//==========================================================================
bool Car::SaveCarToFile(wxString fileName, std::ofstream *poutFile) const
{
	// Open the specified file
	std::ofstream outFile(fileName.mb_str(), ios::out | ios::binary);
	if (!outFile.is_open() || !outFile.good())
		return false;

	BinaryWriter binFile(outFile);

	WriteFileHeader(binFile);

	// Call the write functions for each sub-system class
	// NOTE:  The order that these calls are made must be identical to the
	// order of the Read() calls in LoadCarFromFile().
	aerodynamics->Write(binFile);
	brakes->Write(binFile);
	drivetrain->Write(binFile);
	engine->Write(binFile);
	massProperties->Write(binFile);
	suspension->Write(binFile);
	tires->Write(binFile);

	// If we're saving the options (done elsewhere), open the additional file
	if (poutFile != NULL)
	{
		poutFile->open(fileName.mb_str(), ios::out | ios::binary);
		poutFile->seekp(outFile.tellp());
	}

	outFile.close();

	return true;
}

//==========================================================================
// Class:			Car
// Function:		LoadCarFromFile
//
// Description:		Calls the Read() functions for each sub-system class.
//
// Input Arguments:
//		fileName	= wxString specifying the location to read from
//		pinFile		= std::ifstream* used to allow reading of the appearance options in
//					  an external function
//
// Output Arguments:
//		fileVersion	= int* specifying the version of the file we're reading from
//
// Return Value:
//		None
//
//==========================================================================
bool Car::LoadCarFromFile(wxString fileName, std::ifstream *pinFile, int *fileVersion)
{
	std::ifstream inFile(fileName.mb_str(), ios::in | ios::binary);
	if (!inFile.is_open() || !inFile.good())
		return false;

	BinaryReader binFile(inFile);
	FileHeaderInfo header = ReadFileHeader(binFile);

	// Check to make sure the version matches
	if (header.fileVersion != currentFileVersion)
		Debugger::GetInstance() << "Warning:  Opening file with out-of-date file format." << Debugger::PriorityHigh;

	// Call the read function for each sub-system class
	// NOTE:  The order that these Read() calls are made must match the order
	// of the Write() calls in SaveCarToFile().
	aerodynamics->Read(binFile, header.fileVersion);
	brakes->Read(binFile, header.fileVersion);
	drivetrain->Read(binFile, header.fileVersion);
	engine->Read(binFile, header.fileVersion);
	massProperties->Read(binFile, header.fileVersion);
	suspension->Read(binFile, header.fileVersion);
	tires->Read(binFile, header.fileVersion);

	// If we're reading the options (done elsewhere), open the additional file
	if (pinFile != NULL)
	{
		pinFile->open(fileName.mb_str(), ios::in | ios::binary);
		pinFile->seekg(inFile.tellg());
		*fileVersion = header.fileVersion;
	}
	else
		inFile.close();

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
	suspension->ComputeWheelCenters(tires->rightFront->diameter, tires->leftFront->diameter,
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
	if (drivetrain->driveType == Drivetrain::DriveAllWheel ||
		drivetrain->driveType == Drivetrain::DriveFrontWheel ||
		brakes->frontBrakesInboard)
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
	if (drivetrain->driveType == Drivetrain::DriveAllWheel ||
		drivetrain->driveType == Drivetrain::DriveRearWheel ||
		brakes->rearBrakesInboard)
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
	// Check for self-assignment
	if (this == &car)
		return *this;

	assert(mComponents.size() == car.mComponents.size());
	std::copy(car.mComponents.begin(), car.mComponents.end(), mComponents.begin());
	return *this;
}

Car& Car::operator=(Car&& car)
{
	// Check for self-assignment
	if (this == &car)
		return *this;

	mComponents.clear();
	mComponents = std::move(car.mComponents);
	return *this;
}

std::vector<std::unique_ptr<Subsystem>> Car::CreateComponents()
{
	std::vector<std::unique_ptr<Subsystem>> components(mComponentManager.GetInfo().size());
	unsigned int i;
	for (i = 0; i < components.size(); ++i)
		components[i] = mComponentManager.GetInfo()[i].Create();

	return components;
}

}// namespace VVASE

