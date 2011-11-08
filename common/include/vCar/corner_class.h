/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  corner_class.h
// Created:  3/23/2008
// Author:  K. Loux
// Description:  Contains class declaration for CORNER class.  This class contains
//				 the suspension information for one corner of the car (four instances
//				 are required to describe the entire suspension).
// History:
//	2/16/2009	- Changed to use enumeration style array of points instead of having each
//				  point declared individually, K. Loux.
//	11/22/2009	- Moved to vCar.lib, K. Loux.

#ifndef _CORNER_CLASS_H_
#define _CORNER_CLASS_H_

// Standard C++ headers
#include <iosfwd>// forward declarations of fstream objects

// VVASE headers
#include "vCar/spring_class.h"
#include "vCar/damper_class.h"
#include "vMath/vector_class.h"

// VVASE forward declarations
class Debugger;

class CORNER
{
public:
	// Enumeration describing the physical location of the corner on the car
	enum LOCATION
	{
		LocationRightFront,
		LocationLeftFront,
		LocationRightRear,
		LocationLeftRear,

		NumberOfLocations
	};

	// Constructor
	CORNER(const LOCATION &_Location, const Debugger &_debugger);

	// Destructor
	~CORNER();

	// File read/write functions
	void Write(std::ofstream *OutFile) const;
	void Read(std::ifstream *InFile, int FileVersion);

	// Calculates the wheel center location based on static toe and camber
	void ComputeWheelCenter(const double &TireDiameter);

	// Enumeration that describes the different suspension actuation types
	// we can model
	enum ACTUATION_ATTACHMENT
	{
		AttachmentLowerAArm,
		AttachmentUpperAArm,
		AttachmentUpright,

		NumberOfAttachments
	};

	// Enumeration for actuation type
	enum ACTUATION_TYPE
	{
		ActuationPushPullrod,
		ActuationOutboard,

		NumberOfActuationTypes
	};

	// Enumberation for suspension hardpoints
	enum HARDPOINTS
	{
		LowerFrontTubMount,
		LowerRearTubMount,
		LowerBallJoint,
		UpperFrontTubMount,
		UpperRearTubMount,
		UpperBallJoint,
		OutboardTieRod,
		InboardTieRod,
		OutboardPushrod,		// Also pullrod
		InboardPushrod,			// Also pullrod
		BellCrankPivot1,
		BellCrankPivot2,
		OutboardSpring,
		InboardSpring,
		OutboardShock,
		InboardShock,
		ContactPatch,
		WheelCenter,			// Not accessable to the user - this value is calcualted
		OutboardBarLink,
		InboardBarLink,
		BarArmAtPivot,			// For T-bar, this is at base of stem, for others, it is where the arm meets the torsion member
		GearEndBarShaft,		// Gear style bars only

		// Drivetrain geometery
		// These were previously a member of the DRIVETRAIN class, but there were
		// some heap allocation problems (crashes and memory leaks as a result of
		// trying to delete the dynamic GearRatio variable) due to creating new CAR
		// objects for STATIC_ANALYSIS.  Puting these here allows us to declare only
		// new SUSPENSION objects instead of whole cars, which fixes the memory
		// problem and makes some of the code more concise and neater.
		OutboardHalfShaft,
		InboardHalfShaft,

		NumberOfHardpoints
	};

	// For getting the names of the elements of this class
	static wxString GetHardpointName(const HARDPOINTS &Point);
	static wxString GetActuationAttachmentName(const ACTUATION_ATTACHMENT &_Attachment);
	static wxString GetActuationTypeName(const ACTUATION_TYPE &_Type);
	static wxString GetLocationName(const LOCATION &_Location);

	// Suspension parameters
	double StaticCamber;			// [rad] - positive top away from the center car
	double StaticToe;				// [rad] - positive front away the center of the car
	SPRING Spring;
	DAMPER Damper;
	// Bump stops (with non-linear rates?)
	// Droop limiters

	// Defines how the shock/spring is actuated for this CORNER
	ACTUATION_ATTACHMENT ActuationAttachment;

	// Defines what kind of actuation we have for this CORNER
	ACTUATION_TYPE ActuationType;

	// The location of this corner on the car
	const LOCATION Location;

	// The actual hardpoint locations
	Vector Hardpoints[NumberOfHardpoints];

	// Operators
	CORNER& operator=(const CORNER& Corner);

private:
	// Debugger message printing utility
	const Debugger &debugger;
};

#endif// _CORNER_CLASS_H_