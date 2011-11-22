/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  tireSet.cpp
// Created:  3/9/2008
// Author:  K. Loux
// Description:  Class that contains a set of four tires.  Required to manage
//				 dynamic memory allocation of a set of tires.
// History:
//	11/22/2009	- Moved to vCar.lib, K. Loux.

// Standard C++ headers
#include <fstream>

// VVASE headers
#include "vCar/tire.h"
#include "vCar/tireSet.h"
#include "vUtilities/debugger.h"
#include "vUtilities/machineDefinitions.h"

//==========================================================================
// Class:			TIRE_SET
// Function:		TIRE_SET
//
// Description:		Constructor for the TIRE_SET class.
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
TIRE_SET::TIRE_SET(const Debugger &_debugger) : debugger(_debugger)
{
	// Create four tires
	RightFront = new TIRE(debugger);
	LeftFront = new TIRE(debugger);
	RightRear = new TIRE(debugger);
	LeftRear = new TIRE(debugger);
}

//==========================================================================
// Class:			TIRE_SET
// Function:		TIRE_SET
//
// Description:		Copy constructor for the TIRE_SET class.
//
// Input Arguments:
//		TireSet	= const TIRE_SET& to be copied
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
TIRE_SET::TIRE_SET(const TIRE_SET &TireSet) : debugger(TireSet.debugger)
{
	// Initialize the pointers
	RightFront = NULL;
	LeftFront = NULL;
	RightRear = NULL;
	LeftRear = NULL;

	*this = TireSet;
}

//==========================================================================
// Class:			TIRE_SET
// Function:		~TIRE_SET
//
// Description:		Destructor for the TIRE_SET class.
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
TIRE_SET::~TIRE_SET()
{
	delete RightFront;
	RightFront = NULL;

	delete LeftFront;
	LeftFront = NULL;

	delete RightRear;
	RightRear = NULL;

	delete LeftRear;
	LeftRear = NULL;
}

//==========================================================================
// Class:			TIRE_SET
// Function:		Write
//
// Description:		Writes this tire set to file.
//
// Input Arguments:
//		OutFile	= std::ofstream* point to the file stream to write to
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void TIRE_SET::Write(std::ofstream *OutFile) const
{
	// Call the write functions for each of the four tires
	RightFront->Write(OutFile);
	LeftFront->Write(OutFile);
	RightRear->Write(OutFile);
	LeftRear->Write(OutFile);
}

//==========================================================================
// Class:			TIRE_SET
// Function:		Read
//
// Description:		Read from file to fill this tire set.
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
void TIRE_SET::Read(std::ifstream *InFile, int FileVersion)
{
	// Call the read functions for each of the four tires
	RightFront->Read(InFile, FileVersion);
	LeftFront->Read(InFile, FileVersion);
	RightRear->Read(InFile, FileVersion);
	LeftRear->Read(InFile, FileVersion);
}

//==========================================================================
// Class:			TIRE_SET
// Function:		operator=
//
// Description:		Assignment operator for the TIRE_SET class.
//
// Input Arguments:
//		TireSet	= TIRE_SET& to copy from
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
TIRE_SET& TIRE_SET::operator=(const TIRE_SET &TireSet)
{
	// Check for self-assignment
	if (this == &TireSet)
		return *this;

	// Assign the tires
	RightFront = new TIRE(*TireSet.RightFront);
	LeftFront = new TIRE(*TireSet.LeftFront);
	RightRear = new TIRE(*TireSet.RightRear);
	LeftRear = new TIRE(*TireSet.LeftRear);

	return *this;
}