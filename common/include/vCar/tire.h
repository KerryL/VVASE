/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

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

#ifndef TIRE_H_
#define TIRE_H_

// Standard C++ headers
#include <iosfwd>// forward declarations of fstream objects

// VVASE headers
#include "vMath/vector.h"

class Tire
{
public:
	Tire();
	Tire(const Tire &tire);
	~Tire();

	// File read/write functions
	void Write(std::ofstream *outFile) const;
	void Read(std::ifstream *inFile, int fileVersion);

	// Enumeration describing the tire models we support
	enum TireModel
	{
		ModelConstantMu
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
	double stiffness;			// [lb/in]

	Tire& operator=(const Tire &tire);

private:
	// TODO:  Eventually, add support for a selection of tire models
	// Ideal tire model:  temperature dependant, dynamic, works for combined slips

	// Model type
	TireModel modelType;
};

#endif// TIRE_H_