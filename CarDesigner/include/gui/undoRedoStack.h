/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  undoRedoStack.h
// Created:  10/18/2010
// Author:  K. Loux
// Description:  Class implementing std::stack to store information about recent operations
//				 to provide the opportunity to undo and redo.
// History:

#ifndef UNDO_REDO_STACK_H_
#define UNDO_REDO_STACK_H_

// Standard C++ headers
#include <stack>

// VVASE headers
#include "gui/gaObject.h"

// VVASE forward declarations
class MainFrame;

class UndoRedoStack
{
public:
	UndoRedoStack(MainFrame &mainFrame);
	~UndoRedoStack();

	// Data object for storing undo/redo information
	// Must be defined before some public functions are defined
	struct Operation
	{
		// Data type
		enum OperationDataType
		{
			DataTypeBool,
			DataTypeShort,
			DataTypeInteger,
			DataTypeLong,
			DataTypeFloat,
			DataTypeDouble,
			DataTypeGAGeneAdd,
			DataTypeGAGeneModify,
			DataTypeGAGeneDelete,
			DataTypeGAGoalAdd,
			DataTypeGAGoalModify,
			DataTypeGAGoalDelete
		} dataType;

		// Pointer to changed data
		void *dataLocation;

		// Old data value
		union OperationData
		{
			bool boolean;
			short shortInteger;
			int integer;
			long longInteger;
			float singlePrecision;
			double doublePrecision;

			struct GeneData
			{
				GAObject *optimization;
				GAObject::Gene gene;
			} geneData;

			// FIXME:  Can't do goals because Kinematics::Inputs containts a Vector, which has a user-defined constructor
			/*struct GoalData
			{
				GAObject *optimization;
				GAObject::Goal goal;
			} GoalData;*/

		} oldValue;

		// Associated GUI_OBJECT
		int guiObjectIndex;
	};

	// Methods for maintaining the stacks
	void AddOperation(int index,
		UndoRedoStack::Operation::OperationDataType dataType, void *location);
	void Undo();
	void Redo();
	void ClearStacks();
	void RemoveGuiObjectFromStack(int index);

	// Operator overloads (explicitly overloaded due to warning C4512
	// caused by reference to MAIN_FRAME)
	UndoRedoStack& operator = (const UndoRedoStack &undoRedo);

private:
	MainFrame &mainFrame;

	// Method for applying changes (either undo or redo)
	void ApplyOperation(Operation &operation);

	// Method for getting data from location associated with the OPERATION
	Operation UpdateValue(Operation operation);

	// Method for updating the application as a result of a undo/redo
	void Update() const;

	// The stacks
	std::stack<Operation> undoStack;
	std::stack<Operation> redoStack;
};

#endif// UNDO_REDO_STACK_H_