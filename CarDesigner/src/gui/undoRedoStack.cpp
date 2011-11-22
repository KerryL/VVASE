/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  undoRedoStack.cpp
// Created:  10/18/2010
// Author:  K. Loux
// Description:  Class implementing std::stack to store information about recent operations
//				 to provide the opportunity to undo and redo.
// History:

// VVASE headers
#include "gui/undoRedoStack.h"
#include "gui/components/mainFrame.h"
#include "gui/components/editPanel/editPanel.h"

//==========================================================================
// Class:			UNDO_REDO_STACK
// Function:		UNDO_REDO_STACK
//
// Description:		Constructor for the UNDO_REDO_STACK class.
//
// Input Arguments:
//		_MainFrame	= &MAIN_FRAME, reference to the main frame object
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
UNDO_REDO_STACK::UNDO_REDO_STACK(MAIN_FRAME &_MainFrame) : MainFrame(_MainFrame)
{
}

//==========================================================================
// Class:			UNDO_REDO_STACK
// Function:		~UNDO_REDO_STACK
//
// Description:		Destructor for the UNDO_REDO_STACK class.
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
UNDO_REDO_STACK::~UNDO_REDO_STACK()
{
}

//==========================================================================
// Class:			UNDO_REDO_STACK
// Function:		AddOperation
//
// Description:		Adds an operation to the undo stack.  Operations must be
//					added before the change is committed.  In other words,
//					the only correct calling sequence is:
//						1.  Do all verification that operation is desired/correct
//						2.  Call UNDO_REDO_STACK::AddOperation()
//						3.  Perform the operation
//
// Input Arguments:
//		Index		= int specifying which GUI_OBJECT the operation is
//					  associated with
//		DataType	= OPERATION::OPERATION_DATA_TYPE describing what kind of
//					  data to store
//		Location	= *void, pointing to the location of the unchanged (but
//					  to soon be changed) data
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void UNDO_REDO_STACK::AddOperation(int Index,
								   UNDO_REDO_STACK::OPERATION::OPERATION_DATA_TYPE DataType,
								   void *Location)
{
	// Clear the redo stack
	while (!RedoStack.empty())
		RedoStack.pop();

	// Enable the undo menu item and disable the redo menu item
	MainFrame.EnableUndo();
	MainFrame.DisableRedo();

	// Create an OPERATION object and populate it
	UNDO_REDO_STACK::OPERATION LastOperation;
	LastOperation.DataType = DataType;
	LastOperation.GuiObjectIndex = Index;
	LastOperation.DataLocation = Location;
	LastOperation = UpdateValue(LastOperation);

	// Add the operation to the undo stack
	UndoStack.push(LastOperation);
}

//==========================================================================
// Class:			UNDO_REDO_STACK
// Function:		UpdateValue
//
// Description:		Updates the OldValue for the specified operation.  To be
//					called when undoing/redoing and switching the operation
//					between stacks.
//
// Input Arguments:
//		Operation	= OPERATION to consider
//
// Output Arguments:
//		None
//
// Return Value:
//		OPERATION after change has been applied
//
//==========================================================================
UNDO_REDO_STACK::OPERATION UNDO_REDO_STACK::UpdateValue(OPERATION Operation)
{
	switch (Operation.DataType)
	{
	case OPERATION::DATA_TYPE_BOOL:
		Operation.OldValue.Boolean = *(bool*)Operation.DataLocation;
		break;

	case OPERATION::DATA_TYPE_SHORT:
		Operation.OldValue.ShortInteger = *(short*)Operation.DataLocation;
		break;

	case OPERATION::DATA_TYPE_INTEGER:
		Operation.OldValue.Integer = *(int*)Operation.DataLocation;
		break;

	case OPERATION::DATA_TYPE_LONG:
		Operation.OldValue.LongInteger = *(long*)Operation.DataLocation;
		break;

	case OPERATION::DATA_TYPE_FLOAT:
		Operation.OldValue.Float = *(float*)Operation.DataLocation;
		break;

	case OPERATION::DATA_TYPE_DOUBLE:
		Operation.OldValue.Double = *(double*)Operation.DataLocation;
		break;

	/*case OPERATION::DATA_TYPE_GA_GENE_ADD:
	case OPERATION::DATA_TYPE_GA_GENE_MODIFY:
	case OPERATION::DATA_TYPE_GA_GENE_DELETE:
		Operation.OldValue.GeneData = *(OPERATION::OPERATION_DATA::GENE_DATA*)Operation.DataLocation;
		break;

	case OPERATION::DATA_TYPE_GA_GOAL_ADD:
	case OPERATION::DATA_TYPE_GA_GOAL_MODIFY:
	case OPERATION::DATA_TYPE_GA_GOAL_DELETE:
		Operation.OldValue.GoalData = *(OPERATION::OPERATION_DATA::GOAL_DATA*)Operation.DataLocation;
		break;*/

	default:
		assert(0);// Fail on unknown data types
	}

	return Operation;
}

//==========================================================================
// Class:			UNDO_REDO_STACK
// Function:		Undo
//
// Description:		Undoes the last logged operation.
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
void UNDO_REDO_STACK::Undo(void)
{
	// Make sure the undo stack isn't empty
	if (UndoStack.empty())
		return;

	// Enable the redo menu item
	MainFrame.EnableRedo();

	// Update the value stored in the OPERATION and add it to the redo stack
	RedoStack.push(UpdateValue(UndoStack.top()));

	// Undo the last operation
	ApplyOperation(UndoStack.top());

	// Remove the operation from the undo stack
	UndoStack.pop();

	// If there are no more operations in the undo stack, disable the undo menu item
	if (UndoStack.empty())
		MainFrame.DisableUndo();
}

//==========================================================================
// Class:			UNDO_REDO_STACK
// Function:		Redo
//
// Description:		Re-performs the most recently undone operation.
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
void UNDO_REDO_STACK::Redo(void)
{
	// Make sure the redo stack isn't empty
	if (RedoStack.empty())
		return;

	// Enable the undo menu item
	MainFrame.EnableUndo();

	// Update the value stored in the OPERATION and add it to the undo stack
	UndoStack.push(UpdateValue(RedoStack.top()));

	// Redo the last undone operation
	ApplyOperation(RedoStack.top());

	// Remove the operation from the redo stack
	RedoStack.pop();

	// If there are no more operations to redo, disable the redo menu item
	if (RedoStack.empty())
		MainFrame.DisableRedo();
}

//==========================================================================
// Class:			UNDO_REDO_STACK
// Function:		ApplyOperation
//
// Description:		Applies the specified operation.
//
// Input Arguments:
//		Operation = &OPERATION to apply
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void UNDO_REDO_STACK::ApplyOperation(OPERATION &Operation)
{
	// Apply the value
	switch (Operation.DataType)
	{
	case OPERATION::DATA_TYPE_BOOL:
		*(bool*)Operation.DataLocation = Operation.OldValue.Boolean;
		break;

	case OPERATION::DATA_TYPE_SHORT:
		*(short*)Operation.DataLocation = Operation.OldValue.ShortInteger;
		break;

	case OPERATION::DATA_TYPE_INTEGER:
		*(int*)Operation.DataLocation = Operation.OldValue.Integer;
		break;

	case OPERATION::DATA_TYPE_LONG:
		*(long*)Operation.DataLocation = Operation.OldValue.LongInteger;
		break;

	case OPERATION::DATA_TYPE_FLOAT:
		*(float*)Operation.DataLocation = Operation.OldValue.Float;
		break;

	case OPERATION::DATA_TYPE_DOUBLE:
		*(double*)Operation.DataLocation = Operation.OldValue.Double;
		break;

	case OPERATION::DATA_TYPE_GA_GENE_ADD:
		break;

	case OPERATION::DATA_TYPE_GA_GENE_MODIFY:
		break;

	case OPERATION::DATA_TYPE_GA_GENE_DELETE:
		break;

	case OPERATION::DATA_TYPE_GA_GOAL_ADD:
		break;

	case OPERATION::DATA_TYPE_GA_GOAL_MODIFY:
		break;

	case OPERATION::DATA_TYPE_GA_GOAL_DELETE:
		break;

	default:
		assert(0);
	}

	// Update the GUI screens
	Update();
}

//==========================================================================
// Class:			UNDO_REDO_STACK
// Function:		Update
//
// Description:		Updates the screen following an undo/redo.
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
void UNDO_REDO_STACK::Update(void) const
{
	// Update all areas of the GUI screen
	MainFrame.UpdateAnalysis();
	MainFrame.UpdateOutputPanel();
	MainFrame.GetEditPanel()->UpdateInformation();
}

//==========================================================================
// Class:			UNDO_REDO_STACK
// Function:		ClearStacks
//
// Description:		Clears the undo/redo stacks.  To be called after an operation
//					that is not supported by undo/redo.
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
void UNDO_REDO_STACK::ClearStacks(void)
{
	// Clear the undo stack
	while (!UndoStack.empty())
		UndoStack.pop();

	// Clear the redo stack
	while (!RedoStack.empty())
		RedoStack.pop();
}

//==========================================================================
// Class:			UNDO_REDO_STACK
// Function:		RemoveGuiObjectFromStack
//
// Description:		Removes operations associated with the specified GUI_OBJECT
//					from the stack.  To be called when objects are closed or saved.
//
// Input Arguments:
//		Index	= int specifying which GUI_OJBECT to eliminate
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void UNDO_REDO_STACK::RemoveGuiObjectFromStack(int Index)
{
	// Create temporary stacks
	std::stack<UNDO_REDO_STACK::OPERATION> TempUndo;
	std::stack<UNDO_REDO_STACK::OPERATION> TempRedo;

	// Pop from the class stacks and push into temporary stacks until the
	// class stacks are empty
	// Only push the operations in the temporary stack if they are not
	// associated with the specified Index
	while (!UndoStack.empty())
	{
		if (UndoStack.top().GuiObjectIndex != Index)
			TempUndo.push(UndoStack.top());
		UndoStack.pop();
	}

	while (!RedoStack.empty())
	{
		if (RedoStack.top().GuiObjectIndex != Index)
			TempRedo.push(RedoStack.top());
		RedoStack.pop();
	}

	// Pop rom the temporary stacks as we push back into the class stacks
	while (!TempUndo.empty())
	{
		UndoStack.push(TempUndo.top());
		TempUndo.pop();
	}

	while (!TempRedo.empty())
	{
		RedoStack.push(TempRedo.top());
		TempRedo.pop();
	}

	// Check to see if either stack is empty, and enable/disable the associated
	// menu items as necessary
	if (UndoStack.empty())
		MainFrame.DisableUndo();
	if (RedoStack.empty())
		MainFrame.DisableRedo();
}

//==========================================================================
// Class:			UNDO_REDO_STACK
// Function:		operator=
//
// Description:		Removes operations associated with the specified GUI_OBJECT
//					from the stack.  To be called when objects are closed or saved.
//
// Input Arguments:
//		Index	= int specifying which GUI_OJBECT to eliminate
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
UNDO_REDO_STACK& UNDO_REDO_STACK::operator = (const UNDO_REDO_STACK &UndoRedo)
{
	// Check for self assignment
	if (this == &UndoRedo)
		return *this;

	// Clear out existing data in this object
	ClearStacks();

	// Assign class members to argument
	//MainFrame = UndoRedo.MainFrame;// Since there is only one MainFrame, we can assume it's the same
	UndoStack = UndoRedo.UndoStack;
	RedoStack = UndoRedo.RedoStack;

	return *this;
}