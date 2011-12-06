/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

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

#ifndef _SUSPENSION_H_
#define _SUSPENSION_H_

// Standard C++ headers
#include <iosfwd>// forward declarations of fstream objects

// VVASE headers
#include "vCar/corner.h"
#include "vCar/spring.h"
#include "vCar/damper.h"
#include "vMath/vector.h"
#include "vUtilities/wheelSetStructures.h"

class Suspension
{
public:
	// Constructor
	Suspension();

	// Destructor
	~Suspension();

	// File read/write functions
	void Write(std::ofstream *outFile) const;
	void Read(std::ifstream *inFile, int fileVersion);

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
		FrontThirdSpringInboard,
		FrontThirdSpringOutboard,
		FrontThirdShockInboard,
		FrontThirdShockOutboard,

		RearBarMidPoint,			// U-bar and T-bar only
		RearThirdSpringInboard,
		RearThirdSpringOutboard,
		RearThirdShockInboard,
		RearThirdShockOutboard,

		NumberOfHardpoints
	};

	// For getting the name of the elements of this class
	static wxString GetHardpointName(const Hardpoints &point);
	static wxString GetBarStyleName(const BarStyle &_barStyle);
	static wxString GetBarAttachmentname(const BarAttachment &_barAttachment);

	// Suspension hardpoints
	Corner rightFront;
	Corner leftFront;
	Corner rightRear;
	Corner leftRear;

	// The hardpoints that are not within the corner
	Vector hardpoints[NumberOfHardpoints];

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

	// Third spring and damper objects
	Spring frontThirdSpring;
	Spring rearThirdSpring;
	Damper frontThirdDamper;
	Damper rearThirdDamper;

	// Kinematic solvers
	static bool SolveForPoint(const Vector &center1, const Vector &center2, const Vector &center3,
		const Vector &originalCenter1, const Vector &originalCenter2, const Vector &originalCenter3,
		const Vector &original, Vector &current);
	static bool SolveForXY(const Vector &center1, const Vector &center2, const Vector &originalCenter1,
		const Vector &originalCenter2, const Vector &original, Vector &current);
	static bool SolveForContactPatch(const Vector &wheelCenter, const Vector &wheelPlaneNormal,
		const double &tireRadius, Vector &output);

	// Other functions
	void MoveSteeringRack(const double &travel);

	// Operators
	Suspension& operator=(const Suspension& suspension);
};

#endif// _SUSPENSION_H_