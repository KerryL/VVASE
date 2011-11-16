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
class CAR_RENDERER;
class CAR;
class Debugger;
class APPEARANCE_OPTIONS;

// wxWidgets forward declarations
class wxTreeItemId;

class GUI_CAR : public GUI_OBJECT
{
public:
	// Constructor
	GUI_CAR(MAIN_FRAME &_MainFrame, const Debugger &_debugger,
		wxString _PathAndFileName = wxEmptyString);

	// Destructor
	~GUI_CAR();

	// To be called after either the WorkingCar or the OriginalCar
	// has changed to update the analysis data (mandatory overload)
	void UpdateData(void);

	// Updates the displays associated with this object
	void UpdateDisplay(void);

	// Private member accessors
	inline CAR &GetOriginalCar(void) { return *OriginalCar; };
	inline CAR &GetWorkingCar(void) { return *WorkingCar; };
	inline APPEARANCE_OPTIONS &GetAppearanceOptions(void) { return *AppearanceOptions; };

	// Returns this object's type
	ITEM_TYPE GetType(void) const { return TYPE_CAR; };

	// Enumeration for the subsystems that appear in the systems tree under this car
	enum SUBSYSTEMS
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
	static wxString GetSubsystemName(SUBSYSTEMS Subsystem);

	// The tree IDs associated with each subsystem object
	wxTreeItemId Subsystems[NumberOfSubsystems];

	// Returns a copy of the kinematic output data
	KinematicOutputs GetKinematicOutputs(void) { return kinematicOutputs; };

private:
	// The "real" object containing the original, unmodified information (not rolled, heaved, etc.)
	CAR *OriginalCar;

	// Reference to the object that we manipulate as we do our analyses
	CAR *WorkingCar;

	// Performs the saving and loading to/from file (mandatory overloads)
	bool PerformLoadFromFile(void);
	bool PerformSaveToFile(void);

	// Gets the icon handle for this object's icon
	int GetIconHandle(void) const;

	// Reference to the options that affect this object's appearance
	APPEARANCE_OPTIONS *AppearanceOptions;

	// The object that stores the kinematics information for this car
	KinematicOutputs kinematicOutputs;

	// The scene visible to the user representing this object
	CAR_RENDERER *renderer;
};

#endif// _GUI_CAR_H_