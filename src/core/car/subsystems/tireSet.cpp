/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  tireSet.cpp
// Date:  3/9/2008
// Auth:  K. Loux
// Licn:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Class that contains a set of four tires.  Required to manage
//        dynamic memory allocation of a set of tires.

// Local headers
#include "vCar/tire.h"
#include "vCar/tireSet.h"
#include "vUtilities/debugger.h"
#include "vUtilities/machineDefinitions.h"
#include "vUtilities/binaryReader.h"
#include "vUtilities/binaryWriter.h"

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
//		file	= BinaryWriter&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void TireSet::Write(BinaryWriter& file) const
{
	// Call the write functions for each of the four tires
	rightFront->Write(file);
	leftFront->Write(file);
	rightRear->Write(file);
	leftRear->Write(file);
}

//==========================================================================
// Class:			TireSet
// Function:		Read
//
// Description:		Read from file to fill this tire set.
//
// Input Arguments:
//		file		= BinaryReader&
//		fileVersion	= const int& specifying which file version we're reading from
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void TireSet::Read(BinaryReader& file, const int& fileVersion)
{
	// Call the read functions for each of the four tires
	rightFront->Read(file, fileVersion);
	leftFront->Read(file, fileVersion);
	rightRear->Read(file, fileVersion);
	leftRear->Read(file, fileVersion);
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
