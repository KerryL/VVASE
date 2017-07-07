/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  kinematicOutputs.h
// Date:  3/23/2008
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains class declaration for KinematicOutputs class.  This class does the
//        calculations for all of the kinematic simulation outputs.  That includes any
//        kind of wheel angle/orientation, chassis attitude, spring/damper positions, but
//        doesn't include any thing that requires forces to calculate (force-based roll
//        center, etc.).

#ifndef KINEMATIC_OUTPUTS_H_
#define KINEMATIC_OUTPUTS_H_

// Local headers
#include "VVASE/core/utilities/unitType.h"
// TODO:  Have input/output objects that are strongly typed?  Also interface to undo/redo?

// TODO:  Do I really need these here?
#include "VVASE/core/car/subsystems/corner.h"// TODO:  Need to move this to include/VVASE or users won't be able to include for making plugins
#include "VVASE/core/car/subsystems/suspension.h"

// Eigen headers
#include <Eigen/Eigen>

// wxWidgets headers
#include <wx/thread.h>

namespace VVASE
{

// Local forward declarations
class Car;

class KinematicOutputs
{
public:
	KinematicOutputs();

	// Updates the kinematic variables associated with the current Suspension
	void Update(const Car *original, const Suspension *current);

	// Enumeration for double outputs that get computed for every corner
	enum CornerOutputsDouble
	{
		Caster,						// [rad]
		Camber,						// [rad]
		KPI,						// [rad]
		Steer,						// [rad]
		Spring,						// [in]
		Damper,						// [in]
		AxlePlunge,					// [in]
		CasterTrail,				// [in]
		ScrubRadius,				// [in]
		Scrub,						// [in]
		SpringInstallationRatio,	// [in Spring/in Wheel]
		DamperInstallationRatio,	// [in Damper/in Wheel]
		ARBInstallationRatio,		// [rad Bar/in Wheel] (assumes opposite side stays fixed)
		SpindleLength,				// [in]
		SideViewSwingArmLength,		// [in]
		FrontViewSwingArmLength,	// [in]
		AntiBrakePitch,				// [%] : anti-dive in the front, anti-lift in the rear
		AntiDrivePitch,				// [%] : anti-lift in the front, anti-squat in the rear

		NumberOfCornerOutputDoubles
	};

	// Enumeration for Eigen::Vector3d outputs that get computed for every corner
	enum CornerOutputsVector
	{
		InstantCenter,			// [in]
		InstantAxisDirection,	// [-]

		NumberOfCornerOutputVectors
	};

	// Enumeration for double outputs that only get computed once per car
	enum OutputsDouble
	{
		FrontARBTwist,				// [rad]
		RearARBTwist,				// [rad]
		FrontThirdSpring,			// [in]
		FrontThirdDamper,			// [in]
		RearThirdSpring,			// [in]
		RearThirdDamper,			// [in]
		FrontNetSteer,				// [rad]
		RearNetSteer,				// [rad]
		FrontNetScrub,				// [in]
		RearNetScrub,				// [in]
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

	// Enumeration for Eigen::Vector3d outputs that only get computed once per car
	enum OutputsVector
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
	double doubles[NumberOfOutputDoubles];
	Eigen::Vector3d vectors[NumberOfOutputVectors];

	// The outputs that are associated with just one corner of the car
	double rightFront[NumberOfCornerOutputDoubles];
	double leftFront[NumberOfCornerOutputDoubles];
	double rightRear[NumberOfCornerOutputDoubles];
	double leftRear[NumberOfCornerOutputDoubles];

	Eigen::Vector3d rightFrontVectors[NumberOfCornerOutputVectors];
	Eigen::Vector3d leftFrontVectors[NumberOfCornerOutputVectors];
	Eigen::Vector3d rightRearVectors[NumberOfCornerOutputVectors];
	Eigen::Vector3d leftRearVectors[NumberOfCornerOutputVectors];

	// Enumeration that encompasses all of the outputs for the whole car
	// This makes referencing these outputs from other classes a little easier
	enum OutputsComplete
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

	enum class VectorComponent : int
	{
		X = 0,
		Y,
		Z
	};

	// For converting from an output + location to OutputsComplete
	static OutputsComplete OutputsCompleteIndex(const Corner::Location &location,
		const CornerOutputsDouble &cornerDouble, const CornerOutputsVector &cornerVector,
		const OutputsDouble &midDouble, const OutputsVector &vector, const VectorComponent& component);

	// For accessing an output via the OutputsComplete list
	double GetOutputValue(const OutputsComplete &output) const;

	// For determining unit type of the outputs
	static UnitType GetOutputUnitType(const OutputsComplete &output);

	// For determining the name of an output from the OutputsComplete list
	static wxString GetOutputName(const OutputsComplete &output);

private:
	const Car *originalCar;
	const Suspension *currentSuspension;

	// Updates the outputs associated with the associated corner
	void UpdateCorner(const Corner *originalCorner, const Corner *currentCorner);

	// For retrieving names of the outputs
	static wxString GetCornerDoubleName(const CornerOutputsDouble &output);
	static wxString GetCornerVectorName(const CornerOutputsVector &output);
	static wxString GetDoubleName(const OutputsDouble &output);
	static wxString GetVectorName(const OutputsVector &output);

	// For retrieving units of the outputs
	static UnitType GetCornerDoubleUnitType(const CornerOutputsDouble &output);
	static UnitType GetCornerVectorUnitType(const CornerOutputsVector &output);
	static UnitType GetDoubleUnitType(const OutputsDouble &output);
	static UnitType GetVectorUnitType(const OutputsVector &output);

	void InitializeAllOutputs();

	void ComputeNetSteer();
	void ComputeNetScrub();
	void ComputeFrontARBTwist(const Car *original, const Suspension *current);
	void ComputeRearARBTwist(const Car *original, const Suspension *current);
	void ComputeTrack(const Suspension *current);
	void ComputeWheelbase(const Suspension *current);
	void ComputeFrontRollCenter(const Suspension *current);
	void ComputeRearRollCenter(const Suspension *current);
	void ComputeLeftPitchCenter(const Suspension *current);
	void ComputeRightPitchCenter(const Suspension *current);

	double ComputeARBTwist(const Corner& originalLeft, const Corner& originalRight,
		const Corner& currentLeft, const Corner& currentRight,
		const Suspension::BarStyle &barStyle, const Eigen::Vector3d& originalMidPoint,
		const Eigen::Vector3d& originalPivot, const Eigen::Vector3d& currentMidPoint,
		const Eigen::Vector3d& currentPivot, const bool& signGreaterThan) const;
	double ComputeUBarTwist(const Corner& originalLeft, const Corner& originalRight,
		const Corner& currentLeft, const Corner& currentRight, const bool& signGreaterThan) const;
	double ComputeTBarTwist(const Corner& originalLeft, const Corner& originalRight,
		const Corner& currentLeft, const Corner& currentRight,
		const Eigen::Vector3d& originalMidPoint, const Eigen::Vector3d& originalPivot,
		const Eigen::Vector3d& currentMidPoint, const Eigen::Vector3d& currentPivot, const bool& signGreaterThan) const;
	double ComputeGearedBarTwist(const Corner& originalLeft, const Corner& originalRight,
		const Corner& currentLeft, const Corner& currentRight, const bool& signGreaterThan) const;

	bool ComputeKinematicCenter(const Corner &corner1, const Corner &corner2,
		const Eigen::Vector3d *cornerVectors1, const Eigen::Vector3d *cornerVectors2,
		const Eigen::Vector3d &planeNormal, Eigen::Vector3d &center, Eigen::Vector3d &direction) const;

	void ComputeCaster(const Corner &corner, double *cornerDoubles);
	void ComputeKingPinInclincation(const Corner &corner, const short &sign, double *cornerDoubles);
	void ComputeCasterTrail(const Corner &corner, double *cornerDoubles);
	void ComputeScrubRadius(const Corner &corner, const short &sign, double *cornerDoubles);
	void ComputeSpindleLength(const Corner &corner, const short &sign, double *cornerDoubles);
	void ComputeCamberAndSteer(const Corner &originalCorner, const Corner &currentCorner,
		const short &sign, double *cornerDoubles);
	void ComputeSpringDisplacement(const Corner &originalCorner,
		const Corner &currentCorner, double *cornerDoubles);
	void ComputeDamperDisplacement(const Corner &originalCorner,
		const Corner &currentCorner, double *cornerDoubles);
	void ComputeScrub(const Corner &originalCorner, const Corner &currentCorner,
		const short &sign, double *cornerDoubles);
	void ComputeAxlePlunge(const Corner &originalCorner,
		const Corner &currentCorner, double *cornerDoubles);
};

}// namespace VVASE

#endif// KINEMATIC_OUTPUTS_H_
