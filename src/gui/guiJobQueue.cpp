/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  guiJobQueue.cpp
// Date:  8/25/2017
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  JobQueue wrapper when communication with GUI thread is required.

// Local headers
#include "VVASE/gui/guiJobQueue.h"
#include "VVASE/gui/threadEvent.h"
#include "VVASE/core/threads/threadData.h"

namespace VVASE
{

//==========================================================================
// Class:			GuiJobQueue
// Function:		GuiJobQueue
//
// Description:		Constructor for the GuiJobQueue class.
//
// Input Arguments:
//		_parent		= wxEventHandler* pointing to the main application object
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
GuiJobQueue::GuiJobQueue(wxEvtHandler *parent) : parent(parent)
{
}

//==========================================================================
// Class:			GuiJobQueue
// Function:		Report
//
// Description:		Posts an event to the main thread.
//
// Input Arguments:
//		Command		= const ThreadCommand& to report
//		Message		= const wxString& containing string information
//		ThreadId	= const std::thread::id& representing the thread's ID
//		ObjectId	= int representing the object's ID
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GuiJobQueue::Report(const ThreadJob::ThreadCommand& command, const std::thread::id& threadId, int objectId)
{
	wxCommandEvent evt(EVT_THREAD, command);

	evt.SetId(std::hash<std::thread::id>()(threadId));// TODO:  Not guaranteed unique?
	evt.SetInt(static_cast<int>(command));
	evt.SetExtraLong(objectId);

	parent->AddPendingEvent(evt);
}

}// namespace VVASE
