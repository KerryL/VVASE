/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  tire.h
// Created:  11/3/2007
// Author:  K. Loux
// Description:  Contains class declaration for TIRE class.  Contains several tire models.
// History:
//	2/25/2008	- Named TIRE_MODEL enum, K. Loux.
//	3/9/2008	- Moved enumerations inside class body and changed the structure of the Debugger
//				  class, K. Loux.
//	11/22/2009	- Moved to vCar.lib, K. Loux.

#ifndef _TIRE_H_
#define _TIRE_H_

// Standard C++ headers
#include <iosfwd>// forward declarations of fstream objects

// VVASE headers
#include "vMath/vector.h"
#include "vUtilities/debugger.h"

class Tire
{
public:
	// Constructor
	Tire(const Debugger &_debugger);
	Tire(const Tire &tire);

	// Destructor
	~Tire();

	// File read/write functions
	void Write(std::ofstream *outFile) const;
	void Read(std::ifstream *inFile, int fileVersion);

	// Enumeration describing the tire models we support
	enum TireModel
	{
		Pacejka96,
		MRANonDimensional,
		Lugre
	};

	// The functions that calculate the tire's forces and moments
	Vector GetTireForces(const double &normalLoad, const double &slipAngle,
		const double &slipRatio, const double &localMu);// [lbf]
	Vector GetTireMoments(const double &normalLoad, const double &slipAngle,
		const double &slipRatio, const double &localMu);// [in-lbf]

	// Tire characteristics
	double diameter;			// [in] (unloaded)
	double width;				// [in]
	double tirePressure;		// [psi]

	// Operators
	Tire& operator = (const Tire &tire);

private:
	// Debugger message printing utility
	const Debugger &debugger;

	// FIXME:  Eventually, add support for a selection of tire models
	// These parameters come from SAE Paper # 2006-01-1968, Tire Asymmetries and Pressure
	// Variations in the Radt/Milliken Tire Model, by Kasprzak and Milliken.  This is an
	// expansion of the MRA tire model to include asymmetric tire data.
	// Ideal tire model:  temperature dependant, dynamic, works for combined slips
	double bright;
	double cright;
	double dright;
	double eright;
	double bleft;
	double cleft;
	double dleft;
	double eleft;

	// Model type
	TireModel modelType;
};

#endif// _TIRE_H_