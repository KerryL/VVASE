/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  vectorEditDialog.cpp
// Date:  7/6/2016
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Class for adding menu items to context menus that function more like dialogs.

// Local headers
#include "gui/dialogs/vectorEditDialog.h"
#include "vMath/vector.h"
#include "vUtilities/unitConverter.h"

namespace VVASE
{

//==========================================================================
// Class:			VectorEditDialog
// Function:		VectorEditDialog
//
// Description:		Constructor for VectorEditMenuItem class.
//
// Input Arguments:
//		parent	= wxWindow&
//		vector	= Vector&
//		name	= const wxString&
//		style	= long
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
VectorEditDialog::VectorEditDialog(wxWindow& parent, Vector& vector,
	const wxString& name, long style) : wxDialog(&parent, wxID_ANY,
	_T("Edit ") + name, wxDefaultPosition, wxDefaultSize, style), vector(vector)
{
	CreateControls();
	Center();
}

//==========================================================================
// Class:			VectorEditDialog
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
BEGIN_EVENT_TABLE(VectorEditDialog, wxDialog)
	EVT_BUTTON(wxID_OK,		VectorEditDialog::OKClickEvent)
	EVT_BUTTON(wxID_CANCEL,	VectorEditDialog::CancelClickEvent)
END_EVENT_TABLE();

//==========================================================================
// Class:			VectorEditDialog
// Function:		CreateControls
//
// Description:		Sets up this form's contents and size.
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
void VectorEditDialog::CreateControls()
{
	wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);

	// Second sizer gives more space around the controls
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	topSizer->Add(mainSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

	UnitConverter& converter(UnitConverter::GetInstance());
	wxString units(_T("(") + converter.GetUnits(converter.GetDistanceUnits()) + _T(")"));

	wxGridSizer *inputSizer = new wxGridSizer(3, 2, 5);
	mainSizer->Add(inputSizer);

	inputSizer->Add(new wxStaticText(this, wxID_ANY, _T("X ") + units), 0, wxALIGN_CENTER_HORIZONTAL);
	inputSizer->Add(new wxStaticText(this, wxID_ANY, _T("Y ") + units), 0, wxALIGN_CENTER_HORIZONTAL);
	inputSizer->Add(new wxStaticText(this, wxID_ANY, _T("Z ") + units), 0, wxALIGN_CENTER_HORIZONTAL);

	xInput = new wxTextCtrl(this, wxID_ANY, converter.FormatNumber(converter.ConvertDistanceOutput(vector.x)));
	yInput = new wxTextCtrl(this, wxID_ANY, converter.FormatNumber(converter.ConvertDistanceOutput(vector.y)));
	zInput = new wxTextCtrl(this, wxID_ANY, converter.FormatNumber(converter.ConvertDistanceOutput(vector.z)));

	inputSizer->Add(xInput);
	inputSizer->Add(yInput);
	inputSizer->Add(zInput);

	mainSizer->AddSpacer(15);

	mainSizer->Add(CreateButtonSizer(wxOK | wxCANCEL), 0, wxGROW);

	topSizer->SetSizeHints(this);
	SetSizer(topSizer);
}

//==========================================================================
// Class:			VectorEditDialog
// Function:		OKClickEvent
//
// Description:		Handles OK button click events.
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
void VectorEditDialog::OKClickEvent(wxCommandEvent& WXUNUSED(event))
{
	double xValue, yValue, zValue;
	if (!xInput->GetValue().ToDouble(&xValue) ||
		!yInput->GetValue().ToDouble(&yValue) ||
		!zInput->GetValue().ToDouble(&zValue))
	{
		wxMessageBox(_T("Input values must be numeric."), _T("Error processing input values"), wxOK, this);
		return;
	}

	UnitConverter& converter(UnitConverter::GetInstance());
	vector.x = converter.ConvertDistanceInput(xValue);
	vector.y = converter.ConvertDistanceInput(yValue);
	vector.z = converter.ConvertDistanceInput(zValue);

	// The way we handle this changes depending on how this form was displayed
	if (IsModal())
		EndModal(wxOK);
	else
	{
		SetReturnCode(wxOK);
		Show(false);
	}
}

//==========================================================================
// Class:			VectorEditDialog
// Function:		CancelClickEvent
//
// Description:		Handles cancel button click events.
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
void VectorEditDialog::CancelClickEvent(wxCommandEvent& WXUNUSED(event))
{
	// The way we handle this changes depending on how this form was displayed
	if (IsModal())
		EndModal(wxID_CANCEL);
	else
	{
		SetReturnCode(wxID_CANCEL);
		Show(false);
	}
}

}// namespace VVASE
