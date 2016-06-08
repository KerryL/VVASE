/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

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
// Class:			Differential
// Function:		Differential
//
// Description:		Constructor for the Differential class.
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
Differential::Differential()
{
}

//==========================================================================
// Class:			Differential
// Function:		Differential
//
// Description:		Copy constructor for the Differential class.
//
// Input Arguments:
//		differential	= const Differential& to copy to this object
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Differential::Differential(const Differential &differential)
{
	*this = differential;
}

//==========================================================================
// Class:			Differential
// Function:		Differential
//
// Description:		Copy constructor for the Differential class.
//
// Input Arguments:
//		biasRatio	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Differential::Differential(const double& biasRatio)
{
	this->biasRatio = biasRatio;
}

//==========================================================================
// Class:			Differential
// Function:		~Differential
//
// Description:		Destructor for the Differential class.
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
Differential::~Differential()
{
}

//==========================================================================
// Class:			Differential
// Function:		Write
//
// Description:		Writes this differential to file.
//
// Input Arguments:
//		outFile	= std::ofstream* pointing to the files stream to write to
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Differential::Write(std::ofstream* outFile) const
{
	// Write this object to file
	outFile->write((char*)&biasRatio, sizeof(biasRatio));
}

//==========================================================================
// Class:			Differential
// Function:		Read
//
// Description:		Read from file to fill this differential.
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
void Differential::Read(std::ifstream* inFile, int fileVersion)
{
	// Read this object from file accoring to the file version we're using
	if (fileVersion >= 5)
	{
		inFile->read((char*)&biasRatio, sizeof(biasRatio));
	}
	//else// Not used
}

//==========================================================================
// Class:			Differential
// Function:		operator=
//
// Description:		Assignment operator for Differential class.
//
// Input Arguments:
//		differential	= const Differential& to assign to this object
//
// Output Arguments:
//		None
//
// Return Value:
//		Differential&, reference to this object
//
//==========================================================================
Differential& Differential::operator=(const Differential &differential)
{
	// Check for self-assignment
	if (this == &differential)
		return *this;

	biasRatio = differential.biasRatio;

	return *this;
}
