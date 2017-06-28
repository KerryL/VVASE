/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  driver.h
// Date:  11/4/2007
// Author:  K. Loux
// Desc:  Contains class declaration for DRIVER class.

#ifndef DRIVER_H_
#define DRIVER_H_

namespace VVASE
{

// Local forward declarations
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

}// namespace VVASE

#endif// DRIVER_H_
