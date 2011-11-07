/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  edit_corner_panel_class.cpp
// Created:  2/10/2009
// Author:  K. Loux
// Description:  Contains the class definition for the EDIT_CORNER_PANEL class.  This
//				 class is used to edit the hard points on a particular corner of the
//				 car.
// History:
//	2/26/2009	- Changed text box initialization/updating to use ChangeValue() function
//				  instead of SetValue() function to avoid triggering unnecessary events, K. Loux.

// VVASE headers
#include "vCar/corner_class.h"
#include "vCar/car_class.h"
#include "gui/gui_car_class.h"
#include "gui/renderer/car_renderer_class.h"
#include "gui/components/main_frame_class.h"
#include "gui/components/edit_panel/edit_panel_class.h"
#include "gui/components/edit_panel/gui_car/edit_corner_panel_class.h"
#include "gui/components/edit_panel/gui_car/edit_suspension_notebook_class.h"
#include "vUtilities/convert_class.h"
#include "vMath/vector_class.h"

//==========================================================================
// Class:			EDIT_CORNER_PANEL
// Function:		EDIT_CORNER_PANEL
//
// Description:		Constructor for EDIT_CORNER_PANEL class.  Initializes the form
//					and creates the controls, etc.
//
// Input Arguments:
//		_Parent		= EDIT_SUSPENSION_NOTEBOOK&, referenc to this object's owner
//		id			= wxWindowID for passing to parent class's constructor
//		pos			= wxPoint& for passing to parent class's constructor
//		size		= wxSize& for passing to parent class's constructor
//		_Debugger	= const DEBUGGER& reference to applications debug printing utility
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
EDIT_CORNER_PANEL::EDIT_CORNER_PANEL(EDIT_SUSPENSION_NOTEBOOK & _Parent, wxWindowID id,
									 const wxPoint& pos, const wxSize& size,
									 const DEBUGGER &_Debugger) : wxPanel(&_Parent, id, pos, size),
									 Debugger(_Debugger),
									 Converter(_Parent.GetParent().GetMainFrame().GetConverter()),
									 Parent(_Parent)
{
	// Create the controls
	CreateControls();
}

//==========================================================================
// Class:			EDIT_CORNER_PANEL
// Function:		~EDIT_CORNER_PANEL
//
// Description:		Destructor for EDIT_CORNER_PANEL class.
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
EDIT_CORNER_PANEL::~EDIT_CORNER_PANEL()
{
}

//==========================================================================
// Class:			EDIT_CORNER_PANEL
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
BEGIN_EVENT_TABLE(EDIT_CORNER_PANEL, wxPanel)
	EVT_GRID_SELECT_CELL(						EDIT_CORNER_PANEL::SelectCellEvent)
	EVT_GRID_CELL_CHANGE(						EDIT_CORNER_PANEL::GridCellChangedEvent)
	EVT_COMBOBOX(ComboBoxActuationAttachment,	EDIT_CORNER_PANEL::ActuationAttachmentChangeEvent)
	EVT_COMBOBOX(ComboBoxActuationType,			EDIT_CORNER_PANEL::ActuationTypeChangeEvent)
	EVT_TEXT(TextBoxStaticCamber,				EDIT_CORNER_PANEL::StaticCamberChangeEvent)
	EVT_TEXT(TextBoxStaticToe,					EDIT_CORNER_PANEL::StaticToeChangeEvent)
END_EVENT_TABLE();

//==========================================================================
// Class:			EDIT_CORNER_PANEL
// Function:		UpdateInformation
//
// Description:		Updates the information on this panel.
//
// Input Arguments:
//		_CurrentCorner	= CORNER* pointing to the associated corner
//		BarStyle		= SUSPENSION::BAR_STYLE describing what kind of sway bar
//						  is at this end of the car
//		HasHalfShaft	= bool, true if this corner gets a half shaft
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void EDIT_CORNER_PANEL::UpdateInformation(CORNER *_CurrentCorner,
										  SUSPENSION::BAR_STYLE BarStyle, bool HasHalfShaft)
{
	// Update the class member
	CurrentCorner = _CurrentCorner;

	// Update the combo boxes
	ActuationType->SetSelection(CurrentCorner->ActuationType);
	ActuationAttachment->SetSelection(CurrentCorner->ActuationAttachment);

	// Update the static toe and camber
	StaticCamber->ChangeValue(Converter.FormatNumber(Converter.ConvertAngle(CurrentCorner->StaticCamber)));
	StaticToe->ChangeValue(Converter.FormatNumber(Converter.ConvertAngle(CurrentCorner->StaticToe)));

	// And their units
	CamberUnitsLabel->SetLabel(Converter.GetUnitType(CONVERT::UNIT_TYPE_ANGLE));
	ToeUnitsLabel->SetLabel(Converter.GetUnitType(CONVERT::UNIT_TYPE_ANGLE));

	// Begin batch edit of the grid
	Hardpoints->BeginBatch();

	// Update the unit labels
	wxString UnitString;
	UnitString.Printf("(%s)", Converter.GetUnitType(CONVERT::UNIT_TYPE_DISTANCE).c_str());
	Hardpoints->SetCellValue(0, 1, UnitString);
	Hardpoints->SetCellValue(0, 2, UnitString);
	Hardpoints->SetCellValue(0, 3, UnitString);

	// Hide or show rows according to this object's configuration
	// Geared sway bar
	if (BarStyle == SUSPENSION::SwayBarGeared)
		Hardpoints->SetRowHeight(CORNER::GearEndBarShaft + 1, Hardpoints->GetRowHeight(0));
	else
		Hardpoints->SetRowHeight(CORNER::GearEndBarShaft + 1, 0);

	if (BarStyle == SUSPENSION::SwayBarNone)
	{
		Hardpoints->SetRowHeight(CORNER::OutboardBarLink + 1, 0);
		Hardpoints->SetRowHeight(CORNER::InboardBarLink + 1, 0);
		Hardpoints->SetRowHeight(CORNER::BarArmAtPivot + 1, 0);
	}
	else
	{
		Hardpoints->SetRowHeight(CORNER::OutboardBarLink + 1, Hardpoints->GetRowHeight(0));
		Hardpoints->SetRowHeight(CORNER::InboardBarLink + 1, Hardpoints->GetRowHeight(0));
		Hardpoints->SetRowHeight(CORNER::BarArmAtPivot + 1, Hardpoints->GetRowHeight(0));
	}

	// Half shaft
	if (HasHalfShaft)
	{
		// Show the half shaft points
		Hardpoints->SetRowHeight(CORNER::InboardHalfShaft + 1, Hardpoints->GetRowHeight(0));
		Hardpoints->SetRowHeight(CORNER::OutboardHalfShaft + 1, Hardpoints->GetRowHeight(0));
	}
	else
	{
		// Hide the half shaft points
		Hardpoints->SetRowHeight(CORNER::InboardHalfShaft + 1, 0);
		Hardpoints->SetRowHeight(CORNER::OutboardHalfShaft + 1, 0);
	}

	// Outboard springs/shocks vs. pushrod actuated springs/shocks
	if (CurrentCorner->ActuationType == CORNER::ActuationPushPullrod)
	{
		Hardpoints->SetRowHeight(CORNER::InboardPushrod + 1, Hardpoints->GetRowHeight(0));
		Hardpoints->SetRowHeight(CORNER::OutboardPushrod + 1, Hardpoints->GetRowHeight(0));
		Hardpoints->SetRowHeight(CORNER::BellCrankPivot1 + 1, Hardpoints->GetRowHeight(0));
		Hardpoints->SetRowHeight(CORNER::BellCrankPivot2 + 1, Hardpoints->GetRowHeight(0));
	}
	else if (CurrentCorner->ActuationType == CORNER::ActuationOutboard)
	{
		Hardpoints->SetRowHeight(CORNER::InboardPushrod + 1, 0);
		Hardpoints->SetRowHeight(CORNER::OutboardPushrod + 1, 0);
		Hardpoints->SetRowHeight(CORNER::BellCrankPivot1 + 1, 0);
		Hardpoints->SetRowHeight(CORNER::BellCrankPivot2 + 1, 0);
	}

	// Update the values of all of the points
	VECTOR Point;
	int i;
	for (i = 0; i < CORNER::NumberOfHardpoints; i++)
	{
		// Get the location of this hardpoint (don't forget to convert it!)
		Point = Converter.ConvertDistance(CurrentCorner->Hardpoints[i]);

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
// Class:			EDIT_CORNER_PANEL
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
void EDIT_CORNER_PANEL::CreateControls()
{
	// Top-level sizer
	wxBoxSizer *TopSizer = new wxBoxSizer(wxVERTICAL);

	// Second sizer gives more space around the controls
	wxBoxSizer *MainSizer = new wxBoxSizer(wxVERTICAL);
	TopSizer->Add(MainSizer, 1, wxALIGN_CENTER_HORIZONTAL | wxGROW | wxALL, 5);

	// The column widths
	int LabelColumnWidth = 120;
	int InputColumnWidth = 45;

	// Create the grid for the hard point entry
	Hardpoints = new wxGrid(this, wxID_ANY);
	Hardpoints->CreateGrid(CORNER::NumberOfHardpoints + 1, 4, wxGrid::wxGridSelectRows);

	// Begin a batch edit of the grid
	Hardpoints->BeginBatch();

	// Make make the heading row read-only
	int i;
	for (i = 0; i < Hardpoints->GetNumberCols(); i++)
		Hardpoints->SetReadOnly(0, i, true);

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

	// We never want the wheel center to be visible
	Hardpoints->SetRowHeight(CORNER::WheelCenter + 1, 0);

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

	// Do the processing that needs to be done for each row
	for (i = 0; i < CORNER::NumberOfHardpoints; i++)
	{
		// Make the first column read-only
		Hardpoints->SetReadOnly(i + 1, 0, true);

		// Set the alignment for all of the cells that contain numbers to the right
		Hardpoints->SetCellAlignment(i + 1, 1, wxALIGN_RIGHT, wxALIGN_TOP);
		Hardpoints->SetCellAlignment(i + 1, 2, wxALIGN_RIGHT, wxALIGN_TOP);
		Hardpoints->SetCellAlignment(i + 1, 3, wxALIGN_RIGHT, wxALIGN_TOP);

		// Add the names of all of the points to the grid
		Hardpoints->SetCellValue(i + 1, 0, CORNER::GetHardpointName((CORNER::HARDPOINTS)i));
	}

	// Don't let the user move or re-size the rows or move the columns
	Hardpoints->EnableDragColMove(false);
	Hardpoints->EnableDragColSize(true);
	Hardpoints->EnableDragGridSize(false);
	Hardpoints->EnableDragRowSize(false);

	// End the batch mode edit and re-paint the control
	Hardpoints->EndBatch();

	// Re-size the columns
	LabelColumnWidth = 100;
	InputColumnWidth = 100;

	// Create the combo-boxes
	// Actuation type
	wxArrayString Choices;
	Choices.Clear();
	for (i = 0; i < CORNER::NumberOfActuationTypes; i++)
		Choices.Add(CORNER::GetActuationTypeName((CORNER::ACTUATION_TYPE)i));

	wxBoxSizer *ActuationTypeSizer = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText *ActuationTypeLabel = new wxStaticText(this, wxID_ANY,
		_T("Actuation Type"), wxDefaultPosition, wxSize(LabelColumnWidth, -1));
	ActuationType = new wxComboBox(this, ComboBoxActuationType, wxEmptyString, wxDefaultPosition,
		wxSize(InputColumnWidth, -1), Choices, wxCB_READONLY);

	ActuationTypeSizer->Add(ActuationTypeLabel, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
	ActuationTypeSizer->Add(ActuationType, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

	// Actuation attachment
	Choices.Clear();
	for (i = 0; i < CORNER::NumberOfAttachments; i++)
		Choices.Add(CORNER::GetActuationAttachmentName((CORNER::ACTUATION_ATTACHMENT)i));

	wxBoxSizer *AttachmentSizer = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText *AttachmentLabel = new wxStaticText(this, wxID_ANY,
		_T("Actuation Attachment"), wxDefaultPosition, wxSize(LabelColumnWidth, -1));
	ActuationAttachment = new wxComboBox(this, ComboBoxActuationAttachment, wxEmptyString, wxDefaultPosition,
		wxSize(InputColumnWidth, -1), Choices, wxCB_READONLY);

	AttachmentSizer->Add(AttachmentLabel, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
	AttachmentSizer->Add(ActuationAttachment, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

	// Create the text controls
	// Camber angle
	wxBoxSizer *CamberSizer = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText *CamberLabel = new wxStaticText(this, wxID_ANY,
		_T("Static Camber"), wxDefaultPosition, wxSize(LabelColumnWidth, -1));
	CamberUnitsLabel = new wxStaticText(this, wxID_ANY,
		wxEmptyString, wxDefaultPosition);
	StaticCamber = new wxTextCtrl(this, TextBoxStaticCamber, wxEmptyString,
		wxDefaultPosition, wxSize(InputColumnWidth, -1));

	CamberSizer->Add(CamberLabel, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
	CamberSizer->Add(StaticCamber, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
	CamberSizer->Add(CamberUnitsLabel, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

	// Toe angle
	wxBoxSizer *ToeSizer = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText *ToeLabel = new wxStaticText(this, wxID_ANY,
		_T("Static Toe"), wxDefaultPosition, wxSize(LabelColumnWidth, -1));
	ToeUnitsLabel = new wxStaticText(this, wxID_ANY,
		wxEmptyString, wxDefaultPosition);
	StaticToe = new wxTextCtrl(this, TextBoxStaticToe, wxEmptyString,
		wxDefaultPosition, wxSize(InputColumnWidth, -1));

	ToeSizer->Add(ToeLabel, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
	ToeSizer->Add(StaticToe, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
	ToeSizer->Add(ToeUnitsLabel, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

	// Add the individial sizers to the main sizer
	MainSizer->Add(ActuationTypeSizer, 0, wxALIGN_BOTTOM);
	MainSizer->Add(AttachmentSizer, 0, wxALIGN_BOTTOM);
	MainSizer->Add(CamberSizer, 0, wxALIGN_BOTTOM);
	MainSizer->Add(ToeSizer, 0, wxALIGN_BOTTOM);

	// Assign the top level sizer to the dialog
	SetSizer(TopSizer);

	// Initialize the last row selected variable
	LastRowSelected = -1;

	return;
}

//==========================================================================
// Class:			EDIT_CORNER_PANEL
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
void EDIT_CORNER_PANEL::SelectCellEvent(wxGridEvent &event)
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
			(CORNER::HARDPOINTS)(event.GetRow() - 1), CurrentCorner->Location, SUSPENSION::NumberOfHardpoints);

		// Update the analyses
		Parent.GetParent().GetCurrentObject()->UpdateData();

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
// Class:			EDIT_CORNER_PANEL
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
void EDIT_CORNER_PANEL::GridCellChangedEvent(wxGridEvent &event)
{
	// Make sure the row is a valid row
	if (event.GetRow() > 0)
	{
		// Get a lock on the car
		wxMutex *Mutex = Parent.GetParent().GetCurrentMutex();
		Mutex->Lock();

		// For reading/writing values we use a string
		wxString ValueString;

		// If this is a contact patch, don't allow modification of the Z location
		if (event.GetCol() == 3 && event.GetRow() - 1 == CORNER::ContactPatch)
		{
			// Reset to zero
			ValueString.Printf("%0.3f", 0.0);
			Hardpoints->SetCellValue(event.GetRow(), event.GetCol(), ValueString);

			return;
		}

		// Get the new value
		ValueString = Hardpoints->GetCellValue(event.GetRow(), event.GetCol());
		double Value;

		// Make sure the value is numeric
		if (!ValueString.ToDouble(&Value))
			// The value is non-numeric - don't do anything
			return;

		// Update the point (don't forget to convert)
		if (event.GetCol() == 1)// X
		{
			// Add the operation to the undo/redo stack
			Parent.GetParent().GetMainFrame().GetUndoRedoStack().AddOperation(
				Parent.GetParent().GetMainFrame().GetActiveIndex(),
				UNDO_REDO_STACK::OPERATION::DATA_TYPE_DOUBLE,
				&(CurrentCorner->Hardpoints[event.GetRow() - 1].X));

			CurrentCorner->Hardpoints[event.GetRow() - 1].X = Converter.ReadDistance(Value);
		}
		else if (event.GetCol() == 2)// Y
		{
			// Add the operation to the undo/redo stack
			Parent.GetParent().GetMainFrame().GetUndoRedoStack().AddOperation(
				Parent.GetParent().GetMainFrame().GetActiveIndex(),
				UNDO_REDO_STACK::OPERATION::DATA_TYPE_DOUBLE,
				&(CurrentCorner->Hardpoints[event.GetRow() - 1].Y));

			CurrentCorner->Hardpoints[event.GetRow() - 1].Y = Converter.ReadDistance(Value);
		}
		else// Z
		{
			// Add the operation to the undo/redo stack
			Parent.GetParent().GetMainFrame().GetUndoRedoStack().AddOperation(
				Parent.GetParent().GetMainFrame().GetActiveIndex(),
				UNDO_REDO_STACK::OPERATION::DATA_TYPE_DOUBLE,
				&(CurrentCorner->Hardpoints[event.GetRow() - 1].Z));

			CurrentCorner->Hardpoints[event.GetRow() - 1].Z = Converter.ReadDistance(Value);
		}

		// Call the UpdateSymmetry method in case this is a symmetric suspension
		Parent.UpdateSymmetry();
		
		// Unlock the car
		Mutex->Unlock();

		// Tell the car object that it was modified
		Parent.GetParent().GetCurrentObject()->SetModified();

		// Set the position of the helper orb
		static_cast<CAR_RENDERER*>(Parent.GetParent().GetCurrentObject()->GetRenderer())->SetHelperOrbPosition(
			(CORNER::HARDPOINTS)(event.GetRow() - 1), CurrentCorner->Location, SUSPENSION::NumberOfHardpoints);

		// Update the display and the kinematic outputs
		Parent.GetParent().GetMainFrame().UpdateAnalysis();
		Parent.GetParent().GetMainFrame().UpdateOutputPanel();
	}

	event.Skip();

	return;
}

//==========================================================================
// Class:			EDIT_CORNER_PANEL
// Function:		ActuationAttachmentChangeEvent
//
// Description:		Event that fires when the actuation attachment changes.
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
void EDIT_CORNER_PANEL::ActuationAttachmentChangeEvent(wxCommandEvent &event)
{
	// Add the operation to the undo/redo stack
	Parent.GetParent().GetMainFrame().GetUndoRedoStack().AddOperation(
		Parent.GetParent().GetMainFrame().GetActiveIndex(),
		UNDO_REDO_STACK::OPERATION::DATA_TYPE_INTEGER,
		&(CurrentCorner->ActuationAttachment));

	// Get a lock on the car
	wxMutex *Mutex = Parent.GetParent().GetCurrentMutex();
	Mutex->Lock();

	// Set the value of the actuation attachment enum to the value of this combobox
	CurrentCorner->ActuationAttachment = (CORNER::ACTUATION_ATTACHMENT)event.GetSelection();

	// Call the UpdateSymmetry method in case this is a symmetric suspension
	Parent.UpdateSymmetry();
	
	// Unlock the car
	Mutex->Unlock();

	// Tell the car object that it was modified
	Parent.GetParent().GetCurrentObject()->SetModified();

	// Update the display and the kinematic outputs
	Parent.GetParent().GetMainFrame().UpdateAnalysis();
	Parent.GetParent().GetMainFrame().UpdateOutputPanel();

	event.Skip();

	return;
}

//==========================================================================
// Class:			EDIT_CORNER_PANEL
// Function:		ActuationTypeChangeEvent
//
// Description:		Event that fires when the actuation type changes.
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
void EDIT_CORNER_PANEL::ActuationTypeChangeEvent(wxCommandEvent &event)
{
	// Add the operation to the undo/redo stack
	Parent.GetParent().GetMainFrame().GetUndoRedoStack().AddOperation(
		Parent.GetParent().GetMainFrame().GetActiveIndex(),
		UNDO_REDO_STACK::OPERATION::DATA_TYPE_INTEGER,
		&(CurrentCorner->ActuationType));

	// Get a lock on the car
	wxMutex *Mutex = Parent.GetParent().GetCurrentMutex();
	Mutex->Lock();

	// Set the value of the actuation attachment enum to the value of this combobox
	CurrentCorner->ActuationType = (CORNER::ACTUATION_TYPE)event.GetSelection();

	// Call the UpdateSymmetry method in case this is a symmetric suspension
	Parent.UpdateSymmetry();
	
	// Unlock the car
	Mutex->Unlock();

	// Tell the car object that it was modified
	Parent.GetParent().GetCurrentObject()->SetModified();

	// Update the display and the kinematic outputs
	Parent.GetParent().GetMainFrame().UpdateAnalysis();
	Parent.GetParent().GetMainFrame().UpdateOutputPanel();

	// Update this panel in case the editable hardpoints changed
	Parent.UpdateInformation();

	event.Skip();
}

//==========================================================================
// Class:			EDIT_CORNER_PANEL
// Function:		StaticCamberChangeEvent
//
// Description:		Event that fires when the static camber changes.
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
void EDIT_CORNER_PANEL::StaticCamberChangeEvent(wxCommandEvent &event)
{
	// Get the new value
	wxString ValueString = StaticCamber->GetValue();
	double Value;

	// Make sure the value is numeric
	if (!ValueString.ToDouble(&Value))
	{
		// Restore the previous value
		Value = CurrentCorner->StaticCamber;
		ValueString.Printf("%0.3f", Converter.ConvertAngle(Value));
		StaticCamber->SetValue(ValueString);

		return;
	}

	// Add the operation to the undo/redo stack
	Parent.GetParent().GetMainFrame().GetUndoRedoStack().AddOperation(
		Parent.GetParent().GetMainFrame().GetActiveIndex(),
		UNDO_REDO_STACK::OPERATION::DATA_TYPE_DOUBLE,
		&(CurrentCorner->StaticCamber));

	// Get a lock on the car
	wxMutex *Mutex = Parent.GetParent().GetCurrentMutex();
	Mutex->Lock();

	// Update the corner object
	CurrentCorner->StaticCamber = Converter.ReadAngle(Value);

	// Call the UpdateSymmetry method in case this is a symmetric suspension
	Parent.UpdateSymmetry();
	
	// Unlock the car
	Mutex->Unlock();

	// Tell the car object that it was modified
	Parent.GetParent().GetCurrentObject()->SetModified();

	// Update the display and the kinematic outputs
	Parent.GetParent().GetMainFrame().UpdateAnalysis();
	Parent.GetParent().GetMainFrame().UpdateOutputPanel();

	event.Skip();

	return;
}

//==========================================================================
// Class:			EDIT_CORNER_PANEL
// Function:		StaticToeChangeEvent
//
// Description:		Event that fires when the static toe changes.
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
void EDIT_CORNER_PANEL::StaticToeChangeEvent(wxCommandEvent &event)
{
	// Get the new value
	wxString ValueString = StaticToe->GetValue();
	double Value;

	// Make sure the value is numeric
	if (!ValueString.ToDouble(&Value))
	{
		// Restore the previous value
		Value = CurrentCorner->StaticToe;
		ValueString.Printf("%0.3f", Converter.ConvertAngle(Value));
		StaticToe->SetValue(ValueString);

		return;
	}

	// Add the operation to the undo/redo stack
	Parent.GetParent().GetMainFrame().GetUndoRedoStack().AddOperation(
		Parent.GetParent().GetMainFrame().GetActiveIndex(),
		UNDO_REDO_STACK::OPERATION::DATA_TYPE_DOUBLE,
		&(CurrentCorner->StaticToe));

	// Get a lock on the car
	wxMutex *Mutex = Parent.GetParent().GetCurrentMutex();
	Mutex->Lock();

	// Update the corner object
	CurrentCorner->StaticToe = Converter.ReadAngle(Value);

	// Call the UpdateSymmetry method in case this is a symmetric suspension
	Parent.UpdateSymmetry();
	
	// Unlock the car
	Mutex->Unlock();

	// Tell the car object that it was modified
	Parent.GetParent().GetCurrentObject()->SetModified();

	// Update the display and the kinematic outputs
	Parent.GetParent().GetMainFrame().UpdateAnalysis();
	Parent.GetParent().GetMainFrame().UpdateOutputPanel();

	event.Skip();

	return;
}