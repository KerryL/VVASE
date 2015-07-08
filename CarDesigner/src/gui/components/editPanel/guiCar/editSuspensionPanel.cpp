/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  editSuspensionPanel.cpp
// Created:  2/10/2009
// Author:  K. Loux
// Description:  Contains the class definition for the EditSuspensionPanel class.  This
//				 class is used to edit the hard points on a particular corner of the
//				 car.
// History:
//	3/28/2009	- Changed action on non-numeric input to "do nothing" instead of "replace
//				  with last value," K. Loux

// CarDesigner headers
#include "vCar/drivetrain.h"
#include "vCar/suspension.h"
#include "gui/renderer/carRenderer.h"
#include "gui/guiCar.h"
#include "gui/superGrid.h"
#include "gui/components/mainFrame.h"
#include "gui/components/editPanel/editPanel.h"
#include "gui/components/editPanel/guiCar/editSuspensionPanel.h"
#include "gui/components/editPanel/guiCar/editSuspensionNotebook.h"
#include "vUtilities/convert.h"
#include "vUtilities/wxRelatedUtilities.h"
#include "vMath/vector.h"

//==========================================================================
// Class:			EditSuspensionPanel
// Function:		EditSuspensionPanel
//
// Description:		Constructor for EditSuspensionPanel class.  Initializes the form
//					and creates the controls, etc.
//
// Input Arguments:
//		_parent		= EditSuspensionNotebook&, reference to this object's owner
//		id			= wxWindowID for passing to parent class's constructor
//		pos			= wxPoint& for passing to parent class's constructor
//		size		= wxSize& for passing to parent class's constructor
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
EditSuspensionPanel::EditSuspensionPanel(EditSuspensionNotebook &_parent, wxWindowID id,
											 const wxPoint& pos, const wxSize& size) :
											 wxScrolledWindow(&_parent, id, pos, size),
											 parent(_parent)
{
	// Create the controls
	CreateControls();
}

//==========================================================================
// Class:			EditSuspensionPanel
// Function:		~EditSuspensionPanel
//
// Description:		Destructor for EditSuspensionPanel class.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
EditSuspensionPanel::~EditSuspensionPanel()
{
}

//==========================================================================
// Class:			EditSuspensionPanel
// Function:		Event Table
//
// Description:		Links GUI events with event handler functions.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
BEGIN_EVENT_TABLE(EditSuspensionPanel, wxPanel)
	EVT_GRID_SELECT_CELL(						EditSuspensionPanel::SelectCellEvent)
	EVT_GRID_CELL_CHANGE(						EditSuspensionPanel::GridCellChangedEvent)
	EVT_CHECKBOX(CheckBoxIsSymmetric,			EditSuspensionPanel::SymmetricCheckboxEvent)
	EVT_CHECKBOX(CheckBoxFrontHasThirdSpring,	EditSuspensionPanel::FrontThirdCheckboxEvent)
	EVT_CHECKBOX(CheckBoxRearHasThirdSpring,	EditSuspensionPanel::RearThirdCheckboxEvent)
	EVT_COMBOBOX(ComboBoxFrontBarStyle,			EditSuspensionPanel::FrontBarStyleChangeEvent)
	EVT_COMBOBOX(ComboBoxRearBarStyle,			EditSuspensionPanel::RearBarStyleChangeEvent)
END_EVENT_TABLE();

//==========================================================================
// Class:			EditSuspensionPanel
// Function:		UpdateInformation
//
// Description:		Updates the information on this panel, if the associated
//					object already exists.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void EditSuspensionPanel::UpdateInformation(void)
{
	// Make sure the suspension object exists
	if (currentSuspension)
		// Call the method that performs the update
		UpdateInformation(currentSuspension);
}

//==========================================================================
// Class:			EditSuspensionPanel
// Function:		UpdateInformation
//
// Description:		Updates the information on this panel.
//
// Input Arguments:
//		_currentSuspension	= Suspension* pointing to the associated suspension
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void EditSuspensionPanel::UpdateInformation(Suspension *_currentSuspension)
{
	// Update the class members
	currentSuspension = _currentSuspension;

	// Update the check boxes
	isSymmetric->SetValue(currentSuspension->isSymmetric);
	frontHasThirdSpring->SetValue(currentSuspension->frontHasThirdSpring);
	rearHasThirdSpring->SetValue(currentSuspension->rearHasThirdSpring);

	// Update the combo boxes
	frontBarStyle->SetSelection(currentSuspension->frontBarStyle);
	rearBarStyle->SetSelection(currentSuspension->rearBarStyle);

	// Begin batch edit of the grid
	hardpoints->BeginBatch();

	// Update the unit labels
	wxString UnitString;
	UnitString.Printf("(%s)", Convert::GetInstance().GetUnitType(Convert::UnitTypeDistance).c_str());
	hardpoints->SetCellValue(0, 1, UnitString);
	hardpoints->SetCellValue(0, 2, UnitString);
	hardpoints->SetCellValue(0, 3, UnitString);

	// Hide or show rows according to this object's configuration
	// Front third spring
	if (currentSuspension->frontHasThirdSpring)
	{
		hardpoints->SetRowHeight(Suspension::FrontThirdShockInboard + 1, hardpoints->GetRowHeight(0));
		hardpoints->SetRowHeight(Suspension::FrontThirdShockOutboard + 1, hardpoints->GetRowHeight(0));
		hardpoints->SetRowHeight(Suspension::FrontThirdSpringInboard + 1, hardpoints->GetRowHeight(0));
		hardpoints->SetRowHeight(Suspension::FrontThirdSpringOutboard + 1, hardpoints->GetRowHeight(0));
	}
	else
	{
		hardpoints->SetRowHeight(Suspension::FrontThirdShockInboard + 1, 0);
		hardpoints->SetRowHeight(Suspension::FrontThirdShockOutboard + 1, 0);
		hardpoints->SetRowHeight(Suspension::FrontThirdSpringInboard + 1, 0);
		hardpoints->SetRowHeight(Suspension::FrontThirdSpringOutboard + 1, 0);
	}

	// Rear third spring
	if (currentSuspension->rearHasThirdSpring)
	{
		hardpoints->SetRowHeight(Suspension::RearThirdShockInboard + 1, hardpoints->GetRowHeight(0));
		hardpoints->SetRowHeight(Suspension::RearThirdShockOutboard + 1, hardpoints->GetRowHeight(0));
		hardpoints->SetRowHeight(Suspension::RearThirdSpringInboard + 1, hardpoints->GetRowHeight(0));
		hardpoints->SetRowHeight(Suspension::RearThirdSpringOutboard + 1, hardpoints->GetRowHeight(0));
	}
	else
	{
		hardpoints->SetRowHeight(Suspension::RearThirdShockInboard + 1, 0);
		hardpoints->SetRowHeight(Suspension::RearThirdShockOutboard + 1, 0);
		hardpoints->SetRowHeight(Suspension::RearThirdSpringInboard + 1, 0);
		hardpoints->SetRowHeight(Suspension::RearThirdSpringOutboard + 1, 0);
	}

	// Front sway bar
	// Removed bar mid points from list - this should be automatically calculated by VVASE
	/*if (currentSuspension->frontBarStyle == Suspension::SwayBarUBar ||
		currentSuspension->frontBarStyle == Suspension::SwayBarTBar)
		hardpoints->SetRowHeight(Suspension::FrontBarMidPoint + 1, hardpoints->GetRowHeight(0));
	else*/
		hardpoints->SetRowHeight(Suspension::FrontBarMidPoint + 1, 0);

	// Rear swaybar
	/*if (currentSuspension->rearBarStyle == Suspension::SwayBarUBar ||
		currentSuspension->rearBarStyle == Suspension::SwayBarTBar)
		hardpoints->SetRowHeight(Suspension::RearBarMidPoint + 1, hardpoints->GetRowHeight(0));
	else*/
		hardpoints->SetRowHeight(Suspension::RearBarMidPoint + 1, 0);

	// Update the values of all of the points
	Vector point;
	int i;
	for (i = 0; i < Suspension::NumberOfHardpoints; i++)
	{
		// Get the location of this hardpoint (don't forget to convert it!)
		point = Convert::GetInstance().ConvertDistance(currentSuspension->hardpoints[i]);

		// Set the X value
		hardpoints->SetCellValue(i + 1, 1, Convert::GetInstance().FormatNumber(point.x));

		// Set the Y value
		hardpoints->SetCellValue(i + 1, 2, Convert::GetInstance().FormatNumber(point.y));

		// Set the Z value
		hardpoints->SetCellValue(i + 1, 3, Convert::GetInstance().FormatNumber(point.z));
	}

	// Adjust the height of the grid based on the number of rows
	hardpoints->FitHeight();

	// End batch edit of the grid
	hardpoints->EndBatch();

	// Resize the sizers in case hardpoint rows were hidden or shown
	Layout();

	// FIXME:  Need way to turn grid scrollbars off
}

//==========================================================================
// Class:			EditSuspensionPanel
// Function:		CreateControls
//
// Description:		Creates the controls for this panel.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void EditSuspensionPanel::CreateControls()
{
	// Enable scrolling
	SetScrollRate(1, 1);

	// Top-level sizer
	wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);

	// Second sizer gives more space around the controls
	wxFlexGridSizer *mainSizer = new wxFlexGridSizer(1, 2, 2);
	topSizer->Add(mainSizer, 1, wxALIGN_CENTER_HORIZONTAL | wxGROW | wxALL, 5);

	// Create the grid for the hard point entry
	hardpoints = new SuperGrid(this, wxID_ANY);
	hardpoints->CreateGrid(Suspension::NumberOfHardpoints + 1, 4, wxGrid::wxGridSelectRows);

	// Begin a batch edit of the grid
	hardpoints->BeginBatch();

	// Make make the heading row read-only
	int i;
	for (i = 0; i < hardpoints->GetNumberCols(); i++)
		hardpoints->SetReadOnly(0, i, true);

	// Do the processing that needs to be done for each row
	for (i = 0; i < Suspension::NumberOfHardpoints; i++)
	{
		// Make the first column read-only
		hardpoints->SetReadOnly(i + 1, 0, true);

		// Set the alignment for all of the cells that contain numbers to the right
		hardpoints->SetCellAlignment(i + 1, 1, wxALIGN_RIGHT, wxALIGN_TOP);
		hardpoints->SetCellAlignment(i + 1, 2, wxALIGN_RIGHT, wxALIGN_TOP);
		hardpoints->SetCellAlignment(i + 1, 3, wxALIGN_RIGHT, wxALIGN_TOP);

		// Add the names of all of the points to the grid
		hardpoints->SetCellValue(i + 1, 0, Suspension::GetHardpointName((Suspension::Hardpoints)i));
	}

	// Hide the label column and set the size for the label row
	hardpoints->SetRowLabelSize(0);
	hardpoints->SetColLabelSize(hardpoints->GetRowSize(0));

	// To enable hiding of the non-label rows, we need to set the minimum height to zero
	hardpoints->SetRowMinimalAcceptableHeight(0);

	// Add the grid to the sizer
	mainSizer->Add(hardpoints, 0, wxALIGN_TOP | wxEXPAND);
	mainSizer->AddGrowableCol(0);

	// Set the column headings
	hardpoints->SetColLabelValue(0, _T("Hardpoint"));
	hardpoints->SetColLabelValue(1, _T("X"));
	hardpoints->SetColLabelValue(2, _T("Y"));
	hardpoints->SetColLabelValue(3, _T("Z"));

	// Center the cells for the column headings
	for (i = 0; i < hardpoints->GetNumberCols(); i++)
		// Center the contents
		hardpoints->SetCellAlignment(0, i, wxALIGN_CENTER, wxALIGN_TOP);

	// Size the columns
	// The X, Y, and Z columns should be big enough to fit 80.0 as formatted
	// by the converter.  First column is stretchable
	hardpoints->SetCellValue(3, 3, Convert::GetInstance().FormatNumber(80.0));
	hardpoints->AutoSizeColumn(3);
	hardpoints->SetColumnWidth(1, hardpoints->GetColSize(3));
	hardpoints->SetColumnWidth(2, hardpoints->GetColSize(3));
	// The value we just put in cell (3,3) will get overwritten with a call to UpdateInformation()
	hardpoints->AutoStretchColumn(0);

	// Don't let the user move or re-size the rows or move the columns
	hardpoints->EnableDragColMove(false);
	hardpoints->EnableDragColSize(true);
	hardpoints->EnableDragGridSize(false);
	hardpoints->EnableDragRowSize(false);

	// End the batch mode edit and re-paint the control
	hardpoints->EndBatch();

// When setting the control width, we need to account for the width of the
	// "expand" button, etc., so we specify that here
#ifdef __WXGTK__
	unsigned int additionalWidth = 40;
#else
	unsigned int additionalWidth = 30;
#endif

	// Create the combo-boxes
	// Front bar style
	wxArrayString choices;
	choices.Clear();
	for (i = 0; i < Suspension::NumberOfBarStyles; i++)
		choices.Add(Suspension::GetBarStyleName((Suspension::BarStyle)i));

	wxFlexGridSizer *comboSizer = new wxFlexGridSizer(2, 3, 3);
	comboSizer->SetFlexibleDirection(wxHORIZONTAL);
	mainSizer->Add(comboSizer, 0, wxEXPAND);
	wxStaticText *frontBarLabel = new wxStaticText(this, wxID_ANY, _T("Front Sway Bar Style"));
	frontBarStyle = new wxComboBox(this, ComboBoxFrontBarStyle, wxEmptyString, wxDefaultPosition,
		wxDefaultSize, choices, wxCB_READONLY);
	SetMinimumWidthFromContents(frontBarStyle, additionalWidth);

	comboSizer->Add(frontBarLabel, 0, wxALIGN_CENTER_VERTICAL);
	comboSizer->Add(frontBarStyle, 0, wxEXPAND);

	// Rear bar style
	wxStaticText *rearBarLabel = new wxStaticText(this, wxID_ANY, _T("Rear Sway Bar Style"));
	rearBarStyle = new wxComboBox(this, ComboBoxRearBarStyle, wxEmptyString, wxDefaultPosition,
		wxDefaultSize, choices, wxCB_READONLY);
	SetMinimumWidthFromContents(rearBarStyle, additionalWidth);

	comboSizer->Add(rearBarLabel, 0, wxALIGN_CENTER_VERTICAL);
	comboSizer->Add(rearBarStyle, 0, wxEXPAND);

	// Create the check boxes
	// Symmetry
	isSymmetric = new wxCheckBox(this, CheckBoxIsSymmetric, _T("Is Symmetric"));
	mainSizer->Add(isSymmetric, 0, wxALIGN_LEFT);

	// Front third shock/spring
	frontHasThirdSpring = new wxCheckBox(this, CheckBoxFrontHasThirdSpring, _T("Front Has Third Spring"));
	mainSizer->Add(frontHasThirdSpring, 0, wxALIGN_LEFT);

	// Rear third shock/spring
	rearHasThirdSpring = new wxCheckBox(this, CheckBoxRearHasThirdSpring, _T("Rear Has Third Spring"));
	mainSizer->Add(rearHasThirdSpring, 0, wxALIGN_LEFT);

	// Assign the top level sizer to the panel
	SetSizer(topSizer);

	// Initialize the last row selected variable
	lastRowSelected = -1;
}

//==========================================================================
// Class:			EditSuspensionPanel
// Function:		SelectCellEvent
//
// Description:		Event that fires when a cell is clicked.  Highlights the
//					corresponding point in the 3D panel.
//
// Input Arguments:
//		event = wxGridEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void EditSuspensionPanel::SelectCellEvent(wxGridEvent &event)
{
	// Make sure the selection happened in a valid row
	if (event.GetRow() > 0)
	{
		// Test to see if this row is the same as the previously selected row
		if (lastRowSelected == event.GetRow())
		{
			// If the new row is the same as the old one, don't do anything.  This
			// helps save some unnecessary calls to drawing functions.
			event.Skip();// Important - if we don't skip this event, we can't edit values!

			return;
		}

		// Set the position of the helper orb
		static_cast<CarRenderer*>(parent.GetParent().GetCurrentObject()->GetNotebookTab())->SetHelperOrbPosition(
			Corner::NumberOfHardpoints, Corner::LocationRightFront, (Suspension::Hardpoints)(event.GetRow() - 1));

		// Update the display
		parent.GetParent().GetCurrentObject()->UpdateDisplay();

		// Update the last row selected variable
		lastRowSelected = event.GetRow();
	}
	else
		// Reset the last row selected variable
		lastRowSelected = -1;

	event.Skip();
}

//==========================================================================
// Class:			EditSuspensionPanel
// Function:		GridCellChangedEvent
//
// Description:		Event that fires when a cell is changed.  Updates the
//					3D model display.
//
// Input Arguments:
//		event = wxGridEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void EditSuspensionPanel::GridCellChangedEvent(wxGridEvent &event)
{
	// Make sure the row is a valid row
	if (event.GetRow() > 0)
	{
		// Get the new value
		wxString valueString = hardpoints->GetCellValue(event.GetRow(), event.GetCol());
		double value;

		// Make sure the value is numeric
		if (!valueString.ToDouble(&value))
			// The value is non-numeric - don't do anything
			return;

		// Get a lock on the car
		wxMutex *mutex = parent.GetParent().GetCurrentMutex();
		mutex->Lock();

		// Update the point (don't forget to convert)
		if (event.GetCol() == 1)// X
		{
			// Add the operation to the undo/redo stack
			parent.GetParent().GetMainFrame().GetUndoRedoStack().AddOperation(
				parent.GetParent().GetMainFrame().GetActiveIndex(),
				UndoRedoStack::Operation::DataTypeDouble,
				&(currentSuspension->hardpoints[event.GetRow() - 1].x));

			currentSuspension->hardpoints[event.GetRow() - 1].x = Convert::GetInstance().ReadDistance(value);
		}
		else if (event.GetCol() == 2)// Y
		{
			// Add the operation to the undo/redo stack
			parent.GetParent().GetMainFrame().GetUndoRedoStack().AddOperation(
				parent.GetParent().GetMainFrame().GetActiveIndex(),
				UndoRedoStack::Operation::DataTypeDouble,
				&(currentSuspension->hardpoints[event.GetRow() - 1].y));

			currentSuspension->hardpoints[event.GetRow() - 1].y = Convert::GetInstance().ReadDistance(value);
		}
		else// Z
		{
			// Add the operation to the undo/redo stack
			parent.GetParent().GetMainFrame().GetUndoRedoStack().AddOperation(
				parent.GetParent().GetMainFrame().GetActiveIndex(),
				UndoRedoStack::Operation::DataTypeDouble,
				&(currentSuspension->hardpoints[event.GetRow() - 1].z));

			currentSuspension->hardpoints[event.GetRow() - 1].z = Convert::GetInstance().ReadDistance(value);
		}

		// Unlock the car
		mutex->Unlock();

		// Tell the car object that it was modified
		parent.GetParent().GetCurrentObject()->SetModified();

		// Set the position of the helper orb
		static_cast<CarRenderer*>(parent.GetParent().GetCurrentObject()->GetNotebookTab())->SetHelperOrbPosition(
			Corner::NumberOfHardpoints, Corner::LocationRightFront, (Suspension::Hardpoints)(event.GetRow() - 1));

		// Update the display and the kinematic outputs
		parent.GetParent().GetMainFrame().UpdateAnalysis();
		parent.GetParent().GetMainFrame().UpdateOutputPanel();
	}

	event.Skip();
}

//==========================================================================
// Class:			EditSuspensionPanel
// Function:		SymmetricCheckboxEvent
//
// Description:		Event that fires when the symmetry checkbox is clicked.
//
// Input Arguments:
//		event = wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void EditSuspensionPanel::SymmetricCheckboxEvent(wxCommandEvent &event)
{
	// Add the operation to the undo/redo stack
	parent.GetParent().GetMainFrame().GetUndoRedoStack().AddOperation(
		parent.GetParent().GetMainFrame().GetActiveIndex(),
		UndoRedoStack::Operation::DataTypeBool,
		&(currentSuspension->isSymmetric));

	// Get a lock on the car
	wxMutex *mutex = parent.GetParent().GetCurrentMutex();
	mutex->Lock();

	// Set the value of the symmetry flag to the value of this checkbox
	currentSuspension->isSymmetric = event.IsChecked();

	// Unlock the car
	mutex->Unlock();

	// In the case that this was symmetric but is not any longer, we should
	// capture a pointer to the current object so that we can still update the
	// display, even after this page has been deleted and re-created at a different
	// address.
	GuiObject *tempCurrentObject = parent.GetParent().GetCurrentObject();

	// Call the update function for the parent control (normally, we just do
	// the update method for this object, but here we might need to add/remove
	// tabs from the notebook).
	parent.GetParent().UpdateInformation();

	// Tell the car object that it was modified
	tempCurrentObject->SetModified();

	// Update the display and the kinematic outputs
	tempCurrentObject->GetMainFrame().UpdateAnalysis();
	tempCurrentObject->GetMainFrame().UpdateOutputPanel();
}

//==========================================================================
// Class:			EditSuspensionPanel
// Function:		FrontThirdCheckboxEvent
//
// Description:		Event that fires when the front third spring checkbox is
//					clicked.
//
// Input Arguments:
//		event = wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void EditSuspensionPanel::FrontThirdCheckboxEvent(wxCommandEvent &event)
{
	// Add the operation to the undo/redo stack
	parent.GetParent().GetMainFrame().GetUndoRedoStack().AddOperation(
		parent.GetParent().GetMainFrame().GetActiveIndex(),
		UndoRedoStack::Operation::DataTypeBool,
		&(currentSuspension->frontHasThirdSpring));

	// Get a lock on the car
	wxMutex *mutex = parent.GetParent().GetCurrentMutex();
	mutex->Lock();

	// Set the value of the front third spring flag to the value of this checkbox
	currentSuspension->frontHasThirdSpring = event.IsChecked();

	// Unlock the car
	mutex->Unlock();

	// Call the update function for this object
	UpdateInformation();

	// Tell the car object that it was modified
	parent.GetParent().GetCurrentObject()->SetModified();

	// Update the display and the kinematic outputs
	parent.GetParent().GetMainFrame().UpdateAnalysis();
	parent.GetParent().GetMainFrame().UpdateOutputPanel();

	event.Skip();
}

//==========================================================================
// Class:			EditSuspensionPanel
// Function:		RearThirdCheckboxEvent
//
// Description:		Event that fires when the rear third spring checkbox is
//					clicked.
//
// Input Arguments:
//		event = wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void EditSuspensionPanel::RearThirdCheckboxEvent(wxCommandEvent &event)
{
	// Add the operation to the undo/redo stack
	parent.GetParent().GetMainFrame().GetUndoRedoStack().AddOperation(
		parent.GetParent().GetMainFrame().GetActiveIndex(),
		UndoRedoStack::Operation::DataTypeBool,
		&(currentSuspension->rearHasThirdSpring));

	// Get a lock on the car
	wxMutex *mutex = parent.GetParent().GetCurrentMutex();
	mutex->Lock();

	// Set the value of the rear third spring flag to the value of this checkbox
	currentSuspension->rearHasThirdSpring = event.IsChecked();

	// Unlock the car
	mutex->Unlock();

	// Call the update function for this object
	UpdateInformation();

	// Tell the car object that it was modified
	parent.GetParent().GetCurrentObject()->SetModified();

	// Update the display and the kinematic outputs
	parent.GetParent().GetMainFrame().UpdateAnalysis();
	parent.GetParent().GetMainFrame().UpdateOutputPanel();

	event.Skip();
}

//==========================================================================
// Class:			EditSuspensionPanel
// Function:		FrontBarStyleChangeEvent
//
// Description:		Event that fires when the front bar style changes.
//
// Input Arguments:
//		event = wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void EditSuspensionPanel::FrontBarStyleChangeEvent(wxCommandEvent &event)
{
	// Add the operation to the undo/redo stack
	parent.GetParent().GetMainFrame().GetUndoRedoStack().AddOperation(
		parent.GetParent().GetMainFrame().GetActiveIndex(),
		UndoRedoStack::Operation::DataTypeInteger,
		&(currentSuspension->frontBarStyle));

	// Get a lock on the car
	wxMutex *mutex = parent.GetParent().GetCurrentMutex();
	mutex->Lock();

	// Set the value of the front bar style flag to the value of this checkbox
	currentSuspension->frontBarStyle = (Suspension::BarStyle)event.GetSelection();

	// Unlock the car
	mutex->Unlock();

	// Call the update function for the parent object
	parent.UpdateInformation();

	// Tell the car object that it was modified
	parent.GetParent().GetCurrentObject()->SetModified();

	// Update the display and the kinematic outputs
	parent.GetParent().GetMainFrame().UpdateAnalysis();
	parent.GetParent().GetMainFrame().UpdateOutputPanel();
}

//==========================================================================
// Class:			EditSuspensionPanel
// Function:		RearBarStyleChangeEvent
//
// Description:		Event that fires when the rear bar style changes.
//
// Input Arguments:
//		event = wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void EditSuspensionPanel::RearBarStyleChangeEvent(wxCommandEvent &event)
{
	// Add the operation to the undo/redo stack
	parent.GetParent().GetMainFrame().GetUndoRedoStack().AddOperation(
		parent.GetParent().GetMainFrame().GetActiveIndex(),
		UndoRedoStack::Operation::DataTypeInteger,
		&(currentSuspension->rearBarStyle));

	// Get a lock on the car
	wxMutex *mutex = parent.GetParent().GetCurrentMutex();
	mutex->Lock();

	// Set the value of the rear bar style flag to the value of this checkbox
	currentSuspension->rearBarStyle = (Suspension::BarStyle)event.GetSelection();

	// Unlock the car
	mutex->Unlock();

	// Call the update function for the parent object
	parent.UpdateInformation();

	// Tell the car object that it was modified
	parent.GetParent().GetCurrentObject()->SetModified();

	// Update the display and the kinematic outputs
	parent.GetParent().GetMainFrame().UpdateAnalysis();
	parent.GetParent().GetMainFrame().UpdateOutputPanel();
}
