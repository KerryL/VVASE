/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

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

#ifndef BRAKES_H_
#define BRAKES_H_

// vUtilities headers
#include "vUtilities/wheelSetStructures.h"

// Local forward declarations
class BinaryReader;
class BinaryWriter;

class Brakes
{
public:
	Brakes();
	Brakes(const Brakes &brakes);
	~Brakes();

	// File read/write functions
	void Write(BinaryWriter& file) const;
	void Read(BinaryReader& file, const int& fileVersion);

	// Get the braking torque at each wheel as a function of pedal force
	WheelSet GetBrakingTorque(const double &pedalForce) const;	// [in-lbf]

	// Class properties
	FrontRearInteger numberOfDisks;
	WheelSet brakeDiameter;						// [in] - This is effective diameter, twice the moment arm
	WheelSet pistonArea;						// [in^2]
	FrontRearDouble masterCylinderArea;			// [in^2]
	double pedalRatio;							// [-]
	FrontRearDouble linePressure;				// [psi]
	double biasRatio;							// [-]	Front/Rear
	double percentFrontBraking;					// [-]
	// FIXME:  Make temperature dependant with thermal and cooling model (include aerodynamics?)

	// Important for determining anti-geometry (see RCVD p. 168)
	bool frontBrakesInboard;
	bool rearBrakesInboard;

	Brakes& operator=(const Brakes &brakes);
};

#endif// BRAKES_H_