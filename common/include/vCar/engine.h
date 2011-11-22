/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  engine.h
// Created:  11/3/2007
// Author:  K. Loux
// Description:  Contains class declaration for ENGINE class.
// History:
//	3/9/2008	- Changed the structure of the Debugger class, K. Loux.
//	11/22/2009	- Moved to vCar.lib, K. Loux.

#ifndef _ENGINE_H_
#define _ENGINE_H_

// Standard C++ headers
#include <iosfwd>// forward declarations of fstream objects

// vUtilites forward declarations
class Debugger;

class Engine
{
public:
	// Constructor
	Engine(const Debugger &_debugger);
	Engine(const Engine &engine);

	// Destructor
	~Engine();

	// File read/write functions
	void Write(std::ofstream *outFile) const;
	void Read(std::ifstream *inFile, int fileVersion);

	// Get the outputs from this portion of the simulation
	double GetEngineSpeed(void) { return crankshaftSpeed; };// [rad/sec]
	double TorqueOutput(const double &engineSpeed, const double &throttlePosition);// [in-lb]

	// Overloaded operators
	Engine& operator = (const Engine &engine);

private:
	// Debugger message printing utility
	const Debugger &debugger;

	// The crankshaft speed
	double crankshaftSpeed;// [rad/sec]
};

#endif// _ENGINE_H_