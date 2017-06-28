/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016
===================================================================================*/

// File:  drivetrain.h
// Date:  11/3/2007
// Author:  K. Loux
// Desc:  Contains class declaration for Drivetrain class (transmission).

#ifndef DRIVETRAIN_H_
#define DRIVETRAIN_H_

// Local headers
#include "subsystem.h"

// Standard C++ headers
#include <vector>

// wxWidgets headers
#include <wx/wx.h>

namespace VVASE
{

// Local forward declarations
class Differential;
class BinaryReader;
class BinaryWriter;

class Drivetrain : public Subsystem
{
public:
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

	static wxString GetDriveWheelsName(const DriveWheels &driveWheels);

	// The value describing the wheels driven by this drivetrain
	DriveWheels driveType;

	// Sets the number of gears available with this drivetrain
	void SetNumberOfGears(const short &numGears);

	DriveWheels GetDriveWheels() const { return driveType; }
	void SetAllWheelDrive(const double& rearBias, const double& midBias, const double& frontBias);
	void SetFrontWheelDrive(const double& bias);
	void SetRearWheelDrive(const double& bias);

	std::vector<double> GetBiasRatios() const;

    // Required by RegisterableComponent
    static std::unique_ptr<Drivetrain> Create() { return std::make_unique<Drivetrain>(); }
    static std::string GetName() { return _T("Drivetrain"); }

private:
	std::unique_ptr<Differential> rearDifferential;
    std::unique_ptr<Differential> midDifferential;
    std::unique_ptr<Differential> frontDifferential;

	// Array of gear ratios for each gear (not including final
	// drive - that is in the Differential object)
	std::vector<double> gearRatios;// [-]

	// Clutch stuff in here, too?
};

}// namespace VVASE

#endif// DRIVETRAIN_H_
