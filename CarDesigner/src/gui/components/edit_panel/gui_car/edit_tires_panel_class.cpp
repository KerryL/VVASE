/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  edit_tires_panel_class.cpp
// Created:  2/10/2009
// Author:  K. Loux
// Description:  Contains the class definition for the EDIT_TIRES_PANEL class.  This
//				 class is used to edit the hard points on a particular corner of the
//				 car.
// History:

// CarDesigner headers
#include "vCar/tire_class.h"
#include "vCar/tireset_class.h"
#include "gui/renderer/car_renderer_class.h"
#include "gui/gui_car_class.h"
#include "gui/components/main_frame_class.h"
#include "gui/components/edit_panel/edit_panel_class.h"
#include "gui/components/edit_panel/gui_car/edit_tires_panel_class.h"
#include "vUtilities/convert_class.h"
#include "vMath/vector_class.h"

//==========================================================================
// Class:			EDIT_TIRES_PANEL
// Function:		EDIT_TIRES_PANEL
//
// Description:		Constructor for EDIT_TIRES_PANEL class.
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
EDIT_TIRES_PANEL::EDIT_TIRES_PANEL(EDIT_PANEL &_Parent, wxWindowID id,
								   const wxPoint& pos, const wxSize& size,
								   const Debugger &_debugger)
								   : wxPanel(&_Parent, id, pos, size),
								   Debugger(_debugger),
								   Converter(_Parent.GetMainFrame().GetConverter()),
								   Parent(_Parent)
										   
{
	// Create the controls
	CreateControls();
}

//==========================================================================
// Class:			EDIT_TIRES_PANEL
// Function:		EDIT_TIRES_PANEL
//
// Description:		Destructor for EDIT_TIRES_PANEL class.
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
EDIT_TIRES_PANEL::~EDIT_TIRES_PANEL()
{
}

//==========================================================================
// Class:			EDIT_TIRES_PANEL
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
BEGIN_EVENT_TABLE(EDIT_TIRES_PANEL, wxPanel)
	EVT_TEXT(TextBoxRightFrontTireDiameter,		EDIT_TIRES_PANEL::RightFrontTireDiameterChangeEvent)
	EVT_TEXT(TextBoxRightFrontTireWidth,		EDIT_TIRES_PANEL::RightFrontTireWidthChangeEvent)
	EVT_TEXT(TextBoxLeftFrontTireDiameter,		EDIT_TIRES_PANEL::LeftFrontTireDiameterChangeEvent)
	EVT_TEXT(TextBoxLeftFrontTireWidth,			EDIT_TIRES_PANEL::LeftFrontTireWidthChangeEvent)
	EVT_TEXT(TextBoxRightRearTireDiameter,		EDIT_TIRES_PANEL::RightRearTireDiameterChangeEvent)
	EVT_TEXT(TextBoxRightRearTireWidth,			EDIT_TIRES_PANEL::RightRearTireWidthChangeEvent)
	EVT_TEXT(TextBoxLeftRearTireDiameter,		EDIT_TIRES_PANEL::LeftRearTireDiameterChangeEvent)
	EVT_TEXT(TextBoxLeftRearTireWidth,			EDIT_TIRES_PANEL::LeftRearTireWidthChangeEvent)
END_EVENT_TABLE();

//==========================================================================
// Class:			EDIT_TIRES_PANEL
// Function:		UpdateInformation
//
// Description:		Updates the information on this panel.
//
// Input Arguments:
//		_CurrentTireSet	= TIRE_SET* pointing to the associated tire set
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void EDIT_TIRES_PANEL::UpdateInformation(TIRE_SET *_CurrentTireSet)
{
	// Update the class member
	CurrentTireSet = _CurrentTireSet;

	// FIXME:  Handle symmetry

	// Update the text boxes
	RightFrontTireDiameter->ChangeValue(Converter.FormatNumber(Converter.ConvertDistance(
		CurrentTireSet->RightFront->Diameter)));
	RightFrontTireWidth->ChangeValue(Converter.FormatNumber(Converter.ConvertDistance(
		CurrentTireSet->RightFront->Width)));

	LeftFrontTireDiameter->ChangeValue(Converter.FormatNumber(Converter.ConvertDistance(
		CurrentTireSet->LeftFront->Diameter)));
	LeftFrontTireWidth->ChangeValue(Converter.FormatNumber(Converter.ConvertDistance(
		CurrentTireSet->LeftFront->Width)));

	RightRearTireDiameter->ChangeValue(Converter.FormatNumber(Converter.ConvertDistance(
		CurrentTireSet->RightRear->Diameter)));
	RightRearTireWidth->ChangeValue(Converter.FormatNumber(Converter.ConvertDistance(
		CurrentTireSet->RightRear->Width)));

	LeftRearTireDiameter->ChangeValue(Converter.FormatNumber(Converter.ConvertDistance(
		CurrentTireSet->LeftRear->Diameter)));
	LeftRearTireWidth->ChangeValue(Converter.FormatNumber(Converter.ConvertDistance(
		CurrentTireSet->LeftRear->Width)));

	// And their units
	RightFrontDiameterUnitsLabel->SetLabel(Converter.GetUnitType(Convert::UNIT_TYPE_DISTANCE));
	RightFrontWidthUnitsLabel->SetLabel(Converter.GetUnitType(Convert::UNIT_TYPE_DISTANCE));

	LeftFrontDiameterUnitsLabel->SetLabel(Converter.GetUnitType(Convert::UNIT_TYPE_DISTANCE));
	LeftFrontWidthUnitsLabel->SetLabel(Converter.GetUnitType(Convert::UNIT_TYPE_DISTANCE));

	RightRearDiameterUnitsLabel->SetLabel(Converter.GetUnitType(Convert::UNIT_TYPE_DISTANCE));
	RightRearWidthUnitsLabel->SetLabel(Converter.GetUnitType(Convert::UNIT_TYPE_DISTANCE));

	LeftRearDiameterUnitsLabel->SetLabel(Converter.GetUnitType(Convert::UNIT_TYPE_DISTANCE));
	LeftRearWidthUnitsLabel->SetLabel(Converter.GetUnitType(Convert::UNIT_TYPE_DISTANCE));

	return;
}

//==========================================================================
// Class:			EDIT_TIRES_PANEL
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
void EDIT_TIRES_PANEL::CreateControls()
{
	// Top-level sizer
	wxBoxSizer *TopSizer = new wxBoxSizer(wxVERTICAL);

	// Second sizer gives more space around the controls
	wxBoxSizer *MainSizer = new wxBoxSizer(wxVERTICAL);
	TopSizer->Add(MainSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

	// The column widths
	int LabelColumnWidth = 100;
	int InputColumnWidth = 70;
	int UnitsColumnWidth = 50;

	// Create the text controls
	// Right front
	wxBoxSizer *RightFrontSizer = new wxBoxSizer(wxVERTICAL);
	wxStaticText *RightFrontLabel = new wxStaticText(this, wxID_ANY, _T("Right Front"));

	// Diameter
	wxBoxSizer *RightFrontDiameterSizer = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText *RightFrontDiameterLabel = new wxStaticText(this, wxID_ANY,
		_T("Diameter"), wxDefaultPosition, wxSize(LabelColumnWidth, -1));
	RightFrontDiameterUnitsLabel = new wxStaticText(this, wxID_ANY,
		wxEmptyString, wxDefaultPosition, wxSize(UnitsColumnWidth, -1));
	RightFrontTireDiameter = new wxTextCtrl(this, TextBoxRightFrontTireDiameter,
		wxEmptyString, wxDefaultPosition, wxSize(InputColumnWidth, -1));

	RightFrontDiameterSizer->Add(RightFrontDiameterLabel, 0, wxALIGN_CENTER_HORIZONTAL | wxLEFT | wxRIGHT, 5);
	RightFrontDiameterSizer->Add(RightFrontTireDiameter, 0, wxALIGN_CENTER_HORIZONTAL | wxLEFT | wxRIGHT, 5);
	RightFrontDiameterSizer->Add(RightFrontDiameterUnitsLabel, 0, wxALIGN_CENTER_HORIZONTAL | wxLEFT | wxRIGHT, 5);

	// Width
	wxBoxSizer *RightFrontWidthSizer = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText *RightFrontWidthLabel = new wxStaticText(this, wxID_ANY,
		_T("Width"), wxDefaultPosition, wxSize(LabelColumnWidth, -1));
	RightFrontWidthUnitsLabel = new wxStaticText(this, wxID_ANY,
		wxEmptyString, wxDefaultPosition, wxSize(UnitsColumnWidth, -1));
	RightFrontTireWidth = new wxTextCtrl(this, TextBoxRightFrontTireWidth,
		wxEmptyString, wxDefaultPosition, wxSize(InputColumnWidth, -1));

	RightFrontWidthSizer->Add(RightFrontWidthLabel, 0, wxALIGN_CENTER_HORIZONTAL | wxLEFT | wxRIGHT, 5);
	RightFrontWidthSizer->Add(RightFrontTireWidth, 0, wxALIGN_CENTER_HORIZONTAL | wxLEFT | wxRIGHT, 5);
	RightFrontWidthSizer->Add(RightFrontWidthUnitsLabel, 0, wxALIGN_CENTER_HORIZONTAL | wxLEFT | wxRIGHT, 5);

	RightFrontSizer->Add(RightFrontLabel, 0, wxALIGN_TOP | wxBOTTOM, 5);
	RightFrontSizer->Add(RightFrontDiameterSizer, 0, wxALIGN_TOP | wxBOTTOM, 1);
	RightFrontSizer->Add(RightFrontWidthSizer, 0, wxALIGN_TOP | wxTOP, 1);

	// Left front
	wxBoxSizer *LeftFrontSizer = new wxBoxSizer(wxVERTICAL);
	wxStaticText *LeftFrontLabel = new wxStaticText(this, wxID_ANY, _T("Left Front"));

	// Diameter
	wxBoxSizer *LeftFrontDiameterSizer = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText *LeftFrontDiameterLabel = new wxStaticText(this, wxID_ANY,
		_T("Diameter"), wxDefaultPosition, wxSize(LabelColumnWidth, -1));
	LeftFrontDiameterUnitsLabel = new wxStaticText(this, wxID_ANY,
		wxEmptyString, wxDefaultPosition, wxSize(UnitsColumnWidth, -1));
	LeftFrontTireDiameter = new wxTextCtrl(this, TextBoxLeftFrontTireDiameter,
		wxEmptyString, wxDefaultPosition, wxSize(InputColumnWidth, -1));

	LeftFrontDiameterSizer->Add(LeftFrontDiameterLabel, 0, wxALIGN_CENTER_HORIZONTAL | wxLEFT | wxRIGHT, 5);
	LeftFrontDiameterSizer->Add(LeftFrontTireDiameter, 0, wxALIGN_CENTER_HORIZONTAL | wxLEFT | wxRIGHT, 5);
	LeftFrontDiameterSizer->Add(LeftFrontDiameterUnitsLabel, 0, wxALIGN_CENTER_HORIZONTAL | wxLEFT | wxRIGHT, 5);

	// Width
	wxBoxSizer *LeftFrontWidthSizer = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText *LeftFrontWidthLabel = new wxStaticText(this, wxID_ANY,
		_T("Width"), wxDefaultPosition, wxSize(LabelColumnWidth, -1));
	LeftFrontWidthUnitsLabel = new wxStaticText(this, wxID_ANY,
		wxEmptyString, wxDefaultPosition, wxSize(UnitsColumnWidth, -1));
	LeftFrontTireWidth = new wxTextCtrl(this, TextBoxLeftFrontTireWidth,
		wxEmptyString, wxDefaultPosition, wxSize(InputColumnWidth, -1));

	LeftFrontWidthSizer->Add(LeftFrontWidthLabel, 0, wxALIGN_CENTER_HORIZONTAL | wxLEFT | wxRIGHT, 5);
	LeftFrontWidthSizer->Add(LeftFrontTireWidth, 0, wxALIGN_CENTER_HORIZONTAL | wxLEFT | wxRIGHT, 5);
	LeftFrontWidthSizer->Add(LeftFrontWidthUnitsLabel, 0, wxALIGN_CENTER_HORIZONTAL | wxLEFT | wxRIGHT, 5);

	LeftFrontSizer->Add(LeftFrontLabel, 0, wxALIGN_TOP | wxBOTTOM, 5);
	LeftFrontSizer->Add(LeftFrontDiameterSizer, 0, wxALIGN_TOP | wxBOTTOM, 1);
	LeftFrontSizer->Add(LeftFrontWidthSizer, 0, wxALIGN_TOP | wxTOP, 1);

	// Right rear
	wxBoxSizer *RightRearSizer = new wxBoxSizer(wxVERTICAL);
	wxStaticText *RightRearLabel = new wxStaticText(this, wxID_ANY, _T("Right Rear"));

	// Diameter
	wxBoxSizer *RightRearDiameterSizer = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText *RightRearDiameterLabel = new wxStaticText(this, wxID_ANY,
		_T("Diameter"), wxDefaultPosition, wxSize(LabelColumnWidth, -1));
	RightRearDiameterUnitsLabel = new wxStaticText(this, wxID_ANY,
		wxEmptyString, wxDefaultPosition, wxSize(UnitsColumnWidth, -1));
	RightRearTireDiameter = new wxTextCtrl(this, TextBoxRightRearTireDiameter,
		wxEmptyString, wxDefaultPosition, wxSize(InputColumnWidth, -1));

	RightRearDiameterSizer->Add(RightRearDiameterLabel, 0, wxALIGN_CENTER_HORIZONTAL | wxLEFT | wxRIGHT, 5);
	RightRearDiameterSizer->Add(RightRearTireDiameter, 0, wxALIGN_CENTER_HORIZONTAL | wxLEFT | wxRIGHT, 5);
	RightRearDiameterSizer->Add(RightRearDiameterUnitsLabel, 0, wxALIGN_CENTER_HORIZONTAL | wxLEFT | wxRIGHT, 5);

	// Width
	wxBoxSizer *RightRearWidthSizer = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText *RightRearWidthLabel = new wxStaticText(this, wxID_ANY,
		_T("Width"), wxDefaultPosition, wxSize(LabelColumnWidth, -1));
	RightRearWidthUnitsLabel = new wxStaticText(this, wxID_ANY,
		wxEmptyString, wxDefaultPosition, wxSize(UnitsColumnWidth, -1));
	RightRearTireWidth = new wxTextCtrl(this, TextBoxRightRearTireWidth,
		wxEmptyString, wxDefaultPosition, wxSize(InputColumnWidth, -1));

	RightRearWidthSizer->Add(RightRearWidthLabel, 0, wxALIGN_CENTER_HORIZONTAL | wxLEFT | wxRIGHT, 5);
	RightRearWidthSizer->Add(RightRearTireWidth, 0, wxALIGN_CENTER_HORIZONTAL | wxLEFT | wxRIGHT, 5);
	RightRearWidthSizer->Add(RightRearWidthUnitsLabel, 0, wxALIGN_CENTER_HORIZONTAL | wxLEFT | wxRIGHT, 5);

	RightRearSizer->Add(RightRearLabel, 0, wxALIGN_TOP | wxBOTTOM, 5);
	RightRearSizer->Add(RightRearDiameterSizer, 0, wxALIGN_TOP | wxBOTTOM, 1);
	RightRearSizer->Add(RightRearWidthSizer, 0, wxALIGN_TOP | wxTOP, 1);

	// Left rear
	wxBoxSizer *LeftRearSizer = new wxBoxSizer(wxVERTICAL);
	wxStaticText *LeftRearLabel = new wxStaticText(this, wxID_ANY, _T("Left Rear"));

	// Diameter
	wxBoxSizer *LeftRearDiameterSizer = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText *LeftRearDiameterLabel = new wxStaticText(this, wxID_ANY,
		_T("Diameter"), wxDefaultPosition, wxSize(LabelColumnWidth, -1));
	LeftRearDiameterUnitsLabel = new wxStaticText(this, wxID_ANY,
		wxEmptyString, wxDefaultPosition, wxSize(UnitsColumnWidth, -1));
	LeftRearTireDiameter = new wxTextCtrl(this, TextBoxLeftRearTireDiameter,
		wxEmptyString, wxDefaultPosition, wxSize(InputColumnWidth, -1));

	LeftRearDiameterSizer->Add(LeftRearDiameterLabel, 0, wxALIGN_CENTER_HORIZONTAL | wxLEFT | wxRIGHT, 5);
	LeftRearDiameterSizer->Add(LeftRearTireDiameter, 0, wxALIGN_CENTER_HORIZONTAL | wxLEFT | wxRIGHT, 5);
	LeftRearDiameterSizer->Add(LeftRearDiameterUnitsLabel, 0, wxALIGN_CENTER_HORIZONTAL | wxLEFT | wxRIGHT, 5);

	// Width
	wxBoxSizer *LeftRearWidthSizer = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText *LeftRearWidthLabel = new wxStaticText(this, wxID_ANY,
		_T("LR Width"), wxDefaultPosition, wxSize(LabelColumnWidth, -1));
	LeftRearWidthUnitsLabel = new wxStaticText(this, wxID_ANY,
		wxEmptyString, wxDefaultPosition, wxSize(UnitsColumnWidth, -1));
	LeftRearTireWidth = new wxTextCtrl(this, TextBoxLeftRearTireWidth,
		wxEmptyString, wxDefaultPosition, wxSize(InputColumnWidth, -1));

	LeftRearWidthSizer->Add(LeftRearWidthLabel, 0, wxALIGN_CENTER_HORIZONTAL | wxLEFT | wxRIGHT, 5);
	LeftRearWidthSizer->Add(LeftRearTireWidth, 0, wxALIGN_CENTER_HORIZONTAL | wxLEFT | wxRIGHT, 5);
	LeftRearWidthSizer->Add(LeftRearWidthUnitsLabel, 0, wxALIGN_CENTER_HORIZONTAL | wxLEFT | wxRIGHT, 5);

	LeftRearSizer->Add(LeftRearLabel, 0, wxALIGN_TOP | wxBOTTOM, 5);
	LeftRearSizer->Add(LeftRearDiameterSizer, 0, wxALIGN_TOP | wxBOTTOM, 1);
	LeftRearSizer->Add(LeftRearWidthSizer, 0, wxALIGN_TOP | wxTOP, 1);

	// Add the sizer for each corner to the main sizer
	MainSizer->Add(RightFrontSizer, 0, wxALIGN_BOTTOM | wxALL, 5);
	MainSizer->Add(LeftFrontSizer, 0, wxALIGN_BOTTOM | wxALL, 5);
	MainSizer->Add(RightRearSizer, 0, wxALIGN_BOTTOM | wxALL, 5);
	MainSizer->Add(LeftRearSizer, 0, wxALIGN_BOTTOM | wxALL, 5);

	// Assign the top level sizer to the dialog
	SetSizer(TopSizer);

	return;
}

//==========================================================================
// Class:			EDIT_TIRES_PANEL
// Function:		RightFrontTireDiameterChangeEvent
//
// Description:		Event that fires when the right front tire diameter changes.
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
void EDIT_TIRES_PANEL::RightFrontTireDiameterChangeEvent(wxCommandEvent &event)
{
	// Get the new value
	wxString ValueString = RightFrontTireDiameter->GetValue();
	double Value;

	// Make sure the value is numeric
	if (!ValueString.ToDouble(&Value))
		return;

	// Add the operation to the undo/redo stack
	Parent.GetMainFrame().GetUndoRedoStack().AddOperation(
		Parent.GetMainFrame().GetActiveIndex(),
		UNDO_REDO_STACK::OPERATION::DATA_TYPE_DOUBLE,
		&(CurrentTireSet->RightFront->Diameter));

	// Get a lock on the car
	wxMutex *Mutex = Parent.GetCurrentMutex();
	Mutex->Lock();

	// Update the tire object
	CurrentTireSet->RightFront->Diameter = Converter.ReadDistance(Value);

	// Unlock the car
	Mutex->Unlock();

	// Call the UpdateSymmetry method in case this is a symmetric suspension
	//Parent.UpdateSymmetry();// FIXME:  What if this is symmetric?

	// Tell the car object that it was modified
	Parent.GetCurrentObject()->SetModified();

	// Update the display and the kinematic outputs
	Parent.GetMainFrame().UpdateAnalysis();

	event.Skip();

	return;
}

//==========================================================================
// Class:			EDIT_TIRES_PANEL
// Function:		RightFrontTireWidthChangeEvent
//
// Description:		Event that fires when the right front tire width changes.
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
void EDIT_TIRES_PANEL::RightFrontTireWidthChangeEvent(wxCommandEvent &event)
{
	// Get the new value
	wxString ValueString = RightFrontTireWidth->GetValue();
	double Value;

	// Make sure the value is numeric
	if (!ValueString.ToDouble(&Value))
		return;

	// Add the operation to the undo/redo stack
	Parent.GetMainFrame().GetUndoRedoStack().AddOperation(
		Parent.GetMainFrame().GetActiveIndex(),
		UNDO_REDO_STACK::OPERATION::DATA_TYPE_DOUBLE,
		&(CurrentTireSet->RightFront->Width));

	// Get a lock on the car
	wxMutex *Mutex = Parent.GetCurrentMutex();
	Mutex->Lock();

	// Update the tire object
	CurrentTireSet->RightFront->Width = Converter.ReadDistance(Value);

	// Unlock the car
	Mutex->Unlock();

	// Call the UpdateSymmetry method in case this is a symmetric suspension
	//Parent.UpdateSymmetry();// FIXME:  What if this is symmetric?

	// Tell the car object that it was modified
	Parent.GetCurrentObject()->SetModified();

	// Update the display and the kinematic outputs
	Parent.GetMainFrame().UpdateAnalysis();

	event.Skip();

	return;
}

//==========================================================================
// Class:			EDIT_TIRES_PANEL
// Function:		LeftFrontTireDiameterChangeEvent
//
// Description:		Event that fires when the left front tire diameter changes.
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
void EDIT_TIRES_PANEL::LeftFrontTireDiameterChangeEvent(wxCommandEvent &event)
{
	// Get the new value
	wxString ValueString = LeftFrontTireDiameter->GetValue();
	double Value;

	// Make sure the value is numeric
	if (!ValueString.ToDouble(&Value))
		return;

	// Add the operation to the undo/redo stack
	Parent.GetMainFrame().GetUndoRedoStack().AddOperation(
		Parent.GetMainFrame().GetActiveIndex(),
		UNDO_REDO_STACK::OPERATION::DATA_TYPE_DOUBLE,
		&(CurrentTireSet->LeftFront->Diameter));

	// Get a lock on the car
	wxMutex *Mutex = Parent.GetCurrentMutex();
	Mutex->Lock();

	// Update the tire object
	CurrentTireSet->LeftFront->Diameter = Converter.ReadDistance(Value);

	// Unlock the car
	Mutex->Unlock();

	// Call the UpdateSymmetry method in case this is a symmetric suspension
	//Parent.UpdateSymmetry();// FIXME:  What if this is symmetric?

	// Tell the car object that it was modified
	Parent.GetCurrentObject()->SetModified();

	// Update the display and the kinematic outputs
	Parent.GetMainFrame().UpdateAnalysis();

	event.Skip();

	return;
}

//==========================================================================
// Class:			EDIT_TIRES_PANEL
// Function:		LeftFrontTireWidthChangeEvent
//
// Description:		Event that fires when the left front tire width changes.
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
void EDIT_TIRES_PANEL::LeftFrontTireWidthChangeEvent(wxCommandEvent &event)
{
	// Get the new value
	wxString ValueString = LeftFrontTireWidth->GetValue();
	double Value;

	// Make sure the value is numeric
	if (!ValueString.ToDouble(&Value))
		return;

	// Add the operation to the undo/redo stack
	Parent.GetMainFrame().GetUndoRedoStack().AddOperation(
		Parent.GetMainFrame().GetActiveIndex(),
		UNDO_REDO_STACK::OPERATION::DATA_TYPE_DOUBLE,
		&(CurrentTireSet->LeftFront->Width));

	// Get a lock on the car
	wxMutex *Mutex = Parent.GetCurrentMutex();
	Mutex->Lock();

	// Update the tire object
	CurrentTireSet->LeftFront->Width = Converter.ReadDistance(Value);

	// Unlock the car
	Mutex->Unlock();

	// Call the UpdateSymmetry method in case this is a symmetric suspension
	//Parent.UpdateSymmetry();// FIXME:  What if this is symmetric?

	// Tell the car object that it was modified
	Parent.GetCurrentObject()->SetModified();

	// Update the display and the kinematic outputs
	Parent.GetMainFrame().UpdateAnalysis();

	event.Skip();

	return;
}

//==========================================================================
// Class:			EDIT_TIRES_PANEL
// Function:		RightRearTireDiameterChangeEvent
//
// Description:		Event that fires when the right rear tire diameter changes.
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
void EDIT_TIRES_PANEL::RightRearTireDiameterChangeEvent(wxCommandEvent &event)
{
	// Get the new value
	wxString ValueString = RightRearTireDiameter->GetValue();
	double Value;

	// Make sure the value is numeric
	if (!ValueString.ToDouble(&Value))
		return;

	// Add the operation to the undo/redo stack
	Parent.GetMainFrame().GetUndoRedoStack().AddOperation(
		Parent.GetMainFrame().GetActiveIndex(),
		UNDO_REDO_STACK::OPERATION::DATA_TYPE_DOUBLE,
		&(CurrentTireSet->RightRear->Diameter));

	// Get a lock on the car
	wxMutex *Mutex = Parent.GetCurrentMutex();
	Mutex->Lock();

	// Update the tire object
	CurrentTireSet->RightRear->Diameter = Converter.ReadDistance(Value);

	// Unlock the car
	Mutex->Unlock();

	// Call the UpdateSymmetry method in case this is a symmetric suspension
	//Parent.UpdateSymmetry();// FIXME:  What if this is symmetric?

	// Tell the car object that it was modified
	Parent.GetCurrentObject()->SetModified();

	// Update the display and the kinematic outputs
	Parent.GetMainFrame().UpdateAnalysis();

	event.Skip();

	return;
}

//==========================================================================
// Class:			EDIT_TIRES_PANEL
// Function:		RightRearTireWidthChangeEvent
//
// Description:		Event that fires when the right rear tire width changes.
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
void EDIT_TIRES_PANEL::RightRearTireWidthChangeEvent(wxCommandEvent &event)
{
	// Get the new value
	wxString ValueString = RightRearTireWidth->GetValue();
	double Value;

	// Make sure the value is numeric
	if (!ValueString.ToDouble(&Value))
		return;

	// Add the operation to the undo/redo stack
	Parent.GetMainFrame().GetUndoRedoStack().AddOperation(
		Parent.GetMainFrame().GetActiveIndex(),
		UNDO_REDO_STACK::OPERATION::DATA_TYPE_DOUBLE,
		&(CurrentTireSet->RightRear->Width));

	// Get a lock on the car
	wxMutex *Mutex = Parent.GetCurrentMutex();
	Mutex->Lock();

	// Update the tire object
	CurrentTireSet->RightRear->Width = Converter.ReadDistance(Value);

	// Unlock the car
	Mutex->Unlock();

	// Call the UpdateSymmetry method in case this is a symmetric suspension
	//Parent.UpdateSymmetry();// FIXME:  What if this is symmetric?

	// Tell the car object that it was modified
	Parent.GetCurrentObject()->SetModified();

	// Update the display and the kinematic outputs
	Parent.GetMainFrame().UpdateAnalysis();

	event.Skip();

	return;
}

//==========================================================================
// Class:			EDIT_TIRES_PANEL
// Function:		LeftRearTireDiameterChangeEvent
//
// Description:		Event that fires when the left rear tire diameter changes.
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
void EDIT_TIRES_PANEL::LeftRearTireDiameterChangeEvent(wxCommandEvent &event)
{
	// Get the new value
	wxString ValueString = LeftRearTireDiameter->GetValue();
	double Value;

	// Make sure the value is numeric
	if (!ValueString.ToDouble(&Value))
		return;

	// Add the operation to the undo/redo stack
	Parent.GetMainFrame().GetUndoRedoStack().AddOperation(
		Parent.GetMainFrame().GetActiveIndex(),
		UNDO_REDO_STACK::OPERATION::DATA_TYPE_DOUBLE,
		&(CurrentTireSet->LeftRear->Diameter));

	// Get a lock on the car
	wxMutex *Mutex = Parent.GetCurrentMutex();
	Mutex->Lock();

	// Update the tire object
	CurrentTireSet->LeftRear->Diameter = Converter.ReadDistance(Value);

	// Unlock the car
	Mutex->Unlock();

	// Call the UpdateSymmetry method in case this is a symmetric suspension
	//Parent.UpdateSymmetry();// FIXME:  What if this is symmetric?

	// Tell the car object that it was modified
	Parent.GetCurrentObject()->SetModified();

	// Update the display and the kinematic outputs
	Parent.GetMainFrame().UpdateAnalysis();

	event.Skip();

	return;
}

//==========================================================================
// Class:			EDIT_TIRES_PANEL
// Function:		LeftRearTireWidthChangeEvent
//
// Description:		Event that fires when the left rear tire width changes.
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
void EDIT_TIRES_PANEL::LeftRearTireWidthChangeEvent(wxCommandEvent &event)
{
	// Get the new value
	wxString ValueString = LeftRearTireWidth->GetValue();
	double Value;

	// Make sure the value is numeric
	if (!ValueString.ToDouble(&Value))
		return;

	// Add the operation to the undo/redo stack
	Parent.GetMainFrame().GetUndoRedoStack().AddOperation(
		Parent.GetMainFrame().GetActiveIndex(),
		UNDO_REDO_STACK::OPERATION::DATA_TYPE_DOUBLE,
		&(CurrentTireSet->LeftRear->Width));

	// Get a lock on the car
	wxMutex *Mutex = Parent.GetCurrentMutex();
	Mutex->Lock();

	// Update the tire object
	CurrentTireSet->LeftRear->Width = Converter.ReadDistance(Value);

	// Unlock the car
	Mutex->Unlock();

	// Call the UpdateSymmetry method in case this is a symmetric suspension
	//Parent.UpdateSymmetry();// FIXME:  What if this is symmetric?

	// Tell the car object that it was modified
	Parent.GetCurrentObject()->SetModified();

	// Update the display and the kinematic outputs
	Parent.GetMainFrame().UpdateAnalysis();

	event.Skip();

	return;
}