/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  textInputDialog.h
// Date:  5/20/2013
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Dialog box similar to ::wxGetTextFromUser() but allows differentiation between
//        canceling and returning an empty string.

#ifndef TEXT_INPUT_DIALOG_H_
#define TEXT_INPUT_DIALOG_H_

// wxWidgets headers
#include <wx/dialog.h>

namespace VVASE

class TextInputDialog : public wxDialog
{
public:
	TextInputDialog(const wxString &message, const wxString &title, const wxString &defaultText, wxWindow *parent);
	virtual ~TextInputDialog();

	wxString GetText(void) const { return text->GetValue(); }

private:
	wxTextCtrl *text;

	void CreateControls(const wxString &message, const wxString &defaultText);
};

}// namespace VVASE

#endif// TEXT_INPUT_DIALOG_H_
