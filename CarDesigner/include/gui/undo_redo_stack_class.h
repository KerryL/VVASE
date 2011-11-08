/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  undo_redo_stack_class.h
// Created:  10/18/2010
// Author:  K. Loux
// Description:  Class implementing std::stack to store information about recent operations
//				 to provide the opportunity to undo and redo.
// History:

#ifndef _UNDO_REDO_STACK_CLASS_H_
#define _UNDO_REDO_STACK_CLASS_H_

// Standard C++ headers
#include <stack>

// VVASE headers
#include "gui/ga_object_class.h"

// VVASE forward declarations
class MAIN_FRAME;

// The undo/redo stack class
class UNDO_REDO_STACK
{
public:
	// Constructor
	UNDO_REDO_STACK(MAIN_FRAME &_MainFrame);

	// Destructor
	~UNDO_REDO_STACK();

	// Data object for storing undo/redo information
	// Must be defined before some public functions are defined
	struct OPERATION
	{
		// Data type
		enum OPERATION_DATA_TYPE
		{
			DATA_TYPE_BOOL,
			DATA_TYPE_SHORT,
			DATA_TYPE_INTEGER,
			DATA_TYPE_LONG,
			DATA_TYPE_FLOAT,
			DATA_TYPE_DOUBLE,
			DATA_TYPE_GA_GENE_ADD,
			DATA_TYPE_GA_GENE_MODIFY,
			DATA_TYPE_GA_GENE_DELETE,
			DATA_TYPE_GA_GOAL_ADD,
			DATA_TYPE_GA_GOAL_MODIFY,
			DATA_TYPE_GA_GOAL_DELETE
		} DataType;

		// Pointer to changed data
		void *DataLocation;

		// Old data value
		union OPERATION_DATA
		{
			bool Boolean;
			short ShortInteger;
			int Integer;
			long LongInteger;
			float Float;
			double Double;

			struct GENE_DATA
			{
				GA_OBJECT *Optimization;
				GA_OBJECT::GENE Gene;
			} GeneData;

			// FIXME:  Can't do goals because KINEMATICS::INPUTS containts a Vector, which has a user-defined constructor
			/*struct GOAL_DATA
			{
				GA_OBJECT *Optimization;
				GA_OBJECT::GOAL Goal;
			} GoalData;*/

		} OldValue;

		// Associated GUI_OBJECT
		int GuiObjectIndex;
	};

	// Methods for maintaining the stacks
	void AddOperation(int Index,
		UNDO_REDO_STACK::OPERATION::OPERATION_DATA_TYPE DataType, void *Location);
	void Undo(void);
	void Redo(void);
	void ClearStacks(void);
	void RemoveGuiObjectFromStack(int Index);

	// Operator overloads (explicitly overloaded due to warning C4512
	// caused by reference to MAIN_FRAME)
	UNDO_REDO_STACK& operator = (const UNDO_REDO_STACK &UndoRedo);

private:
	// Main frame reference
	MAIN_FRAME &MainFrame;

	// Method for applying changes (either undo or redo)
	void ApplyOperation(OPERATION &Operation);

	// Method for getting data from location associated with the OPERATION
	OPERATION UpdateValue(OPERATION Operation);

	// Method for updating the application as a result of a undo/redo
	void Update(void) const;

	// The stacks
	std::stack<OPERATION> UndoStack;
	std::stack<OPERATION> RedoStack;
};

#endif// _UNDO_REDO_STACK_CLASS_H_