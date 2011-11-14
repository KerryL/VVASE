/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  gaGeneDialog.h
// Created:  7/30/2009
// Author:  K. Loux
// Description:  Dialog for editing gene properties.
// History:

// wxWidgets headers
#include <wx/wx.h>

// CarDesigner headers
#include "vCar/corner.h"
#include "vUtilities/convert.h"

// CarDesigner forward declarations
class MAIN_FRAME;

class GA_GENE_DIALOG : public wxDialog
{
public:
	// Constructor
	GA_GENE_DIALOG(wxWindow *Parent, const Convert &_Converter, const CORNER::HARDPOINTS &_Hardpoint,
		const CORNER::HARDPOINTS &_TiedTo, const Vector::Axis &_AxisDirection, const CORNER::LOCATION &_CornerLocation,
		const double &_Minimum, const double &_Maximum, const unsigned int &_NumberOfValues,
		wxWindowID Id, const wxPoint &Position, long Style = wxDEFAULT_DIALOG_STYLE);

	// Destructor
	~GA_GENE_DIALOG();

	// Private data accessors
	CORNER::HARDPOINTS GetHardpoint(void) const { return Hardpoint; };
	CORNER::HARDPOINTS GetTiedTo(void) const { return TiedTo; };
	Vector::Axis GetAxisDirection(void) const { return AxisDirection; };
	CORNER::LOCATION GetCornerLocation(void) const { return CornerLocation; };
	double GetMinimum(void) const { return Minimum; };
	double GetMaximum(void) const { return Maximum; };
	unsigned long GetNumberOfValues(void) const { return NumberOfValues; };

private:
	// The object that handles the unit conversions between the input and output
	const Convert &Converter;

	// Method for creating controls
	void CreateControls(void);

	// Controls
	wxComboBox *HardpointCombo;
	wxComboBox *TiedToCombo;
	wxComboBox *AxisDirectionCombo;
	wxComboBox *CornerLocationCombo;

	wxTextCtrl *MinimumText;
	wxTextCtrl *MaximumText;
	wxTextCtrl *NumberOfValuesText;

	wxStaticText *Resolution;

	// Values (populated when OK is clicked)
	CORNER::HARDPOINTS Hardpoint;
	CORNER::HARDPOINTS TiedTo;
	Vector::Axis AxisDirection;
	CORNER::LOCATION CornerLocation;
	double Minimum;
	double Maximum;
	unsigned long NumberOfValues;

	// Event handlers
	virtual void OKClickEvent(wxCommandEvent &event);
	virtual void CancelClickEvent(wxCommandEvent &event);
	virtual void TextChangeEvent(wxCommandEvent &event);

	// For the event table
	DECLARE_EVENT_TABLE();
};