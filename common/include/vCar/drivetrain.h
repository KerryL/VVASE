/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  drivetrain.h
// Created:  11/3/2007
// Author:  K. Loux
// Description:  Contains class declaration for DRIVETRAIN class (transmission).
// History:
//	2/24/2008	- Moved half shaft points from here into SUSPENSION object, K. Loux.
//	2/25/2008	- Named DRIVE_WHEELS enum, K. Loux.
//	3/9/2008	- Moved enumerations inside class structure and changed the structure of the
//				  Debugger class, K. Loux.
//	11/22/2009	- Moved to vCar.lib, K. Loux.

#ifndef _DRIVETRAIN_CLASS_H_
#define _DRIVETRAIN_CLASS_H_

// Standard C++ headers
#include <iosfwd>// forward declarations of fstream objects

// wxWidgets headers
#include <wx/wx.h>

// vUtilities forward declarations
class Debugger;

// vCar forward declarations
class DIFFERENTIAL;

class DRIVETRAIN
{
public:
	// Constructor
	DRIVETRAIN(const Debugger &_debugger);
	DRIVETRAIN(const DRIVETRAIN &Drivetrain);

	// Destructor
	~DRIVETRAIN();

	// File read/write functions
	void Write(std::ofstream *OutFile) const;
	void Read(std::ifstream *InFile, int FileVersion);

	// The selected gear
	short Gear;

	// Retreives the outputs for this class
	double OutputTorque(const double &InputTorque) const;	// [in-lbf]
	double OutputSpeed(const double &InputSpeed) const;		// [rad/sec]

	// Enumeration that describe the wheels driven by this drivetrain
	enum DRIVE_WHEELS
	{
		DRIVE_REAR_WHEEL,
		DRIVE_FRONT_WHEEL,
		DRIVE_ALL_WHEEL,

		NumberOfDriveTypes
	};

	static wxString GetDriveWheelsName(const DRIVE_WHEELS &_DriveWheels);

	// The value describing the wheels driven by this drivetrain
	DRIVE_WHEELS DriveType;

	// Sets the number of gears available with this drivetrain
	void SetNumberOfGears(const short &NumGears);

	// Operators
	DRIVETRAIN& operator = (const DRIVETRAIN &Drivetrain);

private:
	// Debugger message printing utility
	const Debugger &debugger;

	// The differential for this drivetrain
	DIFFERENTIAL *Differential;

	// The number of gears available
	short NumberOfGears;

	// The inertia for the rotating components in the drivetrain
	double TransmissionInertia;					// [slug-ft^2] (w.r.t. high speed side)

	// Array of gear ratios for each gear (not including final
	// drive - that is in the DIFFERENTIAL object)
	double *GearRatio;							// [-]

	// Clutch stuff in here, too?
};

#endif// _DRIVETRAIN_CLASS_H_