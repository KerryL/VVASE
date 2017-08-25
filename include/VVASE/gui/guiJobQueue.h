/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  guiJobQueue.h
// Date:  8/25/2017
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  JobQueue wrapper when communication with GUI thread is required.

// Local headers
#include "VVASE/core/threads/jobQueue.h"

// wxWidgets forward declarations
class wxEvtHandler;

namespace VVASE
{

class GuiJobQueue : public JobQueue
{
public:
	GuiJobQueue(wxEvtHandler *parent);

	// Reports a message back to the main event handler
	void Report(const ThreadJob::ThreadCommand& command, const std::thread::id& threadId, int objectId) override;

	wxEvtHandler *GetParent() { return parent; }

private:
	wxEvtHandler *parent;
};

}// namespace VVASE
