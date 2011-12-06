/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  kinematics.h
// Created:  11/3/2007
// Author:  K. Loux
// Description:  Contains class declaration for the Kinematics class.
// History:
//	2/24/2008	- Changed Static from type Car to Suspension.  Removed Drivetrain object, K. Loux.
//	3/9/2008	- Changed structure of Debugger class and added input quantities to StaticOutputs
//				  structure, K. Loux.
//	3/23/2008	- Changed units for class members and functions from degrees to radians and
//				  renamed class from Kinematics.  Also introduce Outputs class and CCORNERorner class
//				  (major restructuring of kinematic solvers), K. Loux.
//	3/24/2009	- Moved (physical location) to physics folder, K. Loux.
//	4/13/2009	- Added boost::threads for SolveCorner(), K. Loux.

#ifndef _KINEMATICS_H_
#define _KINEMATICS_H_

// VVASE headers
#include "vSolver/physics/kinematicOutputs.h"

// VVASE forward declarations
class Car;

class Kinematics
{
public:
	// Constructor
	Kinematics();

	// Destructor
	~Kinematics();

	// Definition for the inputs to the kinematics solver
	struct Inputs
	{
		double pitch;						// [rad]
		double roll;						// [rad]
		double heave;						// [in]
		double rackTravel;					// [in]
		Vector centerOfRotation;			// [in]
		Vector::Axis firstRotation;

		// Operators
		bool operator == (const Kinematics::Inputs &target) const
		{
			if (pitch == target.pitch &&
				roll == target.roll &&
				heave == target.heave &&
				rackTravel == target.rackTravel &&
				centerOfRotation == target.centerOfRotation &&
				firstRotation == target.firstRotation)
				return true;

			return false;
		}
		bool operator != (const Kinematics::Inputs &target) const { return !(*this == target); };
	};

	// Private data accessors (SET)
	inline void SetPitch(const double &pitch) { inputs.pitch = pitch; };
	inline void SetRoll(const double &roll) { inputs.roll = roll; };
	inline void SetHeave(const double &heave) { inputs.heave = heave; };
	inline void SetRackTravel(const double &travel) { inputs.rackTravel = travel; };
	inline void SetCenterOfRotation(const Vector &center) { inputs.centerOfRotation = center; };
	inline void SetFirstEulerRotation(const Vector::Axis &first) { inputs.firstRotation = first; };
	inline void SetInputs(Inputs _inputs) { inputs = _inputs; };

	// Main work function for this class
	void UpdateKinematics(const Car* _OriginalCar, Car* _WorkingCar, wxString Name);

	// Private data accessors (GET)
	inline KinematicOutputs GetOutputs(void) { return outputs; };
	inline double GetPitch(void) { return inputs.pitch; };
	inline double GetRoll(void) { return inputs.roll; };
	inline double GetHeave(void) { return inputs.heave; };
	inline double GetRackTravel(void) { return inputs.rackTravel; };
	inline Vector GetCenterOfRotation(void) { return inputs.centerOfRotation; };
	inline Vector::Axis GetFirstEulerRotation(void) { return inputs.firstRotation; };
	inline Inputs GetInputs(void) { return inputs; };

private:
	// The inputs to the kinematic solver
	Inputs inputs;

	const Car *originalCar;// Pointer to original car data
	Car *workingCar;// Pointer to working (manipulated) car data
	Suspension *localSuspension;// Suspension in which we move the points around

	KinematicOutputs outputs;// Where we store everything after we UpdateOutputs

	// Kinematic solving routine for suspension points at each "corner" of the car
	bool SolveCorner(Corner &corner, const Corner &original, 
		const Vector &rotations, const Vector::Axis &secondRotation);

	// Performs all of the output value calculations (angles, distances, centers, etc.)
	void UpdateOutputs(void);
};

#endif// _KINEMATICS_H_