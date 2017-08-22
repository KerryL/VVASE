/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  tireSet.cpp
// Date:  3/9/2008
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Class that contains a set of four tires.  Required to manage
//        dynamic memory allocation of a set of tires.

// Local headers
#include "VVASE/core/car/subsystems/tire.h"
#include "VVASE/core/car/subsystems/tireSet.h"
#include "VVASE/core/utilities/debugger.h"
#include "VVASE/core/utilities/binaryReader.h"
#include "VVASE/core/utilities/binaryWriter.h"

namespace VVASE
{

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
	rightFront = std::make_unique<Tire>();
	leftFront = std::make_unique<Tire>();
	rightRear = std::make_unique<Tire>();
	leftRear = std::make_unique<Tire>();
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
// Function:		Create
//
// Description:		Creates a new TireSet object.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		std::unique_ptr<TireSet>
//
//==========================================================================
std::unique_ptr<TireSet> TireSet::Create()
{
	return std::make_unique<TireSet>();
}

wxPanel* TireSet::GetEditPanel()
{
	// TODO:  Implement
	return nullptr;
}

wxTreeListItem* TireSet::GetTreeItem()
{
	// TODO:  Implement
	return nullptr;
}

}// namespace VVASE
