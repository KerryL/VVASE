/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

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

#ifndef DIFFERENTIAL_H_
#define DIFFERENTIAL_H_

// Standard C++ headers
#include <iosfwd>// forward declarations of fstream objects

class Differential
{
public:
	Differential();
	Differential(const double& biasRatio);
	Differential(const Differential &differential);
	~Differential();

	// File read/write functions
	void Write(std::ofstream *outFile) const;
	void Read(std::ifstream *inFile, int fileVersion);

	// Overloaded operators
	Differential& operator=(const Differential &differential);

	double biasRatio;
};

#endif// DIFFERENTIAL_H_