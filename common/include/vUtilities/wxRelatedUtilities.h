/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  wxRelatedUtilites.h
// Created:  11/5/2011
// Author:  K. Loux
// Description:  Contains helper functions and wrappers for working with wxWidgets.
// History:

#ifndef _WX_RELATED_UTILIEIS_H_
#define _WX_RELATED_UTILIEIS_H_

// wxWidgets headers
#include <wx/wx.h>

// Method for finding the width of the longest string in the list
void SetMinimumWidthFromContents(wxComboBox *control, unsigned int additional);

#endif// _WX_RELATED_UTILIEIS_H_