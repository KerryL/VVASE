/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  drivetrain.h
// Date:  11/3/2007
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains class declaration for Drivetrain class (transmission).

#ifndef DRIVETRAIN_H_
#define DRIVETRAIN_H_

// Local headers
#include "subsystem.h"
#include "VVASE/core/utilities/vvaseString.h"
#include "VVASE/core/car/subsystems/differential.h"

// Standard C++ headers
#include <vector>

namespace VVASE
{

// Local forward declarations
class Differential;
class BinaryReader;
class BinaryWriter;

class Drivetrain : public Subsystem
{
public:
	Drivetrain();

	// File read/write functions
	void Write(BinaryWriter& file) const override;
	void Read(BinaryReader& file, const int& fileVersion) override;

	short gear;

	// Retreives the outputs for this class
	double OutputTorque(const double &inputTorque) const;	// [in-lbf]
	double OutputSpeed(const double &inputSpeed) const;		// [rad/sec]

	// Enumeration that describe the wheels driven by this drivetrain
	enum DriveWheels
	{
		DriveRearWheel,
		DriveFrontWheel,
		DriveAllWheel,

		NumberOfDriveTypes
	};

	static vvaseString GetDriveWheelsName(const DriveWheels &driveWheels);

	// The value describing the wheels driven by this drivetrain
	DriveWheels driveType;

	// Sets the number of gears available with this drivetrain
	void SetNumberOfGears(const short &numGears);

	DriveWheels GetDriveWheels() const { return driveType; }
	void SetAllWheelDrive(const double& rearBias, const double& midBias, const double& frontBias);
	void SetFrontWheelDrive(const double& bias);
	void SetRearWheelDrive(const double& bias);

    // Required by RegisterableComponent
    static std::unique_ptr<Subsystem> Create() { return std::make_unique<Drivetrain>(); }
    static vvaseString GetName() { return _T("Drivetrain"); }

    // Required by Subsystem
    wxPanel* GetEditPanel() override;
	wxTreeListItem* GetTreeItem() override;

private:
	std::unique_ptr<Differential> rearDifferential;
    std::unique_ptr<Differential> midDifferential;
    std::unique_ptr<Differential> frontDifferential;

	static void WriteDifferential(BinaryWriter& file, const Differential* differential);
	static void ReadDifferential(BinaryReader& file, std::unique_ptr<Differential>& differential, const int& fileVersion);

	// Array of gear ratios for each gear (not including final
	// drive - that is in the Differential object)
	std::vector<double> gearRatios;// [-]

	// Clutch stuff in here, too?
};

}// namespace VVASE

#endif// DRIVETRAIN_H_
