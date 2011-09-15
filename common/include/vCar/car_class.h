/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  car_class.h
// Created:  11/2/2007
// Author:  K. Loux
// Description:  Contains class declaration for CAR class.
// History:
//	3/9/2008	- Changed structure of DEBUGGER class, K. Loux.
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
//  (7) Right front shock displacement (needs modification to be compatable with monoshock)
//  (8) Left front shock displacement (needs modification to be compatable with monoshock)
//  (9) Right rear shock displacement (needs modification to be compatable with monoshock)
// (10) Left rear shock displacement (needs modification to be compatable with monoshock)
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

#ifndef _CAR_CLASS_H_
#define _CAR_CLASS_H_

// Standard C++ headers
#include <iosfwd>// for forward declarations of fstream objects

// wxWidgets headers
#include <wx/thread.h>

// wxWidgets forward declarations
class wxString;

// vCar forward declarations
class AERODYNAMICS;
class BRAKES;
class DEBUGGER;
class DRIVETRAIN;
class ENGINE;
class MASS_PROPERTIES;
class SUSPENSION;
class TIRE_SET;

class CAR
{
public:
	// Constructor
	CAR(const DEBUGGER &_Debugger);
	CAR(const CAR &Car);

	// Destructor
	~CAR();

	// Utility methods
	bool SaveCarToFile(wxString FileName, std::ofstream *_OutFile = NULL) const;
	bool LoadCarFromFile(wxString FileName, std::ifstream *_InFile = NULL, int *FileVersion = NULL);

	// Computes the wheel center locations - to be done prior to each analysis
	void ComputeWheelCenters(void);

	// For ease of determining whether or not a car has certain features
	bool HasFrontHalfShafts(void) const;
	bool HasRearHalfShafts(void) const;

	// This class contains dynamically allocated memory - overload the assigment operator
	CAR& operator = (const CAR &Car);

	// These properties are modifiable ONLY by the user... this program
	// can only reference these properties, any changes (i.e. locations of
	// suspension hardpoints during dynamic analysis) will be applied to a
	// DUPLICATE set of properties to avoid loosing the original data
	AERODYNAMICS *Aerodynamics;
	BRAKES *Brakes;
	DRIVETRAIN *Drivetrain;
	ENGINE *Engine;
	MASS_PROPERTIES *MassProperties;
	SUSPENSION *Suspension;
	TIRE_SET *Tires;

	// Mutex accessor
	wxMutex &GetMutex(void) const { return CarMutex; };

private:
	// Debugger message printing utility
	const DEBUGGER &Debugger;

	// File header information
	struct FILE_HEADER_INFO
	{
		int FileVersion;
	};

	// Writes and reads the file header information for saved cars
	void WriteFileHeader(std::ofstream *OutFile) const;
	FILE_HEADER_INFO ReadFileHeader(std::ifstream *InFile) const;

	// Our current file verstion
	static const int CurrentFileVersion;

	// Syncronization object
	mutable wxMutex CarMutex;
};

#endif// _CAR_CLASS_H_