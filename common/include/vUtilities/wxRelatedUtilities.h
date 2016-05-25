/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  wxRelatedUtilities.h
// Created:  11/5/2011
// Author:  K. Loux
// Description:  Contains helper functions and wrappers for working with wxWidgets.
// History:

#ifndef WX_RELATED_UTILIEIS_H_
#define WX_RELATED_UTILIEIS_H_

// wxWidgets headers
#include <wx/wx.h>

// Method for finding the width of the longest string in the list
void SetMinimumWidthFromContents(wxComboBox *control, unsigned int additional);

// Method for finding the selected index of a combo box, even if it is still the
// default value (no selection has been made - fix for GTK)
int SafelyGetComboBoxSelection(wxComboBox *control);

// Mouse event don't propegate - but we can force them to by binding controls to this method
void SkipMouseEvent(wxMouseEvent &event);

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

#endif// WX_RELATED_UTILIEIS_H_