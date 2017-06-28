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
#include "utilities/componentManager.h"

namespace VVASE
{

class Subsystem : public RegisterableComponent
{
public:
	virtual wxPanel* GetEditPanel() = 0;
	virtual wxTreeListItem* GetTreeItem() = 0;

	// File read/write functions
	virtual void Write(BinaryWriter& file) const = 0;
	virtual void Read(BinaryReader& file, const int& fileVersion) = 0;
};

}// namespace VVASE

#endif// SUBSYSTEM_H_
