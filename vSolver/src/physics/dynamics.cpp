/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  dynamics_class.cpp
// Created:  11/3/2007
// Author:  K. Loux
// Description:  Contains class functionality for dynamic state class.
// History:
//	3/9/2008	- Changed the structure of the Debugger class, K. Loux.

// VVASE headers
#include "vSolver/physics/dynamics.h"
#include "vSolver/physics/integrator.h"
#include "vUtilities/debugger.h"
#include "vMath/vector.h"

// wxWidgets headers
#include <wx/stopwatch.h>
#include <wx/utils.h>

//==========================================================================
// Class:			DYNAMICS
// Function:		DYNAMICS
//
// Description:		Constructor for the DYNAMICS class.
//
// Input Arguments:
//		_debugger	= const Debugger&, reference to the debug message printing utility
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
DYNAMICS::DYNAMICS(const Debugger &_debugger) : debugger(_debugger)
{
	// Create the integrator object
	Integrator = new INTEGRATOR(INTEGRATOR::INT_ADAMS_BASHFORTH_3, 100.0, debugger);

	// Create the driver
	Driver = new DRIVER(debugger);
}

//==========================================================================
// Class:			DYNAMICS
// Function:		DYNAMICS
//
// Description:		Copy constructor for the DYNAMICS class.
//
// Input Arguments:
//		Dynamics	= const DYNAMICS& to copy to this object
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
DYNAMICS::DYNAMICS(const DYNAMICS &Dynamics) : debugger(Dynamics.debugger)
{
	// Initialize the pointers
	Driver = NULL;
	Integrator = NULL;

	// Do the copy
	*this = Dynamics;
}

//==========================================================================
// Class:			DYNAMICS
// Function:		~DYNAMICS
//
// Description:		Destructor for the DYNAMICS class.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
DYNAMICS::~DYNAMICS()
{
	// Delete the integrator object
	delete Integrator;
	Integrator = NULL;

	// Delete the driver object
	delete Driver;
	Driver = NULL;
}

//==========================================================================
// Class:			DYNAMICS
// Function:		ResetSimulation
//
// Description:		Resets the simulation to zero initial conditions
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void DYNAMICS::ResetSimulation(void)
{
	return;
}

//==========================================================================
// Class:			DYNAMICS
// Function:		CalculateStateDerivatives
//
// Description:		Calculates the state derivatives for this simulation.
//
// Input Arguments:
//		State	= STATE, current state of the vehicle
//		Inputs	= DRIVER::INPUTS representing the commands from the driver
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void DYNAMICS::CalculateStateDerivative(STATE State, DRIVER::INPUTS Inputs)
{
	return;
}

//==========================================================================
// Class:			DYNAMICS
// Function:		RunSimulation
//
// Description:		Runs the current simulation.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void DYNAMICS::RunSimulation(void)
{
	// Initialize the simulation (initial conditions, environment, etc.)

	// Choose and enter the correct run loop
	wxStopWatch Timer;
	while (1/*SIMULATION_HAS_NOT_ENDED*/ && 1/*REAL_TIME_LOOP*/)
	{
		// Start loop timer
		Timer.Start(0);

		// Get inputs
		Driver->CalculateInputs();

		// Calculate the next physics step
		State = Integrator->ComputeNewState();

		// Store the inputs and outputs in the time history queues

		// Stop the timer and calculate the elapsed time.  Sleep for the correct amount of time
		// to meet the deisred running frequency
		wxMilliSleep(1000/*DESIRED_TIME_STEP_IN_MSEC*/ - Timer.Time());
	}

	while (1/*SIMULATION_HAS_NOT_ENDED*/ && 1/*CANNED_RUN_LOOP*/)
	{
		// Get/calculate the inputs
		Driver->CalculateInputs();

		// Calculate the next physics step
		State = Integrator->ComputeNewState();

		// Store the inputs and outputs in the time history queues
	}

	return;
}

//==========================================================================
// Class:			DYNAMICS
// Function:		operator =
//
// Description:		Assignment operator for DYNAMICS class.
//
// Input Arguments:
//		Dynamics	= const DYNAMICS& to assign to this object
//
// Output Arguments:
//		None
//
// Return Value:
//		DYNAMICS&, reference to this object
//
//==========================================================================
DYNAMICS& DYNAMICS::operator = (const DYNAMICS &Dynamics)
{
	// Check for self-assignment
	if (this == &Dynamics)
		return *this;

	// Delete existing class members
	delete Driver;
	delete Integrator;

	// Initialize the pointers
	Driver = NULL;
	Integrator = NULL;

	// Perform the assignment
	State			= Dynamics.State;
	StateDerivative	= Dynamics.StateDerivative;

	Driver			= new DRIVER(*Dynamics.Driver);
	Integrator		= new INTEGRATOR(*Dynamics.Integrator);

	return *this;
}