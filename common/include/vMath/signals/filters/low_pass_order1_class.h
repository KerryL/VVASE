/*===================================================================================
                                    CarDesigner
                           Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  low_pass_order1_class.h
// Created:  5/16/2011
// Author:  K. Loux
// Description:  First order low-pass digital filter.
// History:

#ifndef _LOW_PASS_ORDER1_CLASS_H_
#define _LOW_PASS_ORDER1_CLASS_H_

// Local headers
#include "vMath/signals/filters/filter_base_class.h"

class LowPassFirstOrderFilter : public FilterBase
{
public:
	// Constructor
	LowPassFirstOrderFilter(const double& cutoffFrequency,
		const double& sampleRate, const double& initialValue = 0.0);

	// Mandatory overloads from FilterBase
	// Resets all internal variables to initialize the filter to the specified value
	virtual void Initialize(const double &initialValue);

	// Main method for filtering incoming data
	virtual double Apply(const double &_u);
};

#endif// _LOW_PASS_ORDER1_CLASS_H_