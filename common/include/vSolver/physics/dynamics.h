/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  dynamics.h
// Created:  11/3/2007
// Author:  K. Loux
// Description:  Contains class declaration for DYNAMIC_SIMULATION class.
// History:
//	3/9/2008	- Changed the structure of the Debugger class, K. Loux.

#ifndef _DYNAMICS_CLASS_H_
#define _DYNAMICS_CLASS_H_

// VVASE headers
#include "vSolver/physics/state.h"
#include "vUtilities/wheelSetStructuress.h"
#include "vCar/driver.h"

// VVASE forward declarations
class Debugger;
class DRIVER;
class INTEGRATOR;

// The outputs from a dynamic simulation
struct DYNAMIC_OUTPUTS
{
	WheelSet AxleTwist;// [rad]
	WheelSet TireDeflection;// [in]
	WheelSet ComplianceToe;// [rad]
	WheelSet ComplianceCamber;// [rad]
	WheelSet SlipAngle;// [rad]
	WheelSet SlipRatio;// [-]
};

class DYNAMICS
{
public:
	// Constructor
	DYNAMICS(const Debugger &_debugger);
	DYNAMICS(const DYNAMICS &Dynamics);

	// Destructor
	~DYNAMICS();

	// Simulation control methods
	void ResetSimulation(void);
	void RunSimulation(void);
	void PauseSimulation(void);
	void StopSimulation(void);

	// An alternative to a simulation is to replay track data
	void ComputeStatesForPlayback(void);

	// Overloaded operators
	DYNAMICS& operator = (const DYNAMICS &Dynamics);

private:
	// Debugger message printing utility
	const Debugger &debugger;

	// The states and their derivatives
	STATE State;
	STATE StateDerivative;

	// The driver inputs
	DRIVER *Driver;

	// The integrator
	INTEGRATOR *Integrator;
	void CalculateStateDerivative(STATE State, DRIVER::INPUTS Inputs);

	// Time history of inputs and outputs (store in a queue or a linked list?)
};

#endif// _DYNAMICS_CLASS_H_