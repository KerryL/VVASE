/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  aerodynamics.cpp
// Created:  11/3/2007
// Author:  K. Loux
// Description:  Contains class functionality for aerodynamics class.
// History:
//	3/9/2008	- Changed the structure of the Debugger class, K. Loux.
//	11/22/2009	- Moved to vCar.lib, K. Loux.

// VVASE headers
#include "vCar/aerodynamics.h"
#include "vUtilities/debugger.h"
#include "vUtilities/machineDefinitions.h"
#include "vUtilities/binaryReader.h"
#include "vUtilities/binaryWriter.h"

//==========================================================================
// Class:			Aerodynamics
// Function:		Aerodynamics
//
// Description:		Constructor for the Aerodynamics class.
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
Aerodynamics::Aerodynamics()
{
}

//==========================================================================
// Class:			Aerodynamics
// Function:		Aerodynamics
//
// Description:		Copy constructor for the Aerodynamics class.
//
// Input Arguments:
//		Aerodynamics	= const Aerodynamics& to copy to this object
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Aerodynamics::Aerodynamics(const Aerodynamics &aerodynamics)
{
	*this = aerodynamics;
}

//==========================================================================
// Class:			Aerodynamics
// Function:		~Aerodynamics
//
// Description:		Destructor for the Aerodynamics class.
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
Aerodynamics::~Aerodynamics()
{
}

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

//==========================================================================
// Class:			Aerodynamics
// Function:		operator =
//
// Description:		Assignment operator for Aerodynamics class.
//
// Input Arguments:
//		aerodynamics	= const Aerodynamics& to assign to this object
//
// Output Arguments:
//		None
//
// Return Value:
//		Aerodynamics&, reference to this object
//
//==========================================================================
Aerodynamics& Aerodynamics::operator = (const Aerodynamics &aerodynamics)
{
	// Check for self-assignment
	if (this == &aerodynamics)
		return *this;

	centerOfPressure		= aerodynamics.centerOfPressure;
	referenceArea			= aerodynamics.referenceArea;
	airDensity				= aerodynamics.airDensity;
	coefficientOfDownforce	= aerodynamics.coefficientOfDownforce;
	coefficientOfDrag		= aerodynamics.coefficientOfDrag;
	coefficientOfMoment		= aerodynamics.coefficientOfMoment;

	return *this;
}
