/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  wxRelatedUtilities.h
// Date:  11/5/2011
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains helper functions and wrappers for working with wxWidgets.

#ifndef WX_RELATED_UTILIEIS_H_
#define WX_RELATED_UTILIEIS_H_

// Core headers
#include "VVASE/core/utilities/vvaseString.h"

// wxWidgets headers
#include <wx/wx.h>

namespace VVASE
{

namespace wxUtilities
{

// Method for finding the width of the longest string in the list
void SetMinimumWidthFromContents(wxComboBox *control, unsigned int additional);

// Method for finding the selected index of a combo box, even if it is still the
// default value (no selection has been made - fix for GTK)
int SafelyGetComboBoxSelection(wxComboBox *control);

// Mouse event don't propegate - but we can force them to by binding controls to this method
void SkipMouseEvent(wxMouseEvent &event);

vvaseString ToVVASEString(const wxString& s);

}// namespace wxUtilities

class EventWindowData : public wxObject
{
public:
	EventWindowData() {}
	EventWindowData(wxWindow* window) { SetWindow(window); }

	void SetWindow(wxWindow* window) { this->window = window; }
	wxWindow* GetWindow() const { return window; }

private:
	wxWindow *window;
};

}// namespace VVASE

#endif// WX_RELATED_UTILIEIS_H_
