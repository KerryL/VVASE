/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  brakes.h
// Created:  11/6/2007
// Author:  K. Loux
// Description:  Contains class declaration for BRAKES class (disk brakes only).
// History:
//	3/9/2008	- Changed the structure of the Debugger class, K. Loux.
//	11/22/2009	- Moved to vCar.lib, K. Loux.

#ifndef _BRAKES_H_
#define _BRAKES_H_

// Standard C++ headers
#include <iosfwd>// forward declarations of fstream objects

// vUtilities headers
#include "vUtilities/wheelSetStructures.h"

// CarDesigner forward declarations
class Debugger;

class BRAKES
{
public:
	// Constructor
	BRAKES(const Debugger &_debugger);
	BRAKES(const BRAKES &Brakes);

	// Destructor
	~BRAKES();

	// File read/write functions
	void Write(std::ofstream *OutFile) const;
	void Read(std::ifstream *InFile, int FileVersion);

	// Get the braking torque at each wheel as a function of pedal force
	WheelSet GetBrakingTorque(const double &PedalForce) const;	// [in-lbf]

	// Class properties
	FrontRearInteger NumberOfDisks;
	WheelSet BrakeDiameter;						// [in] - This is effective diameter, twice the moment arm
	WheelSet PistonArea;							// [in^2]
	FrontRearDouble MasterCylinderArea;			// [in^2]
	double PedalRatio;								// [-]
	FrontRearDouble LinePressure;					// [psi]
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
	const Debugger &debugger;
};

#endif// _BRAKES_H_