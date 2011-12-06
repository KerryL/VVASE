/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  differential.h
// Created:  11/6/2007
// Author:  K. Loux
// Description:  Contains class declaration for DIFFERENTIAL class.
// History:
//	2/25/2008	- Named DIFFERENTIAL_STYLE enum, K. Loux.
//	3/9/2008	- Changed the structure of the Debugger class and moved the enumerations
//				  inside the class body, K. Loux.
//	11/22/2009	- Moved to vCar.lib, K. Loux.

#ifndef _DIFFERENTIAL_H_
#define _DIFFERENTIAL_H_

// Standard C++ headers
#include <iosfwd>// forward declarations of fstream objects

class Differential
{
public:
	// Constructor
	Differential();
	Differential(const Differential &differential);

	// Destructor
	~Differential();

	// File read/write functions
	void Write(std::ofstream *outFile) const;
	void Read(std::ifstream *inFile, int fileVersion);

	// Retrieves the output torque on each side of the differential
	double GetTorque1(const double &inputTorque) const;// Front or right
	double GetToruqe2(const double &inputTorque) const;// Rear or left

	// Enumeration describing the different differentials that can be modeled
	enum DifferentialStyle
	{
		TorsionType1,
		TorsionType2,
		Salisbury,
		Locked,
		Open,
		CamAndPawl,

		NumberOfStyles
	};

	// Overloaded operators
	Differential& operator = (const Differential &differential);

private:
	// The parameters that describe the differential physics
	double biasRatio;		// [-]
	double preload;			// [lbf]

	// The style differential modeled by this object
	DifferentialStyle style;
};

#endif// _DIFFERENTIAL_H_