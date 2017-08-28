/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  brakes.cpp
// Date:  11/6/2007
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains class functionality for brakes class.

// Local headers
#include "VVASE/core/car/subsystems/brakes.h"
#include "VVASE/core/utilities/debugger.h"
#include "VVASE/core/utilities/binaryReader.h"
#include "VVASE/core/utilities/binaryWriter.h"

namespace VVASE
{

//==========================================================================
// Class:			Brakes
// Function:		Write
//
// Description:		Writes these brakes to file.
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
void Brakes::Write(BinaryWriter& file) const
{
	file.Write(percentFrontBraking);
	file.Write(frontBrakesInboard);
	file.Write(rearBrakesInboard);
}

//==========================================================================
// Class:			Brakes
// Function:		Read
//
// Description:		Read from file to fill these brakes.
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
void Brakes::Read(BinaryReader& file, const int& fileVersion)
{
	// Read this object from file according to the file version we're using
	if (fileVersion >= 0)// All versions
	{
		file.Read(percentFrontBraking);
		file.Read(frontBrakesInboard);
		file.Read(rearBrakesInboard);
	}
	else
		assert(false);
}

//==========================================================================
// Class:			Brakes
// Function:		operator=
//
// Description:		Assignment operator for Brakes class.
//
// Input Arguments:
//		brakes	= const Brakes& to assign to this object
//
// Output Arguments:
//		None
//
// Return Value:
//		Brakes&, reference to this object
//
//==========================================================================
/*Brakes& Brakes::operator=(const Brakes &brakes)
{
	// Check for self-assignment
	if (this == &brakes)
		return *this;

	numberOfDisks		= brakes.numberOfDisks;
	brakeDiameter		= brakes.brakeDiameter;
	pistonArea			= brakes.pistonArea;
	masterCylinderArea	= brakes.masterCylinderArea;
	pedalRatio			= brakes.pedalRatio;
	linePressure		= brakes.linePressure;
	biasRatio			= brakes.biasRatio;
	percentFrontBraking	= brakes.percentFrontBraking;

	frontBrakesInboard	= brakes.frontBrakesInboard;
	rearBrakesInboard	= brakes.rearBrakesInboard;

	return *this;
}*/

wxPanel* Brakes::GetEditPanel()
{
	// TODO:  Implement
	return nullptr;
}

wxTreeListItem* Brakes::GetTreeItem()
{
	// TODO:  Implement
	return nullptr;
}

void Brakes::CloneTo(Subsystem* target) const
{
	auto* t(dynamic_cast<Brakes*>(target));
	assert(t);

	*t = *this;
}

}// namespace VVASE
