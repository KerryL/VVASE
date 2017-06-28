/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016
===================================================================================*/

// File:  threadEvent.cpp
// Date:  11/3/2009
// Author:  K. Loux
// Desc:  Eventually might house an actual event class - for now just an event
//        definition.  This event is used for communication from the worker threads
//        to the GUI thread.

// VVASE headers
#include "vSolver/threads/threadEvent.h"

// Define the EVT_THREAD event type
DEFINE_LOCAL_EVENT_TYPE(EVT_THREAD)
