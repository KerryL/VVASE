/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  drivetrain.h
// Created:  11/3/2007
// Author:  K. Loux
// Description:  Contains class declaration for Drivetrain class (transmission).
// History:
//	2/24/2008	- Moved half shaft points from here into Suspension object, K. Loux.
//	2/25/2008	- Named DriveWheels enum, K. Loux.
//	3/9/2008	- Moved enumerations inside class structure and changed the structure of the
//				  Debugger class, K. Loux.
//	11/22/2009	- Moved to vCar.lib, K. Loux.

#ifndef DRIVETRAIN_H_
#define DRIVETRAIN_H_

// Standard C++ headers
#include <vector>

// wxWidgets headers
#include <wx/wx.h>

// Local forward declarations
class Differential;
class BinaryReader;
class BinaryWriter;

class Drivetrain
{
public:
	Drivetrain();
	Drivetrain(const Drivetrain &drivetrain);
	~Drivetrain();

	// File read/write functions
	void Write(BinaryWriter& file) const;
	void Read(BinaryReader& file, const int& fileVersion);

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

	Drivetrain& operator=(const Drivetrain &drivetrain);

	DriveWheels GetDriveWheels() const { return driveType; }
	void SetAllWheelDrive(const double& rearBias, const double& midBias, const double& frontBias);
	void SetFrontWheelDrive(const double& bias);
	void SetRearWheelDrive(const double& bias);

	std::vector<double> GetBiasRatios() const;

private:
	std::vector<Differential*> differentials;
	// Should either have one or three diffs
	// In case of three diffs, first one is rear, second is mid, third is front

	// Array of gear ratios for each gear (not including final
	// drive - that is in the Differential object)
	std::vector<double> gearRatios;// [-]

	void DeleteDifferentials();

	// Clutch stuff in here, too?
};

#endif// DRIVETRAIN_H_