/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  edit_brakes_panel_class.cpp
// Created:  2/10/2009
// Author:  K. Loux
// Description:  Contains the class definition for the EDIT_BRAKES_PANEL class.
// History:

// CarDesigner headers
#include "vCar/brakes_class.h"
#include "gui/gui_car_class.h"
#include "gui/components/main_frame_class.h"
#include "gui/components/edit_panel/edit_panel_class.h"
#include "gui/components/edit_panel/gui_car/edit_brakes_panel_class.h"
#include "vUtilities/convert_class.h"
#include "vMath/vector_class.h"

//==========================================================================
// Class:			EDIT_BRAKES_PANEL
// Function:		EDIT_BRAKES_PANEL
//
// Description:		Constructor for EDIT_BRAKES_PANEL class.
//
// Input Arguments:
//		_Parent		= EDIT_PANEL&, reference to this object's owner
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
EDIT_BRAKES_PANEL::EDIT_BRAKES_PANEL(EDIT_PANEL &_Parent, wxWindowID id,
									 const wxPoint& pos, const wxSize& size,
									 const Debugger &_debugger) :
									 wxPanel(&_Parent, id, pos, size), Debugger(_debugger),
									 Converter(_Parent.GetMainFrame().GetConverter()),
									 Parent(_Parent)
{
	// Create the controls
	CreateControls();
}

//==========================================================================
// Class:			EDIT_BRAKES_PANEL
// Function:		EDIT_BRAKES_PANEL
//
// Description:		Destructor for EDIT_BRAKES_PANEL class.
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
EDIT_BRAKES_PANEL::~EDIT_BRAKES_PANEL()
{
}

//==========================================================================
// Class:			EDIT_BRAKES_PANEL
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
BEGIN_EVENT_TABLE(EDIT_BRAKES_PANEL, wxPanel)
	EVT_TEXT(wxID_ANY,		EDIT_BRAKES_PANEL::TextBoxEditEvent)
	EVT_CHECKBOX(wxID_ANY,	EDIT_BRAKES_PANEL::CheckBoxChange)
END_EVENT_TABLE();

//==========================================================================
// Class:			EDIT_BRAKES_PANEL
// Function:		UpdateInformation
//
// Description:		Updates the information on this panel.
//
// Input Arguments:
//		_CurrentBrakes	= BRAKES* pointing to the associated brakes
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void EDIT_BRAKES_PANEL::UpdateInformation(BRAKES *_CurrentBrakes)
{
	// Update the class member
	CurrentBrakes = _CurrentBrakes;

	// Update the check boxes
	FrontBrakesInboard->SetValue(CurrentBrakes->FrontBrakesInboard);
	RearBrakesInboard->SetValue(CurrentBrakes->RearBrakesInboard);

	// Update the text boxes
	PercentFrontBraking->ChangeValue(Converter.FormatNumber(CurrentBrakes->PercentFrontBraking));

	return;
}

//==========================================================================
// Class:			EDIT_BRAKES_PANEL
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
void EDIT_BRAKES_PANEL::CreateControls()
{
	// Top-level sizer
	wxBoxSizer *TopSizer = new wxBoxSizer(wxVERTICAL);

	// Second sizer gives more space around the controls
	wxBoxSizer *MainSizer = new wxBoxSizer(wxVERTICAL);
	TopSizer->Add(MainSizer, 1, wxALIGN_CENTER_HORIZONTAL | wxGROW | wxALL, 5);

	// The column widths
	int LabelColumnWidth = 120;
	int InputColumnWidth = 50;

	// Create the check boxes
	wxBoxSizer *FrontBrakesInboardSizer = new wxBoxSizer(wxHORIZONTAL);
	FrontBrakesInboard = new wxCheckBox(this, CheckBoxFrontBrakesInboard, _T("Front Brakes Inboard"));
	FrontBrakesInboardSizer->Add(FrontBrakesInboard, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

	wxBoxSizer *RearBrakesInboardSizer = new wxBoxSizer(wxHORIZONTAL);
	RearBrakesInboard = new wxCheckBox(this, CheckBoxRearBrakesInboard, _T("Rear Brakes Inboard"));
	RearBrakesInboardSizer->Add(RearBrakesInboard, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

	// Create the text input boxes
	wxBoxSizer *PercentFrontSizer = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText *PercentFrontBrakingLabel = new wxStaticText(this, wxID_ANY, _T("Percent Front Braking"),
		wxDefaultPosition, wxSize(LabelColumnWidth, -1));
	PercentFrontBraking = new wxTextCtrl(this, TextBoxPercentFrontBraking, wxEmptyString, wxDefaultPosition,
		wxSize(InputColumnWidth, -1));
	PercentFrontSizer->Add(PercentFrontBrakingLabel, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
	PercentFrontSizer->Add(PercentFrontBraking, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

	// Add the sizers to the main sizer
	MainSizer->Add(FrontBrakesInboardSizer);
	MainSizer->Add(RearBrakesInboardSizer);
	MainSizer->AddSpacer(15);
	MainSizer->Add(PercentFrontSizer);

	// Assign the top level sizer to the dialog
	SetSizer(TopSizer);

	return;
}

//==========================================================================
// Class:			EDIT_BRAKES_PANEL
// Function:		TextBoxEditEvent
//
// Description:		Event that fires when any text box changes.
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
void EDIT_BRAKES_PANEL::TextBoxEditEvent(wxCommandEvent &event)
{
	// The place where the data will be written
	double *DataLocation = NULL;

	// The limits for the data
	double MinValue, MaxValue;

	// We also need to know what kind of data we're retrieving
	Convert::UNIT_TYPE Units;

	// A pointer to the text box
	wxTextCtrl *TextBox = NULL;

	// Take different action depending on the event ID
	switch (event.GetId())
	{
	case TextBoxPercentFrontBraking:
		// Get the text box, the location to write it, and the units
		TextBox = PercentFrontBraking;
		DataLocation = &CurrentBrakes->PercentFrontBraking;
		Units = Convert::UNIT_TYPE_UNITLESS;
		MinValue = 0.0;
		MaxValue = 1.0;
		break;

	default:
		// Don't do anything if we don't recognize the ID
		return;
		break;
	}

	// Make sure the value is numeric
	wxString ValueString = TextBox->GetValue();
	double Value;
	if (!ValueString.ToDouble(&Value))
	{
		// Restore the previous value
		Value = *DataLocation;

		return;
	}

	// Add the operation to the undo/redo stack
	Parent.GetMainFrame().GetUndoRedoStack().AddOperation(
		Parent.GetMainFrame().GetActiveIndex(),
		UNDO_REDO_STACK::OPERATION::DATA_TYPE_DOUBLE,
		DataLocation);

	// Get a lock on the car
	wxMutex *Mutex = Parent.GetCurrentMutex();
	Mutex->Lock();

	// Update the brakes object
	*DataLocation = Converter.Read(Value, Units);

	// Check the limits on the data value
	if (*DataLocation > MaxValue)
		*DataLocation = MaxValue;
	else if (*DataLocation < MinValue)
		*DataLocation = MinValue;

	// Unlock the car
	Mutex->Unlock();

	// Tell the car object that it was modified
	Parent.GetCurrentObject()->SetModified();

	// Update the display and the kinematic outputs
	Parent.GetMainFrame().UpdateAnalysis();
	Parent.GetMainFrame().UpdateOutputPanel();

	event.Skip();

	return;
}

//==========================================================================
// Class:			EDIT_BRAKES_PANEL
// Function:		CheckBoxChange
//
// Description:		Event that fires when any check box changes.
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
void EDIT_BRAKES_PANEL::CheckBoxChange(wxCommandEvent &event)
{
	// Get a lock on the car
	wxMutex *Mutex = Parent.GetCurrentMutex();
	Mutex->Lock();

	// Take different action depending on the event ID
	switch (event.GetId())
	{
	case CheckBoxFrontBrakesInboard:
		// Add the operation to the undo/redo stack
		Parent.GetMainFrame().GetUndoRedoStack().AddOperation(
			Parent.GetMainFrame().GetActiveIndex(),
			UNDO_REDO_STACK::OPERATION::DATA_TYPE_BOOL,
			&(CurrentBrakes->FrontBrakesInboard));

		// Set the front brakes inboard flag to the value of the check box
		CurrentBrakes->FrontBrakesInboard = event.IsChecked();
		break;

	case CheckBoxRearBrakesInboard:
		// Add the operation to the undo/redo stack
		Parent.GetMainFrame().GetUndoRedoStack().AddOperation(
			Parent.GetMainFrame().GetActiveIndex(),
			UNDO_REDO_STACK::OPERATION::DATA_TYPE_BOOL,
			&(CurrentBrakes->RearBrakesInboard));

		// Set the rear brakes inboard flag to the value of the check box
		CurrentBrakes->RearBrakesInboard = event.IsChecked();
		break;

	default:
		// Don't do anything if we don't recognize the ID
		return;
		break;
	}

	// Unlock the car
	Mutex->Unlock();

	// Tell the car object that it was modified
	Parent.GetCurrentObject()->SetModified();

	// Update the display and the kinematic outputs
	Parent.GetMainFrame().UpdateAnalysis();
	Parent.GetMainFrame().UpdateOutputPanel();

	return;
}