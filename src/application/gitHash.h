/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  gitHash.h
// Date:  5/17/2008
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Description:  Defines the application-level constants.

#ifndef GIT_HASH_H_
#define GIT_HASH_H_

// wxWidgets headers
#include <wx/wx.h>

namespace VVASE
{

// The current version number
extern const wxString vvaseVersion;
extern const wxString vvaseGitHash;

// The application name
static const wxString vvaseName = "VVASE";
static const wxString vvaseLongName = "Virtual Vehicle Analysis and Simulation Environment";

}// namespace VVASE

#endif// GIT_HASH_H_
