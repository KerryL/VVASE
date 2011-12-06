/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

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

#ifndef _DRIVETRAIN_H_
#define _DRIVETRAIN_H_

// Standard C++ headers
#include <iosfwd>// forward declarations of fstream objects

// wxWidgets headers
#include <wx/wx.h>

// Local forward declarations
class Differential;

class Drivetrain
{
public:
	// Constructor
	Drivetrain();
	Drivetrain(const Drivetrain &drivetrain);

	// Destructor
	~Drivetrain();

	// File read/write functions
	void Write(std::ofstream *outFile) const;
	void Read(std::ifstream *inFile, int fileVersion);

	// The selected gear
	short Gear;

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

	static wxString GetDriveWheelsName(const DriveWheels &_driveWheels);

	// The value describing the wheels driven by this drivetrain
	DriveWheels driveType;

	// Sets the number of gears available with this drivetrain
	void SetNumberOfGears(const short &numGears);

	// Operators
	Drivetrain& operator = (const Drivetrain &drivetrain);

private:
	// The differential for this drivetrain
	Differential *differential;

	// The number of gears available
	short numberOfGears;

	// The inertia for the rotating components in the drivetrain
	double transmissionInertia;					// [slug-ft^2] (w.r.t. high speed side)

	// Array of gear ratios for each gear (not including final
	// drive - that is in the Differential object)
	double *gearRatio;							// [-]

	// Clutch stuff in here, too?
};

#endif// _DRIVETRAIN_H_