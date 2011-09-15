/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  kinematic_outputs_class.h
// Created:  3/23/2008
// Author:  K. Loux
// Description:  Contains class declaration for KINEMATIC_OUTPUTS class.  This class does the
//				 calculations for all of the kinematic simulation outputs.  That includes any
//				 kind of wheel angle/orientation, chassis attitude, spring/shock positions, but
//				 doesn't include any thing that requires forces to calculate (force-based roll
//				 center, etc.).
// History:
//	3/24/2008	- Created CORNER_OUTPUTS structure, K. Loux.
//	2/23/2009	- Changed to enum/array style for outputs instead of individually declared variables
//				  for each output, K. Loux.
//	3/11/2009	- Finished implementation of enum/array style data members, K. Loux.

#ifndef _KINEMATIC_OUTPUTS_CLASS_H_
#define _KINEMATIC_OUTPUTS_CLASS_H_

// wxWidgets headers
#include <wx/thread.h>

// VVASE headers
#include "vMath/vector_class.h"
#include "vUtilities/convert_class.h"
#include "vCar/corner_class.h"

// VVASE forward declarations
class CAR;
class DEBUGGER;
class SUSPENSION;

class KINEMATIC_OUTPUTS
{
public:
	// Constructor
	KINEMATIC_OUTPUTS();

	// Destructor
	~KINEMATIC_OUTPUTS();

	// Sets the pointer to the DEBUGGER object
	static inline void SetDebugger(const DEBUGGER &_Debugger) { Debugger = &_Debugger; };

	// Updates the kinematic variables associated with the Current SUSPENSION
	void Update(const CAR *Original, const SUSPENSION *Current);

	// Enumeration for double outputs that get computed for every corner
	enum CORNER_OUTPUTS_DOUBLE
	{
		Caster,						// [rad]
		Camber,						// [rad]
		KPI,						// [rad]
		Steer,						// [rad]
		Spring,						// [in]
		Shock,						// [in]
		AxlePlunge,					// [in]
		CasterTrail,				// [in]
		ScrubRadius,				// [in]
		Scrub,						// [in]
		SpringInstallationRatio,	// [in Spring/in Wheel]
		ShockInstallationRatio,		// [in Shock/in Wheel]
		SpindleLength,				// [in]
		SideViewSwingArmLength,		// [in]
		FrontViewSwingArmLength,	// [in]
		AntiBrakePitch,				// [%] : anti-dive in the front, anti-lift in the rear
		AntiDrivePitch,				// [%] : anti-lift in the front, anti-squat in the rear

		NumberOfCornerOutputDoubles
	};

	// Enumeration for VECTOR outputs that get computed for every corner
	enum CORNER_OUTPUTS_VECTOR
	{
		InstantCenter,			// [in]
		InstantAxisDirection,	// [-]

		NumberOfCornerOutputVectors
	};

	// Enumeration for double outputs that only get computed once per car
	enum OUTPUTS_DOUBLE
	{
		FrontARBTwist,				// [rad]
		RearARBTwist,				// [rad]
		FrontThirdSpring,			// [in]
		FrontThirdShock,			// [in]
		RearThirdSpring,			// [in]
		RearThirdShock,				// [in]
		FrontNetSteer,				// [rad]
		RearNetSteer,				// [rad]
		FrontNetScrub,				// [in]
		RearNetScrub,				// [in]
		FrontARBMotionRatio,		// [rad/in]
		RearARBMotionRatio,			// [rad/in]
		FrontTrackGround,			// [in]
		RearTrackGround,			// [in]
		RightWheelbaseGround,		// [in]
		LeftWheelbaseGround,		// [in]
		FrontTrackHub,				// [in]
		RearTrackHub,				// [in]
		RightWheelbaseHub,			// [in]
		LeftWheelbaseHub,			// [in]

		NumberOfOutputDoubles
	};

	// Enumeration for VECTOR outputs that only get computed once per car
	enum OUTPUTS_VECTOR
	{
		// Kinematic centers
		FrontKinematicRC,			// [in]
		RearKinematicRC,			// [in]
		RightKinematicPC,			// [in]
		LeftKinematicPC,			// [in]

		// Kinematic axis
		FrontRollAxisDirection,
		RearRollAxisDirection,
		RightPitchAxisDirection,
		LeftPitchAxisDirection,

		NumberOfOutputVectors
	};

	// Sway bar twist will be total for all bar types... normal for U-bar and T-bar
	// but geared bars will include the twist along both lengths of bar.  This way
	// it can be used to determine forces/stresses directly.
	double Doubles[NumberOfOutputDoubles];
	VECTOR Vectors[NumberOfOutputVectors];

	// The outputs that are associated with just one corner of the car
	double RightFront[NumberOfCornerOutputDoubles];
	double LeftFront[NumberOfCornerOutputDoubles];
	double RightRear[NumberOfCornerOutputDoubles];
	double LeftRear[NumberOfCornerOutputDoubles];

	VECTOR RightFrontVectors[NumberOfCornerOutputVectors];
	VECTOR LeftFrontVectors[NumberOfCornerOutputVectors];
	VECTOR RightRearVectors[NumberOfCornerOutputVectors];
	VECTOR LeftRearVectors[NumberOfCornerOutputVectors];

	// Enumeration that encompasses all of the outputs for the whole car
	// This makes referencing these outputs from other classes a little easier
	enum OUTPUTS_COMPLETE
	{
		StartRightFrontDoubles,
		EndRightFrontDoubles = StartRightFrontDoubles + NumberOfCornerOutputDoubles - 1,
		StartRightFrontVectors,
		EndRightFrontVectors = StartRightFrontVectors + 3 * NumberOfCornerOutputVectors - 1,

		StartLeftFrontDoubles,
		EndLeftFrontDoubles = StartLeftFrontDoubles + NumberOfCornerOutputDoubles - 1,
		StartLeftFrontVectors,
		EndLeftFrontVectors = StartLeftFrontVectors + 3 * NumberOfCornerOutputVectors - 1,

		StartRightRearDoubles,
		EndRightRearDoubles = StartRightRearDoubles + NumberOfCornerOutputDoubles - 1,
		StartRightRearVectors,
		EndRightRearVectors = StartRightRearVectors + 3 * NumberOfCornerOutputVectors - 1,

		StartLeftRearDoubles,
		EndLeftRearDoubles = StartLeftRearDoubles + NumberOfCornerOutputDoubles - 1,
		StartLeftRearVectors,
		EndLeftRearVectors = StartLeftRearVectors + 3 * NumberOfCornerOutputVectors - 1,

		StartDoubles,
		EndDoubles = StartDoubles + NumberOfOutputDoubles - 1,

		StartVectors,
		EndVectors = StartVectors + 3 * NumberOfOutputVectors - 1,

		NumberOfOutputScalars// Called "scalars" because each vector component is treated as one output
	};

	// For converting from an output + location to OUTPUTS_COMPLETE
	static OUTPUTS_COMPLETE OutputsCompleteIndex(const CORNER::LOCATION &Location,
		const CORNER_OUTPUTS_DOUBLE &CornerDouble, const CORNER_OUTPUTS_VECTOR &CornerVector,
		const OUTPUTS_DOUBLE &Double, const OUTPUTS_VECTOR &Vector, const VECTOR::AXIS &Axis);

	// For accessing an output via the OUTPUTS_COMPLETE list
	double GetOutputValue(const OUTPUTS_COMPLETE &_Output) const;

	// For determining unit type of the outputs
	static CONVERT::UNIT_TYPE GetOutputUnitType(const OUTPUTS_COMPLETE &_Output);

	// For determining the name of an output from the OUTPUTS_COMPLETE list
	static wxString GetOutputName(const OUTPUTS_COMPLETE &_Output);

	// Mutex accessor
	//wxMutex &GetMutex(void) const { return KinematicOutputsMutex; };

private:
	// Debugger message printing utility
	static const DEBUGGER *Debugger;

	// The currently associated car object
	const CAR *CurrentCar;

	// Updates the outputs associated with the associated corner
	void UpdateCorner(const CORNER *OriginalCorner, const CORNER *CurrentCorner);

	// For retrieving names of the outputs
	static wxString GetCornerDoubleName(const CORNER_OUTPUTS_DOUBLE &_Output);
	static wxString GetCornerVectorName(const CORNER_OUTPUTS_VECTOR &_Output);
	static wxString GetDoubleName(const OUTPUTS_DOUBLE &_Output);
	static wxString GetVectorName(const OUTPUTS_VECTOR &_Output);

	// For retrieving units of the outputs
	static CONVERT::UNIT_TYPE GetCornerDoubleUnitType(const CORNER_OUTPUTS_DOUBLE &_Output);
	static CONVERT::UNIT_TYPE GetCornerVectorUnitType(const CORNER_OUTPUTS_VECTOR &_Output);
	static CONVERT::UNIT_TYPE GetDoubleUnitType(const OUTPUTS_DOUBLE &_Output);
	static CONVERT::UNIT_TYPE GetVectorUnitType(const OUTPUTS_VECTOR &_Output);

	// Initializes all outputs to QNAN
	void InitializeAllOutputs(void);
};

#endif// _KINEMATIC_OUTPUTS_CLASS_H_