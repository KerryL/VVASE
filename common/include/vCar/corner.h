/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  corner.h
// Created:  3/23/2008
// Author:  K. Loux
// Description:  Contains class declaration for CORNER class.  This class contains
//				 the suspension information for one corner of the car (four instances
//				 are required to describe the entire suspension).
// History:
//	2/16/2009	- Changed to use enumeration style array of points instead of having each
//				  point declared individually, K. Loux.
//	11/22/2009	- Moved to vCar.lib, K. Loux.

#ifndef _CORNER_H_
#define _CORNER_H_

// Standard C++ headers
#include <iosfwd>// forward declarations of fstream objects

// VVASE headers
#include "vCar/spring.h"
#include "vCar/damper.h"
#include "vMath/vector.h"

// VVASE forward declarations
class Debugger;

class Corner
{
public:
	// Enumeration describing the physical location of the corner on the car
	enum Location
	{
		LocationRightFront,
		LocationLeftFront,
		LocationRightRear,
		LocationLeftRear,

		NumberOfLocations
	};

	// Constructor
	Corner(const Location &_location, const Debugger &_debugger);

	// Destructor
	~Corner();

	// File read/write functions
	void Write(std::ofstream *outFile) const;
	void Read(std::ifstream *inFile, int fileVersion);

	// Calculates the wheel center location based on static toe and camber
	void ComputeWheelCenter(const double &tireDiameter);

	// Enumeration that describes the different suspension actuation types
	// we can model
	enum ActuationAttachment
	{
		AttachmentLowerAArm,
		AttachmentUpperAArm,
		AttachmentUpright,

		NumberOfAttachments
	};

	// Enumeration for actuation type
	enum ActuationType
	{
		ActuationPushPullrod,
		ActuationOutboard,

		NumberOfActuationTypes
	};

	// Enumberation for suspension hardpoints
	enum Hardpoints
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
		// These were previously a member of the Drivetrain class, but there were
		// some heap allocation problems (crashes and memory leaks as a result of
		// trying to delete the dynamic GearRatio variable) due to creating new Car
		// objects for Kinematics.  Puting these here allows us to declare only
		// new Suspension objects instead of whole cars, which fixes the memory
		// problem and makes some of the code more concise and neater.
		OutboardHalfShaft,
		InboardHalfShaft,

		NumberOfHardpoints
	};

	// For getting the names of the elements of this class
	static wxString GetHardpointName(const Hardpoints &point);
	static wxString GetActuationAttachmentName(const ActuationAttachment &_attachment);
	static wxString GetActuationTypeName(const ActuationType &_type);
	static wxString GetLocationName(const Location &_location);

	// Suspension parameters
	double staticCamber;			// [rad] - positive top away from the center car
	double staticToe;				// [rad] - positive front away the center of the car
	Spring spring;
	Damper damper;
	// Bump stops (with non-linear rates?)
	// Droop limiters

	// Defines how the shock/spring is actuated for this CORNER
	ActuationAttachment actuationAttachment;

	// Defines what kind of actuation we have for this CORNER
	ActuationType actuationType;

	// The location of this corner on the car
	const Location location;

	// The actual hardpoint locations
	Vector hardpoints[NumberOfHardpoints];

	// Operators
	Corner& operator=(const Corner& corner);

private:
	// Debugger message printing utility
	const Debugger &debugger;
};

#endif// _CORNER_H_