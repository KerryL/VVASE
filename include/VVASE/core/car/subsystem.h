/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  subsystem.h
// Date:  6/20/2017
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Abstract base class for car components.

#ifndef SUBSYSTEM_H_
#define SUBSYSTEM_H_

// Local headers
#include "VVASE/core/utilities/componentManager.h"

namespace VVASE
{

class Subsystem : public RegisterableComponent
{
public:
	// GUI factory methods
	virtual wxPanel* GetEditPanel() = 0;
	virtual wxTreeListItem* GetTreeItem() = 0;
	// TODO:  Also, a factory method for primitives (or return nullptr if it's not required to render anything?)

	// File read/write functions
	virtual void Write(BinaryWriter& file) const = 0;
	virtual void Read(BinaryReader& file, const int& fileVersion) = 0;
};

}// namespace VVASE

#endif// SUBSYSTEM_H_
