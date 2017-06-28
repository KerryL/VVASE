/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  dropTarget.h
// Date:  10/20/2010
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Derives from wxFileDropTarget and overrides OnDropFiles to load files
//        when the user drags-and-drops them onto the main window.

#ifndef DROP_TARGET_H_
#define DROP_TARGET_H_

// wxWidgets headers
#include <wx/dnd.h>

// VVASE forward declarations
class MainFrame;

// The main class declaration
class DropTarget : public wxFileDropTarget
{
public:
	DropTarget(MainFrame &mainFrame);
	~DropTarget();

private:
	MainFrame &mainFrame;

	// Required override of virtual OnDropFiles handler
	virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString &filenames);
};

#endif// DROP_TARGET_H_