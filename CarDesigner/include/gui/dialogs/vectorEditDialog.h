/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  VectorEditDialog.h
// Created:  7/6/2016
// Author:  K. Loux
// Description:  Class for adding menu items to context menus that function more like dialogs.

#ifndef VECTOR_EDIT_DIALOG_H_
#define VECTOR_EDIT_DIALOG_H_

// wxWidgets headers
#include <wx/wx.h>

// Local forward declarations
class Vector;

class VectorEditDialog : public wxDialog
{
public:
	VectorEditDialog(wxWindow &parent, Vector& vector, const wxString& name,
		long style = wxDEFAULT_DIALOG_STYLE);

private:
	Vector& vector;

	void CreateControls();

	wxTextCtrl* xInput;
	wxTextCtrl* yInput;
	wxTextCtrl* zInput;

	// Event handlers-----------------------------------------------------
	void OKClickEvent(wxCommandEvent &event);
	void CancelClickEvent(wxCommandEvent &event);
	// End event handlers-------------------------------------------------

	DECLARE_EVENT_TABLE()
};

#endif// VECTOR_EDIT_DIALOG_H_