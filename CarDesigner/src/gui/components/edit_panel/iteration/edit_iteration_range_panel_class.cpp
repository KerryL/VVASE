/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  edit_iteration_options_panel_class.cpp
// Created:  11/14/2010
// Author:  K. Loux
// Description:  Contains the class definition for the EDIT_ITERATION_RANGE_PANEL
//				 class.
// History:

// CarDesigner headers
#include "gui/iteration_class.h"
#include "gui/components/main_frame_class.h"
#include "gui/components/edit_panel/edit_panel_class.h"
#include "gui/components/edit_panel/iteration/edit_iteration_range_panel_class.h"
#include "gui/components/edit_panel/iteration/edit_iteration_notebook_class.h"
#include "vUtilities/convert_class.h"

//==========================================================================
// Class:			EDIT_ITERATION_RANGE_PANEL
// Function:		EDIT_ITERATION_RANGE_PANEL
//
// Description:		Constructor for EDIT_ITERATION_RANGE_PANEL class.  Initializes the form
//					and creates the controls, etc.
//
// Input Arguments:
//		_Parent		= EDIT_ITERATION_NOTEBOOK&, reference to this object's owner
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
EDIT_ITERATION_RANGE_PANEL::EDIT_ITERATION_RANGE_PANEL(EDIT_ITERATION_NOTEBOOK &_Parent,
													   wxWindowID id, const wxPoint& pos,
													   const wxSize& size, const Debugger &_debugger) :
													   wxPanel(&_Parent, id, pos, size), debugger(_debugger),
													   Converter(_Parent.GetParent().GetMainFrame().GetConverter()),
													   Parent(_Parent)
{
	// Initialize the current object variable
	CurrentIteration = NULL;

	// Create the controls
	CreateControls();
}

//==========================================================================
// Class:			EDIT_ITERATION_RANGE_PANEL
// Function:		~EDIT_ITERATION_RANGE_PANEL
//
// Description:		Destructor for EDIT_ITERATION_RANGE_PANEL class.
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
EDIT_ITERATION_RANGE_PANEL::~EDIT_ITERATION_RANGE_PANEL()
{
}

//==========================================================================
// Class:			EDIT_ITERATION_RANGE_PANEL
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
BEGIN_EVENT_TABLE(EDIT_ITERATION_RANGE_PANEL, wxPanel)
	EVT_TEXT(RangeTextBox,	EDIT_ITERATION_RANGE_PANEL::RangeTextBoxChangeEvent)
	EVT_TEXT(RangeTextBox,	EDIT_ITERATION_RANGE_PANEL::RangeTextBoxChangeEvent)
END_EVENT_TABLE();

//==========================================================================
// Class:			EDIT_ITERATION_RANGE_PANEL
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
void EDIT_ITERATION_RANGE_PANEL::UpdateInformation(void)
{
	// Make sure the suspension object exists
	if (CurrentIteration)
		// Call the method that performs the update
		UpdateInformation(CurrentIteration);

	return;
}

//==========================================================================
// Class:			EDIT_ITERATION_RANGE_PANEL
// Function:		UpdateInformation
//
// Description:		Updates the information on this panel.
//
// Input Arguments:
//		_CurrentIteration	= ITERATION* pointing to the associated iteration
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void EDIT_ITERATION_RANGE_PANEL::UpdateInformation(ITERATION *_CurrentIteration)
{
	// Update the class members
	CurrentIteration = _CurrentIteration;

	// Make sure the iteration object exists
	if (!CurrentIteration)
		return;

	// Update the values of our Range and NumberOfPoints to the text boxes
	// Don't forget to convert the range values to the user specified units!
	StartPitchInput->ChangeValue(Converter.FormatNumber(Converter.ConvertAngle(CurrentIteration->GetRange().StartPitch)));
	StartRollInput->ChangeValue(Converter.FormatNumber(Converter.ConvertAngle(CurrentIteration->GetRange().StartRoll)));
	StartHeaveInput->ChangeValue(Converter.FormatNumber(Converter.ConvertDistance(CurrentIteration->GetRange().StartHeave)));
	StartSteerInput->ChangeValue(Converter.FormatNumber(Converter.ConvertDistance(CurrentIteration->GetRange().StartRackTravel)));
	EndPitchInput->ChangeValue(Converter.FormatNumber(Converter.ConvertAngle(CurrentIteration->GetRange().EndPitch)));
	EndRollInput->ChangeValue(Converter.FormatNumber(Converter.ConvertAngle(CurrentIteration->GetRange().EndRoll)));
	EndHeaveInput->ChangeValue(Converter.FormatNumber(Converter.ConvertDistance(CurrentIteration->GetRange().EndHeave)));
	EndSteerInput->ChangeValue(Converter.FormatNumber(Converter.ConvertDistance(CurrentIteration->GetRange().EndRackTravel)));

	wxString Temp;
	Temp.Printf("%i", CurrentIteration->GetNumberOfPoints());
	NumberOfPointsInput->ChangeValue(Temp);

	return;
}

//==========================================================================
// Class:			EDIT_ITERATION_RANGE_PANEL
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
void EDIT_ITERATION_RANGE_PANEL::CreateControls()
{
	// Top-level sizer
	wxBoxSizer *TopSizer = new wxBoxSizer(wxVERTICAL);

	// Second sizer gives more space around the controls
	wxBoxSizer *MainSizer = new wxBoxSizer(wxVERTICAL);
	TopSizer->Add(MainSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

	// Add a horizontal spacer for the text across the top
	wxBoxSizer *TopCaptionSizer = new wxBoxSizer(wxHORIZONTAL);
	MainSizer->Add(TopCaptionSizer, 0, wxALIGN_TOP);

	// The column widths
	int LabelColumnWidth = 60;
	int InputColumnWidth = 50;

	// Add the text to this sizer
	wxStaticText *TextStart = new wxStaticText(this, wxID_STATIC, _T("Start"),
		wxDefaultPosition, wxSize(InputColumnWidth, -1), 0);
	wxStaticText *TextEnd = new wxStaticText(this, wxID_STATIC, _T("End"),
		wxDefaultPosition, wxSize(InputColumnWidth, -1), 0);
	TopCaptionSizer->Add(LabelColumnWidth, -1, 0, wxALL, 5);
	TopCaptionSizer->Add(TextStart, 0, wxALL, 5);
	TopCaptionSizer->Add(TextEnd, 0, wxALL, 5);

	// Create a sizer for each of the inputs
	wxBoxSizer *PitchSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *RollSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *HeaveSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *SteerSizer = new wxBoxSizer(wxHORIZONTAL);

	// Add the static text and text controls to these sizers
	// Pitch
	wxStaticText *PitchLabelText = new wxStaticText(this, wxID_STATIC, _T("Pitch"),
		wxDefaultPosition, wxSize(LabelColumnWidth, -1), 0);
	StartPitchInput = new wxTextCtrl(this, RangeTextBox, wxEmptyString, wxDefaultPosition, wxSize(InputColumnWidth, -1), 0);
	EndPitchInput = new wxTextCtrl(this, RangeTextBox, wxEmptyString, wxDefaultPosition, wxSize(InputColumnWidth, -1), 0);
	wxStaticText *PitchUnitsText = new wxStaticText(this, wxID_STATIC,
		Converter.GetUnitType(Convert::UnitTypeAngle), wxDefaultPosition, wxDefaultSize, 0);
	PitchSizer->Add(PitchLabelText, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	PitchSizer->Add(StartPitchInput, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	PitchSizer->Add(EndPitchInput, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	PitchSizer->Add(PitchUnitsText, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	// Roll
	wxStaticText *RollLabelText = new wxStaticText(this, wxID_STATIC, _T("Roll"),
		wxDefaultPosition, wxSize(LabelColumnWidth, -1), 0);
	StartRollInput = new wxTextCtrl(this, RangeTextBox, wxEmptyString, wxDefaultPosition, wxSize(InputColumnWidth, -1), 0);
	EndRollInput = new wxTextCtrl(this, RangeTextBox, wxEmptyString, wxDefaultPosition, wxSize(InputColumnWidth, -1), 0);
	wxStaticText *RollUnitsText = new wxStaticText(this, wxID_STATIC,
		Converter.GetUnitType(Convert::UnitTypeAngle), wxDefaultPosition, wxDefaultSize, 0);
	RollSizer->Add(RollLabelText, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	RollSizer->Add(StartRollInput, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	RollSizer->Add(EndRollInput, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	RollSizer->Add(RollUnitsText, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	// Heave
	wxStaticText *HeaveLabelText = new wxStaticText(this, wxID_STATIC, _T("Heave"),
		wxDefaultPosition, wxSize(LabelColumnWidth, -1), 0);
	StartHeaveInput = new wxTextCtrl(this, RangeTextBox, wxEmptyString, wxDefaultPosition, wxSize(InputColumnWidth, -1), 0);
	EndHeaveInput = new wxTextCtrl(this, RangeTextBox, wxEmptyString, wxDefaultPosition, wxSize(InputColumnWidth, -1), 0);
	wxStaticText *HeaveUnitsText = new wxStaticText(this, wxID_STATIC,
		Converter.GetUnitType(Convert::UnitTypeDistance), wxDefaultPosition, wxDefaultSize, 0);
	HeaveSizer->Add(HeaveLabelText, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	HeaveSizer->Add(StartHeaveInput, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	HeaveSizer->Add(EndHeaveInput, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	HeaveSizer->Add(HeaveUnitsText, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	// Steer
	wxStaticText *SteerLabelText = new wxStaticText(this, wxID_STATIC, _T("Rack Travel"),
		wxDefaultPosition, wxSize(LabelColumnWidth, -1), 0);
	StartSteerInput = new wxTextCtrl(this, RangeTextBox, wxEmptyString, wxDefaultPosition, wxSize(InputColumnWidth, -1), 0);
	EndSteerInput = new wxTextCtrl(this, RangeTextBox, wxEmptyString, wxDefaultPosition, wxSize(InputColumnWidth, -1), 0);
	wxStaticText *SteerUnitsText = new wxStaticText(this, wxID_STATIC,
		Converter.GetUnitType(Convert::UnitTypeDistance), wxDefaultPosition, wxDefaultSize, 0);
	SteerSizer->Add(SteerLabelText, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	SteerSizer->Add(StartSteerInput, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	SteerSizer->Add(EndSteerInput, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	SteerSizer->Add(SteerUnitsText, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	// Add the sizers to the MainSizer
	MainSizer->Add(PitchSizer, 0, wxALIGN_TOP);
	MainSizer->Add(RollSizer, 0, wxALIGN_TOP);
	MainSizer->Add(HeaveSizer, 0, wxALIGN_TOP);
	MainSizer->Add(SteerSizer, 0, wxALIGN_TOP);

	// Add a sizer for the number of points input
	wxBoxSizer *NumberOfPointsSizer = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText *NumberOfPointsLabel = new wxStaticText(this, wxID_STATIC, _T("Number of points"),
		wxDefaultPosition, wxDefaultSize, 0);
	NumberOfPointsInput = new wxTextCtrl(this, RangeTextBox, wxEmptyString, wxDefaultPosition, wxSize(InputColumnWidth, -1), 0);
	NumberOfPointsSizer->Add(NumberOfPointsLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	NumberOfPointsSizer->Add(NumberOfPointsInput, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	MainSizer->Add(NumberOfPointsSizer, 0, wxALIGN_TOP);

	// Assign the top level sizer to the panel
	SetSizer(TopSizer);

	return;
}

//==========================================================================
// Class:			EDIT_ITERATION_RANGE_PANEL
// Function:		RangeTextBoxChangeEvent
//
// Description:		Event handler for any text box change on this panel.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void EDIT_ITERATION_RANGE_PANEL::RangeTextBoxChangeEvent(wxCommandEvent& WXUNUSED(event))
{
	ITERATION::RANGE Range;
	unsigned long NumberOfPoints(0);

	// Make sure all of the values are numeric as we assign them to our
	// Range variable
	if (!StartPitchInput->GetValue().ToDouble(&Range.StartPitch) ||
		!StartRollInput->GetValue().ToDouble(&Range.StartRoll) ||
		!StartHeaveInput->GetValue().ToDouble(&Range.StartHeave) ||
		!StartSteerInput->GetValue().ToDouble(&Range.StartRackTravel) ||
		!EndPitchInput->GetValue().ToDouble(&Range.EndPitch) ||
		!EndRollInput->GetValue().ToDouble(&Range.EndRoll) ||
		!EndHeaveInput->GetValue().ToDouble(&Range.EndHeave) ||
		!EndSteerInput->GetValue().ToDouble(&Range.EndRackTravel) ||
		!NumberOfPointsInput->GetValue().ToULong(&NumberOfPoints))
	{
		// Data is not valid - wait for the user to correct it before proceeding
		return;
	}

	// Convert the range from the user specified units
	Range.StartPitch = Converter.ReadAngle(Range.StartPitch);
	Range.StartRoll = Converter.ReadAngle(Range.StartRoll);
	Range.StartHeave = Converter.ReadDistance(Range.StartHeave);
	Range.StartRackTravel = Converter.ReadDistance(Range.StartRackTravel);
	Range.EndPitch = Converter.ReadAngle(Range.EndPitch);
	Range.EndRoll = Converter.ReadAngle(Range.EndRoll);
	Range.EndHeave = Converter.ReadDistance(Range.EndHeave);
	Range.EndRackTravel = Converter.ReadDistance(Range.EndRackTravel);

	// Update the iteration's range and number of points
	CurrentIteration->SetRange(Range);

	// Make sure the number of points is at least 2
	if (NumberOfPoints < 2)
		return;

	// Update the iteration's number of points
	CurrentIteration->SetNumberOfPoints(NumberOfPoints);

	// Update the display
	CurrentIteration->UpdateData();

	return;
}