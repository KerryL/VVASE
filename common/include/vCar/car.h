/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  car.h
// Created:  11/2/2007
// Author:  K. Loux
// Description:  Contains class declaration for Car class.
// History:
//	3/9/2008	- Changed structure of Debugger class, K. Loux.
//	11/22/2009	- Moved to vCar.lib, K. Loux.

// Car coordinate system:  (SAE vehicle coordinate system)
// For some reason, SAE uses one coordinate system for vehicle development, and another one for
// vehicle dynamics simulation.  I like their vehicle dynamics coordinate system better, so I'll
// use that one for everything.  Here it is: (Ref. http://www.eng-tips.com/viewthread.cfm?qid=106763&page=2)
//  X-axis - positive rearward, origin at the front axle
//  Y-axis - positive to the driver's right, origin on longitudinal centerline
//  Z-axis - positive up, origin on the ground plane

// While we're discussing conventions, we will use standard English units (inches, pounds, slugs,
// seconds) for all calculations.  Angles will be in radians.  More detail can be found in
// convert_class.h.

// The 14 DOF being solved in the dynamic simulation are:
//  (1) Global X-position of sprung mass
//  (2) Global Y-position of sprung mass
//  (3) Global Z-position of sprung mass
//  (4) Global pitch of sprung mass
//  (5) Global roll of sprung mass
//  (6) Global heading of sprung mass
//  (7) Right front shock displacement (needs modification to be compatible with monoshock)
//  (8) Left front shock displacement (needs modification to be compatible with monoshock)
//  (9) Right rear shock displacement (needs modification to be compatible with monoshock)
// (10) Left rear shock displacement (needs modification to be compatible with monoshock)
// (11) Right front wheel rotation rate
// (12) Left front wheel rotation rate
// (13) Right rear wheel rotation rate
// (14) Left rear wheel rotation rate
// Additional DOF may be added to solve for driver inputs and chassis/component flex.
// One DOF for each tire deflection (spring/damper model)?
// One DOF for each tire force (time-lag for tires to deflect before full force is reached)?

// Pitch and roll (in local frame) are Euler angles, first rotation is pitch, second is roll.
// I'm going to assume that the car will never have a pitch of 90 or -90 so I don't have to
// use quarternions.  This will be valid unless it becomes some kind of computer game (driving
// off of cliffs or ramps?).

#ifndef _CAR_H_
#define _CAR_H_

// Standard C++ headers
#include <iosfwd>// for forward declarations of fstream objects

// wxWidgets headers
#include <wx/thread.h>

// wxWidgets forward declarations
class wxString;

// vCar forward declarations
class Aerodynamics;
class Brakes;
class Drivetrain;
class Engine;
class MassProperties;
class Suspension;
class TireSet;

class Car
{
public:
	// Constructor
	Car();
	Car(const Car &car);

	// Destructor
	~Car();

	// Utility methods
	bool SaveCarToFile(wxString fileName, std::ofstream *_outFile = NULL) const;
	bool LoadCarFromFile(wxString fileName, std::ifstream *_inFile = NULL, int *fileVersion = NULL);

	// Computes the wheel center locations - to be done prior to each analysis
	void ComputeWheelCenters(void);

	// For ease of determining whether or not a car has certain features
	bool HasFrontHalfShafts(void) const;
	bool HasRearHalfShafts(void) const;

	// This class contains dynamically allocated memory - overload the assignment operator
	Car& operator = (const Car &car);

	// These properties are modifiable ONLY by the user... this program
	// can only reference these properties, any changes (i.e. locations of
	// suspension hardpoints during dynamic analysis) will be applied to a
	// DUPLICATE set of properties to avoid loosing the original data
	Aerodynamics *aerodynamics;
	Brakes *brakes;
	Drivetrain *drivetrain;
	Engine *engine;
	MassProperties *massProperties;
	Suspension *suspension;
	TireSet *tires;

	// Mutex accessor
	wxMutex &GetMutex(void) const { return carMutex; };

private:
	// File header information
	struct FileHeaderInfo
	{
		int fileVersion;
	};

	// Writes and reads the file header information for saved cars
	void WriteFileHeader(std::ofstream *outFile) const;
	FileHeaderInfo ReadFileHeader(std::ifstream *inFile) const;

	// Our current file version
	static const int currentFileVersion;

	// Synchronization object
	mutable wxMutex carMutex;
};

#endif// _CAR_H_