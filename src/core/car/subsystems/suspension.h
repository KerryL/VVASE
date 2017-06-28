/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016
===================================================================================*/

// File:  suspension.h
// Date:  11/3/2007
// Author:  K. Loux
// Desc:  Contains class declaration for Suspension class.

#ifndef SUSPENSION_H_
#define SUSPENSION_H_

// Local headers
#include "subsystem.h"
#include "corner.h"
#include "spring.h"
#include "damper.h"
#include "utilities/wheelSetStructures.h"

// Eigen headers
#include <Eigen/Eigen>

// Standard C++ headers
#include <vector>

// Local forward declarations
class BinaryReader;
class BinaryWriter;

class Suspension : public Subsystem
{
public:
	// File read/write functions
	void Write(BinaryWriter& file) const override;
	void Read(BinaryReader& file, const int& fileVersion) override;

    // Required by RegisterableComponent
    static std::unique_ptr<Suspension> Create() { return std::make_unique<Suspension>(); }
    static std::string GetName() { return _T("Suspension"); }

	// Calls the methods that calculate the wheel center location at each corner
	void ComputeWheelCenters(const double &rfTireDiameter, const double &lfTireDiameter,
		const double &rrTireDiameter, const double &lrTireDiameter);

	// Enumeration describing the available sway bar models
	enum BarStyle
	{
		SwayBarNone,
		SwayBarUBar,
		SwayBarTBar,
		SwayBarGeared,

		NumberOfBarStyles
	};

	// Enumeration describing the available sway bar attachment points
	enum BarAttachment
	{
		BarAttachmentBellcrank,
		BarAttachmentLowerAArm,
		BarAttachmentUpperAArm,
		BarAttachmentUpright,

		NumberOfBarAttachments
	};

	// Enumeration for all of the hardpoints included in this class
	enum Hardpoints
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

		NumberOfHardpoints
	};

	// For getting the name of the elements of this class
	static wxString GetHardpointName(const Hardpoints &point);
	static wxString GetBarStyleName(const BarStyle &barStyle);
	static wxString GetBarAttachmentname(const BarAttachment &barAttachment);

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

#endif// SUSPENSION_H_
