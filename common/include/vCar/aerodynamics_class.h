/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  aerodynamics_class.h
// Created:  11/3/2007
// Author:  K. Loux
// Description:  Contains class declaration for aerodynamics class.
// History:
//	3/9/2008	- Changed the structure of the DEBUGGER class, K. Loux.
//	11/22/2009	- Moved to vCar.lib, K. Loux.

#ifndef _AERODYNAMICS_CLASS_H_
#define _AERODYNAMICS_CLASS_H_

// Standard C++ headers
#include <iosfwd>// forward declarations of fstream objects

// vMath headers
#include "vMath/vector_class.h"

// vUtilities forward declarations
class DEBUGGER;

class AERODYNAMICS
{
public:
	// Constructor
	AERODYNAMICS(const DEBUGGER &_Debugger);
	AERODYNAMICS(const AERODYNAMICS &Aerodynamics);

	// Destructor
	~AERODYNAMICS();

	// File read/write functions
	void Write(std::ofstream *OutFile) const;
	void Read(std::ifstream *InFile, int FileVersion);

	// Functions that retrieve the aerodynamic forces and moments
	VECTOR GetAeroForces(void) const;			// [lbf]
	VECTOR GetAeroMoments(void) const;			// [in-lbf]

	// Overloaded operators
	AERODYNAMICS& operator = (const AERODYNAMICS &Aerodynamics);

private:
	// Debugger message printing utility
	const DEBUGGER &Debugger;

	// Class properties
	VECTOR CenterOfPressure;			// [in]
	double ReferenceArea;				// [in^2]
	double AirDensity;					// [slug/in^3]
	double CoefficientOfDownforce;		// [-]
	double CoefficientOfDrag;			// [-]
	double CoefficientOfMoment;			// [-]
	// FIXME:  Add functionality for non-linear look-up table (user option)
};

#endif// _AERODYNAMICS_CLASS_H_