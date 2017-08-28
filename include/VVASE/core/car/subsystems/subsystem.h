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

// wxWidgets forward declarations
class wxPanel;
class wxTreeListItem;

namespace VVASE
{

// Local forward declarations
class BinaryReader;
class BinaryWriter;

class Subsystem : public RegisterableComponent
{
public:
	virtual ~Subsystem() = default;

	// GUI factory methods
	virtual wxPanel* GetEditPanel() = 0;
	virtual wxTreeListItem* GetTreeItem() = 0;
	// TODO:  Also, a factory method for primitives (or return nullptr if it's not required to render anything?)
	// TODO:  Also possibly a "GetIcon" method?  See MainTree

	virtual void CloneTo(Subsystem* target) const = 0;

	// File read/write functions
	virtual void Write(BinaryWriter& file) const = 0;
	virtual void Read(BinaryReader& file, const int& fileVersion) = 0;
};

}// namespace VVASE

#endif// SUBSYSTEM_H_
