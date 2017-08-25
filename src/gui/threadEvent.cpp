/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  threadEvent.cpp
// Date:  11/3/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Definitiona for event used to communicate from the worker threads to
//        the GUI thread.

// Local headers
#include "VVASE/gui/threadEvent.h"

// Define the EVT_THREAD event type
DEFINE_LOCAL_EVENT_TYPE(EVT_THREAD)
