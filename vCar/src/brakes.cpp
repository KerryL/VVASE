/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

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

// Standard C++ headers
#include <fstream>

// VVASE headers
#include "vCar/brakes.h"
#include "vUtilities/debugger.h"
#include "vUtilities/machineDefinitions.h"

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
	// Do the copy
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
//		outFile	= std::ofstream* pointing to the file stream to write to
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Brakes::Write(std::ofstream *outFile) const
{
	// Write this object to file
	outFile->write((char*)&percentFrontBraking, sizeof(double));
	outFile->write((char*)&frontBrakesInboard, sizeof(bool));
	outFile->write((char*)&rearBrakesInboard, sizeof(bool));
}

//==========================================================================
// Class:			Brakes
// Function:		Read
//
// Description:		Read from file to fill these brakes.
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
void Brakes::Read(std::ifstream *inFile, int fileVersion)
{
	// Read this object from file according to the file version we're using
	if (fileVersion >= 0)// All versions
	{
		inFile->read((char*)&percentFrontBraking, sizeof(double));
		inFile->read((char*)&frontBrakesInboard, sizeof(bool));
		inFile->read((char*)&rearBrakesInboard, sizeof(bool));
	}
	else
		assert(0);
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

	// Perform the assignment
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