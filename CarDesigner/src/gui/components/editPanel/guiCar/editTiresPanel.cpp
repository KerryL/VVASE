/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  editTiresPanel.cpp
// Created:  2/10/2009
// Author:  K. Loux
// Description:  Contains the class definition for the EDIT_TIRES_PANEL class.  This
//				 class is used to edit the hard points on a particular corner of the
//				 car.
// History:

// CarDesigner headers
#include "vCar/tire.h"
#include "vCar/tireSet.h"
#include "gui/renderer/carRenderer.h"
#include "gui/guiCar.h"
#include "gui/components/mainFrame.h"
#include "gui/components/editPanel/editPanel.h"
#include "gui/components/editPanel/guiCar/editTiresPanel.h"
#include "vUtilities/convert.h"
#include "vMath/vector.h"

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
								   : wxScrolledWindow(&_Parent, id, pos, size),
								   debugger(_debugger),
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
	RightFrontDiameterUnitsLabel->SetLabel(Converter.GetUnitType(Convert::UnitTypeDistance));
	RightFrontWidthUnitsLabel->SetLabel(Converter.GetUnitType(Convert::UnitTypeDistance));

	LeftFrontDiameterUnitsLabel->SetLabel(Converter.GetUnitType(Convert::UnitTypeDistance));
	LeftFrontWidthUnitsLabel->SetLabel(Converter.GetUnitType(Convert::UnitTypeDistance));

	RightRearDiameterUnitsLabel->SetLabel(Converter.GetUnitType(Convert::UnitTypeDistance));
	RightRearWidthUnitsLabel->SetLabel(Converter.GetUnitType(Convert::UnitTypeDistance));

	LeftRearDiameterUnitsLabel->SetLabel(Converter.GetUnitType(Convert::UnitTypeDistance));
	LeftRearWidthUnitsLabel->SetLabel(Converter.GetUnitType(Convert::UnitTypeDistance));

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
	// Enable scrolling
	SetScrollRate(1, 1);

	// Top-level sizer
	wxBoxSizer *TopSizer = new wxBoxSizer(wxVERTICAL);

	// Second sizer gives more space around the controls
	wxFlexGridSizer *MainSizer = new wxFlexGridSizer(3, 5, 5);
	TopSizer->Add(MainSizer, 0, wxALL | wxEXPAND, 5);

	// Create the text controls
	// Right front
	wxStaticText *RightFrontLabel = new wxStaticText(this, wxID_ANY, _T("Right Front"));

	MainSizer->Add(RightFrontLabel, wxALIGN_CENTER_VERTICAL);
	MainSizer->AddSpacer(-1);
	MainSizer->AddSpacer(-1);

	// Diameter
	wxStaticText *RightFrontDiameterLabel = new wxStaticText(this, wxID_ANY, _T("Diameter"));
	RightFrontDiameterUnitsLabel = new wxStaticText(this, wxID_ANY, wxEmptyString);
	RightFrontTireDiameter = new wxTextCtrl(this, TextBoxRightFrontTireDiameter);

	MainSizer->Add(RightFrontDiameterLabel, 0, wxALIGN_CENTER_VERTICAL);
	MainSizer->Add(RightFrontTireDiameter, 0, wxEXPAND);
	MainSizer->Add(RightFrontDiameterUnitsLabel, 0, wxALIGN_CENTER_VERTICAL);

	// Width
	wxStaticText *RightFrontWidthLabel = new wxStaticText(this, wxID_ANY, _T("Width"));
	RightFrontWidthUnitsLabel = new wxStaticText(this, wxID_ANY, wxEmptyString);
	RightFrontTireWidth = new wxTextCtrl(this, TextBoxRightFrontTireWidth);

	MainSizer->Add(RightFrontWidthLabel, 0, wxALIGN_CENTER_VERTICAL);
	MainSizer->Add(RightFrontTireWidth, 0, wxEXPAND);
	MainSizer->Add(RightFrontWidthUnitsLabel, 0, wxALIGN_CENTER_VERTICAL);

	// Left front
	wxStaticText *LeftFrontLabel = new wxStaticText(this, wxID_ANY, _T("Left Front"));

	MainSizer->Add(LeftFrontLabel, wxALIGN_CENTER_VERTICAL);
	MainSizer->AddSpacer(-1);
	MainSizer->AddSpacer(-1);

	// Diameter
	wxStaticText *LeftFrontDiameterLabel = new wxStaticText(this, wxID_ANY, _T("Diameter"));
	LeftFrontDiameterUnitsLabel = new wxStaticText(this, wxID_ANY, wxEmptyString);
	LeftFrontTireDiameter = new wxTextCtrl(this, TextBoxLeftFrontTireDiameter);

	MainSizer->Add(LeftFrontDiameterLabel, 0, wxALIGN_CENTER_VERTICAL);
	MainSizer->Add(LeftFrontTireDiameter, 0, wxEXPAND);
	MainSizer->Add(LeftFrontDiameterUnitsLabel, 0, wxALIGN_CENTER_VERTICAL);

	// Width
	wxStaticText *LeftFrontWidthLabel = new wxStaticText(this, wxID_ANY, _T("Width"));
	LeftFrontWidthUnitsLabel = new wxStaticText(this, wxID_ANY, wxEmptyString);
	LeftFrontTireWidth = new wxTextCtrl(this, TextBoxLeftFrontTireWidth);

	MainSizer->Add(LeftFrontWidthLabel, 0, wxALIGN_CENTER_VERTICAL);
	MainSizer->Add(LeftFrontTireWidth, 0, wxEXPAND);
	MainSizer->Add(LeftFrontWidthUnitsLabel, 0, wxALIGN_CENTER_VERTICAL);

	// Right rear
	wxStaticText *RightRearLabel = new wxStaticText(this, wxID_ANY, _T("Right Rear"));

	MainSizer->Add(RightRearLabel, wxALIGN_CENTER_VERTICAL);
	MainSizer->AddSpacer(-1);
	MainSizer->AddSpacer(-1);

	// Diameter
	wxStaticText *RightRearDiameterLabel = new wxStaticText(this, wxID_ANY, _T("Diameter"));
	RightRearDiameterUnitsLabel = new wxStaticText(this, wxID_ANY, wxEmptyString);
	RightRearTireDiameter = new wxTextCtrl(this, TextBoxRightRearTireDiameter);

	MainSizer->Add(RightRearDiameterLabel, 0, wxALIGN_CENTER_VERTICAL);
	MainSizer->Add(RightRearTireDiameter, 0, wxEXPAND);
	MainSizer->Add(RightRearDiameterUnitsLabel, 0, wxALIGN_CENTER_VERTICAL);

	// Width
	wxStaticText *RightRearWidthLabel = new wxStaticText(this, wxID_ANY, _T("Width"));
	RightRearWidthUnitsLabel = new wxStaticText(this, wxID_ANY, wxEmptyString);
	RightRearTireWidth = new wxTextCtrl(this, TextBoxRightRearTireWidth);

	MainSizer->Add(RightRearWidthLabel, 0, wxALIGN_CENTER_VERTICAL);
	MainSizer->Add(RightRearTireWidth, 0, wxEXPAND);
	MainSizer->Add(RightRearWidthUnitsLabel, 0, wxALIGN_CENTER_VERTICAL);

	// Left rear
	wxStaticText *LeftRearLabel = new wxStaticText(this, wxID_ANY, _T("Left Rear"));

	MainSizer->Add(LeftRearLabel, wxALIGN_CENTER_VERTICAL);
	MainSizer->AddSpacer(-1);
	MainSizer->AddSpacer(-1);

	// Diameter
	wxStaticText *LeftRearDiameterLabel = new wxStaticText(this, wxID_ANY, _T("Diameter"));
	LeftRearDiameterUnitsLabel = new wxStaticText(this, wxID_ANY, wxEmptyString);
	LeftRearTireDiameter = new wxTextCtrl(this, TextBoxLeftRearTireDiameter);

	MainSizer->Add(LeftRearDiameterLabel, 0, wxALIGN_CENTER_VERTICAL);
	MainSizer->Add(LeftRearTireDiameter, 0, wxEXPAND);
	MainSizer->Add(LeftRearDiameterUnitsLabel, 0, wxALIGN_CENTER_VERTICAL);

	// Width
	wxStaticText *LeftRearWidthLabel = new wxStaticText(this, wxID_ANY, _T("Width"));
	LeftRearWidthUnitsLabel = new wxStaticText(this, wxID_ANY, wxEmptyString);
	LeftRearTireWidth = new wxTextCtrl(this, TextBoxLeftRearTireWidth);

	MainSizer->Add(LeftRearWidthLabel, 0, wxALIGN_CENTER_VERTICAL);
	MainSizer->Add(LeftRearTireWidth, 0, wxEXPAND);
	MainSizer->Add(LeftRearWidthUnitsLabel, 0, wxALIGN_CENTER_VERTICAL);

	// Assign the top level sizer to the dialog
	SetSizerAndFit(TopSizer);
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