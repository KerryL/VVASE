/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  tireSet.h
// Created:  3/9/2008
// Author:  K. Loux
// Description:  Contains class declaration for TIRE_SET class.  This class contains a set
//				 of four tires.  Required to manage dynamic memory allocation of a set of tires.
// History:
//	11/22/2009	- Moved to vCar.lib, K. Loux.

#ifndef _TIRESET_CLASS_H_
#define _TIRESET_CLASS_H_

// Standard C++ headers
#include <iosfwd>// forward declarations of fstream objects

// vCar forward declarations
class TIRE;

// vUtilities forward declarations
class Debugger;

class TIRE_SET
{
public:
	// Constructor
	TIRE_SET(const Debugger &_debugger);
	TIRE_SET(const TIRE_SET &TireSet);

	// Destructor
	~TIRE_SET();

	// File read/write functions
	void Write(std::ofstream *OutFile) const;
	void Read(std::ifstream *InFile, int FileVersion);

	// The four tires
	TIRE *RightFront;
	TIRE *LeftFront;
	TIRE *RightRear;
	TIRE *LeftRear;

	// This class contains dynamically allocated memory - overload the assigment operator
	TIRE_SET& operator = (const TIRE_SET &TireSet);

private:
	// Debugger message printing utility
	const Debugger &debugger;
};

#endif// _TIRESET_CLASS_H_