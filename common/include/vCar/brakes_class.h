/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  brakes_class.h
// Created:  11/6/2007
// Author:  K. Loux
// Description:  Contains class declaration for BRAKES class (disk brakes only).
// History:
//	3/9/2008	- Changed the structure of the DEBUGGER class, K. Loux.
//	11/22/2009	- Moved to vCar.lib, K. Loux.

#ifndef _BRAKES_CLASS_H_
#define _BRAKES_CLASS_H_

// Standard C++ headers
#include <iosfwd>// forward declarations of fstream objects

// vUtilities headers
#include "vUtilities/wheelset_structs.h"

// CarDesigner forward declarations
class DEBUGGER;

class BRAKES
{
public:
	// Constructor
	BRAKES(const DEBUGGER &_Debugger);
	BRAKES(const BRAKES &Brakes);

	// Destructor
	~BRAKES();

	// File read/write functions
	void Write(std::ofstream *OutFile) const;
	void Read(std::ifstream *InFile, int FileVersion);

	// Get the braking torque at each wheel as a function of pedal force
	WHEEL_SET GetBrakingTorque(const double &PedalForce) const;	// [in-lbf]

	// Class properties
	FRONT_REAR_INTEGER NumberOfDisks;
	WHEEL_SET BrakeDiameter;						// [in] - This is effective diameter, twice the moment arm
	WHEEL_SET PistonArea;							// [in^2]
	FRONT_REAR_DOUBLE MasterCylinderArea;			// [in^2]
	double PedalRatio;								// [-]
	FRONT_REAR_DOUBLE LinePressure;					// [psi]
	double BiasRatio;								// [-]			Front/Rear
	double PercentFrontBraking;						// [-]
	// FIXME:  Make temperature dependant with thermal and cooling model (include aerodynamics?)

	// Important for determining anti-geometry (see RCVD p. 168)
	bool FrontBrakesInboard;
	bool RearBrakesInboard;

	// Overloaded operators
	BRAKES& operator = (const BRAKES &Brakes);

private:
	// Debugger message printing utility
	const DEBUGGER &Debugger;
};

#endif// _BRAKES_CLASS_H_