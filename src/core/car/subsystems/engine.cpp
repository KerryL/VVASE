/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  engine.cpp
// Date:  11/3/2007
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains class functionality for engine class.

// Standard C++ headers
#include <cassert>

// Local headers
#include "VVASE/core/car/subsystems/engine.h"
#include "VVASE/core/utilities/binaryReader.h"
#include "VVASE/core/utilities/binaryWriter.h"

namespace VVASE
{

//==========================================================================
// Class:			Engine
// Function:		Write
//
// Description:		Writes this engine to file.
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
void Engine::Write(BinaryWriter& /*file*/) const
{
	// Not yet used
}

//==========================================================================
// Class:			Engine
// Function:		Read
//
// Description:		Read from file to fill this engine.
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
void Engine::Read(BinaryReader& file, const int& fileVersion)
{
	// Read this object from file according to the file version we're using
	if (fileVersion >= 5)
	{
		// Not used
	}
	else if (fileVersion >= 0)// All versions
	{
		double dummy;
		file.Read(dummy);
	}
	else
		assert(false);
}

wxPanel* Engine::GetEditPanel()
{
	// TODO:  Implement
	return nullptr;
}

wxTreeListItem* Engine::GetTreeItem()
{
	// TODO:  Implement
	return nullptr;
}

void Engine::CloneTo(Subsystem* target) const
{
	auto* t(dynamic_cast<Engine*>(target));
	assert(t);

	// TODO:  Implement
}

}// namespace VVASE
