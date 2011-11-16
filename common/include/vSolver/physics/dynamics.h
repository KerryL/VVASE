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

#ifndef _DYNAMICS_H_
#define _DYNAMICS_H_

// VVASE headers
#include "vSolver/physics/state.h"
#include "vUtilities/wheelSetStructures.h"
#include "vCar/driver.h"

// VVASE forward declarations
class Debugger;
class Integrator;

// The outputs from a dynamic simulation
struct DynamicOutputs
{
	WheelSet axleTwist;// [rad]
	WheelSet tireDeflection;// [in]
	WheelSet complianceToe;// [rad]
	WheelSet complianceCamber;// [rad]
	WheelSet slipAngle;// [rad]
	WheelSet slipRatio;// [-]
};

class Dynamics
{
public:
	// Constructor
	Dynamics(const Debugger &_debugger);
	Dynamics(const Dynamics &dynamics);

	// Destructor
	~Dynamics();

	// Simulation control methods
	void ResetSimulation(void);
	void RunSimulation(void);
	void PauseSimulation(void);
	void StopSimulation(void);

	// An alternative to a simulation is to replay track data
	void ComputeStatesForPlayback(void);

	// Overloaded operators
	Dynamics& operator = (const Dynamics &dynamics);

private:
	// Debugger message printing utility
	const Debugger &debugger;

	// The states and their derivatives
	State state;
	State stateDerivative;

	// The driver inputs
	DRIVER *driver;

	// The integrator
	Integrator *integrator;
	void CalculateStateDerivative(State state, DRIVER::INPUTS inputs);

	// Time history of inputs and outputs (store in a queue or a linked list?)
};

#endif// _DYNAMICS_H_