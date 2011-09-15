/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  dynamics_class.h
// Created:  11/3/2007
// Author:  K. Loux
// Description:  Contains class declaration for DYNAMIC_SIMULATION class.
// History:
//	3/9/2008	- Changed the structure of the DEBUGGER class, K. Loux.

#ifndef _DYNAMICS_CLASS_H_
#define _DYNAMICS_CLASS_H_

// VVASE headers
#include "vSolver/physics/state_class.h"
#include "vUtilities/wheelset_structs.h"
#include "vCar/driver_class.h"

// VVASE forward declarations
class DEBUGGER;
class DRIVER;
class INTEGRATOR;

// The outputs from a dynamic simulation
struct DYNAMIC_OUTPUTS
{
	WHEEL_SET AxleTwist;// [rad]
	WHEEL_SET TireDeflection;// [in]
	WHEEL_SET ComplianceToe;// [rad]
	WHEEL_SET ComplianceCamber;// [rad]
	WHEEL_SET SlipAngle;// [rad]
	WHEEL_SET SlipRatio;// [-]
};

class DYNAMICS
{
public:
	// Constructor
	DYNAMICS(const DEBUGGER &_Debugger);
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
	const DEBUGGER &Debugger;

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