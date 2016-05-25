/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  threadEvent.cpp
// Created:  11/3/2009
// Author:  K. Loux
// Description:  Eventually might house an actual event class - for now just an event
//				 definition.  This event is used for communication from the worker threads
//				 to the GUI thread.
// History:

// VVASE headers
#include "vSolver/threads/threadEvent.h"

// Define the EVT_THREAD event type
DEFINE_LOCAL_EVENT_TYPE(EVT_THREAD)