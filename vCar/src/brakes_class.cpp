/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  brakes_class.cpp
// Created:  11/6/2007
// Author:  K. Loux
// Description:  Contains class functionality for brakes class.
// History:
//	3/9/2008	- Changed the structure of the Debugger class, K. Loux.
//	11/22/2009	- Moved to vCar.lib, K. Loux.

// Standard C++ headers
#include <fstream>

// VVASE headers
#include "vCar/brakes_class.h"
#include "vUtilities/debug_class.h"
#include "vUtilities/machine_defs.h"

//==========================================================================
// Class:			BRAKES
// Function:		BRAKES
//
// Description:		Constructor for the BRAKES class.
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
BRAKES::BRAKES(const Debugger &_debugger) : debugger(_debugger)
{
	// Initialize the class members
	FrontBrakesInboard = false;
	RearBrakesInboard = false;
}

//==========================================================================
// Class:			BRAKES
// Function:		BRAKES
//
// Description:		Copy constructor for the BRAKES class.
//
// Input Arguments:
//		Brakes	= const BRAKES& to copy to this object
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
BRAKES::BRAKES(const BRAKES &Brakes) : debugger(Brakes.debugger)
{
	// Do the copy
	*this = Brakes;
}

//==========================================================================
// Class:			BRAKES
// Function:		~BRAKES
//
// Description:		Destructor for the BRAKES class.
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
BRAKES::~BRAKES()
{
}

//==========================================================================
// Class:			BRAKES
// Function:		Write
//
// Description:		Writes these brakes to file.
//
// Input Arguments:
//		OutFile	= std::ofstream* pointing to the file stream to write to
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void BRAKES::Write(std::ofstream *OutFile) const
{
	// Write this object to file
	OutFile->write((char*)&PercentFrontBraking, sizeof(double));
	OutFile->write((char*)&FrontBrakesInboard, sizeof(bool));
	OutFile->write((char*)&RearBrakesInboard, sizeof(bool));

	return;
}

//==========================================================================
// Class:			BRAKES
// Function:		Read
//
// Description:		Read from file to fill these brakes.
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
void BRAKES::Read(std::ifstream *InFile, int FileVersion)
{
	// Read this object from file according to the file version we're using
	if (FileVersion >= 0)// All versions
	{
		InFile->read((char*)&PercentFrontBraking, sizeof(double));
		InFile->read((char*)&FrontBrakesInboard, sizeof(bool));
		InFile->read((char*)&RearBrakesInboard, sizeof(bool));
	}
	else
		assert(0);

	return;
}

//==========================================================================
// Class:			BRAKES
// Function:		operator =
//
// Description:		Assignment operator for BRAKES class.
//
// Input Arguments:
//		Brakes	= const BRAKES& to assign to this object
//
// Output Arguments:
//		None
//
// Return Value:
//		BRAKES&, reference to this object
//
//==========================================================================
BRAKES& BRAKES::operator = (const BRAKES &Brakes)
{
	// Check for self-assignment
	if (this == &Brakes)
		return *this;

	// Perform the assignment
	NumberOfDisks		= Brakes.NumberOfDisks;
	BrakeDiameter		= Brakes.BrakeDiameter;
	PistonArea			= Brakes.PistonArea;
	MasterCylinderArea	= Brakes.MasterCylinderArea;
	PedalRatio			= Brakes.PedalRatio;
	LinePressure		= Brakes.LinePressure;
	BiasRatio			= Brakes.BiasRatio;
	PercentFrontBraking	= Brakes.PercentFrontBraking;

	FrontBrakesInboard	= Brakes.FrontBrakesInboard;
	RearBrakesInboard	= Brakes.RearBrakesInboard;

	return *this;
}