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
#include <cassert>

// VVASE headers
#include "vCar/tire.h"
#include "vUtilities/machineDefinitions.h"
#include "vUtilities/binaryReader.h"
#include "vUtilities/binaryWriter.h"

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
		modelType = ModelConstantMu;
	}
	else if (fileVersion >= 0)// All versions
	{
		file.Read(diameter);
		file.Read(width);
		file.Read(tirePressure);
		stiffness = 1000.0;
		modelType = ModelConstantMu;
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