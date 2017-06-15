/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  brakes.cpp
// Created:  11/6/2007
// Author:  K. Loux
// Description:  Contains class functionality for brakes class.
// History:
//	3/9/2008	- Changed the structure of the Debugger class, K. Loux.
//	11/22/2009	- Moved to vCar.lib, K. Loux.

// VVASE headers
#include "vCar/brakes.h"
#include "vUtilities/debugger.h"
#include "vUtilities/machineDefinitions.h"
#include "vUtilities/binaryReader.h"
#include "vUtilities/binaryWriter.h"

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