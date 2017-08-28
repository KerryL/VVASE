/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  engine.h
// Date:  11/3/2007
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains class declaration for ENGINE class.

#ifndef ENGINE_H_
#define ENGINE_H_

// Local headers
#include "subsystem.h"
#include "VVASE/core/utilities/vvaseString.h"

namespace VVASE
{

// Local forward declarations
class BinaryReader;
class BinaryWriter;

class Engine : public Subsystem
{
public:
	// File read/write functions
	void Write(BinaryWriter& file) const;
	void Read(BinaryReader& file, const int& fileVersion);

	// Get the outputs from this portion of the simulation
	double GetEngineSpeed();// [rad/sec]
	double TorqueOutput(const double &engineSpeed, const double &throttlePosition);// [in-lb]

    // Required by RegisterableComponent
    static std::unique_ptr<Subsystem> Create() { return std::make_unique<Engine>(); }
    static vvaseString GetName() { return _T("Engine"); }

    // Required by Subsystem
    wxPanel* GetEditPanel() override;
	wxTreeListItem* GetTreeItem() override;

	void CloneTo(Subsystem* target) const override;
};

}// namespace VVASE

#endif// ENGINE_H_
