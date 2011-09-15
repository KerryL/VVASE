/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  shaker_rig_class.h
// Created:  3/23/2008
// Author:  K. Loux
// Description:  Contains class declaration for SHAKER_RIG class.  This is a type of dynamic
//				 analysis that uses a Fourier transform to create Bode plots of the vehicle's
//				 response to different inputs.
// History:

#ifndef _SHAKER_RIG_CLASS_H_
#define _SHAKER_RIG_CLASS_H_

// CarDesigner forward declarations
class DEBUGGER;

class SHAKER_RIG
{
public:
	// Constructor
	SHAKER_RIG(const DEBUGGER &_Debugger);
	SHAKER_RIG(const SHAKER_RIG &ShakerRig);

	// Destructor
	~SHAKER_RIG();

	// Overloaded operators
	SHAKER_RIG& operator = (const SHAKER_RIG &ShakerRig);

private:
	// Debugger message printing utility
	const DEBUGGER &Debugger;
};

#endif// _SHAKER_RIG_CLASS_H_