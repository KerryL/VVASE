/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  integrator.cpp
// Created:  7/12/2008
// Author:  K. Loux
// Description:  Contains class definition for INTEGRATOR class.
// History:

// wxWidgets headers
#include <wx/wx.h>

// CarDesigner headers
#include "vSolver/physics/integrator.h"

//==========================================================================
// Class:			INTEGRATOR
// Function:		INTEGRATOR
//
// Description:		Constructor for the INTEGRATOR class.  Sets all of the
//					parameters that are required for integration to begin.
//
// Input Arguments:
//		_IntegrationMethod	= const INTEGRATION_METHOD& to use for future integrations
//		_Frequency			= const Frequency& at which the integration will occur [Hz]
//		_debugger			= const Debugger&, reference to debug message printing utility
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
INTEGRATOR::INTEGRATOR(const INTEGRATION_METHOD &_IntegrationMethod, const double &_Frequency,
					   const Debugger &_debugger) : debugger(_debugger)
{
	// Assign the arguments to the class members
	Frequency = _Frequency;

	// Assign the integration method
	SetIntegrationMethod(_IntegrationMethod);

	// Initialize the integrator
	Reset();
}

//==========================================================================
// Class:			INTEGRATOR
// Function:		INTEGRATOR
//
// Description:		Copy constructor for the INTEGRATOR class.
//
// Input Arguments:
//		Integrator	= const INTEGRATOR& to copy to this object
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
INTEGRATOR::INTEGRATOR(const INTEGRATOR &Integrator) : debugger(Integrator.debugger)
{
	// Initialize the pointers
	OldStateDerivatives = NULL;

	// Do the copy
	*this = Integrator;
}

//==========================================================================
// Class:			INTEGRATOR
// Function:		~INTEGRATOR
//
// Description:		Destructor for the INTEGRATOR class.
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
INTEGRATOR::~INTEGRATOR()
{
	// Delete the stored state derivatives
	delete [] OldStateDerivatives;
	OldStateDerivatives = 0;
}

//==========================================================================
// Class:			INTEGRATOR
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
//		STATE of the system at the end of this new time step
//
//==========================================================================
STATE INTEGRATOR::ComputeNewState(void)
{
	// Choose the correct integration function
	switch (IntegrationMethod)
	{
	case INT_EULER_SEMI_IMPLICIT:
		return DoEulerSemiImplicit();
	case INT_RUNGE_KUTTA_2:
		return DoRungeKuttaSecondOrder();
	case INT_RUNGE_KUTTA_4:
		return DoRungeKuttaFourthOrder();
	case INT_ADAMS_BASHFORTH_3:
		return DoAdamsBashforthThirdOrder();
	default:
		return DoRungeKuttaFourthOrder();
	}
}

//==========================================================================
// Class:			INTEGRATOR
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
//		STATE of the system at the end of this new time step
//
//==========================================================================
STATE INTEGRATOR::DoEulerSemiImplicit(void)
{
	STATE NextState;

	return NextState;
}

//==========================================================================
// Class:			INTEGRATOR
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
//		STATE of the system at the end of this new time step
//
//==========================================================================
STATE INTEGRATOR::DoRungeKuttaSecondOrder(void)
{
	STATE NextState;

	return NextState;
}

//==========================================================================
// Class:			INTEGRATOR
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
//		STATE of the system at the end of this new time step
//
//==========================================================================
STATE INTEGRATOR::DoRungeKuttaFourthOrder(void)
{
	STATE NextState;

	return NextState;
}

//==========================================================================
// Class:			INTEGRATOR
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
//		STATE of the system at the end of this new time step
//
//==========================================================================
STATE INTEGRATOR::DoAdamsBashforthThirdOrder(void)
{
	STATE NextState;

	return NextState;
}

//==========================================================================
// Class:			INTEGRATOR
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
void INTEGRATOR::Reset(STATE InitialConditions)
{
	// Initialize the integration step variable
	IntegrationStep = 0;

	return;
}

//==========================================================================
// Class:			INTEGRATOR
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
void INTEGRATOR::Reset(void)
{
	STATE InitialConditions;

	// Reset the integrator with these zero initial conditions
	Reset(InitialConditions);

	return;
}

//==========================================================================
// Class:			INTEGRATOR
// Function:		SetIntegrationMethod
//
// Description:		Sets this object's integration method and allocates
//					memory for the correct number of state derivatives to
//					store.
//
// Input Arguments:
//		_IntegrationMethod	= INTEGRATION_METHOD to use for future integration
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void INTEGRATOR::SetIntegrationMethod(INTEGRATION_METHOD _IntegrationMethod)
{
	// Assign the integration method to our class member
	IntegrationMethod = _IntegrationMethod;

	// Determine the number of state derivatives we need to store
	switch (IntegrationMethod)
	{
	case INT_EULER_SEMI_IMPLICIT:
		NumberOfStoredDerivatives = 1;
		break;
	case INT_RUNGE_KUTTA_2:
		NumberOfStoredDerivatives = 1;
		break;
	case INT_RUNGE_KUTTA_4:
		NumberOfStoredDerivatives = 1;
		break;
	case INT_ADAMS_BASHFORTH_3:
		NumberOfStoredDerivatives = 3;
		break;
	default:
		assert(0);
		break;
	}

	// Delete the OldStateDerivatives array and reallocate it with its new size
	delete [] OldStateDerivatives;
	OldStateDerivatives = new STATE[NumberOfStoredDerivatives];
	Reset();

	return;
}

//==========================================================================
// Class:			INTEGRATOR
// Function:		SetFrequency
//
// Description:		Sets this object's integration frequency.
//
// Input Arguments:
//		_Frequency	= double specifying the frequency to use for future
//					  integrations
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void INTEGRATOR::SetFrequency(double _Frequency)
{
	// Assign the frequency to the class member
	Frequency = _Frequency;

	return;
}

//==========================================================================
// Class:			INTEGRATOR
// Function:		operator =
//
// Description:		Assignment operator for INTEGRATOR class.
//
// Input Arguments:
//		Integrator	= const INTEGRATOR& to assign to this object
//
// Output Arguments:
//		None
//
// Return Value:
//		INTEGRATOR&, reference to this object
//
//==========================================================================
INTEGRATOR& INTEGRATOR::operator = (const INTEGRATOR &Integrator)
{
	// Check for self-assignment
	if (this == &Integrator)
		return *this;

	// FIXME:  What do I do with OldStateDerivatives?
	OldStateDerivatives = NULL;

	// Perform the assignment
	IntegrationMethod			= Integrator.IntegrationMethod;
	Frequency					= Integrator.Frequency;
	NumberOfStoredDerivatives	= Integrator.NumberOfStoredDerivatives;
	IntegrationStep				= Integrator.IntegrationStep;

	return *this;
}