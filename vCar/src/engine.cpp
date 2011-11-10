/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  engine_class.cpp
// Created:  11/3/2007
// Author:  K. Loux
// Description:  Contains class functionality for engine class.
// History:
//	3/9/2008	- Changed the structure of the Debugger class, K. Loux.
//	11/22/2009	- Moved to vCar.lib, K. Loux.

// Standard C++ headers
#include <fstream>

// VVASE headers
#include "vCar/engine_class.h"
#include "vUtilities/debug_class.h"
#include "vUtilities/machine_defs.h"

//==========================================================================
// Class:			ENGINE
// Function:		ENGINE
//
// Description:		Constructor for the ENGINE class.
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
ENGINE::ENGINE(const Debugger &_debugger) : debugger(_debugger)
{
	// Initialize this object
	CrankshaftSpeed = 0.0;
}

//==========================================================================
// Class:			ENGINE
// Function:		ENGINE
//
// Description:		Copy constructor for the ENGINE class.
//
// Input Arguments:
//		Engine	= const ENGINE& to copy to this object
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
ENGINE::ENGINE(const ENGINE &Engine) : debugger(Engine.debugger)
{
	// Do the copy
	*this = Engine;
}

//==========================================================================
// Class:			ENGINE
// Function:		~ENGINE
//
// Description:		Destructor for the ENGINE class.
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
ENGINE::~ENGINE()
{
}

//==========================================================================
// Class:			ENGINE
// Function:		Write
//
// Description:		Writes this engine to file.
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
void ENGINE::Write(std::ofstream *OutFile) const
{
	// Write this object to file
	OutFile->write((char*)&CrankshaftSpeed, sizeof(double));

	return;
}

//==========================================================================
// Class:			ENGINE
// Function:		Read
//
// Description:		Read from file to fill this engine.
//
// Input Arguments:
//		InFile		= std::ifstream* pointing to the input stream
//		FileVersion	= int specifying which file version we're reading from
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void ENGINE::Read(std::ifstream *InFile, int FileVersion)
{
	// Read this object from file according to the file version we're using
	if (FileVersion >= 0)// All versions
	{
		InFile->read((char*)&CrankshaftSpeed, sizeof(double));
	}
	else
		assert(0);

	return;
}

//==========================================================================
// Class:			ENGINE
// Function:		operator =
//
// Description:		Assignment operator for ENGINE class.
//
// Input Arguments:
//		Engine	= const ENGINE& to assign to this object
//
// Output Arguments:
//		None
//
// Return Value:
//		ENGINE&, reference to this object
//
//==========================================================================
ENGINE& ENGINE::operator = (const ENGINE &Engine)
{
	// Check for self-assignment
	if (this == &Engine)
		return *this;

	// Perform the assignment
	CrankshaftSpeed	= Engine.CrankshaftSpeed;

	return *this;
}