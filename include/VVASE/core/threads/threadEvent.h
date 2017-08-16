/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  threadEvent.h
// Date:  11/3/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Eventually might house an actual event class - for now just an event
//        declaration.  This event is used for communication from the worker threads
//        to the GUI thread.

#ifndef THREAD_EVENT_H_
#define THREAD_EVENT_H_

// wxWidgets headers
#include <wx/event.h>// TODO:  Remove

// Declaration of the EVT_THREAD event
DECLARE_LOCAL_EVENT_TYPE(EVT_THREAD, -1)

#endif// THREAD_EVENT_H_
