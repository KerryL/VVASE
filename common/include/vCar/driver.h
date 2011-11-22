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

#ifndef _DRIVER_H_
#define _DRIVER_H_

// vUtilities forward declarations
class Debugger;

class Driver
{
public:
	// Constructor
	Driver(const Debugger &_debugger);
	Driver(const Driver &driver);

	// Destructor
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
	void CalculateInputs(void);

	// Retreives the calculated inputs
	Inputs GetInputs(void) { return driverInputs; };

	// Overloaded operators
	Driver& operator = (const Driver &driver);

private:
	// Debugger message printing utility
	const Debugger &debugger;

	// The driver's inputs
	Inputs driverInputs;
};

#endif// _DRIVER_H_