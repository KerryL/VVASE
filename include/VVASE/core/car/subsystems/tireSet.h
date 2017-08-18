/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  tireSet.h
// Date:  3/9/2008
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains class declaration for TireSet class.  This class contains a set
//        of four tires.  Required to manage dynamic memory allocation of a set of tires.

#ifndef TIRE_SET_H_
#define TIRE_SET_H_

// Local headers
#include "VVASE/core/car/subsystems/subsystem.h"
#include "VVASE/core/utilities/vvaseString.h"

// Standard C++ headers
#include <memory>
#include <string>

// wxWidgets forward declarations
class wxPanel;
class wxTreeListItem;

namespace VVASE
{

// Local forward declarations
class Tire;
class BinaryReader;
class BinaryWriter;

class TireSet : public Subsystem
{
public:
	TireSet();

    // Required by RegisterableComponent
    static std::unique_ptr<TireSet> Create();
    static vvaseString GetName() { return _T("Tires"); }

    // Required by Subsystem
    wxPanel* GetEditPanel() override;
	wxTreeListItem* GetTreeItem() override;

	// File read/write functions
	void Write(BinaryWriter& file) const override;
	void Read(BinaryReader& file, const int& fileVersion) override;

	std::unique_ptr<Tire> rightFront;
	std::unique_ptr<Tire> leftFront;
	std::unique_ptr<Tire> rightRear;
	std::unique_ptr<Tire> leftRear;
};

}// namespace VVASE

#endif// TIRE_SET_H_
