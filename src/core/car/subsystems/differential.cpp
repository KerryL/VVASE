/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  differential.cpp
// Date:  11/6/2007
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains class functionality for differential class.

// Local headers
#include "VVASE/core/car/subsystems/differential.h"
#include "VVASE/core/utilities/debugger.h"
#include "VVASE/core/utilities/binaryReader.h"
#include "VVASE/core/utilities/binaryWriter.h"

namespace VVASE
{

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
// Function:		Write
//
// Description:		Writes this differential to file.
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
void Differential::Write(BinaryWriter& file) const
{
	// Write this object to file
	file.Write(biasRatio);
}

//==========================================================================
// Class:			Differential
// Function:		Read
//
// Description:		Read from file to fill this differential.
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
void Differential::Read(BinaryReader& file, const int& fileVersion)
{
	// Read this object from file accoring to the file version we're using
	if (fileVersion >= 5)
	{
		file.Read(biasRatio);
	}
	else if (fileVersion >= 0)
	{
		biasRatio = 1.0;
	}
	else
		assert(false);
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

}// namespace VVASE
