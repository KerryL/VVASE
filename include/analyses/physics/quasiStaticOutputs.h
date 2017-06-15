/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  quasiStaticOutputs.h
// Created:  6/2/2016
// Author:  K. Loux
// Description:  Quasi-static analysis outputs.
// History:

#ifndef QUASI_STATIC_OUTPUTS_H_
#define QUASI_STATIC_OUTPUTS_H_

// Local headers
#include "vUtilities/wheelSetStructures.h"

class QuasiStaticOutputs
{
public:
	WheelSet wheelLoads;// [lbf]
	double pitch;// [rad]
	double roll;// [rad]
	double heave;// [in]
};

#endif// QUASI_STATIC_OUTPUTS_H_
