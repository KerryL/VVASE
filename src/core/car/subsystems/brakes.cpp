/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  brakes.cpp
// Date:  11/6/2007
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains class functionality for brakes class.

// Local headers
#include "vCar/brakes.h"
#include "vUtilities/debugger.h"
#include "vUtilities/machineDefinitions.h"
#include "vUtilities/binaryReader.h"
#include "vUtilities/binaryWriter.h"

namespace VVASE
{

//==========================================================================
// Class:			Brakes
// Function:		Brakes
//
// Description:		Constructor for the Brakes class.
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
Brakes::Brakes()
{
	// Initialize the class members
	frontBrakesInboard = false;
	rearBrakesInboard = false;
}

//==========================================================================
// Class:			Brakes
// Function:		Brakes
//
// Description:		Copy constructor for the Brakes class.
//
// Input Arguments:
//		brakes	= const Brakes& to copy to this object
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Brakes::Brakes(const Brakes &brakes)
{
	*this = brakes;
}

//==========================================================================
// Class:			Brakes
// Function:		~Brakes
//
// Description:		Destructor for the Brakes class.
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
Brakes::~Brakes()
{
}

//==========================================================================
// Class:			Brakes
// Function:		Write
//
// Description:		Writes these brakes to file.
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
void Brakes::Write(BinaryWriter& file) const
{
	file.Write(percentFrontBraking);
	file.Write(frontBrakesInboard);
	file.Write(rearBrakesInboard);
}

//==========================================================================
// Class:			Brakes
// Function:		Read
//
// Description:		Read from file to fill these brakes.
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
void Brakes::Read(BinaryReader& file, const int& fileVersion)
{
	// Read this object from file according to the file version we're using
	if (fileVersion >= 0)// All versions
	{
		file.Read(percentFrontBraking);
		file.Read(frontBrakesInboard);
		file.Read(rearBrakesInboard);
	}
	else
		assert(false);
}

//==========================================================================
// Class:			Brakes
// Function:		operator =
//
// Description:		Assignment operator for Brakes class.
//
// Input Arguments:
//		brakes	= const Brakes& to assign to this object
//
// Output Arguments:
//		None
//
// Return Value:
//		Brakes&, reference to this object
//
//==========================================================================
Brakes& Brakes::operator = (const Brakes &brakes)
{
	// Check for self-assignment
	if (this == &brakes)
		return *this;

	numberOfDisks		= brakes.numberOfDisks;
	brakeDiameter		= brakes.brakeDiameter;
	pistonArea			= brakes.pistonArea;
	masterCylinderArea	= brakes.masterCylinderArea;
	pedalRatio			= brakes.pedalRatio;
	linePressure		= brakes.linePressure;
	biasRatio			= brakes.biasRatio;
	percentFrontBraking	= brakes.percentFrontBraking;

	frontBrakesInboard	= brakes.frontBrakesInboard;
	rearBrakesInboard	= brakes.rearBrakesInboard;

	return *this;
}

}// namespace VVASE
