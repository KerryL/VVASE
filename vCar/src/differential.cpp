/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  differential.cpp
// Created:  11/6/2007
// Author:  K. Loux
// Description:  Contains class functionality for differential class.
// History:
//	3/9/2008	- Changed the structure of the Debugger class, K. Loux.
//	11/22/2009	- Moved to vCar.lib, K. Loux.

// Standard C++ headers
#include <fstream>

// VVASE headers
#include "vCar/differential.h"
#include "vUtilities/debugger.h"
#include "vUtilities/machineDefinitions.h"

//==========================================================================
// Class:			DIFFERENTIAL
// Function:		DIFFERENTIAL
//
// Description:		Constructor for the DIFFERENTIAL class.
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
DIFFERENTIAL::DIFFERENTIAL(const Debugger &_debugger) : debugger(_debugger)
{
}

//==========================================================================
// Class:			DIFFERENTIAL
// Function:		DIFFERENTIAL
//
// Description:		Copy constructor for the DIFFERENTIAL class.
//
// Input Arguments:
//		Differential	= const DIFFERENTIAL& to copy to this object
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
DIFFERENTIAL::DIFFERENTIAL(const DIFFERENTIAL &Differential) : debugger(Differential.debugger)
{
	// Do the copy
	*this = Differential;
}

//==========================================================================
// Class:			DIFFERENTIAL
// Function:		~DIFFERENTIAL
//
// Description:		Destructor for the DIFFERENTIAL class.
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
DIFFERENTIAL::~DIFFERENTIAL()
{
}

//==========================================================================
// Class:			DIFFERENTIAL
// Function:		Write
//
// Description:		Writes this differential to file.
//
// Input Arguments:
//		OutFile	= std::ofstream* pointing to the files stream to write to
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void DIFFERENTIAL::Write(std::ofstream *OutFile) const
{
	// Write this object to file
	// Not yet used

	return;
}

//==========================================================================
// Class:			DIFFERENTIAL
// Function:		Read
//
// Description:		Read from file to fill this differential.
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
void DIFFERENTIAL::Read(std::ifstream *InFile, int FileVersion)
{
	// Read this object from file accoring to the file version we're using
	if (FileVersion >= 0)// All versions
	{
		// Not yet used
	}
	else
		assert(0);

	return;
}

//==========================================================================
// Class:			DIFFERENTIAL
// Function:		operator =
//
// Description:		Assignment operator for DIFFERENTIAL class.
//
// Input Arguments:
//		Differential	= const DIFFERENTIAL& to assign to this object
//
// Output Arguments:
//		None
//
// Return Value:
//		DIFFERENTIAL&, reference to this object
//
//==========================================================================
DIFFERENTIAL& DIFFERENTIAL::operator = (const DIFFERENTIAL &Differential)
{
	// Check for self-assignment
	if (this == &Differential)
		return *this;

	// Perform the assignment
	BiasRatio	= Differential.BiasRatio;
	Preload		= Differential.Preload;

	Style		= Differential.Style;

	return *this;
}