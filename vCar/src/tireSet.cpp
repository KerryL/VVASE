/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

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
// Class:			TireSet
// Function:		TireSet
//
// Description:		Constructor for the TireSet class.
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
TireSet::TireSet()
{
	// Create four tires
	rightFront = new Tire();
	leftFront = new Tire();
	rightRear = new Tire();
	leftRear = new Tire();
}

//==========================================================================
// Class:			TireSet
// Function:		TireSet
//
// Description:		Copy constructor for the TireSet class.
//
// Input Arguments:
//		TireSet	= const TireSet& to be copied
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
TireSet::TireSet(const TireSet &tireSet)
{
	// Create four tires
	rightFront = new Tire();
	leftFront = new Tire();
	rightRear = new Tire();
	leftRear = new Tire();

	// Make the assignment
	*this = tireSet;
}

//==========================================================================
// Class:			TireSet
// Function:		~TireSet
//
// Description:		Destructor for the TireSet class.
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
TireSet::~TireSet()
{
	delete rightFront;
	rightFront = NULL;

	delete leftFront;
	leftFront = NULL;

	delete rightRear;
	rightRear = NULL;

	delete leftRear;
	leftRear = NULL;
}

//==========================================================================
// Class:			TireSet
// Function:		Write
//
// Description:		Writes this tire set to file.
//
// Input Arguments:
//		outFile	= std::ofstream* point to the file stream to write to
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void TireSet::Write(std::ofstream *outFile) const
{
	// Call the write functions for each of the four tires
	rightFront->Write(outFile);
	leftFront->Write(outFile);
	rightRear->Write(outFile);
	leftRear->Write(outFile);
}

//==========================================================================
// Class:			TireSet
// Function:		Read
//
// Description:		Read from file to fill this tire set.
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
void TireSet::Read(std::ifstream *inFile, int fileVersion)
{
	// Call the read functions for each of the four tires
	rightFront->Read(inFile, fileVersion);
	leftFront->Read(inFile, fileVersion);
	rightRear->Read(inFile, fileVersion);
	leftRear->Read(inFile, fileVersion);
}

//==========================================================================
// Class:			TireSet
// Function:		operator=
//
// Description:		Assignment operator for the TireSet class.
//
// Input Arguments:
//		tireSet	= TireSet& to copy from
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
TireSet& TireSet::operator=(const TireSet &tireSet)
{
	// Check for self-assignment
	if (this == &tireSet)
		return *this;

	// Assign the tires
	*rightFront = *tireSet.rightFront;
	*leftFront = *tireSet.leftFront;
	*rightRear = *tireSet.rightRear;
	*leftRear = *tireSet.leftRear;

	return *this;
}