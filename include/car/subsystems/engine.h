/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

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

#ifndef ENGINE_H_
#define ENGINE_H_

// Local forward declarations
class BinaryReader;
class BinaryWriter;

class Engine
{
public:
	Engine();
	Engine(const Engine &engine);
	~Engine();

	// File read/write functions
	void Write(BinaryWriter& file) const;
	void Read(BinaryReader& file, const int& fileVersion);

	// Get the outputs from this portion of the simulation
	double GetEngineSpeed();// [rad/sec]
	double TorqueOutput(const double &engineSpeed, const double &throttlePosition);// [in-lb]

	// Overloaded operators
	Engine& operator=(const Engine &engine);
};

#endif// ENGINE_H_