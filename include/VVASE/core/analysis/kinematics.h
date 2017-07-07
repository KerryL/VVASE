/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  kinematics.h
// Date:  11/3/2007
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains class declaration for the Kinematics class.

#ifndef KINEMATICS_H_
#define KINEMATICS_H_

// Local headers
#include "VVASE/core/analysis/analysis.h"
#include "VVASE/core/analysis/kinematicOutputs.h"

namespace VVASE
{

// Local forward declarations
class Car;

class Kinematics : public Analysis
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
		Eigen::Vector3d centerOfRotation;			// [in]
		Eigen::Vector3d::Axis firstRotation;
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
	inline void SetCenterOfRotation(const Eigen::Vector3d &center) { inputs.centerOfRotation = center; }
	inline void SetFirstEulerRotation(const Eigen::Vector3d::Axis &first) { inputs.firstRotation = first; }
	inline void SetInputs(const Inputs& inputs) { this->inputs = inputs; }
	inline void SetTireDeflections(const WheelSet& deflections) { inputs.tireDeflections = deflections; }

	void UpdateKinematics(const Car* originalCar, Car* workingCar, wxString name);

	inline KinematicOutputs GetOutputs() const { return outputs; }
	inline double GetPitch() const { return inputs.pitch; }
	inline double GetRoll() const { return inputs.roll; }
	inline double GetHeave() const { return inputs.heave; }
	inline double GetRackTravel() const { return inputs.rackTravel; }
	inline Eigen::Vector3d GetCenterOfRotation() const { return inputs.centerOfRotation; }
	inline Eigen::Vector3d::Axis GetFirstEulerRotation() const { return inputs.firstRotation; }
	inline WheelSet GetTireDeflections() const { return inputs.tireDeflections; }
	inline Inputs GetInputs() const { return inputs; }

private:
	Inputs inputs;

	const Car *originalCar;
	Car *workingCar;
	Suspension *localSuspension;

	KinematicOutputs outputs;

	bool SolveCorner(Corner &corner, const Corner &original,
		const Eigen::Vector3d &rotations, const Eigen::Vector3d::Axis &secondRotation, const double& tireDeflection);

	void UpdateOutputs();

	// Kinematic solvers
	static bool SolveForPoint(const Eigen::Vector3d &center1, const Eigen::Vector3d &center2, const Eigen::Vector3d &center3,
		const Eigen::Vector3d &originalCenter1, const Eigen::Vector3d &originalCenter2, const Eigen::Vector3d &originalCenter3,
		const Eigen::Vector3d &original, Eigen::Vector3d &current);
	static bool SolveForPoint(const Corner::Hardpoints &target, const Corner::Hardpoints& reference1,
		const Corner::Hardpoints& reference2, const Corner::Hardpoints& reference3,
		const Corner& originalCorner, Corner& currentCorner);
	static bool SolveForXY(const Corner::Hardpoints &target, const Corner::Hardpoints& reference1,
		const Corner::Hardpoints& reference2, const Corner& originalCorner, Corner& currentCorner);
	static bool SolveForContactPatch(const Eigen::Vector3d &wheelCenter, const Eigen::Vector3d &wheelPlaneNormal,
		const double &tireRadius, Eigen::Vector3d &output);
	static bool SolveInboardTBarPoints(const Eigen::Vector3d &leftOutboard, const Eigen::Vector3d &rightOutboard,
		const Eigen::Vector3d &centerPivot, const Eigen::Vector3d &pivotAxisPoint,
		const Eigen::Vector3d &originalLeftOutboard, const Eigen::Vector3d &originalRightOutboard,
		const Eigen::Vector3d &originalCenterPivot, const Eigen::Vector3d &originalPivotAxisPoint,
		const Eigen::Vector3d &originalLeftInboard, const Eigen::Vector3d &originalRightInboard,
		Eigen::Vector3d &leftInboard, Eigen::Vector3d &rightInboard);

	// Other functions
	void MoveSteeringRack(const double &travel) const;
	void UpdateCGs(const Eigen::Vector3d& cor, const Eigen::Vector3d& angles, const Eigen::Vector3d::Axis& first,
		const Eigen::Vector3d::Axis& second, const double& heave, const WheelSet& tireDeflections, Car* workingCar) const;

	static Eigen::Vector3d FindPerpendicularVector(const Eigen::Vector3d &v);
	static double OptimizeCircleParameter(const Eigen::Vector3d &center, const Eigen::Vector3d &a,
		const Eigen::Vector3d &b, const Eigen::Vector3d &target);

	static bool SolveForXY(const Eigen::Vector3d &center1, const Eigen::Vector3d &center2, const Eigen::Vector3d &originalCenter1,
		const Eigen::Vector3d &originalCenter2, const Eigen::Vector3d &original, Eigen::Vector3d &current);
};

}// namespace VVASE

#endif// KINEMATICS_H_
