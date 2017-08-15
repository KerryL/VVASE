/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  aerodynamics.cpp
// Date:  11/3/2007
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains class functionality for aerodynamics class.

// Local headers
#include "VVASE/core/car/subsystems/aerodynamics.h"
#include "VVASE/core/utilities/binaryReader.h"
#include "VVASE/core/utilities/binaryWriter.h"

namespace VVASE
{

//==========================================================================
// Class:			Aerodynamics
// Function:		Write
//
// Description:		Writes these aerodynamics to file.
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
void Aerodynamics::Write(BinaryWriter& /*file*/) const
{
	// Write this object to file
	// Not yet used
}

//==========================================================================
// Class:			Aerodynamics
// Function:		Read
//
// Description:		Read from file to fill this aerodynamics.
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
void Aerodynamics::Read(BinaryReader& /*file*/, const int& fileVersion)
{
	// Read this object from file according to the file version we're using
	if (fileVersion >= 0)// All versions
	{
		// Not yet used yet
	}
	else
		assert(false);
}

}// namespace VVASE
