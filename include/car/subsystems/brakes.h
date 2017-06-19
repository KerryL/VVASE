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

#ifndef BRAKES_H_
#define BRAKES_H_

// Local headers
#include "utilities/wheelSetStructures.h"

// Local forward declarations
class BinaryReader;
class BinaryWriter;

class Brakes : public Subsystem
{
public:
	// File read/write functions
	void Write(BinaryWriter& file) const override;
	void Read(BinaryReader& file, const int& fileVersion) override;

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

	// Required by RegisterableComponent
    static std::unique_ptr<Brakes> Create() { return std::make_unique<Brakes>(); }
    static std::string GetName() { return _T("Brake"); }
};

#endif// BRAKES_H_