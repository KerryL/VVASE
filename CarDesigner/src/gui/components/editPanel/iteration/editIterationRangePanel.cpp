/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  editIterationRangePanel.cpp
// Created:  11/14/2010
// Author:  K. Loux
// Description:  Contains the class definition for the EDIT_ITERATION_RANGE_PANEL
//				 class.
// History:

// CarDesigner headers
#include "gui/iteration.h"
#include "gui/components/mainFrame.h"
#include "gui/components/editPanel/editPanel.h"
#include "gui/components/editPanel/iteration/editIterationRangePanel.h"
#include "gui/components/editPanel/iteration/editIterationNotebook.h"
#include "vUtilities/convert.h"

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
													   wxScrolledWindow(&_Parent, id, pos, size), debugger(_debugger),
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
	// Enable scrolling
	SetScrollRate(1, 1);

	// Top-level sizer
	wxBoxSizer *TopSizer = new wxBoxSizer(wxVERTICAL);

	// Second sizer gives more space around the controls
	wxFlexGridSizer *MainSizer = new wxFlexGridSizer(4, 5, 5);
	TopSizer->Add(MainSizer, 0, wxEXPAND | wxALL, 5);

	// Add the text to this sizer
	wxStaticText *TextStart = new wxStaticText(this, wxID_STATIC, _T("Start"));
	wxStaticText *TextEnd = new wxStaticText(this, wxID_STATIC, _T("End"));
	MainSizer->AddSpacer(-1);
	MainSizer->Add(TextStart, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL);
	MainSizer->Add(TextEnd, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL);
	MainSizer->AddSpacer(-1);

	// Add the static text and text controls to these sizers
	// Pitch
	int textBoxWidth;
	GetTextExtent(Converter.FormatNumber(-888.0), &textBoxWidth, NULL);

	wxStaticText *PitchLabelText = new wxStaticText(this, wxID_STATIC, _T("Pitch"));
	StartPitchInput = new wxTextCtrl(this, RangeTextBox);
	EndPitchInput = new wxTextCtrl(this, RangeTextBox);
	wxStaticText *PitchUnitsText = new wxStaticText(this, wxID_STATIC,
		Converter.GetUnitType(Convert::UnitTypeAngle));
	MainSizer->Add(PitchLabelText, 0, wxALIGN_CENTER_VERTICAL);
	MainSizer->Add(StartPitchInput, 0, wxALIGN_CENTER_VERTICAL | wxEXPAND);
	MainSizer->Add(EndPitchInput, 0, wxALIGN_CENTER_VERTICAL | wxEXPAND);
	MainSizer->Add(PitchUnitsText, 0, wxALIGN_CENTER_VERTICAL);

	// Roll
	wxStaticText *RollLabelText = new wxStaticText(this, wxID_STATIC, _T("Roll"));
	StartRollInput = new wxTextCtrl(this, RangeTextBox);
	EndRollInput = new wxTextCtrl(this, RangeTextBox);
	wxStaticText *RollUnitsText = new wxStaticText(this, wxID_STATIC,
		Converter.GetUnitType(Convert::UnitTypeAngle));
	MainSizer->Add(RollLabelText, 0, wxALIGN_CENTER_VERTICAL);
	MainSizer->Add(StartRollInput, 0, wxALIGN_CENTER_VERTICAL | wxEXPAND);
	MainSizer->Add(EndRollInput, 0, wxALIGN_CENTER_VERTICAL | wxEXPAND);
	MainSizer->Add(RollUnitsText, 0, wxALIGN_CENTER_VERTICAL);

	// Heave
	wxStaticText *HeaveLabelText = new wxStaticText(this, wxID_STATIC, _T("Heave"));
	StartHeaveInput = new wxTextCtrl(this, RangeTextBox);
	EndHeaveInput = new wxTextCtrl(this, RangeTextBox);
	wxStaticText *HeaveUnitsText = new wxStaticText(this, wxID_STATIC,
		Converter.GetUnitType(Convert::UnitTypeDistance));
	MainSizer->Add(HeaveLabelText, 0, wxALIGN_CENTER_VERTICAL);
	MainSizer->Add(StartHeaveInput, 0, wxALIGN_CENTER_VERTICAL | wxEXPAND);
	MainSizer->Add(EndHeaveInput, 0, wxALIGN_CENTER_VERTICAL | wxEXPAND);
	MainSizer->Add(HeaveUnitsText, 0, wxALIGN_CENTER_VERTICAL);

	// Steer
	wxStaticText *SteerLabelText = new wxStaticText(this, wxID_STATIC, _T("Rack Travel"));
	StartSteerInput = new wxTextCtrl(this, RangeTextBox);
	EndSteerInput = new wxTextCtrl(this, RangeTextBox);
	wxStaticText *SteerUnitsText = new wxStaticText(this, wxID_STATIC,
		Converter.GetUnitType(Convert::UnitTypeDistance));
	MainSizer->Add(SteerLabelText, 0, wxALIGN_CENTER_VERTICAL);
	MainSizer->Add(StartSteerInput, 0, wxALIGN_CENTER_VERTICAL | wxEXPAND);
	MainSizer->Add(EndSteerInput, 0, wxALIGN_CENTER_VERTICAL | wxEXPAND);
	MainSizer->Add(SteerUnitsText, 0, wxALIGN_CENTER_VERTICAL);

	// Add a sizer for the number of points input
	wxStaticText *NumberOfPointsLabel = new wxStaticText(this, wxID_STATIC, _T("Number of points"));
	NumberOfPointsInput = new wxTextCtrl(this, RangeTextBox);
	MainSizer->Add(NumberOfPointsLabel, 0, wxALIGN_CENTER_VERTICAL);
	MainSizer->Add(NumberOfPointsInput, 0, wxALIGN_CENTER_VERTICAL | wxEXPAND);

	// Set minimum widths for text controls
	StartPitchInput->SetMinSize(wxSize(textBoxWidth, -1));
	StartRollInput->SetMinSize(wxSize(textBoxWidth, -1));
	StartHeaveInput->SetMinSize(wxSize(textBoxWidth, -1));
	StartSteerInput->SetMinSize(wxSize(textBoxWidth, -1));
	EndPitchInput->SetMinSize(wxSize(textBoxWidth, -1));
	EndRollInput->SetMinSize(wxSize(textBoxWidth, -1));
	EndHeaveInput->SetMinSize(wxSize(textBoxWidth, -1));
	EndSteerInput->SetMinSize(wxSize(textBoxWidth, -1));
	NumberOfPointsInput->SetMinSize(wxSize(textBoxWidth, -1));

	// Make the middle two columns the same width
	MainSizer->AddGrowableCol(1, 1);
	MainSizer->AddGrowableCol(2, 1);

	// Assign the top level sizer to the panel
	SetSizer(TopSizer);
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
}