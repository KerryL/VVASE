/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  dynamics.cpp
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
// Class:			Dynamics
// Function:		Dynamics
//
// Description:		Constructor for the Dynamics class.
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
Dynamics::Dynamics(const Debugger &_debugger) : debugger(_debugger)
{
	// Create the integrator object
	integrator = new Integrator(Integrator::MethodAdamsBashforth3, 100.0, debugger);

	// Create the driver
	driver = new DRIVER(debugger);
}

//==========================================================================
// Class:			Dynamics
// Function:		Dynamics
//
// Description:		Copy constructor for the Dynamics class.
//
// Input Arguments:
//		dynamics	= const Dynamics& to copy to this object
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Dynamics::Dynamics(const Dynamics &dynamics) : debugger(dynamics.debugger)
{
	// Initialize the pointers
	driver = NULL;
	integrator = NULL;

	// Do the copy
	*this = dynamics;
}

//==========================================================================
// Class:			Dynamics
// Function:		~Dynamics
//
// Description:		Destructor for the Dynamics class.
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
Dynamics::~Dynamics()
{
	// Delete the integrator object
	delete integrator;
	integrator = NULL;

	// Delete the driver object
	delete driver;
	driver = NULL;
}

//==========================================================================
// Class:			Dynamics
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
void Dynamics::ResetSimulation(void)
{
}

//==========================================================================
// Class:			Dynamics
// Function:		CalculateStateDerivatives
//
// Description:		Calculates the state derivatives for this simulation.
//
// Input Arguments:
//		state	= State, current state of the vehicle
//		inputs	= DRIVER::INPUTS representing the commands from the driver
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Dynamics::CalculateStateDerivative(State state, DRIVER::INPUTS inputs)
{
}

//==========================================================================
// Class:			Dynamics
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
void Dynamics::RunSimulation(void)
{
	// Initialize the simulation (initial conditions, environment, etc.)

	// Choose and enter the correct run loop
	wxStopWatch timer;
	while (1/*SIMULATION_HAS_NOT_ENDED*/ && 1/*REAL_TIME_LOOP*/)
	{
		// Start loop timer
		timer.Start(0);

		// Get inputs
		driver->CalculateInputs();

		// Calculate the next physics step
		state = integrator->ComputeNewState();

		// Store the inputs and outputs in the time history queues

		// Stop the timer and calculate the elapsed time.  Sleep for the correct amount of time
		// to meet the desired running frequency
		wxMilliSleep(1000/*DESIRED_TIME_STEP_IN_MSEC*/ - timer.Time());
	}

	while (1/*SIMULATION_HAS_NOT_ENDED*/ && 1/*CANNED_RUN_LOOP*/)
	{
		// Get/calculate the inputs
		driver->CalculateInputs();

		// Calculate the next physics step
		state = integrator->ComputeNewState();

		// Store the inputs and outputs in the time history queues
	}

	return;
}

//==========================================================================
// Class:			Dynamics
// Function:		operator =
//
// Description:		Assignment operator for Dynamics class.
//
// Input Arguments:
//		dynamics	= const Dynamics& to assign to this object
//
// Output Arguments:
//		None
//
// Return Value:
//		Dynamics&, reference to this object
//
//==========================================================================
Dynamics& Dynamics::operator = (const Dynamics &dynamics)
{
	// Check for self-assignment
	if (this == &dynamics)
		return *this;

	// Delete existing class members
	delete driver;
	delete integrator;

	// Initialize the pointers
	driver = NULL;
	integrator = NULL;

	// Perform the assignment
	state			= dynamics.state;
	stateDerivative	= dynamics.stateDerivative;

	driver			= new DRIVER(*dynamics.driver);
	integrator		= new Integrator(*dynamics.integrator);

	return *this;
}