/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  undoRedoStack.cpp
// Date:  10/18/2010
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Class implementing std::stack to store information about recent operations
//        to provide the opportunity to undo and redo.

// Local headers
#include "gui/undoRedoStack.h"
#include "gui/components/mainFrame.h"
#include "gui/components/editPanel/editPanel.h"
#include "gui/guiCar.h"
#include "vCar/car.h"
#include "vCar/suspension.h"

namespace VVASE
{

//==========================================================================
// Class:			UndoRedoStack
// Function:		UndoRedoStack
//
// Description:		Constructor for the UndoRedoStack class.
//
// Input Arguments:
//		mainFrame	= &MainFrame, reference to the main frame object
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
UndoRedoStack::UndoRedoStack(MainFrame &mainFrame) : mainFrame(mainFrame)
{
}

//==========================================================================
// Class:			UndoRedoStack
// Function:		~UndoRedoStack
//
// Description:		Destructor for the UndoRedoStack class.
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
UndoRedoStack::~UndoRedoStack()
{
}

//==========================================================================
// Class:			UndoRedoStack
// Function:		AddOperation
//
// Description:		Adds an operation to the undo stack.  Operations must be
//					added before the change is committed.  In other words,
//					the only correct calling sequence is:
//						1.  Do all verification that operation is desired/correct
//						2.  Call UndoRedoStack::AddOperation()
//						3.  Perform the operation
//
// Input Arguments:
//		index		= int specifying which GuiObject the operation is
//					  associated with
//		dataType	= Operation::OperationDataType describing what kind of
//					  data to store
//		location	= *void, pointing to the location of the unchanged (but
//					  to soon be changed) data
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void UndoRedoStack::AddOperation(int index,
								   UndoRedoStack::Operation::OperationDataType dataType,
								   void *location)
{
	// Clear the redo stack
	while (!redoStack.empty())
		redoStack.pop();

	// Enable the undo menu item and disable the redo menu item
	mainFrame.EnableUndo();
	mainFrame.DisableRedo();

	// Create an Operation object and populate it
	UndoRedoStack::Operation lastOperation;
	lastOperation.dataType = dataType;
	lastOperation.guiObjectIndex = index;
	lastOperation.dataLocation = location;
	lastOperation = UpdateValue(lastOperation);

	// Add the operation to the undo stack
	undoStack.push(lastOperation);
}

//==========================================================================
// Class:			UndoRedoStack
// Function:		UpdateValue
//
// Description:		Updates the OldValue for the specified operation.  To be
//					called when undoing/redoing and switching the operation
//					between stacks.
//
// Input Arguments:
//		Operation	= Operation to consider
//
// Output Arguments:
//		None
//
// Return Value:
//		Operation after change has been applied
//
//==========================================================================
UndoRedoStack::Operation UndoRedoStack::UpdateValue(Operation operation)
{
	switch (operation.dataType)
	{
	case Operation::DataTypeBool:
		operation.oldValue.boolean = *static_cast<bool*>(operation.dataLocation);
		break;

	case Operation::DataTypeShort:
		operation.oldValue.shortInteger = *static_cast<short*>(operation.dataLocation);
		break;

	case Operation::DataTypeInteger:
		operation.oldValue.integer = *static_cast<int*>(operation.dataLocation);
		break;

	case Operation::DataTypeLong:
		operation.oldValue.longInteger = *static_cast<long*>(operation.dataLocation);
		break;

	case Operation::DataTypeFloat:
		operation.oldValue.singlePrecision = *static_cast<float*>(operation.dataLocation);
		break;

	case Operation::DataTypeDouble:
		operation.oldValue.doublePrecision = *static_cast<double*>(operation.dataLocation);
		break;

	case Operation::DataTypeVector:
		operation.oldValue.vector[0] = static_cast<double*>(operation.dataLocation)[0];
		operation.oldValue.vector[1] = static_cast<double*>(operation.dataLocation)[1];
		operation.oldValue.vector[2] = static_cast<double*>(operation.dataLocation)[2];
		break;

	/*case Operation::DATA_TYPE_GA_GENE_ADD:
	case Operation::DATA_TYPE_GA_GENE_MODIFY:
	case Operation::DATA_TYPE_GA_GENE_DELETE:
		operation.oldValue.GeneData = *static_cast<Operation::OperationData::GeneData*>(Operation.dataLocation);
		break;

	case Operation::DATA_TYPE_GA_GOAL_ADD:
	case Operation::DATA_TYPE_GA_GOAL_MODIFY:
	case Operation::DATA_TYPE_GA_GOAL_DELETE:
		operation.oldValue.GoalData = *static_cast<Operation::OperationData::GeneData*>(Operation.dataLocation);
		break;*/

	default:
		assert(false);// Fail on unknown data types
	}

	return operation;
}

//==========================================================================
// Class:			UndoRedoStack
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
void UndoRedoStack::Undo()
{
	// Make sure the undo stack isn't empty
	if (undoStack.empty())
		return;

	// Enable the redo menu item
	mainFrame.EnableRedo();

	// Update the value stored in the Operation and add it to the redo stack
	redoStack.push(UpdateValue(undoStack.top()));

	// Undo the last operation
	ApplyOperation(undoStack.top());

	// Remove the operation from the undo stack
	undoStack.pop();

	// If there are no more operations in the undo stack, disable the undo menu item
	if (undoStack.empty())
		mainFrame.DisableUndo();
}

//==========================================================================
// Class:			UndoRedoStack
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
void UndoRedoStack::Redo()
{
	// Make sure the redo stack isn't empty
	if (redoStack.empty())
		return;

	// Enable the undo menu item
	mainFrame.EnableUndo();

	// Update the value stored in the Operation and add it to the undo stack
	undoStack.push(UpdateValue(redoStack.top()));

	// Redo the last undone operation
	ApplyOperation(redoStack.top());

	// Remove the operation from the redo stack
	redoStack.pop();

	// If there are no more operations to redo, disable the redo menu item
	if (redoStack.empty())
		mainFrame.DisableRedo();
}

//==========================================================================
// Class:			UndoRedoStack
// Function:		ApplyOperation
//
// Description:		Applies the specified operation.
//
// Input Arguments:
//		operation = &Operation to apply
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void UndoRedoStack::ApplyOperation(Operation &operation)
{
	// Apply the value
	switch (operation.dataType)
	{
	case Operation::DataTypeBool:
		*static_cast<bool*>(operation.dataLocation) = operation.oldValue.boolean;
		break;

	case Operation::DataTypeShort:
		*static_cast<short*>(operation.dataLocation) = operation.oldValue.shortInteger;
		break;

	case Operation::DataTypeInteger:
		*static_cast<int*>(operation.dataLocation) = operation.oldValue.integer;
		break;

	case Operation::DataTypeLong:
		*static_cast<long*>(operation.dataLocation) = operation.oldValue.longInteger;
		break;

	case Operation::DataTypeFloat:
		*static_cast<float*>(operation.dataLocation) = operation.oldValue.singlePrecision;
		break;

	case Operation::DataTypeDouble:
		*static_cast<double*>(operation.dataLocation) = operation.oldValue.doublePrecision;
		break;

	case Operation::DataTypeVector:
		static_cast<double*>(operation.dataLocation)[0] = operation.oldValue.vector[0];
		static_cast<double*>(operation.dataLocation)[1] = operation.oldValue.vector[1];
		static_cast<double*>(operation.dataLocation)[2] = operation.oldValue.vector[2];
		break;

	case Operation::DataTypeGAGeneAdd:
		break;

	case Operation::DataTypeGAGeneModify:
		break;

	case Operation::DataTypeGAGeneDelete:
		break;

	case Operation::DataTypeGAGoalAdd:
		break;

	case Operation::DataTypeGAGoalModify:
		break;

	case Operation::DataTypeGAGoalDelete:
		break;

	default:
		assert(false);
	}

	// Update the GUI screens
	Update();
}

//==========================================================================
// Class:			UndoRedoStack
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
void UndoRedoStack::Update() const
{
	// In case of a symmetric suspension (where we only store the change to one side here)
	int i;
	for (i = 0; i < mainFrame.GetObjectCount(); i++)
	{
		if (mainFrame.GetObjectByIndex(i)->GetType() == GuiObject::TypeCar)
			dynamic_cast<GuiCar*>(mainFrame.GetObjectByIndex(i))->GetOriginalCar().suspension->UpdateSymmetry();
	}

	// Update all areas of the GUI screen
	mainFrame.UpdateAnalysis();
	mainFrame.UpdateOutputPanel();
	mainFrame.GetEditPanel()->UpdateInformation();
}

//==========================================================================
// Class:			UndoRedoStack
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
void UndoRedoStack::ClearStacks()
{
	// Clear the undo stack
	while (!undoStack.empty())
		undoStack.pop();

	// Clear the redo stack
	while (!redoStack.empty())
		redoStack.pop();
}

//==========================================================================
// Class:			UndoRedoStack
// Function:		RemoveGuiObjectFromStack
//
// Description:		Removes operations associated with the specified GUI_OBJECT
//					from the stack.  To be called when objects are closed or saved.
//
// Input Arguments:
//		index	= int specifying which GUI_OJBECT to eliminate
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void UndoRedoStack::RemoveGuiObjectFromStack(int index)
{
	// Create temporary stacks
	std::stack<UndoRedoStack::Operation> tempUndo;
	std::stack<UndoRedoStack::Operation> tempRedo;

	// Pop from the class stacks and push into temporary stacks until the
	// class stacks are empty
	// Only push the operations in the temporary stack if they are not
	// associated with the specified Index
	while (!undoStack.empty())
	{
		if (undoStack.top().guiObjectIndex != index)
			tempUndo.push(undoStack.top());
		undoStack.pop();
	}

	while (!redoStack.empty())
	{
		if (redoStack.top().guiObjectIndex != index)
			tempRedo.push(redoStack.top());
		redoStack.pop();
	}

	// Pop rom the temporary stacks as we push back into the class stacks
	while (!tempUndo.empty())
	{
		undoStack.push(tempUndo.top());
		tempUndo.pop();
	}

	while (!tempRedo.empty())
	{
		redoStack.push(tempRedo.top());
		tempRedo.pop();
	}

	// Check to see if either stack is empty, and enable/disable the associated
	// menu items as necessary
	if (undoStack.empty())
		mainFrame.DisableUndo();
	if (redoStack.empty())
		mainFrame.DisableRedo();
}

//==========================================================================
// Class:			UndoRedoStack
// Function:		operator=
//
// Description:		Assignment operator for UndoRedoStack.
//
// Input Arguments:
//		undoRedo	= const UndoRedoStack& to assign to this
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
UndoRedoStack& UndoRedoStack::operator = (const UndoRedoStack &undoRedo)
{
	// Check for self assignment
	if (this == &undoRedo)
		return *this;

	// Clear out existing data in this object
	ClearStacks();

	// Assign class members to argument
	//mainFrame = undoRedo.mainFrame;// Since there is only one MainFrame, we can assume it's the same
	undoStack = undoRedo.undoStack;
	redoStack = undoRedo.redoStack;

	return *this;
}

}// namespace VVASE
