/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

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

#ifndef KINEMATICS_H_
#define KINEMATICS_H_

// VVASE headers
#include "vSolver/physics/kinematicOutputs.h"

// VVASE forward declarations
class Car;

class Kinematics
{
public:
	Kinematics();

	// Definition for the inputs to the kinematics solver
	struct Inputs
	{
		double pitch;						// [rad]
		double roll;						// [rad]
		double heave;						// [in]
		double rackTravel;					// [in]
		Vector centerOfRotation;			// [in]
		Vector::Axis firstRotation;
		WheelSet tireDeflections;			// [in]

		// Operators
		bool operator==(const Kinematics::Inputs &target) const
		{
			if (pitch == target.pitch &&
				roll == target.roll &&
				heave == target.heave &&
				rackTravel == target.rackTravel &&
				centerOfRotation == target.centerOfRotation &&
				firstRotation == target.firstRotation &&
				tireDeflections == target.tireDeflections)
				return true;

			return false;
		}
		bool operator!=(const Kinematics::Inputs &target) const { return !(*this == target); }
	};

	inline void SetPitch(const double &pitch) { inputs.pitch = pitch; }
	inline void SetRoll(const double &roll) { inputs.roll = roll; }
	inline void SetHeave(const double &heave) { inputs.heave = heave; }
	inline void SetRackTravel(const double &travel) { inputs.rackTravel = travel; }
	inline void SetCenterOfRotation(const Vector &center) { inputs.centerOfRotation = center; }
	inline void SetFirstEulerRotation(const Vector::Axis &first) { inputs.firstRotation = first; }
	inline void SetInputs(const Inputs& inputs) { this->inputs = inputs; }
	inline void SetTireDeflections(const WheelSet& deflections) { inputs.tireDeflections = deflections; }

	void UpdateKinematics(const Car* originalCar, Car* workingCar, wxString name);

	inline KinematicOutputs GetOutputs() const { return outputs; }
	inline double GetPitch() const { return inputs.pitch; }
	inline double GetRoll() const { return inputs.roll; }
	inline double GetHeave() const { return inputs.heave; }
	inline double GetRackTravel() const { return inputs.rackTravel; }
	inline Vector GetCenterOfRotation() const { return inputs.centerOfRotation; }
	inline Vector::Axis GetFirstEulerRotation() const { return inputs.firstRotation; }
	inline WheelSet GetTireDeflections() const { return inputs.tireDeflections; }
	inline Inputs GetInputs() const { return inputs; }

private:
	Inputs inputs;

	const Car *originalCar;
	Car *workingCar;
	Suspension *localSuspension;

	KinematicOutputs outputs;

	bool SolveCorner(Corner &corner, const Corner &original,
		const Vector &rotations, const Vector::Axis &secondRotation, const double& tireDeflection);

	void UpdateOutputs();

	// Kinematic solvers
	static bool SolveForPoint(const Vector &center1, const Vector &center2, const Vector &center3,
		const Vector &originalCenter1, const Vector &originalCenter2, const Vector &originalCenter3,
		const Vector &original, Vector &current);
	static bool SolveForPoint(const Corner::Hardpoints &target, const Corner::Hardpoints& reference1,
		const Corner::Hardpoints& reference2, const Corner::Hardpoints& reference3,
		const Corner& originalCorner, Corner& currentCorner);
	static bool SolveForXY(const Corner::Hardpoints &target, const Corner::Hardpoints& reference1,
		const Corner::Hardpoints& reference2, const Corner& originalCorner, Corner& currentCorner);
	static bool SolveForContactPatch(const Vector &wheelCenter, const Vector &wheelPlaneNormal,
		const double &tireRadius, Vector &output);
	static bool SolveInboardTBarPoints(const Vector &leftOutboard, const Vector &rightOutboard,
		const Vector &centerPivot, const Vector &pivotAxisPoint,
		const Vector &originalLeftOutboard, const Vector &originalRightOutboard,
		const Vector &originalCenterPivot, const Vector &originalPivotAxisPoint,
		const Vector &originalLeftInboard, const Vector &originalRightInboard,
		Vector &leftInboard, Vector &rightInboard);

	// Other functions
	void MoveSteeringRack(const double &travel) const;
	void UpdateCGs(const Vector& cor, const Vector& angles, const Vector::Axis& first,
		const Vector::Axis& second, const double& heave, const WheelSet& tireDeflections, Car* workingCar) const;

	static Vector FindPerpendicularVector(const Vector &v);
	static double OptimizeCircleParameter(const Vector &center, const Vector &a,
		const Vector &b, const Vector &target);

	static bool SolveForXY(const Vector &center1, const Vector &center2, const Vector &originalCenter1,
		const Vector &originalCenter2, const Vector &original, Vector &current);
};

#endif// KINEMATICS_H_