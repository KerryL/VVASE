/*===================================================================================
                                    CarDesigner
                           Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  plotPanel.h
// Created:  11/9/2011
// Author:  K. Loux
// Description:  Panel containing plot renderer and grid listing active plot channels.
// History:

#ifndef _PLOT_PANEL_H_
#define _PLOT_PANEL_H_

// wxWidgets headers
#include <wx/wx.h>

// Local forward declarations
class PlotRenderer;

class PlotPanel : wxPanel
{
public:
	// Constructor
	PlotPanel(wxWindow *parent);

	// Destructor
	~PlotPanel();

private:
	// Main panel object
	PlotRenderer *renderer;
};

#endif// _PLOT_PANEL_H_