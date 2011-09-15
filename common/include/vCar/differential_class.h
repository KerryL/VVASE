/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  differential_class.h
// Created:  11/6/2007
// Author:  K. Loux
// Description:  Contains class declaration for DIFFERENTIAL class.
// History:
//	2/25/2008	- Named DIFFERENTIAL_STYLE enum, K. Loux.
//	3/9/2008	- Changed the structure of the DEBUGGER class and moved the enumerations
//				  inside the class body, K. Loux.
//	11/22/2009	- Moved to vCar.lib, K. Loux.

#ifndef _DIFFERENTIAL_CLASS_H_
#define _DIFFERENTIAL_CLASS_H_

// Standard C++ headers
#include <iosfwd>// forward declarations of fstream objects

// vUtilities forward declarations
class DEBUGGER;

class DIFFERENTIAL
{
public:
	// Constructor
	DIFFERENTIAL(const DEBUGGER &_Debugger);
	DIFFERENTIAL(const DIFFERENTIAL &Differential);

	// Destructor
	~DIFFERENTIAL();

	// File read/write functions
	void Write(std::ofstream *OutFile) const;
	void Read(std::ifstream *InFile, int FileVersion);

	// Retrieves the output torque on each side of the differential
	double GetTorque1(const double &InputTorque) const;// Front or right
	double GetToruqe2(const double &InputTorque) const;// Rear or left

	// Enumeration describing the different differentials that can be modeled
	enum DIFFERENTIAL_STYLE
	{
		TORSION_TYPE_1,
		TORSION_TYPE_2,
		SALISBURY,
		LOCKED,
		OPEN,
		CAM_AND_PAWL
	};

	// Overloaded operators
	DIFFERENTIAL& operator = (const DIFFERENTIAL &Differential);

private:
	// Debugger message printing utility
	const DEBUGGER &Debugger;

	// The parameters that describe the differential physics
	double BiasRatio;		// [-]
	double Preload;			// [lbf]

	// The style differential modeled by this object
	DIFFERENTIAL_STYLE Style;
};

#endif// _DIFFERENTIAL_CLASS_H_