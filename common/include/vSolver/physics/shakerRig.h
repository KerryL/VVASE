/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  shakerRig.h
// Created:  3/23/2008
// Author:  K. Loux
// Description:  Contains class declaration for SHAKER_RIG class.  This is a type of dynamic
//				 analysis that uses a Fourier transform to create Bode plots of the vehicle's
//				 response to different inputs.
// History:

#ifndef SHAKER_RIG_H_
#define SHAKER_RIG_H_

// CarDesigner forward declarations
class Debugger;

class ShakerRig
{
public:
	ShakerRig(const Debugger &debugger);
	ShakerRig(const ShakerRig &shakerRig);
	~ShakerRig();

	// Overloaded operators
	ShakerRig& operator=(const ShakerRig &shakerRig);

private:
	const Debugger &debugger;
};

#endif// SHAKER_RIG_H_