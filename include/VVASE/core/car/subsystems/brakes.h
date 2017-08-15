/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  brakes.h
// Date:  11/6/2007
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains class declaration for Brakes class (disk brakes only).

#ifndef BRAKES_H_
#define BRAKES_H_

// Local headers
#include "subsystem.h"
#include "VVASE/core/utilities/wheelSetStructures.h"
#include "VVASE/core/utilities/vvaseString.h"

namespace VVASE
{

// Local forward declarations
class BinaryReader;
class BinaryWriter;

class Brakes : public Subsystem
{
public:
	// File read/write functions
	void Write(BinaryWriter& file) const override;
	void Read(BinaryReader& file, const int& fileVersion) override;

	// Get the braking torque at each wheel as a function of pedal force
	WheelSet GetBrakingTorque(const double &pedalForce) const;	// [in-lbf]

	// Class properties
	FrontRearInteger numberOfDisks;
	WheelSet brakeDiameter;						// [in] - This is effective diameter, twice the moment arm
	WheelSet pistonArea;						// [in^2]
	FrontRearDouble masterCylinderArea;			// [in^2]
	double pedalRatio;							// [-]
	FrontRearDouble linePressure;				// [psi]
	double biasRatio;							// [-]	Front/Rear
	double percentFrontBraking;					// [-]
	// FIXME:  Make temperature dependant with thermal and cooling model (include aerodynamics?)

	// Important for determining anti-geometry (see RCVD p. 168)
	bool frontBrakesInboard = false;
	bool rearBrakesInboard = false;

	// Required by RegisterableComponent
    static std::unique_ptr<Brakes> Create() { return std::make_unique<Brakes>(); }
    static vvaseString GetName() { return _T("Brake"); }

    // Required by Subsystem
    wxPanel* GetEditPanel() override;
	wxTreeListItem* GetTreeItem() override;
};

}// namespace VVASE

#endif// BRAKES_H_
