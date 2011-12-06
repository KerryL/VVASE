/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  brakes.h
// Created:  11/6/2007
// Author:  K. Loux
// Description:  Contains class declaration for Brakes class (disk brakes only).
// History:
//	3/9/2008	- Changed the structure of the Debugger class, K. Loux.
//	11/22/2009	- Moved to vCar.lib, K. Loux.

#ifndef _BRAKES_H_
#define _BRAKES_H_

// Standard C++ headers
#include <iosfwd>// forward declarations of fstream objects

// vUtilities headers
#include "vUtilities/wheelSetStructures.h"

class Brakes
{
public:
	// Constructor
	Brakes();
	Brakes(const Brakes &brakes);

	// Destructor
	~Brakes();

	// File read/write functions
	void Write(std::ofstream *outFile) const;
	void Read(std::ifstream *inFile, int fileVersion);

	// Get the braking torque at each wheel as a function of pedal force
	WheelSet GetBrakingTorque(const double &pedalForce) const;	// [in-lbf]

	// Class properties
	FrontRearInteger numberOfDisks;
	WheelSet brakeDiameter;						// [in] - This is effective diameter, twice the moment arm
	WheelSet pistonArea;							// [in^2]
	FrontRearDouble masterCylinderArea;			// [in^2]
	double pedalRatio;								// [-]
	FrontRearDouble linePressure;					// [psi]
	double biasRatio;								// [-]			Front/Rear
	double percentFrontBraking;						// [-]
	// FIXME:  Make temperature dependant with thermal and cooling model (include aerodynamics?)

	// Important for determining anti-geometry (see RCVD p. 168)
	bool frontBrakesInboard;
	bool rearBrakesInboard;

	// Overloaded operators
	Brakes& operator = (const Brakes &brakes);
};

#endif// _BRAKES_H_