/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  engine_class.h
// Created:  11/3/2007
// Author:  K. Loux
// Description:  Contains class declaration for ENGINE class.
// History:
//	3/9/2008	- Changed the structure of the Debugger class, K. Loux.
//	11/22/2009	- Moved to vCar.lib, K. Loux.

#ifndef _ENGINE_CLASS_H_
#define _ENGINE_CLASS_H_

// Standard C++ headers
#include <iosfwd>// forward declarations of fstream objects

// vUtilites forward declarations
class Debugger;

class ENGINE
{
public:
	// Constructor
	ENGINE(const Debugger &_debugger);
	ENGINE(const ENGINE &Engine);

	// Destructor
	~ENGINE();

	// File read/write functions
	void Write(std::ofstream *OutFile) const;
	void Read(std::ifstream *InFile, int FileVersion);

	// Get the outputs from this portion of the simulation
	double GetEngineSpeed(void) { return CrankshaftSpeed; };// [rad/sec]
	double TorqueOutput(const double &EngineSpeed, const double &ThrottlePosition);// [in-lb]

	// Overloaded operators
	ENGINE& operator = (const ENGINE &Engine);

private:
	// Debugger message printing utility
	const Debugger &debugger;

	// The crankshaft speed
	double CrankshaftSpeed;// [rad/sec]
};

#endif// _ENGINE_CLASS_H_