/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  suspension.h
// Created:  11/3/2007
// Author:  K. Loux
// Description:  Contains class declaration for Suspension class.
// History:
//	2/24/2008	- Moved half shaft points into here from Drivetrain object, K. Loux.
//	2/25/2008	- Named BarStyle and ActuationStyle enums, K. Loux.
//	3/9/2008	- Moved enumerations inside class structure and changed the structure of the
//				  Debugger class, K. Loux.
//	3/23/2008	- Changed units for class members and functions from degrees to radians, K. Loux.
//	2/16/2009	- Changed to use enumeration style array of points instead of having each
//				  point declared individually, K. Loux.
//	4/19/2009	- Made all of the Solve...() functions static, and made Debugger static, K. Loux.
//	11/22/2009	- Moved to vCar.lib, K. Loux.

#ifndef SUSPENSION_H_
#define SUSPENSION_H_

// Standard C++ headers
#include <vector>

// VVASE headers
#include "vCar/corner.h"
#include "vCar/spring.h"
#include "vCar/damper.h"
#include "vMath/vector.h"
#include "vUtilities/wheelSetStructures.h"

// Local forward declarations
class BinaryReader;
class BinaryWriter;

class Suspension
{
public:
	Suspension();

	// File read/write functions
	void Write(BinaryWriter& file) const;
	void Read(BinaryReader& file, const int& fileVersion);

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
	std::vector<Vector> hardpoints;

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

	Suspension& operator=(const Suspension& suspension);

	void UpdateSymmetry();
};

#endif// SUSPENSION_H_