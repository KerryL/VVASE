/*===================================================================================
                                    CarDesigner
                           Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  rangeLimitsDialog.h
// Created:  5/12/2011
// Author:  K. Loux
// Description:  Dialog box for entering a minimum and maximum value for an axis.
// History:

#ifndef RANGE_LIMITS_DIALOG_H_
#define RANGE_LIMITS_DIALOG_H_

// wxWidgets headers
#include <wx/wx.h>

class RangeLimitsDialog : public wxDialog
{
public:
	RangeLimitsDialog(wxWindow *parent, const double &min, const double &max);

	double GetMinimum() const;
	double GetMaximum() const;

private:
	wxTextCtrl *minBox;
	wxTextCtrl *maxBox;

	// Overload from wxDialog
	virtual void OnOKButton(wxCommandEvent &event);

	DECLARE_EVENT_TABLE();
};

#endif// RANGE_LIMITS_DIALOG_H_