/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  atmosphere_class.h
// Created:  12/12/2008
// Author:  K. Loux
// Description:  Contains class declaration for ATMOSPHERE class.  This is intended
//				 for use with aerodynamic models and engine models (through SAE
//				 standard conversion from HP to SAE HP).  Although for most sims,
//				 a single number describing the air density and maybe another for
//				 temperature will suffice, if someone wanted to simulate a Pike's
//				 Peak hillclimb, an atmosphere model could add to the fidelity of
//				 the simulation.  This model should be valid between Death Valley
//				 and Pike's Peak for Cold Days, Hot Days, and Standard Days, as well
//				 as reasonable custom values entered by the user.
//
//				 This model is based on the International Standard Atmosphere, which
//				 is the same as the International Civil Aviation Organization
//				 Standard Atmosphere, and at the altitudes that we will be modeling,
//				 it is also the same as the 1976 U.S. Standard Atmosphere and the
//				 MIL-STD Standard Atmosphere.
// History:

#ifndef _ATMOSPHERE_CLASS_H_
#define _ATMOSPHERE_CLASS_H_

class ATMOSPHERE
{
public:
	// Constructor
	ATMOSPHERE();

	// Destructor
	~ATMOSPHERE();

	// This strucutre contains all of the data that changes with
	// weather conditions, and affects things like aerodynamic
	// forces and engine output.
	struct CONDITIONS
	{
		// Air density
		double Density;// [slug/in^3]

		// Air pressure
		double Pressure;// [psi]

		// Air temperature
		double Temperature;// [deg R]

		// Dynamic (absolute) viscosity of air (usually denoted by mu)
		double Viscosity;// [psi-sec]
	};

	// Set and retrieve the parameters for the day
	void SetConditions(CONDITIONS _CurrentDay);
	CONDITIONS GetConditions(void) { return CurrentDay; };

	// Overload for GetConditions - adjusts for not being at sea level
	CONDITIONS GetConditions(double Altitude);

	// Some pre-defined conditions (from coldest to warmest)
	static const CONDITIONS ColdDay;
	static const CONDITIONS PolarDay;
	static const CONDITIONS StandardDay;
	static const CONDITIONS TropicDay;
	static const CONDITIONS HotDay;

private:
	// Today's conditions
	CONDITIONS CurrentDay;
};

#endif// _ATMOSPHERE_CLASS_H_