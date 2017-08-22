/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  suspension.h
// Date:  11/3/2007
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains class declaration for Suspension class.

#ifndef SUSPENSION_H_
#define SUSPENSION_H_

// Local headers
#include "subsystem.h"
#include "corner.h"
#include "spring.h"
#include "damper.h"
#include "VVASE/core/utilities/wheelSetStructures.h"
#include "VVASE/core/utilities/vvaseString.h"

// Eigen headers
#include <Eigen/Eigen>

// Standard C++ headers
#include <vector>

namespace VVASE
{

// Local forward declarations
class BinaryReader;
class BinaryWriter;

class Suspension : public Subsystem
{
public:
	Suspension();

	// File read/write functions
	void Write(BinaryWriter& file) const override;
	void Read(BinaryReader& file, const int& fileVersion) override;

    // Required by RegisterableComponent
    static std::unique_ptr<Suspension> Create() { return std::make_unique<Suspension>(); }
    static vvaseString GetName() { return _T("Suspension"); }

    // Required by Subsystem
    wxPanel* GetEditPanel() override;
	wxTreeListItem* GetTreeItem() override;

	// Calls the methods that calculate the wheel center location at each corner
	void ComputeWheelCenters(const double &rfTireDiameter, const double &lfTireDiameter,
		const double &rrTireDiameter, const double &lrTireDiameter);

	// Enumeration describing the available sway bar models
	enum class BarStyle
	{
		None,
		UBar,
		TBar,
		Geared,

		Count
	};

	// Enumeration describing the available sway bar attachment points
	enum class BarAttachment
	{
		Bellcrank,
		LowerAArm,
		UpperAArm,
		Upright,

		Count
	};

	// Enumeration for all of the hardpoints included in this class
	enum class Hardpoints
	{
		FrontBarMidPoint,			// U-bar and T-bar only
		FrontBarPivotAxis,			// T-bar only
		FrontThirdSpringInboard,
		FrontThirdSpringOutboard,
		FrontThirdDamperInboard,
		FrontThirdDamperOutboard,

		RearBarMidPoint,			// U-bar and T-bar only
		RearBarPivotAxis,			// T-bar only
		RearThirdSpringInboard,
		RearThirdSpringOutboard,
		RearThirdDamperInboard,
		RearThirdDamperOutboard,

		Count
	};

	// For getting the name of the elements of this class
	static vvaseString GetHardpointName(const Hardpoints &point);
	static vvaseString GetBarStyleName(const BarStyle &barStyle);
	static vvaseString GetBarAttachmentname(const BarAttachment &barAttachment);

	// Suspension hardpoints
	Corner rightFront;
	Corner leftFront;
	Corner rightRear;
	Corner leftRear;

	// The hardpoints that are not within the corner
	std::vector<Eigen::Vector3d> hardpoints;

	// Suspension parameters
	FrontRearDouble barRate;			// [in-lb/rad]
	double rackRatio;					// (inches rack travel) / (radians at steering wheel)

	// Flags and styles
	bool isSymmetric;
	BarStyle frontBarStyle;
	BarStyle rearBarStyle;
	BarAttachment frontBarAttachment;
	BarAttachment rearBarAttachment;
	bool frontHasThirdSpring;
	bool rearHasThirdSpring;

	// Flags for enforcing ARB twist sign convention
	bool frontBarSignGreaterThan;
	bool rearBarSignGreaterThan;

	// Third spring and damper objects
	Spring frontThirdSpring;
	Spring rearThirdSpring;
	Damper frontThirdDamper;
	Damper rearThirdDamper;

	void UpdateSymmetry();
};

}// namespace VVASE

#endif// SUSPENSION_H_
