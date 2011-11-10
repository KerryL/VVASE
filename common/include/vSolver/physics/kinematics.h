/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  kinematics.h
// Created:  11/3/2007
// Author:  K. Loux
// Description:  Contains class declaration for the KINEMATICS class.
// History:
//	2/24/2008	- Changed Static from type CAR to SUSPENSION.  Removed DRIVETRAIN object, K. Loux.
//	3/9/2008	- Changed structure of Debugger class and added input quantities to STATIC_OUTPUTS
//				  structure, K. Loux.
//	3/23/2008	- Changed units for class members and functions from degrees to radians and
//				  renamed class from KINEMATICS.  Also introduce OUTPUTS class and CORNER class
//				  (major restructuring of kinematic solvers), K. Loux.
//	3/24/2009	- Moved (physical location) to physics folder, K. Loux.
//	4/13/2009	- Added boost::threads for SolveCorner(), K. Loux.

#ifndef _KINEMATICS_CLASS_H_
#define _KINEMATICS_CLASS_H_

// VVASE headers
#include "vSolver/physics/kinematicOutputs.h"

// VVASE forward declarations
class CAR;

class KINEMATICS
{
public:
	// Constructor
	KINEMATICS(const Debugger &_debugger);

	// Destructor
	~KINEMATICS();

	// Definition for the inputs to the kinematics solver
	struct INPUTS
	{
		double Pitch;						// [rad]
		double Roll;						// [rad]
		double Heave;						// [in]
		double RackTravel;					// [in]
		Vector CenterOfRotation;			// [in]
		Vector::Axis FirstRotation;

		// Operators
		bool operator == (const KINEMATICS::INPUTS &Target) const
		{
			if (Pitch == Target.Pitch &&
				Roll == Target.Roll &&
				Heave == Target.Heave &&
				RackTravel == Target.RackTravel &&
				CenterOfRotation == Target.CenterOfRotation &&
				FirstRotation == Target.FirstRotation)
				return true;

			return false;
		}
		bool operator != (const KINEMATICS::INPUTS &Target) const { return !(*this == Target); };
	};

	// Private data accessors (SET)
	inline void SetPitch(const double &Pitch) { Inputs.Pitch = Pitch; };
	inline void SetRoll(const double &Roll) { Inputs.Roll = Roll; };
	inline void SetHeave(const double &Heave) { Inputs.Heave = Heave; };
	inline void SetRackTravel(const double &Travel) {Inputs.RackTravel = Travel; };
	inline void SetCenterOfRotation(const Vector &Center) { Inputs.CenterOfRotation = Center; };
	inline void SetFirstEulerRotation(const Vector::Axis &First) { Inputs.FirstRotation = First; };
	inline void SetInputs(INPUTS _Inputs) { Inputs = _Inputs; };

	// Main work function for this class
	void UpdateKinematics(const CAR* _OriginalCar, CAR* _WorkingCar, wxString Name);

	// Private data accessors (GET)
	inline KINEMATIC_OUTPUTS GetOutputs(void) { return Outputs; };
	inline double GetPitch(void) { return Inputs.Pitch; };
	inline double GetRoll(void) { return Inputs.Roll; };
	inline double GetHeave(void) { return Inputs.Heave; };
	inline double GetRackTravel(void) { return Inputs.RackTravel; };
	inline Vector GetCenterOfRotation(void) { return Inputs.CenterOfRotation; };
	inline Vector::Axis GetFirstEulerRotation(void) { return Inputs.FirstRotation; };
	inline INPUTS GetInputs(void) { return Inputs; };

private:
	// Debugger message printing utility
	const Debugger &debugger;

	// The inputs to the kinematic solver
	INPUTS Inputs;

	const CAR *OriginalCar;// Pointer to original car data
	CAR *WorkingCar;// Pointer to working (manipulated) car data
	SUSPENSION *Static;// Suspension in which we move the points around

	KINEMATIC_OUTPUTS Outputs;// Where we store everything after we UpdateOutputs

	// Kinematic solving routine for suspension points at each "corner" of the car
	bool SolveCorner(CORNER &Corner, const CORNER &Original, 
		const Vector &Rotations, const Vector::Axis &SecondRotation);

	// Performs all of the output value calculations (angles, distances, centers, etc.)
	void UpdateOutputs(void);
};

#endif// _KINEMATICS_CLASS_H_