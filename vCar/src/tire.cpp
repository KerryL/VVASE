/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  tire.cpp
// Created:  11/3/2007
// Author:  K. Loux
// Description:  Contains class definition for tire class.
// History:
//	3/9/2008	- Changed the structure of the Debugger class, K. Loux.
//	11/22/2009	- Moved to vCar.lib, K. Loux.

// Standard C++ headers
#include <fstream>

// VVASE headers
#include "vCar/tire.h"
#include "vUtilities/machineDefinitions.h"

//==========================================================================
// Class:			TIRE
// Function:		TIRE
//
// Description:		Constructor for the TIRE class.
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
TIRE::TIRE(const Debugger &_debugger) : debugger(_debugger)
{
}

//==========================================================================
// Class:			TIRE
// Function:		TIRE
//
// Description:		Copy constructor for the TIRE class.
//
// Input Arguments:
//		Tire	= const TIRE& to copy from
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
TIRE::TIRE(const TIRE &Tire) : debugger(Tire.debugger)
{
	// Copy from the Tire to this
	*this = Tire;
}

//==========================================================================
// Class:			TIRE
// Function:		~TIRE
//
// Description:		Destructor for the TIRE class.
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
TIRE::~TIRE()
{
}

//==========================================================================
// Class:			TIRE
// Function:		Write
//
// Description:		Writes this tire to file.
//
// Input Arguments:
//		OutFile	= std::ofstream* pointing to the output stream
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void TIRE::Write(std::ofstream *OutFile) const
{
	// Write this object to file
	OutFile->write((char*)&Diameter, sizeof(double));
	OutFile->write((char*)&Width, sizeof(double));
	OutFile->write((char*)&TirePressure, sizeof(double));
}

//==========================================================================
// Class:			TIRE
// Function:		Read
//
// Description:		Read from file to fill this tire.
//
// Input Arguments:
//		InFile		= std::ifstream* pointing to the input stream
//		FileVersion	= int specifying the file version we're reading from
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void TIRE::Read(std::ifstream *InFile, int FileVersion)
{
	// Read this object from file accoring to the file version we're using
	if (FileVersion >= 0)// All versions
	{
		InFile->read((char*)&Diameter, sizeof(double));
		InFile->read((char*)&Width, sizeof(double));
		InFile->read((char*)&TirePressure, sizeof(double));
	}
	else
		assert(0);
}

//==========================================================================
// Class:			TIRE
// Function:		operator=
//
// Description:		Assignment operator for the TIRE class.
//
// Input Arguments:
//		Tire	= const TIRE& to assign to this
//
// Output Arguments:
//		None
//
// Return Value:
//		TIRE& reference to this
//
//==========================================================================
TIRE& TIRE::operator = (const TIRE &Tire)
{
	// Check for self-assignment
	if (this == &Tire)
		return *this;

	// Copy the information to this
	Diameter		= Tire.Diameter;
	Width			= Tire.Width;
	TirePressure	= Tire.TirePressure;

	return *this;
}