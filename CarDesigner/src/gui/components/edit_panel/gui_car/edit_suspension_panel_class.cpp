/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  edit_suspension_panel_class.cpp
// Created:  2/10/2009
// Author:  K. Loux
// Description:  Contains the class definition for the EDIT_SUSPENSION_PANEL class.  This
//				 class is used to edit the hard points on a particular corner of the
//				 car.
// History:
//	3/28/2009	- Changed action on non-numeric input to "do nothing" instead of "replace
//				  with last value," K. Loux

// CarDesigner headers
#include "vCar/drivetrain_class.h"
#include "vCar/suspension_class.h"
#include "gui/renderer/car_renderer_class.h"
#include "gui/gui_car_class.h"
#include "gui/components/main_frame_class.h"
#include "gui/components/edit_panel/edit_panel_class.h"
#include "gui/components/edit_panel/gui_car/edit_suspension_panel_class.h"
#include "gui/components/edit_panel/gui_car/edit_suspension_notebook_class.h"
#include "vUtilities/convert_class.h"
#include "vMath/vector_class.h"

//==========================================================================
// Class:			EDIT_SUSPENSION_PANEL
// Function:		EDIT_SUSPENSION_PANEL
//
// Description:		Constructor for EDIT_SUSPENSION_PANEL class.  Initializes the form
//					and creates the controls, etc.
//
// Input Arguments:
//		_Parent		= EDIT_SUSPENSION_NOTEBOOK&, reference to this object's owner
//		id			= wxWindowID for passing to parent class's constructor
//		pos			= wxPoint& for passing to parent class's constructor
//		size		= wxSize& for passing to parent class's constructor
//		_debugger	= const Debugger& reference to applications debug printing utility
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
EDIT_SUSPENSION_PANEL::EDIT_SUSPENSION_PANEL(EDIT_SUSPENSION_NOTEBOOK &_Parent, wxWindowID id,
											 const wxPoint& pos, const wxSize& size,
											 const Debugger &_debugger) :
											 wxPanel(&_Parent, id, pos, size),
											 Debugger(_debugger),
											 Converter(_Parent.GetParent().GetMainFrame().GetConverter()),
											 Parent(_Parent)
{
	// Create the controls
	CreateControls();
}

//==========================================================================
// Class:			EDIT_SUSPENSION_PANEL
// Function:		~EDIT_SUSPENSION_PANEL
//
// Description:		Destructor for EDIT_SUSPENSION_PANEL class.
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
EDIT_SUSPENSION_PANEL::~EDIT_SUSPENSION_PANEL()
{
}

//==========================================================================
// Class:			EDIT_SUSPENSION_PANEL
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
BEGIN_EVENT_TABLE(EDIT_SUSPENSION_PANEL, wxPanel)
	EVT_GRID_SELECT_CELL(						EDIT_SUSPENSION_PANEL::SelectCellEvent)
	EVT_GRID_CELL_CHANGE(						EDIT_SUSPENSION_PANEL::GridCellChangedEvent)
	EVT_CHECKBOX(CheckBoxIsSymmetric,			EDIT_SUSPENSION_PANEL::SymmetricCheckboxEvent)
	EVT_CHECKBOX(CheckBoxFrontHasThirdSpring,	EDIT_SUSPENSION_PANEL::FrontThirdCheckboxEvent)
	EVT_CHECKBOX(CheckBoxRearHasThirdSpring,	EDIT_SUSPENSION_PANEL::RearThirdCheckboxEvent)
	EVT_COMBOBOX(ComboBoxFrontBarStyle,			EDIT_SUSPENSION_PANEL::FrontBarStyleChangeEvent)
	EVT_COMBOBOX(ComboBoxRearBarStyle,			EDIT_SUSPENSION_PANEL::RearBarStyleChangeEvent)
END_EVENT_TABLE();

//==========================================================================
// Class:			EDIT_SUSPENSION_PANEL
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
void EDIT_SUSPENSION_PANEL::UpdateInformation(void)
{
	// Make sure the suspension object exists
	if (CurrentSuspension)
		// Call the method that performs the update
		UpdateInformation(CurrentSuspension);

	return;
}

//==========================================================================
// Class:			EDIT_SUSPENSION_PANEL
// Function:		UpdateInformation
//
// Description:		Updates the information on this panel.
//
// Input Arguments:
//		_CurrentSuspension	= SUSPENSION* pointing to the associated suspension
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void EDIT_SUSPENSION_PANEL::UpdateInformation(SUSPENSION *_CurrentSuspension)
{
	// Update the class members
	CurrentSuspension = _CurrentSuspension;

	// Update the check boxes
	IsSymmetric->SetValue(CurrentSuspension->IsSymmetric);
	FrontHasThirdSpring->SetValue(CurrentSuspension->FrontHasThirdSpring);
	RearHasThirdSpring->SetValue(CurrentSuspension->RearHasThirdSpring);

	// Update the combo boxes
	FrontBarStyle->SetSelection(CurrentSuspension->FrontBarStyle);
	RearBarStyle->SetSelection(CurrentSuspension->RearBarStyle);

	// Begin batch edit of the grid
	Hardpoints->BeginBatch();

	// Update the unit labels
	wxString UnitString;
	UnitString.Printf("(%s)", Converter.GetUnitType(Convert::UNIT_TYPE_DISTANCE).c_str());
	Hardpoints->SetCellValue(0, 1, UnitString);
	Hardpoints->SetCellValue(0, 2, UnitString);
	Hardpoints->SetCellValue(0, 3, UnitString);

	// Hide or show rows according to this object's configuration
	// Front third spring
	if (CurrentSuspension->FrontHasThirdSpring)
	{
		Hardpoints->SetRowHeight(SUSPENSION::FrontThirdShockInboard + 1, Hardpoints->GetRowHeight(0));
		Hardpoints->SetRowHeight(SUSPENSION::FrontThirdShockOutboard + 1, Hardpoints->GetRowHeight(0));
		Hardpoints->SetRowHeight(SUSPENSION::FrontThirdSpringInboard + 1, Hardpoints->GetRowHeight(0));
		Hardpoints->SetRowHeight(SUSPENSION::FrontThirdSpringOutboard + 1, Hardpoints->GetRowHeight(0));
	}
	else
	{
		Hardpoints->SetRowHeight(SUSPENSION::FrontThirdShockInboard + 1, 0);
		Hardpoints->SetRowHeight(SUSPENSION::FrontThirdShockOutboard + 1, 0);
		Hardpoints->SetRowHeight(SUSPENSION::FrontThirdSpringInboard + 1, 0);
		Hardpoints->SetRowHeight(SUSPENSION::FrontThirdSpringOutboard + 1, 0);
	}

	// Rear third spring
	if (CurrentSuspension->RearHasThirdSpring)
	{
		Hardpoints->SetRowHeight(SUSPENSION::RearThirdShockInboard + 1, Hardpoints->GetRowHeight(0));
		Hardpoints->SetRowHeight(SUSPENSION::RearThirdShockOutboard + 1, Hardpoints->GetRowHeight(0));
		Hardpoints->SetRowHeight(SUSPENSION::RearThirdSpringInboard + 1, Hardpoints->GetRowHeight(0));
		Hardpoints->SetRowHeight(SUSPENSION::RearThirdSpringOutboard + 1, Hardpoints->GetRowHeight(0));
	}
	else
	{
		Hardpoints->SetRowHeight(SUSPENSION::RearThirdShockInboard + 1, 0);
		Hardpoints->SetRowHeight(SUSPENSION::RearThirdShockOutboard + 1, 0);
		Hardpoints->SetRowHeight(SUSPENSION::RearThirdSpringInboard + 1, 0);
		Hardpoints->SetRowHeight(SUSPENSION::RearThirdSpringOutboard + 1, 0);
	}

	// Front sway bar
	if (CurrentSuspension->FrontBarStyle == SUSPENSION::SwayBarUBar ||
		CurrentSuspension->FrontBarStyle == SUSPENSION::SwayBarTBar)
		Hardpoints->SetRowHeight(SUSPENSION::FrontBarMidPoint + 1, Hardpoints->GetRowHeight(0));
	else
		Hardpoints->SetRowHeight(SUSPENSION::FrontBarMidPoint + 1, 0);

	// Rear swaybar
	if (CurrentSuspension->RearBarStyle == SUSPENSION::SwayBarUBar ||
		CurrentSuspension->RearBarStyle == SUSPENSION::SwayBarTBar)
		Hardpoints->SetRowHeight(SUSPENSION::RearBarMidPoint + 1, Hardpoints->GetRowHeight(0));
	else
		Hardpoints->SetRowHeight(SUSPENSION::RearBarMidPoint + 1, 0);

	// Update the values of all of the points
	Vector Point;
	int i;
	for (i = 0; i < SUSPENSION::NumberOfHardpoints; i++)
	{
		// Get the location of this hardpoint (don't forget to convert it!)
		Point = Converter.ConvertDistance(CurrentSuspension->Hardpoints[i]);

		// Set the X value
		Hardpoints->SetCellValue(i + 1, 1, Converter.FormatNumber(Point.X));

		// Set the Y value
		Hardpoints->SetCellValue(i + 1, 2, Converter.FormatNumber(Point.Y));

		// Set the Z value
		Hardpoints->SetCellValue(i + 1, 3, Converter.FormatNumber(Point.Z));
	}

	// End batch edit of the grid
	Hardpoints->EndBatch();

	return;
}

//==========================================================================
// Class:			EDIT_SUSPENSION_PANEL
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
void EDIT_SUSPENSION_PANEL::CreateControls()
{
	// Top-level sizer
	wxBoxSizer *TopSizer = new wxBoxSizer(wxVERTICAL);

	// Second sizer gives more space around the controls
	wxBoxSizer *MainSizer = new wxBoxSizer(wxVERTICAL);
	TopSizer->Add(MainSizer, 1, wxALIGN_CENTER_HORIZONTAL | wxGROW | wxALL, 5);

	// The column widths
	int LabelColumnWidth = 140;
	int InputColumnWidth = 45;

	// Create the grid for the hard point entry
	Hardpoints = new wxGrid(this, wxID_ANY);
	Hardpoints->CreateGrid(SUSPENSION::NumberOfHardpoints + 1, 4, wxGrid::wxGridSelectRows);

	// Begin a batch edit of the grid
	Hardpoints->BeginBatch();

	// Make make the heading row read-only
	int i;
	for (i = 0; i < Hardpoints->GetNumberCols(); i++)
		Hardpoints->SetReadOnly(0, i, true);

	// Do the processing that needs to be done for each row
	for (i = 0; i < SUSPENSION::NumberOfHardpoints; i++)
	{
		// Make the first column read-only
		Hardpoints->SetReadOnly(i + 1, 0, true);

		// Set the alignment for all of the cells that contain numbers to the right
		Hardpoints->SetCellAlignment(i + 1, 1, wxALIGN_RIGHT, wxALIGN_TOP);
		Hardpoints->SetCellAlignment(i + 1, 2, wxALIGN_RIGHT, wxALIGN_TOP);
		Hardpoints->SetCellAlignment(i + 1, 3, wxALIGN_RIGHT, wxALIGN_TOP);

		// Add the names of all of the points to the grid
		Hardpoints->SetCellValue(i + 1, 0, SUSPENSION::GetHardpointName((SUSPENSION::HARDPOINTS)i));
	}

	// Set the column widths
	Hardpoints->SetColumnWidth(0, LabelColumnWidth);
	Hardpoints->SetColumnWidth(1, InputColumnWidth);
	Hardpoints->SetColumnWidth(2, InputColumnWidth);
	Hardpoints->SetColumnWidth(3, InputColumnWidth);

	// Hide the label column and set the size for the label row
	Hardpoints->SetRowLabelSize(0);
	Hardpoints->SetColLabelSize(Hardpoints->GetRowSize(0));

	// To enable hiding of the non-label rows, we need to set the minimum height to zero
	Hardpoints->SetRowMinimalAcceptableHeight(0);

	// Add the grid to the sizer
	MainSizer->Add(Hardpoints, 1, wxALIGN_CENTER_HORIZONTAL | wxGROW | wxALL | wxALIGN_TOP, 5);

	// Set the column headings
	Hardpoints->SetColLabelValue(0, _T("Hardpoint"));
	Hardpoints->SetColLabelValue(1, _T("X"));
	Hardpoints->SetColLabelValue(2, _T("Y"));
	Hardpoints->SetColLabelValue(3, _T("Z"));

	// Center the cells for the column headings
	for (i = 0; i < Hardpoints->GetNumberCols(); i++)
		// Center the contents
		Hardpoints->SetCellAlignment(0, i, wxALIGN_CENTER, wxALIGN_TOP);

	// Don't let the user move or re-size the rows or move the columns
	Hardpoints->EnableDragColMove(false);
	Hardpoints->EnableDragColSize(true);
	Hardpoints->EnableDragGridSize(false);
	Hardpoints->EnableDragRowSize(false);

	// End the batch mode edit and re-paint the control
	Hardpoints->EndBatch();

	// Re-size the columns
	LabelColumnWidth = 100;
	InputColumnWidth = 120;

	// Create the combo-boxes
	// Front bar style
	wxArrayString Choices;
	Choices.Clear();
	for (i = 0; i < SUSPENSION::NumberOfBarStyles; i++)
		Choices.Add(SUSPENSION::GetBarStyleName((SUSPENSION::BAR_STYLE)i));

	wxBoxSizer *FrontBarSizer = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText *FrontBarLabel = new wxStaticText(this, wxID_ANY,
		_T("Front Sway Bar Style"), wxDefaultPosition, wxSize(LabelColumnWidth, -1));
	FrontBarStyle = new wxComboBox(this, ComboBoxFrontBarStyle, wxEmptyString, wxDefaultPosition,
		wxSize(InputColumnWidth, -1), Choices, wxCB_READONLY);

	FrontBarSizer->Add(FrontBarLabel, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
	FrontBarSizer->Add(FrontBarStyle, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

	// Rear bar style
	wxBoxSizer *RearBarSizer = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText *RearBarLabel = new wxStaticText(this, wxID_ANY,
		_T("Rear Sway Bar Style"), wxDefaultPosition, wxSize(LabelColumnWidth, -1));
	RearBarStyle = new wxComboBox(this, ComboBoxRearBarStyle, wxEmptyString, wxDefaultPosition,
		wxSize(InputColumnWidth, -1), Choices, wxCB_READONLY);

	RearBarSizer->Add(RearBarLabel, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
	RearBarSizer->Add(RearBarStyle, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

	// Create the check boxes
	// Symmetry
	wxBoxSizer *SymmetrySizer = new wxBoxSizer(wxHORIZONTAL);
	IsSymmetric = new wxCheckBox(this, CheckBoxIsSymmetric, _T("Is Symmetric"));
	SymmetrySizer->Add(IsSymmetric, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

	// Front thrid shock/spring
	wxBoxSizer *FrontThirdSizer = new wxBoxSizer(wxHORIZONTAL);
	FrontHasThirdSpring = new wxCheckBox(this, CheckBoxFrontHasThirdSpring, _T("Front Has Third Spring"));
	FrontThirdSizer->Add(FrontHasThirdSpring, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

	// Rear third shock/spring
	wxBoxSizer *RearThirdSizer = new wxBoxSizer(wxHORIZONTAL);
	RearHasThirdSpring = new wxCheckBox(this, CheckBoxRearHasThirdSpring, _T("Rear Has Third Spring"));
	RearThirdSizer->Add(RearHasThirdSpring, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

	// Add the individial sizers to the main sizer
	MainSizer->Add(FrontBarSizer, 0, wxALIGN_BOTTOM);
	MainSizer->Add(RearBarSizer, 0, wxALIGN_BOTTOM);
	MainSizer->Add(SymmetrySizer, 0, wxALIGN_BOTTOM);
	MainSizer->Add(FrontThirdSizer, 0, wxALIGN_BOTTOM);
	MainSizer->Add(RearThirdSizer, 0, wxALIGN_BOTTOM);

	// Assign the top level sizer to the panel
	SetSizer(TopSizer);

	// Initialize the last row selected variable
	LastRowSelected = -1;

	return;
}

//==========================================================================
// Class:			EDIT_SUSPENSION_PANEL
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
void EDIT_SUSPENSION_PANEL::SelectCellEvent(wxGridEvent &event)
{
	// Make sure the selection happened in a valid row
	if (event.GetRow() > 0)
	{
		// Test to see if this row is the same as the previously selected row
		if (LastRowSelected == event.GetRow())
		{
			// If the new row is the same as the old one, don't do anything.  This
			// helps save some unnecessary calls to drawing functions.
			event.Skip();// Important - if we don't skip this event, we can't edit values!

			return;
		}

		// Set the position of the helper orb
		static_cast<CAR_RENDERER*>(Parent.GetParent().GetCurrentObject()->GetRenderer())->SetHelperOrbPosition(
			CORNER::NumberOfHardpoints, CORNER::LocationRightFront, (SUSPENSION::HARDPOINTS)(event.GetRow() - 1));

		// Update the display
		Parent.GetParent().GetCurrentObject()->UpdateDisplay();

		// Update the last row selected variable
		LastRowSelected = event.GetRow();
	}
	else
		// Reset the last row selected variable
		LastRowSelected = -1;

	event.Skip();

	return;
}

//==========================================================================
// Class:			EDIT_SUSPENSION_PANEL
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
void EDIT_SUSPENSION_PANEL::GridCellChangedEvent(wxGridEvent &event)
{
	// Make sure the row is a valid row
	if (event.GetRow() > 0)
	{
		// Get the new value
		wxString ValueString = Hardpoints->GetCellValue(event.GetRow(), event.GetCol());
		double Value;

		// Make sure the value is numeric
		if (!ValueString.ToDouble(&Value))
			// The value is non-numeric - don't do anything
			return;

		// Get a lock on the car
		wxMutex *Mutex = Parent.GetParent().GetCurrentMutex();
		Mutex->Lock();

		// Update the point (don't forget to convert)
		if (event.GetCol() == 1)// X
		{
			// Add the operation to the undo/redo stack
			Parent.GetParent().GetMainFrame().GetUndoRedoStack().AddOperation(
				Parent.GetParent().GetMainFrame().GetActiveIndex(),
				UNDO_REDO_STACK::OPERATION::DATA_TYPE_DOUBLE,
				&(CurrentSuspension->Hardpoints[event.GetRow() - 1].X));

			CurrentSuspension->Hardpoints[event.GetRow() - 1].X = Converter.ReadDistance(Value);
		}
		else if (event.GetCol() == 2)// Y
		{
			// Add the operation to the undo/redo stack
			Parent.GetParent().GetMainFrame().GetUndoRedoStack().AddOperation(
				Parent.GetParent().GetMainFrame().GetActiveIndex(),
				UNDO_REDO_STACK::OPERATION::DATA_TYPE_DOUBLE,
				&(CurrentSuspension->Hardpoints[event.GetRow() - 1].Y));

			CurrentSuspension->Hardpoints[event.GetRow() - 1].Y = Converter.ReadDistance(Value);
		}
		else// Z
		{
			// Add the operation to the undo/redo stack
			Parent.GetParent().GetMainFrame().GetUndoRedoStack().AddOperation(
				Parent.GetParent().GetMainFrame().GetActiveIndex(),
				UNDO_REDO_STACK::OPERATION::DATA_TYPE_DOUBLE,
				&(CurrentSuspension->Hardpoints[event.GetRow() - 1].Z));

			CurrentSuspension->Hardpoints[event.GetRow() - 1].Z = Converter.ReadDistance(Value);
		}

		// Unlock the car
		Mutex->Unlock();

		// Tell the car object that it was modified
		Parent.GetParent().GetCurrentObject()->SetModified();

		// Set the position of the helper orb
		static_cast<CAR_RENDERER*>(Parent.GetParent().GetCurrentObject()->GetRenderer())->SetHelperOrbPosition(
			CORNER::NumberOfHardpoints, CORNER::LocationRightFront, (SUSPENSION::HARDPOINTS)(event.GetRow() - 1));

		// Update the display and the kinematic outputs
		Parent.GetParent().GetMainFrame().UpdateAnalysis();
		Parent.GetParent().GetMainFrame().UpdateOutputPanel();
	}

	event.Skip();

	return;
}

//==========================================================================
// Class:			EDIT_SUSPENSION_PANEL
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
void EDIT_SUSPENSION_PANEL::SymmetricCheckboxEvent(wxCommandEvent &event)
{
	// Add the operation to the undo/redo stack
	Parent.GetParent().GetMainFrame().GetUndoRedoStack().AddOperation(
		Parent.GetParent().GetMainFrame().GetActiveIndex(),
		UNDO_REDO_STACK::OPERATION::DATA_TYPE_BOOL,
		&(CurrentSuspension->IsSymmetric));

	// Get a lock on the car
	wxMutex *Mutex = Parent.GetParent().GetCurrentMutex();
	Mutex->Lock();

	// Set the value of the symmetry flag to the value of this checkbox
	CurrentSuspension->IsSymmetric = event.IsChecked();

	// Unlock the car
	Mutex->Unlock();

	// In the case that this was symmetric but is not any longer, we should
	// capture a pointer to the current object so that we can still update the
	// display, even after this page has been deleted and re-created at a different
	// address.
	GUI_OBJECT *TempCurrentObject = Parent.GetParent().GetCurrentObject();

	// Call the update function for the parent control (normally, we just do
	// the update method for this object, but here we might need to add/remove
	// tabs from the notebook).
	Parent.GetParent().UpdateInformation();

	// Tell the car object that it was modified
	TempCurrentObject->SetModified();

	// Update the display and the kinematic outputs
	TempCurrentObject->GetMainFrame().UpdateAnalysis();
	TempCurrentObject->GetMainFrame().UpdateOutputPanel();

	return;
}

//==========================================================================
// Class:			EDIT_SUSPENSION_PANEL
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
void EDIT_SUSPENSION_PANEL::FrontThirdCheckboxEvent(wxCommandEvent &event)
{
	// Add the operation to the undo/redo stack
	Parent.GetParent().GetMainFrame().GetUndoRedoStack().AddOperation(
		Parent.GetParent().GetMainFrame().GetActiveIndex(),
		UNDO_REDO_STACK::OPERATION::DATA_TYPE_BOOL,
		&(CurrentSuspension->FrontHasThirdSpring));

	// Get a lock on the car
	wxMutex *Mutex = Parent.GetParent().GetCurrentMutex();
	Mutex->Lock();

	// Set the value of the front third spring flag to the value of this checkbox
	CurrentSuspension->FrontHasThirdSpring = event.IsChecked();

	// Unlock the car
	Mutex->Unlock();

	// Call the update function for this object
	UpdateInformation();

	// Tell the car object that it was modified
	Parent.GetParent().GetCurrentObject()->SetModified();

	// Update the display and the kinematic outputs
	Parent.GetParent().GetMainFrame().UpdateAnalysis();
	Parent.GetParent().GetMainFrame().UpdateOutputPanel();

	event.Skip();

	return;
}

//==========================================================================
// Class:			EDIT_SUSPENSION_PANEL
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
void EDIT_SUSPENSION_PANEL::RearThirdCheckboxEvent(wxCommandEvent &event)
{
	// Add the operation to the undo/redo stack
	Parent.GetParent().GetMainFrame().GetUndoRedoStack().AddOperation(
		Parent.GetParent().GetMainFrame().GetActiveIndex(),
		UNDO_REDO_STACK::OPERATION::DATA_TYPE_BOOL,
		&(CurrentSuspension->RearHasThirdSpring));

	// Get a lock on the car
	wxMutex *Mutex = Parent.GetParent().GetCurrentMutex();
	Mutex->Lock();

	// Set the value of the rear third spring flag to the value of this checkbox
	CurrentSuspension->RearHasThirdSpring = event.IsChecked();

	// Unlock the car
	Mutex->Unlock();

	// Call the update function for this object
	UpdateInformation();

	// Tell the car object that it was modified
	Parent.GetParent().GetCurrentObject()->SetModified();

	// Update the display and the kinematic outputs
	Parent.GetParent().GetMainFrame().UpdateAnalysis();
	Parent.GetParent().GetMainFrame().UpdateOutputPanel();

	event.Skip();

	return;
}

//==========================================================================
// Class:			EDIT_SUSPENSION_PANEL
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
void EDIT_SUSPENSION_PANEL::FrontBarStyleChangeEvent(wxCommandEvent &event)
{
	// Add the operation to the undo/redo stack
	Parent.GetParent().GetMainFrame().GetUndoRedoStack().AddOperation(
		Parent.GetParent().GetMainFrame().GetActiveIndex(),
		UNDO_REDO_STACK::OPERATION::DATA_TYPE_INTEGER,
		&(CurrentSuspension->FrontBarStyle));

	// Get a lock on the car
	wxMutex *Mutex = Parent.GetParent().GetCurrentMutex();
	Mutex->Lock();

	// Set the value of the front bar style flag to the value of this checkbox
	CurrentSuspension->FrontBarStyle = (SUSPENSION::BAR_STYLE)event.GetSelection();

	// Unlock the car
	Mutex->Unlock();

	// Call the update function for the parent object
	Parent.UpdateInformation();

	// Tell the car object that it was modified
	Parent.GetParent().GetCurrentObject()->SetModified();

	// Update the display and the kinematic outputs
	Parent.GetParent().GetMainFrame().UpdateAnalysis();
	Parent.GetParent().GetMainFrame().UpdateOutputPanel();

	return;
}

//==========================================================================
// Class:			EDIT_SUSPENSION_PANEL
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
void EDIT_SUSPENSION_PANEL::RearBarStyleChangeEvent(wxCommandEvent &event)
{
	// Add the operation to the undo/redo stack
	Parent.GetParent().GetMainFrame().GetUndoRedoStack().AddOperation(
		Parent.GetParent().GetMainFrame().GetActiveIndex(),
		UNDO_REDO_STACK::OPERATION::DATA_TYPE_INTEGER,
		&(CurrentSuspension->RearBarStyle));

	// Get a lock on the car
	wxMutex *Mutex = Parent.GetParent().GetCurrentMutex();
	Mutex->Lock();

	// Set the value of the rear bar style flag to the value of this checkbox
	CurrentSuspension->RearBarStyle = (SUSPENSION::BAR_STYLE)event.GetSelection();

	// Unlock the car
	Mutex->Unlock();

	// Call the update function for the parent object
	Parent.UpdateInformation();

	// Tell the car object that it was modified
	Parent.GetParent().GetCurrentObject()->SetModified();

	// Update the display and the kinematic outputs
	Parent.GetParent().GetMainFrame().UpdateAnalysis();
	Parent.GetParent().GetMainFrame().UpdateOutputPanel();

	return;
}