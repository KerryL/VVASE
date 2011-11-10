/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  main_frame_class.h
// Created:  3/7/2008
// Author:  K. Loux
// Description:  Contains the class declaration for the MAIN_FRAME class.  Uses wxWidgets
//				 for the GUI components.  Also contains the application class definition
//				 and functionality at the bottom of this file (minimal).
// History:

// VVASE headers
#include "gui/drop_target_class.h"
#include "gui/components/main_frame_class.h"

//==========================================================================
// Class:			DROP_TARGET
// Function:		DROP_TARGET
//
// Description:		Constructor for DROP_TARGET class.
//
// Input Arguments:
//		_MainFrame	= &MAIN_FRAME, reference to main application window
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
DROP_TARGET::DROP_TARGET(MAIN_FRAME &_MainFrame) : MainFrame(_MainFrame)
{
}

//==========================================================================
// Class:			DROP_TARGET
// Function:		~DROP_TARGET
//
// Description:		Destructor for DROP_TARGET class.
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
DROP_TARGET::~DROP_TARGET()
{
}

//==========================================================================
// Class:			DROP_TARGET
// Function:		OnDropFiles
//
// Description:		Overloaded virtual method from wxFileDropTarget.
//
// Input Arguments:
//		x			= wxCoord (unused)
//		y			= wxCoord (unused)
//		filenames	= const &wxArrayString containing the list of filenames
//					  being dropped
//
// Output Arguments:
//		None
//
// Return Value:
//		true to accept the data, false to veto
//
//==========================================================================
bool DROP_TARGET::OnDropFiles(wxCoord WXUNUSED(x), wxCoord WXUNUSED(y),
							  const wxArrayString &filenames)
{
	// Load each file
	unsigned int i;
	for (i = 0; i < filenames.Count(); i++)
		MainFrame.LoadFile(filenames[i]);

	return true;
}