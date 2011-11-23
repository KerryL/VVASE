/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  editBrakesPanel.cpp
// Created:  2/10/2009
// Author:  K. Loux
// Description:  Contains the class definition for the EditBrakesPanel class.
// History:

// CarDesigner headers
#include "vCar/brakes.h"
#include "gui/guiCar.h"
#include "gui/components/mainFrame.h"
#include "gui/components/editPanel/editPanel.h"
#include "gui/components/editPanel/guiCar/editBrakesPanel.h"
#include "vUtilities/convert.h"
#include "vMath/vector.h"

//==========================================================================
// Class:			EditBrakesPanel
// Function:		EditBrakesPanel
//
// Description:		Constructor for EditBrakesPanel class.
//
// Input Arguments:
//		_parent		= EditPanel&, reference to this object's owner
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
EditBrakesPanel::EditBrakesPanel(EditPanel &_parent, wxWindowID id,
									 const wxPoint& pos, const wxSize& size,
									 const Debugger &_debugger) :
									 wxScrolledWindow(&_parent, id, pos, size), debugger(_debugger),
									 converter(_parent.GetMainFrame().GetConverter()),
									 parent(_parent)
{
	// Create the controls
	CreateControls();
}

//==========================================================================
// Class:			EditBrakesPanel
// Function:		EditBrakesPanel
//
// Description:		Destructor for EditBrakesPanel class.
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
EditBrakesPanel::~EditBrakesPanel()
{
}

//==========================================================================
// Class:			EditBrakesPanel
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
BEGIN_EVENT_TABLE(EditBrakesPanel, wxPanel)
	EVT_TEXT(wxID_ANY,		EditBrakesPanel::TextBoxEditEvent)
	EVT_CHECKBOX(wxID_ANY,	EditBrakesPanel::CheckBoxChange)
END_EVENT_TABLE();

//==========================================================================
// Class:			EditBrakesPanel
// Function:		UpdateInformation
//
// Description:		Updates the information on this panel.
//
// Input Arguments:
//		_CurrentBrakes	= Brakes* pointing to the associated brakes
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void EditBrakesPanel::UpdateInformation(Brakes *_currentBrakes)
{
	// Update the class member
	currentBrakes = _currentBrakes;

	// Update the check boxes
	frontBrakesInboard->SetValue(currentBrakes->frontBrakesInboard);
	rearBrakesInboard->SetValue(currentBrakes->rearBrakesInboard);

	// Update the text boxes
	percentFrontBraking->ChangeValue(converter.FormatNumber(currentBrakes->percentFrontBraking));
}

//==========================================================================
// Class:			EditBrakesPanel
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
void EditBrakesPanel::CreateControls()
{
	// Enable scrolling
	SetScrollRate(1, 1);

	// Top-level sizer
	wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);

	// Second sizer gives more space around the controls
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	topSizer->Add(mainSizer, 1, wxALIGN_CENTER_HORIZONTAL | wxGROW | wxALL, 5);
	
	int cellPadding(3);

	// Create the check boxes
	frontBrakesInboard = new wxCheckBox(this, CheckBoxFrontBrakesInboard, _T("Front Brakes Inboard"));
	mainSizer->Add(frontBrakesInboard, 0, wxALIGN_LEFT | wxALL, cellPadding);

	rearBrakesInboard = new wxCheckBox(this, CheckBoxRearBrakesInboard, _T("Rear Brakes Inboard"));
	mainSizer->Add(rearBrakesInboard, 0, wxALIGN_LEFT | wxALL, cellPadding);

	// Create the text input boxes
	wxBoxSizer *percentFrontSizer = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText *percentFrontBrakingLabel = new wxStaticText(this, wxID_ANY, _T("Percent Front Braking"));
	percentFrontBraking = new wxTextCtrl(this, TextBoxPercentFrontBraking);
	percentFrontSizer->Add(percentFrontBrakingLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, cellPadding);
	percentFrontSizer->Add(percentFrontBraking, 0, wxALL | wxEXPAND, cellPadding);

	// Set text box minimum size based on formatted text to appear in the box
	int minWidth;
	GetTextExtent(converter.FormatNumber(-10.000), &minWidth, NULL);
	percentFrontBraking->SetMinSize(wxSize(minWidth, -1));

	// Add the sizers to the main sizer
	mainSizer->AddSpacer(10);
	mainSizer->Add(percentFrontSizer, 0, wxEXPAND);

	// Assign the top level sizer to the dialog
	SetSizer(topSizer);
}

//==========================================================================
// Class:			EditBrakesPanel
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
void EditBrakesPanel::TextBoxEditEvent(wxCommandEvent &event)
{
	// The place where the data will be written
	double *dataLocation = NULL;

	// The limits for the data
	double minValue, maxValue;

	// We also need to know what kind of data we're retrieving
	Convert::UnitType units;

	// A pointer to the text box
	wxTextCtrl *textBox = NULL;

	// Take different action depending on the event ID
	switch (event.GetId())
	{
	case TextBoxPercentFrontBraking:
		// Get the text box, the location to write it, and the units
		textBox = percentFrontBraking;
		dataLocation = &currentBrakes->percentFrontBraking;
		units = Convert::UnitTypeUnitless;
		minValue = 0.0;
		maxValue = 1.0;
		break;

	default:
		// Don't do anything if we don't recognize the ID
		return;
		break;
	}

	// Make sure the value is numeric
	wxString valueString = textBox->GetValue();
	double value;
	if (!valueString.ToDouble(&value))
	{
		// Restore the previous value
		value = *dataLocation;

		return;
	}

	// Add the operation to the undo/redo stack
	parent.GetMainFrame().GetUndoRedoStack().AddOperation(
		parent.GetMainFrame().GetActiveIndex(),
		UndoRedoStack::Operation::DataTypeDouble,
		dataLocation);

	// Get a lock on the car
	wxMutex *mutex = parent.GetCurrentMutex();
	mutex->Lock();

	// Update the brakes object
	*dataLocation = converter.Read(value, units);

	// Check the limits on the data value
	if (*dataLocation > maxValue)
		*dataLocation = maxValue;
	else if (*dataLocation < minValue)
		*dataLocation = minValue;

	// Unlock the car
	mutex->Unlock();

	// Tell the car object that it was modified
	parent.GetCurrentObject()->SetModified();

	// Update the display and the kinematic outputs
	parent.GetMainFrame().UpdateAnalysis();
	parent.GetMainFrame().UpdateOutputPanel();

	event.Skip();
}

//==========================================================================
// Class:			EditBrakesPanel
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
void EditBrakesPanel::CheckBoxChange(wxCommandEvent &event)
{
	// Get a lock on the car
	wxMutex *mutex = parent.GetCurrentMutex();
	mutex->Lock();

	// Take different action depending on the event ID
	switch (event.GetId())
	{
	case CheckBoxFrontBrakesInboard:
		// Add the operation to the undo/redo stack
		parent.GetMainFrame().GetUndoRedoStack().AddOperation(
			parent.GetMainFrame().GetActiveIndex(),
			UndoRedoStack::Operation::DataTypeBool,
			&(currentBrakes->frontBrakesInboard));

		// Set the front brakes inboard flag to the value of the check box
		currentBrakes->frontBrakesInboard = event.IsChecked();
		break;

	case CheckBoxRearBrakesInboard:
		// Add the operation to the undo/redo stack
		parent.GetMainFrame().GetUndoRedoStack().AddOperation(
			parent.GetMainFrame().GetActiveIndex(),
			UndoRedoStack::Operation::DataTypeBool,
			&(currentBrakes->rearBrakesInboard));

		// Set the rear brakes inboard flag to the value of the check box
		currentBrakes->rearBrakesInboard = event.IsChecked();
		break;

	default:
		// Don't do anything if we don't recognize the ID
		return;
		break;
	}

	// Unlock the car
	mutex->Unlock();

	// Tell the car object that it was modified
	parent.GetCurrentObject()->SetModified();

	// Update the display and the kinematic outputs
	parent.GetMainFrame().UpdateAnalysis();
	parent.GetMainFrame().UpdateOutputPanel();
}