/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  driver_class.h
// Created:  11/4/2007
// Author:  K. Loux
// Description:  Contains class declaration for DRIVER class.
// History:
//	3/9/2008	- Changed the structure of the DEBUGGER class, K. Loux.
//	11/22/2009	- Moved to vCar.lib, K. Loux.

#ifndef _DRIVER_CLASS_H_
#define _DRIVER_CLASS_H_

// vUtilities forward declarations
class DEBUGGER;

class DRIVER
{
public:
	// Constructor
	DRIVER(const DEBUGGER &_Debugger);
	DRIVER(const DRIVER &Driver);

	// Destructor
	~DRIVER();

	// The driver input structure
	struct INPUTS
	{
		double Throttle;// [%]
		double BrakePedalForce;// [lbf]
		double SteeringWheelAngle;// [rad]

		short Gear;// [-]
	};

	// Issues the command to calculate the driver inputs
	void CalculateInputs(void);

	// Retreives the calculated inputs
	INPUTS GetInputs(void) { return DriverInputs; };

	// Overloaded operators
	DRIVER& operator = (const DRIVER &Driver);

private:
	// Debugger message printing utility
	const DEBUGGER &Debugger;

	// The driver's inputs
	INPUTS DriverInputs;
};

#endif// _DRIVER_CLASS_H_