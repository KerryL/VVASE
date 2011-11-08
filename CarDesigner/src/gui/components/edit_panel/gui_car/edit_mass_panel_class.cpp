/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  edit_mass_panel_class.cpp
// Created:  2/10/2009
// Author:  K. Loux
// Description:  Contains the class definition for the EDIT_MASS_PANEL class.  This
//				 class is used to edit the hard points on a particular corner of the
//				 car.
// History:

// CarDesigner headers
#include "vCar/mass_class.h"
#include "gui/renderer/car_renderer_class.h"
#include "gui/gui_car_class.h"
#include "gui/components/main_frame_class.h"
#include "gui/components/edit_panel/edit_panel_class.h"
#include "gui/components/edit_panel/gui_car/edit_mass_panel_class.h"
#include "vUtilities/convert_class.h"
#include "vMath/vector_class.h"

//==========================================================================
// Class:			EDIT_MASS_PANEL
// Function:		EDIT_MASS_PANEL
//
// Description:		Constructor for EDIT_MASS_PANEL class.
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
EDIT_MASS_PANEL::EDIT_MASS_PANEL(EDIT_PANEL &_Parent, wxWindowID id,
								 const wxPoint& pos, const wxSize& size,
								 const Debugger &_debugger) : wxPanel(&_Parent, id, pos, size),
								 Debugger(_debugger),
								 Converter(_Parent.GetMainFrame().GetConverter()),
								 Parent(_Parent)
{
	// Create the controls
	CreateControls();
}

//==========================================================================
// Class:			EDIT_MASS_PANEL
// Function:		EDIT_MASS_PANEL
//
// Description:		Destructor for EDIT_MASS_PANEL class.
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
EDIT_MASS_PANEL::~EDIT_MASS_PANEL()
{
}

//==========================================================================
// Class:			EDIT_MASS_PANEL
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
BEGIN_EVENT_TABLE(EDIT_MASS_PANEL, wxPanel)
	EVT_TEXT(wxID_ANY,	EDIT_MASS_PANEL::TextBoxEditEvent)
END_EVENT_TABLE();

//==========================================================================
// Class:			EDIT_MASS_PANEL
// Function:		UpdateInformation
//
// Description:		Updates the information on this panel.
//
// Input Arguments:
//		_CurrentMassProperties	= MASS_PROPERTIES* pointing to the associated
//								  mass properties
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void EDIT_MASS_PANEL::UpdateInformation(MASS_PROPERTIES *_CurrentMassProperties)
{
	// Update the class member
	CurrentMassProperties = _CurrentMassProperties;

	// Update the mass
	Mass->ChangeValue(Converter.FormatNumber(Converter.ConvertMass(CurrentMassProperties->Mass)));

	// Update the inertia
	Ixx->ChangeValue(Converter.FormatNumber(Converter.ConvertMass(CurrentMassProperties->Ixx)));
	Iyy->ChangeValue(Converter.FormatNumber(Converter.ConvertMass(CurrentMassProperties->Iyy)));
	Izz->ChangeValue(Converter.FormatNumber(Converter.ConvertMass(CurrentMassProperties->Izz)));
	Ixy->ChangeValue(Converter.FormatNumber(Converter.ConvertMass(CurrentMassProperties->Ixy)));
	Iyx->SetLabel(Converter.FormatNumber(Converter.ConvertMass(CurrentMassProperties->Ixy)));
	Ixz->ChangeValue(Converter.FormatNumber(Converter.ConvertMass(CurrentMassProperties->Ixz)));
	Izx->SetLabel(Converter.FormatNumber(Converter.ConvertMass(CurrentMassProperties->Ixz)));
	Iyz->ChangeValue(Converter.FormatNumber(Converter.ConvertMass(CurrentMassProperties->Iyz)));
	Izy->SetLabel(Converter.FormatNumber(Converter.ConvertMass(CurrentMassProperties->Iyz)));

	// Update the center of gravity
	CenterOfGravityX->ChangeValue(Converter.FormatNumber(
		Converter.ConvertDistance(CurrentMassProperties->CenterOfGravity.X)));
	CenterOfGravityY->ChangeValue(Converter.FormatNumber(
		Converter.ConvertDistance(CurrentMassProperties->CenterOfGravity.Y)));
	CenterOfGravityZ->ChangeValue(Converter.FormatNumber(
		Converter.ConvertDistance(CurrentMassProperties->CenterOfGravity.Z)));

	// Update the units
	InertiaUnitsLabel->SetLabel('(' + Converter.GetUnitType(Convert::UNIT_TYPE_INERTIA) + ')');
	MassUnitsLabel->SetLabel('(' + Converter.GetUnitType(Convert::UNIT_TYPE_MASS) + ')');
	CoGUnitsLabel->SetLabel('(' + Converter.GetUnitType(Convert::UNIT_TYPE_DISTANCE) + ')');

	return;
}

//==========================================================================
// Class:			EDIT_MASS_PANEL
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
void EDIT_MASS_PANEL::CreateControls()
{
	// Top-level sizer
	wxBoxSizer *TopSizer = new wxBoxSizer(wxVERTICAL);

	// Second sizer gives more space around the controls
	wxBoxSizer *MainSizer = new wxBoxSizer(wxVERTICAL);
	TopSizer->Add(MainSizer, 1, wxALIGN_CENTER_HORIZONTAL | wxGROW | wxALL, 5);

	// The column widths
	int LabelColumnWidth = 30;
	int InputColumnWidth = 60;
	int UnitsColumnWidth = 40;

	// Create the inertia inputs
	wxBoxSizer *InertiaSizer = new wxBoxSizer(wxVERTICAL);
	wxStaticText *InertiaCaption = new wxStaticText(this, wxID_ANY,
		_T("Sprung Mass Inertia Tensor (w.r.t. CG)"));
	InertiaUnitsLabel = new wxStaticText(this, wxID_ANY, wxEmptyString);

	InertiaSizer->Add(InertiaCaption, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
	InertiaSizer->Add(InertiaUnitsLabel, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 2);

	// Create the text boxes
	Ixx = new wxTextCtrl(this, TextBoxIxx, wxEmptyString, wxDefaultPosition,
		wxSize(InputColumnWidth, -1));
	Iyy = new wxTextCtrl(this, TextBoxIyy, wxEmptyString, wxDefaultPosition,
		wxSize(InputColumnWidth, -1));
	Izz = new wxTextCtrl(this, TextBoxIzz, wxEmptyString, wxDefaultPosition,
		wxSize(InputColumnWidth, -1));
	Ixy = new wxTextCtrl(this, TextBoxIxy, wxEmptyString, wxDefaultPosition,
		wxSize(InputColumnWidth, -1));
	Ixz = new wxTextCtrl(this, TextBoxIxz, wxEmptyString, wxDefaultPosition,
		wxSize(InputColumnWidth, -1));
	Iyz = new wxTextCtrl(this, TextBoxIyz, wxEmptyString, wxDefaultPosition,
		wxSize(InputColumnWidth, -1));

	// Create the static text controls
	Iyx = new wxStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(InputColumnWidth, -1));
	Izx = new wxStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(InputColumnWidth, -1));
	Izy = new wxStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(InputColumnWidth, -1));

	// First row
	wxBoxSizer *InertiaRow1 = new wxBoxSizer(wxHORIZONTAL);
	InertiaRow1->Add(Ixx, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
	InertiaRow1->Add(Ixy, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
	InertiaRow1->Add(Ixz, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);

	// Second row
	wxBoxSizer *InertiaRow2 = new wxBoxSizer(wxHORIZONTAL);
	InertiaRow2->Add(Iyx, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
	InertiaRow2->Add(Iyy, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
	InertiaRow2->Add(Iyz, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);

	// Third row
	wxBoxSizer *InertiaRow3 = new wxBoxSizer(wxHORIZONTAL);
	InertiaRow3->Add(Izx, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
	InertiaRow3->Add(Izy, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
	InertiaRow3->Add(Izz, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);

	// Add the rows to the inertia sizer
	InertiaSizer->Add(InertiaRow1);
	InertiaSizer->Add(InertiaRow2);
	InertiaSizer->Add(InertiaRow3);

	// Re-size the columns
	InputColumnWidth = 50;

	// Create the mass input
	wxBoxSizer *MassSizer = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText *MassLabel = new wxStaticText(this, wxID_ANY, _T("Mass"), wxDefaultPosition,
		wxSize(LabelColumnWidth, -1));
	Mass = new wxTextCtrl(this, TextBoxMass, wxEmptyString, wxDefaultPosition, wxSize(InputColumnWidth, -1));
	MassUnitsLabel = new wxStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
		wxSize(UnitsColumnWidth, -1));

	MassSizer->Add(MassLabel, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
	MassSizer->Add(Mass, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
	MassSizer->Add(MassUnitsLabel, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

	// Create the center of gravity inputs
	wxBoxSizer *CenterOfGravitySizer = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText *CoGLabel = new wxStaticText(this, wxID_ANY, _T("CG"), wxDefaultPosition,
		wxSize(LabelColumnWidth, -1));
	CenterOfGravityX = new wxTextCtrl(this, TextBoxCenterOfGravityX, wxEmptyString, wxDefaultPosition,
		wxSize(InputColumnWidth, -1));
	CenterOfGravityY = new wxTextCtrl(this, TextBoxCenterOfGravityY, wxEmptyString, wxDefaultPosition,
		wxSize(InputColumnWidth, -1));
	CenterOfGravityZ = new wxTextCtrl(this, TextBoxCenterOfGravityZ, wxEmptyString, wxDefaultPosition,
		wxSize(InputColumnWidth, -1));
	CoGUnitsLabel = new wxStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(UnitsColumnWidth, -1));

	CenterOfGravitySizer->Add(CoGLabel, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
	CenterOfGravitySizer->Add(CenterOfGravityX, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
	CenterOfGravitySizer->Add(CenterOfGravityY, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
	CenterOfGravitySizer->Add(CenterOfGravityZ, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
	CenterOfGravitySizer->Add(CoGUnitsLabel, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

	// Add the sizers to the main sizer
	MainSizer->Add(InertiaSizer);
	MainSizer->AddSpacer(15);
	MainSizer->Add(MassSizer);
	MainSizer->Add(CenterOfGravitySizer);

	// Assign the top level sizer to the dialog
	SetSizer(TopSizer);

	return;
}

//==========================================================================
// Class:			EDIT_MASS_PANEL
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
void EDIT_MASS_PANEL::TextBoxEditEvent(wxCommandEvent &event)
{
	// The place where the data will be written
	double *DataLocation = NULL;

	// We also need to know what kind of data we're retrieving
	Convert::UNIT_TYPE Units;

	// A pointer to the text box
	wxTextCtrl *TextBox = NULL;

	// Take different action depending on the event ID
	switch (event.GetId())
	{
	case TextBoxIxx:
		// Get the text box, the location to write it, and the units
		TextBox = Ixx;
		DataLocation = &CurrentMassProperties->Ixx;
		Units = Convert::UNIT_TYPE_INERTIA;
		break;

	case TextBoxIyy:
		// Get the new data, the location to write it, and the units
		TextBox = Iyy;
		DataLocation = &CurrentMassProperties->Iyy;
		Units = Convert::UNIT_TYPE_INERTIA;
		break;

	case TextBoxIzz:
		// Get the new data, the location to write it, and the units
		TextBox = Izz;
		DataLocation = &CurrentMassProperties->Izz;
		Units = Convert::UNIT_TYPE_INERTIA;
		break;

	case TextBoxIxy:
		// Get the new data, the location to write it, and the units
		TextBox = Ixy;
		DataLocation = &CurrentMassProperties->Ixy;
		Units = Convert::UNIT_TYPE_INERTIA;
		break;

	case TextBoxIxz:
		// Get the new data, the location to write it, and the units
		TextBox = Ixz;
		DataLocation = &CurrentMassProperties->Ixz;
		Units = Convert::UNIT_TYPE_INERTIA;
		break;

	case TextBoxIyz:
		// Get the new data, the location to write it, and the units
		TextBox = Iyz;
		DataLocation = &CurrentMassProperties->Iyz;
		Units = Convert::UNIT_TYPE_INERTIA;
		break;

	case TextBoxMass:
		// Get the new data, the location to write it, and the units
		TextBox = Mass;
		DataLocation = &CurrentMassProperties->Mass;
		Units = Convert::UNIT_TYPE_MASS;
		break;

	case TextBoxCenterOfGravityX:
		// Get the new data, the location to write it, and the units
		TextBox = CenterOfGravityX;
		DataLocation = &CurrentMassProperties->CenterOfGravity.X;
		Units = Convert::UNIT_TYPE_DISTANCE;
		break;

	case TextBoxCenterOfGravityY:
		// Get the new data, the location to write it, and the units
		TextBox = CenterOfGravityY;
		DataLocation = &CurrentMassProperties->CenterOfGravity.Y;
		Units = Convert::UNIT_TYPE_DISTANCE;
		break;

	case TextBoxCenterOfGravityZ:
		// Get the new data, the location to write it, and the units
		TextBox = CenterOfGravityZ;
		DataLocation = &CurrentMassProperties->CenterOfGravity.Z;
		Units = Convert::UNIT_TYPE_DISTANCE;
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
		return;

	// Add the operation to the undo/redo stack
	Parent.GetMainFrame().GetUndoRedoStack().AddOperation(
		Parent.GetMainFrame().GetActiveIndex(),
		UNDO_REDO_STACK::OPERATION::DATA_TYPE_DOUBLE,
		DataLocation);

	// Get a lock on the car
	wxMutex *Mutex = Parent.GetCurrentMutex();
	Mutex->Lock();

	// Update the mass properties object
	*DataLocation = Converter.Read(Value, Units);

	// Unlock the car
	Mutex->Unlock();

	// Tell the car object that it was modified
	Parent.GetCurrentObject()->SetModified();

	// Update the display and the kinematic outputs
	Parent.GetMainFrame().UpdateAnalysis();

	// If one of the off-diagonal inertias was updated, we need to change the
	// corresponding value on the lower side of the diagonal
	Iyx->SetLabel(Converter.FormatNumber(Converter.ConvertMass(CurrentMassProperties->Ixy)));
	Izx->SetLabel(Converter.FormatNumber(Converter.ConvertMass(CurrentMassProperties->Ixz)));
	Izy->SetLabel(Converter.FormatNumber(Converter.ConvertMass(CurrentMassProperties->Iyz)));

	event.Skip();

	return;
}