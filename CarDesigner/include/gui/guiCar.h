/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  guiCar.h
// Created:  4/15/2008
// Author:  K. Loux
// Description:  A high-level car object.  This class defines the interaction between
//				 the GUI and the actual CAR class.
// History:
//	7/27/2008	- Switched to an array of TREE_ITEMs with an enum for their indices instead
//				  of using a new TREE_ITEM for every entry.  This simplifies everything except
//				  the creation of the items, which doesn't get any more complicated,  K. Loux.
//	8/18/2008	- Changed name of TREE_ITEM class to GUI_OBJECT, K. Loux.
//	1/24/2009	- Major application structure change - MAIN_FRAME uses GUI_OBJECT instead of
//				  GUI_CAR.  GUI_OBJECT changed to only contain either GUI_CAR or ITERATION
//				  objects, K. Loux.
//	2/17/2009	- Moved the KINEMATICS object into this class so we can update the 3D car
//				  more easily, K. Loux.
//	2/18/2009	- Added subsystems to allow subsystems to appear in the systems tree, K. Loux.
//	5/19/2009	- Changed to derived class from GUI_OBJECT, K. Loux.

#ifndef _GUI_CAR_H_
#define _GUI_CAR_H_

// wxWidgets headers
#include <wx/treebase.h>

// VVASE headers
#include "gui/guiObject.h"
#include "vSolver/physics/kinematicOutputs.h"

// VVASE forward declarations
class CarRenderer;
class Car;
class Debugger;
class AppearanceOptions;

// wxWidgets forward declarations
class wxTreeItemId;

class GuiCar : public GuiObject
{
public:
	// Constructor
	GuiCar(MainFrame &_mainFrame, const Debugger &_debugger,
		wxString _pathAndFileName = wxEmptyString);

	// Destructor
	~GuiCar();

	// To be called after either the WorkingCar or the OriginalCar
	// has changed to update the analysis data (mandatory overload)
	void UpdateData(void);

	// Updates the displays associated with this object
	void UpdateDisplay(void);

	// Private member accessors
	inline Car &GetOriginalCar(void) { return *originalCar; };
	inline Car &GetWorkingCar(void) { return *workingCar; };
	inline AppearanceOptions &GetAppearanceOptions(void) { return *appearanceOptions; };

	// Returns this object's type
	ItemType GetType(void) const { return TypeCar; };

	// Enumeration for the subsystems that appear in the systems tree under this car
	enum Subsystems
	{
		SubsystemAerodynamics,
		SubsystemBrakes,
		SubsystemDrivetrain,
		SubsystemEngine,
		SubsystemMassProperties,
		SubsystemSuspension,
		SubsystemTires,

		NumberOfSubsystems
	};

	// For naming the subsystems
	static wxString GetSubsystemName(Subsystems subsystem);

	// The tree IDs associated with each subsystem object
	wxTreeItemId subsystems[NumberOfSubsystems];

	// Returns a copy of the kinematic output data
	KinematicOutputs GetKinematicOutputs(void) { return kinematicOutputs; };

private:
	// The "real" object containing the original, unmodified information (not rolled, heaved, etc.)
	Car *originalCar;

	// Reference to the object that we manipulate as we do our analyses
	Car *workingCar;

	// Performs the saving and loading to/from file (mandatory overloads)
	bool PerformLoadFromFile(void);
	bool PerformSaveToFile(void);

	// Gets the icon handle for this object's icon
	int GetIconHandle(void) const;

	// Reference to the options that affect this object's appearance
	AppearanceOptions *appearanceOptions;

	// The object that stores the kinematics information for this car
	KinematicOutputs kinematicOutputs;

	// The scene visible to the user representing this object
	CarRenderer *renderer;
};

#endif// _GUI_CAR_H_