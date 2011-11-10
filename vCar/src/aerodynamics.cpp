/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  aerodynamics.cpp
// Created:  11/3/2007
// Author:  K. Loux
// Description:  Contains class functionality for aerodynamics class.
// History:
//	3/9/2008	- Changed the structure of the Debugger class, K. Loux.
//	11/22/2009	- Moved to vCar.lib, K. Loux.

// Standard C++ headers
#include <fstream>

// VVASE headers
#include "vCar/aerodynamics.h"
#include "vUtilities/debugger.h"
#include "vUtilities/machineDefinitions.h"

//==========================================================================
// Class:			AERODYNAMICS
// Function:		AERODYNAMICS
//
// Description:		Constructor for the AERODYNAMICS class.
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
AERODYNAMICS::AERODYNAMICS(const Debugger &_debugger) : debugger(_debugger)
{
}

//==========================================================================
// Class:			AERODYNAMICS
// Function:		AERODYNAMICS
//
// Description:		Copy constructor for the AERODYNAMICS class.
//
// Input Arguments:
//		Aerodynamics	= const AERODYNAMICS& to copy to this object
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
AERODYNAMICS::AERODYNAMICS(const AERODYNAMICS &Aerodynamics) : debugger(Aerodynamics.debugger)
{
	// Do the copy
	*this = Aerodynamics;
}

//==========================================================================
// Class:			AERODYNAMICS
// Function:		~AERODYNAMICS
//
// Description:		Destructor for the AERODYNAMICS class.
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
AERODYNAMICS::~AERODYNAMICS()
{
}

//==========================================================================
// Class:			AERODYNAMICS
// Function:		Write
//
// Description:		Writes these aerodynamics to file.
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
void AERODYNAMICS::Write(std::ofstream *OutFile) const
{
	// Write this object to file
	// Not yet used

	return;
}

//==========================================================================
// Class:			AERODYNAMICS
// Function:		Read
//
// Description:		Read from file to fill this aerodynamics.
//
// Input Arguments:
//		InFile		= std::ifstream* pointing to the input stream
//		FileVersion	= int specifying what file version we're reading from
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void AERODYNAMICS::Read(std::ifstream *InFile, int FileVersion)
{
	// Read this object from file according to the file version we're using
	if (FileVersion >= 0)// All versions
	{
		// Not yet used yet
	}
	else
		assert(0);

	return;
}

//==========================================================================
// Class:			AERODYNAMICS
// Function:		operator =
//
// Description:		Assignment operator for AERODYNAMICS class.
//
// Input Arguments:
//		Aerodynamics	= const AERODYNAMICS& to assign to this object
//
// Output Arguments:
//		None
//
// Return Value:
//		AERODYNAMICS&, reference to this object
//
//==========================================================================
AERODYNAMICS& AERODYNAMICS::operator = (const AERODYNAMICS &Aerodynamics)
{
	// Check for self-assignment
	if (this == &Aerodynamics)
		return *this;

	// Perform the assignment
	CenterOfPressure		= Aerodynamics.CenterOfPressure;
	ReferenceArea			= Aerodynamics.ReferenceArea;
	AirDensity				= Aerodynamics.AirDensity;
	CoefficientOfDownforce	= Aerodynamics.CoefficientOfDownforce;
	CoefficientOfDrag		= Aerodynamics.CoefficientOfDrag;
	CoefficientOfMoment		= Aerodynamics.CoefficientOfMoment;

	return *this;
}