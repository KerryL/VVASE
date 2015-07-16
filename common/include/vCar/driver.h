/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  driver.h
// Created:  11/4/2007
// Author:  K. Loux
// Description:  Contains class declaration for DRIVER class.
// History:
//	3/9/2008	- Changed the structure of the Debugger class, K. Loux.
//	11/22/2009	- Moved to vCar.lib, K. Loux.

#ifndef DRIVER_H_
#define DRIVER_H_

// vUtilities forward declarations
class Debugger;

class Driver
{
public:
	Driver(const Debugger &debugger);
	Driver(const Driver &driver);
	~Driver();

	// The driver input structure
	struct Inputs
	{
		double Throttle;// [%]
		double BrakePedalForce;// [lbf]
		double SteeringWheelAngle;// [rad]

		short Gear;// [-]
	};

	// Issues the command to calculate the driver inputs
	void CalculateInputs();

	// Retreives the calculated inputs
	Inputs GetInputs() { return driverInputs; };

	Driver& operator=(const Driver &driver);

private:
	const Debugger &debugger;

	Inputs driverInputs;
};

#endif// DRIVER_H_