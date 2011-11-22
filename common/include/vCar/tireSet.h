/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  tireSet.h
// Created:  3/9/2008
// Author:  K. Loux
// Description:  Contains class declaration for TireSet class.  This class contains a set
//				 of four tires.  Required to manage dynamic memory allocation of a set of tires.
// History:
//	11/22/2009	- Moved to vCar.lib, K. Loux.

#ifndef _TIRESET_H_
#define _TIRESET_H_

// Standard C++ headers
#include <iosfwd>// forward declarations of fstream objects

// vCar forward declarations
class Tire;

// vUtilities forward declarations
class Debugger;

class TireSet
{
public:
	// Constructor
	TireSet(const Debugger &_debugger);
	TireSet(const TireSet &tireSet);

	// Destructor
	~TireSet();

	// File read/write functions
	void Write(std::ofstream *outFile) const;
	void Read(std::ifstream *inFile, int fileVersion);

	// The four tires
	Tire *rightFront;
	Tire *leftFront;
	Tire *rightRear;
	Tire *leftRear;

	// This class contains dynamically allocated memory - overload the assigment operator
	TireSet& operator = (const TireSet &tireSet);

private:
	// Debugger message printing utility
	const Debugger &debugger;
};

#endif// _TIRESET_H_