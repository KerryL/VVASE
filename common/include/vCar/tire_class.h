/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  tire_class.h
// Created:  11/3/2007
// Author:  K. Loux
// Description:  Contains class declaration for TIRE class.  Contains several tire models.
// History:
//	2/25/2008	- Named TIRE_MODEL enum, K. Loux.
//	3/9/2008	- Moved enumerations inside class body and changed the structure of the Debugger
//				  class, K. Loux.
//	11/22/2009	- Moved to vCar.lib, K. Loux.

#ifndef _TIRE_CLASS_H_
#define _TIRE_CLASS_H_

// Standard C++ headers
#include <iosfwd>// forward declarations of fstream objects

// VVASE headers
#include "vMath/vector_class.h"
#include "vUtilities/debug_class.h"

class TIRE
{
public:
	// Constructor
	TIRE(const Debugger &_debugger);
	TIRE(const TIRE &Tire);

	// Destructor
	~TIRE();

	// File read/write functions
	void Write(std::ofstream *OutFile) const;
	void Read(std::ifstream *InFile, int FileVersion);

	// Enumeration describing the tire models we support
	enum TIRE_MODEL
	{
		PACEJKA_96,
		MRA_NON_DIMENSIONAL,
		LUGRE
	};

	// The functions that calculate the tire's forces and moments
	Vector GetTireForces(const double &NormalLoad, const double &SlipAngle,
		const double &SlipRatio, const double &LocalMu);// [lbf]
	Vector GetTireMoments(const double &NormalLoad, const double &SlipAngle,
		const double &SlipRatio, const double &LocalMu);// [in-lbf]

	// Tire characteristics
	double Diameter;			// [in] (unloaded)
	double Width;				// [in]
	double TirePressure;		// [psi]

	// Operators
	TIRE& operator = (const TIRE &Tire);

private:
	// Debugger message printing utility
	const Debugger &debugger;

	// FIXME:  Eventually, add support for a selection of tire models
	// These parameters come from SAE Paper # 2006-01-1968, Tire Asymmetries and Pressure
	// Variations in the Radt/Milliken Tire Model, by Kasprzak and Milliken.  This is an
	// expansion of the MRA tire model to include asymmetric tire data.
	// Ideal tire model:  temperature dependant, dynamic, works for combined slips
	double Bright;
	double Cright;
	double Dright;
	double Eright;
	double Bleft;
	double Cleft;
	double Dleft;
	double Eleft;

	// Model type
	TIRE_MODEL ModelType;
};

#endif// _TIRE_CLASS_H_