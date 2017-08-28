/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  tire.cpp
// Date:  11/3/2007
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains class definition for tire class.

// Standard C++ headers
#include <cassert>

// Local headers
#include "VVASE/core/car/subsystems/tire.h"
#include "VVASE/core/utilities/binaryReader.h"
#include "VVASE/core/utilities/binaryWriter.h"

namespace VVASE
{

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
	diameter = 20.5;
	width = 7.0;
	stiffness = 1000.0;
	tirePressure = 12.0;
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
// Function:		Write
//
// Description:		Writes this tire to file.
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
void Tire::Write(BinaryWriter& file) const
{
	// Write this object to file
	file.Write(diameter);
	file.Write(width);
	file.Write(tirePressure);
	file.Write(stiffness);
	file.Write((unsigned int)modelType);
}

//==========================================================================
// Class:			Tire
// Function:		Read
//
// Description:		Read from file to fill this tire.
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
void Tire::Read(BinaryReader& file, const int& fileVersion)
{
	// Read this object from file accoring to the file version we're using
	if (fileVersion >= 5)
	{
		file.Read(diameter);
		file.Read(width);
		file.Read(tirePressure);
		file.Read(stiffness);

		unsigned int temp;
		file.Read(temp);
		modelType = static_cast<TireModel>(temp);
	}
	else if (fileVersion >= 4)
	{
		file.Read(diameter);
		file.Read(width);
		file.Read(tirePressure);
		file.Read(stiffness);
		modelType = TireModel::ConstantStiffnesses;
	}
	else if (fileVersion >= 0)// All versions
	{
		file.Read(diameter);
		file.Read(width);
		file.Read(tirePressure);
		stiffness = 1000.0;
		modelType = TireModel::ConstantStiffnesses;
	}
	else
		assert(false);
}

}// namespace VVASE
