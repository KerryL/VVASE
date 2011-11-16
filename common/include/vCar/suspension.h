/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  suspension.h
// Created:  11/3/2007
// Author:  K. Loux
// Description:  Contains class declaration for SUSPENSION class.
// History:
//	2/24/2008	- Moved half shaft points into here from DRIVETRAIN object, K. Loux.
//	2/25/2008	- Named BAR_STYLE and ACTUATION_STYLE enums, K. Loux.
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
#include "vUtilities/debugger.h"

class SUSPENSION
{
public:
	// Constructor
	SUSPENSION(const Debugger &_debugger);

	// Destructor
	~SUSPENSION();

	// For assigning the debugger pointer
	// (MUST be called before calling other static member functions!)
	inline static void SetDebugger(const Debugger &_debugger) { debugger = &_debugger; };

	// File read/write functions
	void Write(std::ofstream *OutFile) const;
	void Read(std::ifstream *InFile, int FileVersion);

	// Calls the methods that calculate the wheel center location at each corner
	void ComputeWheelCenters(const double &RFTireDiameter, const double &LFTireDiameter,
		const double &RRTireDiameter, const double &LRTireDiameter);

	// Enumeration describing the available sway bar models
	enum BAR_STYLE
	{
		SwayBarNone,
		SwayBarUBar,
		SwayBarTBar,
		SwayBarGeared,

		NumberOfBarStyles
	};

	// Enumeration describing the available sway bar attachment points
	enum BAR_ATTACHMENT
	{
		BarAttachmentBellcrank,
		BarAttachmentLowerAArm,
		BarAttachmentUpperAArm,
		BarAttachmentUpright,

		NumberOfBarAttachments
	};

	// Enumeration for all of the hardpoints included in this class
	enum HARDPOINTS
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
	static wxString GetHardpointName(const HARDPOINTS &Point);
	static wxString GetBarStyleName(const BAR_STYLE &_BarStyle);
	static wxString GetBarAttachmentname(const BAR_ATTACHMENT &_BarAttachment);

	// Suspension hardpoints
	CORNER RightFront;
	CORNER LeftFront;
	CORNER RightRear;
	CORNER LeftRear;

	// The hardpoints that are not within the corner
	Vector Hardpoints[NumberOfHardpoints];

	// Suspension parameters
	FrontRearDouble BarRate;			// [in-lb/rad]
	double RackRatio;					// (inches rack travel) / (radians at steering wheel)

	// Flags and styles
	bool IsSymmetric;
	BAR_STYLE FrontBarStyle;
	BAR_STYLE RearBarStyle;
	BAR_ATTACHMENT FrontBarAttachment;
	BAR_ATTACHMENT RearBarAttachment;
	bool FrontHasThirdSpring;
	bool RearHasThirdSpring;

	// Third spring and damper objects
	SPRING FrontThirdSpring;
	SPRING RearThirdSpring;
	DAMPER FrontThirdDamper;
	DAMPER RearThirdDamper;

	// Kinematic solvers
	static bool SolveForPoint(const Vector &Center1, const Vector &Center2, const Vector &Center3,
		const Vector &OriginalCenter1, const Vector &OriginalCenter2, const Vector &OriginalCenter3,
		const Vector &Original, Vector &Current);
	static bool SolveForXY(const Vector &Center1, const Vector &Center2, const Vector &OriginalCenter1,
		const Vector &OriginalCenter2, const Vector &Original, Vector &Current);
	static bool SolveForContactPatch(const Vector &WheelCenter, const Vector &WheelPlaneNormal,
		const double &TireRadius, Vector &Output);

	// Other functions
	void MoveSteeringRack(const double &Travel);

	// Operators
	SUSPENSION& operator=(const SUSPENSION& Suspension);

private:
	// Debugger message printing utility
	static const Debugger *debugger;
};

#endif// _SUSPENSION_H_