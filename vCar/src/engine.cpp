/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  engine.cpp
// Created:  11/3/2007
// Author:  K. Loux
// Description:  Contains class functionality for engine class.
// History:
//	3/9/2008	- Changed the structure of the Debugger class, K. Loux.
//	11/22/2009	- Moved to vCar.lib, K. Loux.

// Standard C++ headers
#include <fstream>
#include <cassert>

// VVASE headers
#include "vCar/engine.h"
#include "vUtilities/machineDefinitions.h"

//==========================================================================
// Class:			Engine
// Function:		Engine
//
// Description:		Constructor for the Engine class.
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
Engine::Engine()
{
	crankshaftSpeed = 0.0;
}

//==========================================================================
// Class:			Engine
// Function:		Engine
//
// Description:		Copy constructor for the Engine class.
//
// Input Arguments:
//		engine	= const Engine& to copy to this object
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Engine::Engine(const Engine &engine)
{
	*this = engine;
}

//==========================================================================
// Class:			Engine
// Function:		~Engine
//
// Description:		Destructor for the Engine class.
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
Engine::~Engine()
{
}

//==========================================================================
// Class:			Engine
// Function:		Write
//
// Description:		Writes this engine to file.
//
// Input Arguments:
//		outFile	= std::ofstream* pointing to the output stream
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Engine::Write(std::ofstream *outFile) const
{
	outFile->write((char*)&crankshaftSpeed, sizeof(double));
}

//==========================================================================
// Class:			Engine
// Function:		Read
//
// Description:		Read from file to fill this engine.
//
// Input Arguments:
//		inFile		= std::ifstream* pointing to the input stream
//		fileVersion	= int specifying which file version we're reading from
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Engine::Read(std::ifstream *inFile, int fileVersion)
{
	// Read this object from file according to the file version we're using
	if (fileVersion >= 0)// All versions
	{
		inFile->read((char*)&crankshaftSpeed, sizeof(double));
	}
	else
		assert(false);
}

//==========================================================================
// Class:			Engine
// Function:		operator =
//
// Description:		Assignment operator for Engine class.
//
// Input Arguments:
//		engine	= const Engine& to assign to this object
//
// Output Arguments:
//		None
//
// Return Value:
//		Engine&, reference to this object
//
//==========================================================================
Engine& Engine::operator = (const Engine &engine)
{
	// Check for self-assignment
	if (this == &engine)
		return *this;

	crankshaftSpeed	= engine.crankshaftSpeed;

	return *this;
}