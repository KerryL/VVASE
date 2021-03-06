/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  integrator.cpp
// Created:  7/12/2008
// Author:  K. Loux
// Description:  Contains class definition for Integrator class.
// History:

// wxWidgets headers
#include <wx/wx.h>

// CarDesigner headers
#include "vSolver/physics/integrator.h"

//==========================================================================
// Class:			Integrator
// Function:		Integrator
//
// Description:		Constructor for the Integrator class.  Sets all of the
//					parameters that are required for integration to begin.
//
// Input Arguments:
//		integrationMethod	= const IntegrationMethod& to use for future integrations
//		frequency			= const Frequency& at which the integration will occur [Hz]
//		debugger			= const Debugger&, reference to debug message printing utility
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Integrator::Integrator(const IntegrationMethod &integrationMethod,
	const double &frequency, const Debugger &debugger) : debugger(debugger)
{
	this->frequency = frequency;
	SetIntegrationMethod(integrationMethod);
	Reset();
}

//==========================================================================
// Class:			Integrator
// Function:		Integrator
//
// Description:		Copy constructor for the Integrator class.
//
// Input Arguments:
//		integrator	= const Integrator& to copy to this object
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Integrator::Integrator(const Integrator &integrator) : debugger(integrator.debugger)
{
	oldStateDerivatives = NULL;
	*this = integrator;
}

//==========================================================================
// Class:			Integrator
// Function:		~Integrator
//
// Description:		Destructor for the Integrator class.
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
Integrator::~Integrator()
{
	delete [] oldStateDerivatives;
	oldStateDerivatives = 0;
}

//==========================================================================
// Class:			Integrator
// Function:		ComputeNextState
//
// Description:		This is the main work function for this class.  This calls
//					the appropriate integration function, depending on this
//					object's IntegrationMethod.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		State of the system at the end of this new time step
//
//==========================================================================
State Integrator::ComputeNewState()
{
	// Choose the correct integration function
	switch (integrationMethod)
	{
	case MethodEulerSemiImplicit:
		return DoEulerSemiImplicit();
	case MethodRungeKutta2:
		return DoRungeKuttaSecondOrder();
	case MethodRungeKutta4:
		return DoRungeKuttaFourthOrder();
	case MethodAdamsBashforth3:
		return DoAdamsBashforthThirdOrder();
	default:
		return DoRungeKuttaFourthOrder();
	}
}

//==========================================================================
// Class:			Integrator
// Function:		DoEulerSemiImplicit
//
// Description:		Semi-implicit Euler integration.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		State of the system at the end of this new time step
//
//==========================================================================
State Integrator::DoEulerSemiImplicit()
{
	State nextState;

	return nextState;
}

//==========================================================================
// Class:			Integrator
// Function:		DoRungeKuttaSecondOrder
//
// Description:		2nd Order Runge-Kutta integration.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		State of the system at the end of this new time step
//
//==========================================================================
State Integrator::DoRungeKuttaSecondOrder()
{
	State nextState;

	return nextState;
}

//==========================================================================
// Class:			Integrator
// Function:		DoRungeKuttaFourthOrder
//
// Description:		4th Order Runge-Kutta integration.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		State of the system at the end of this new time step
//
//==========================================================================
State Integrator::DoRungeKuttaFourthOrder()
{
	State nextState;

	return nextState;
}

//==========================================================================
// Class:			Integrator
// Function:		DoAdamsBashforthThirdOrder
//
// Description:		3rd Order Adams Bashforth Integration.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		State of the system at the end of this new time step
//
//==========================================================================
State Integrator::DoAdamsBashforthThirdOrder()
{
	State nextState;

	return nextState;
}

//==========================================================================
// Class:			Integrator
// Function:		Reset
//
// Description:		Resets the system to the specified initial conditions.
//
// Input Arguments:
//		InitialConditions	= STATE at which the integration will begin
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Integrator::Reset(State /*initialConditions*/)
{
	// Initialize the integration step variable
	integrationStep = 0;
}

//==========================================================================
// Class:			Integrator
// Function:		Reset
//
// Description:		Resets the system to zero initial conditions.
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
void Integrator::Reset()
{
	State initialConditions;

	// Reset the integrator with these zero initial conditions
	Reset(initialConditions);
}

//==========================================================================
// Class:			Integrator
// Function:		SetIntegrationMethod
//
// Description:		Sets this object's integration method and allocates
//					memory for the correct number of state derivatives to
//					store.
//
// Input Arguments:
//		integrationMethod	= IntegrationMethod to use for future integration
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Integrator::SetIntegrationMethod(IntegrationMethod integrationMethod)
{
	this->integrationMethod = integrationMethod;

	// Determine the number of state derivatives we need to store
	switch (integrationMethod)
	{
	case MethodEulerSemiImplicit:
		numberOfStoredDerivatives = 1;
		break;
	case MethodRungeKutta2:
		numberOfStoredDerivatives = 1;
		break;
	case MethodRungeKutta4:
		numberOfStoredDerivatives = 1;
		break;
	case MethodAdamsBashforth3:
		numberOfStoredDerivatives = 3;
		break;
	default:
		assert(0);
		break;
	}

	// Delete the OldStateDerivatives array and reallocate it with its new size
	delete [] oldStateDerivatives;
	oldStateDerivatives = new State[numberOfStoredDerivatives];
	Reset();
}

//==========================================================================
// Class:			Integrator
// Function:		SetFrequency
//
// Description:		Sets this object's integration frequency.
//
// Input Arguments:
//		frequency	= double specifying the frequency to use for future
//					  integrations
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Integrator::SetFrequency(double frequency)
{
	this->frequency = frequency;
}

//==========================================================================
// Class:			Integrator
// Function:		operator=
//
// Description:		Assignment operator for Integrator class.
//
// Input Arguments:
//		integrator	= const Integrator& to assign to this object
//
// Output Arguments:
//		None
//
// Return Value:
//		Integrator&, reference to this object
//
//==========================================================================
Integrator& Integrator::operator=(const Integrator &integrator)
{
	// Check for self-assignment
	if (this == &integrator)
		return *this;

	// FIXME:  What do I do with OldStateDerivatives?
	oldStateDerivatives = NULL;

	// Perform the assignment
	integrationMethod			= integrator.integrationMethod;
	frequency					= integrator.frequency;
	numberOfStoredDerivatives	= integrator.numberOfStoredDerivatives;
	integrationStep				= integrator.integrationStep;

	return *this;
}
