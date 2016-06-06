/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  guiCar.h
// Created:  4/15/2008
// Author:  K. Loux
// Description:  A high-level car object.  This class defines the interaction between
//				 the GUI and the actual CAR class.
// History:
//	7/27/2008	- Switched to an array of TreeItems with an enum for their indices instead
//				  of using a new TreeItem for every entry.  This simplifies everything except
//				  the creation of the items, which doesn't get any more complicated,  K. Loux.
//	8/18/2008	- Changed name of TreeItem class to GuiObject, K. Loux.
//	1/24/2009	- Major application structure change - MainFrame uses GuiObject instead of
//				  GuICar.  GuiObject changed to only contain either GuiCar or Iteration
//				  objects, K. Loux.
//	2/17/2009	- Moved the Kinematics object into this class so we can update the 3D car
//				  more easily, K. Loux.
//	2/18/2009	- Added subsystems to allow subsystems to appear in the systems tree, K. Loux.
//	5/19/2009	- Changed to derived class from GuiObject, K. Loux.

#ifndef GUI_CAR_H_
#define GUI_CAR_H_

// wxWidgets headers
#include <wx/treebase.h>

// VVASE headers
#include "gui/guiObject.h"
#include "vSolver/physics/kinematicOutputs.h"
#include "vSolver/physics/quasiStaticOutputs.h"

// VVASE forward declarations
class CarRenderer;
class Car;
class AppearanceOptions;

// wxWidgets forward declarations
class wxTreeItemId;

class GuiCar : public GuiObject
{
public:
	GuiCar(MainFrame &mainFrame, wxString pathAndFileName = wxEmptyString);
	~GuiCar();

	// To be called after either the WorkingCar or the OriginalCar
	// has changed to update the analysis data (mandatory overload)
	void UpdateData();
	void UpdateDisplay();

	// Private member accessors
	inline Car& GetOriginalCar() { return *originalCar; }
	inline const Car& GetOriginalCar() const { return *originalCar; }
	inline Car& GetWorkingCar() { return *workingCar; }
	inline const Car& GetWorkingCar() const { return *workingCar; }
	inline AppearanceOptions &GetAppearanceOptions() { return *appearanceOptions; }

	ItemType GetType() const { return TypeCar; }

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

	static wxString GetSubsystemName(Subsystems subsystem);

	// The tree IDs associated with each subsystem object
	wxTreeItemId subsystems[NumberOfSubsystems];
	
	struct CarOutputs
	{
		KinematicOutputs kinematicOutputs;
		QuasiStaticOutputs quasiStaticOutputs;
		
		bool hasQuasiStaticOutputs;
		
		CarOutputs()
		{
			hasQuasiStaticOutputs = false;
		}
	};

	CarOutputs GetOutputs() { return outputs; }
	
	void SetUseOrtho(const bool &useOrtho);

private:
	// The "real" object containing the original, unmodified information (not rolled, heaved, etc.)
	Car *originalCar;

	// Reference to the object that we manipulate as we do our analyses
	Car *workingCar;

	// Performs the saving and loading to/from file (mandatory overloads)
	bool PerformLoadFromFile();
	bool PerformSaveToFile();

	int GetIconHandle() const;

	// Reference to the options that affect this object's appearance
	AppearanceOptions *appearanceOptions;

	// The object that stores the kinematics information for this car
	CarOutputs outputs;

	// The scene visible to the user representing this object
	CarRenderer *renderer;

	void ComputeARBSignConventions();
};

#endif// GUI_CAR_H_