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

#ifndef DYNAMICS_H_
#define DYNAMICS_H_

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
	Dynamics(const Debugger &debugger);
	Dynamics(const Dynamics &dynamics);
	~Dynamics();

	// Simulation control methods
	void ResetSimulation();
	void RunSimulation();
	void PauseSimulation();
	void StopSimulation();

	// An alternative to a simulation is to replay track data
	void ComputeStatesForPlayback();

	Dynamics& operator=(const Dynamics &dynamics);

private:
	const Debugger &debugger;

	// The states and their derivatives
	State state;
	State stateDerivative;

	// The driver inputs
	Driver *driver;

	// The integrator
	Integrator *integrator;
	void CalculateStateDerivative(State state, Driver::Inputs inputs);

	// Time history of inputs and outputs (store in a queue or a linked list?)
};

#endif// DYNAMICS_H_