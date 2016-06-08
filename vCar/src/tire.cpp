/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

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
#include <cassert>

// VVASE headers
#include "vCar/tire.h"
#include "vUtilities/machineDefinitions.h"

//==========================================================================
// Class:			Tire
// Function:		Tire
//
// Description:		Constructor for the Tire class.
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
Tire::Tire()
{
}

//==========================================================================
// Class:			Tire
// Function:		Tire
//
// Description:		Copy constructor for the Tire class.
//
// Input Arguments:
//		Tire	= const Tire& to copy from
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Tire::Tire(const Tire &tire)
{
	*this = tire;
}

//==========================================================================
// Class:			Tire
// Function:		~Tire
//
// Description:		Destructor for the Tire class.
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
Tire::~Tire()
{
}

//==========================================================================
// Class:			Tire
// Function:		Write
//
// Description:		Writes this tire to file.
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
void Tire::Write(std::ofstream *outFile) const
{
	// Write this object to file
	outFile->write((char*)&diameter, sizeof(diameter));
	outFile->write((char*)&width, sizeof(width));
	outFile->write((char*)&tirePressure, sizeof(tirePressure));
	outFile->write((char*)&stiffness, sizeof(stiffness));
	outFile->write((char*)&modelType, sizeof(modelType));
}

//==========================================================================
// Class:			Tire
// Function:		Read
//
// Description:		Read from file to fill this tire.
//
// Input Arguments:
//		inFile		= std::ifstream* pointing to the input stream
//		fileVersion	= int specifying the file version we're reading from
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Tire::Read(std::ifstream *inFile, int fileVersion)
{
	// Read this object from file accoring to the file version we're using
	if (fileVersion >= 5)
	{
		inFile->read((char*)&diameter, sizeof(diameter));
		inFile->read((char*)&width, sizeof(width));
		inFile->read((char*)&tirePressure, sizeof(tirePressure));
		inFile->read((char*)&stiffness, sizeof(stiffness));
		inFile->read((char*)&modelType, sizeof(modelType));
	}
	else if (fileVersion >= 4)
	{
		inFile->read((char*)&diameter, sizeof(diameter));
		inFile->read((char*)&width, sizeof(width));
		inFile->read((char*)&tirePressure, sizeof(tirePressure));
		inFile->read((char*)&stiffness, sizeof(stiffness));
	}
	else if (fileVersion >= 0)// All versions
	{
		inFile->read((char*)&diameter, sizeof(diameter));
		inFile->read((char*)&width, sizeof(width));
		inFile->read((char*)&tirePressure, sizeof(tirePressure));
	}
	else
		assert(false);
}

//==========================================================================
// Class:			Tire
// Function:		operator=
//
// Description:		Assignment operator for the Tire class.
//
// Input Arguments:
//		tire	= const Tire& to assign to this
//
// Output Arguments:
//		None
//
// Return Value:
//		Tire& reference to this
//
//==========================================================================
Tire& Tire::operator = (const Tire &tire)
{
	// Check for self-assignment
	if (this == &tire)
		return *this;

	// Copy the information to this
	diameter		= tire.diameter;
	width			= tire.width;
	tirePressure	= tire.tirePressure;
	stiffness		= tire.stiffness;
	modelType		= tire.modelType;

	return *this;
}