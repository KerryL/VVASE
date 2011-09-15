/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  suspension_class.h
// Created:  11/3/2007
// Author:  K. Loux
// Description:  Contains class declaration for SUSPENSION class.
// History:
//	2/24/2008	- Moved half shaft points into here from DRIVETRAIN object, K. Loux.
//	2/25/2008	- Named BAR_STYLE and ACTUATION_STYLE enums, K. Loux.
//	3/9/2008	- Moved enumerations inside class structure and changed the structure of the
//				  DEBUGGER class, K. Loux.
//	3/23/2008	- Changed units for class members and functions from degrees to radians, K. Loux.
//	2/16/2009	- Changed to use enumeration style array of points instead of having each
//				  point declared individually, K. Loux.
//	4/19/2009	- Made all of the Solve...() functions static, and made Debugger static, K. Loux.
//	11/22/2009	- Moved to vCar.lib, K. Loux.

#ifndef _SUSPENSION_CLASS_H_
#define _SUSPENSION_CLASS_H_

// Standard C++ headers
#include <iosfwd>// forward declarations of fstream objects

// VVASE headers
#include "vCar/corner_class.h"
#include "vCar/spring_class.h"
#include "vCar/damper_class.h"
#include "vMath/vector_class.h"
#include "vUtilities/wheelset_structs.h"
#include "vUtilities/debug_class.h"

class SUSPENSION
{
public:
	// Constructor
	SUSPENSION(const DEBUGGER &_Debugger);

	// Destructor
	~SUSPENSION();

	// For assigning the debugger pointer
	// (MUST be called before calling other static member functions!)
	inline static void SetDebugger(const DEBUGGER &_Debugger) { Debugger = &_Debugger; };

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
	VECTOR Hardpoints[NumberOfHardpoints];

	// Suspension parameters
	FRONT_REAR_DOUBLE BarRate;			// [in-lb/rad]
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
	static bool SolveForPoint(const VECTOR &Center1, const VECTOR &Center2, const VECTOR &Center3,
		const VECTOR &OriginalCenter1, const VECTOR &OriginalCenter2, const VECTOR &OriginalCenter3,
		const VECTOR &Original, VECTOR &Current);
	static bool SolveForXY(const VECTOR &Center1, const VECTOR &Center2, const VECTOR &OriginalCenter1,
		const VECTOR &OriginalCenter2, const VECTOR &Original, VECTOR &Current);
	static bool SolveForContactPatch(const VECTOR &WheelCenter, const VECTOR &WheelPlaneNormal,
		const double &TireRadius, VECTOR &Output);

	// Other functions
	void MoveSteeringRack(const double &Travel);

	// Operators
	SUSPENSION& operator=(const SUSPENSION& Suspension);

private:
	// Debugger message printing utility
	static const DEBUGGER *Debugger;
};

#endif// _SUSPENSION_CLASS_H_