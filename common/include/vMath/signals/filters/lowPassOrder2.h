/*===================================================================================
                                    CarDesigner
                           Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  lowPassOrder2.h
// Created:  5/16/2011
// Author:  K. Loux
// Description:  Second order low-pass digital filter.
// History:

#ifndef LOW_PASS_ORDER2_H_
#define LOW_PASS_ORDER2_H_

// Local headers
#include "vMath/signals/filters/filterBase.h"

class LowPassSecondOrderFilter : public FilterBase
{
public:
	// Constructors
	LowPassSecondOrderFilter(const double& cutoffFrequency, const double& dampingRatio,
		const double& sampleRate, const double& initialValue = 0.0);
	LowPassSecondOrderFilter(const LowPassSecondOrderFilter &f);

	// Mandatory overloads from FilterBase
	// Resets all internal variables to initialize the filter to the specified value
	virtual void Initialize(const double &initialValue);

	// Main method for filtering incoming data
	virtual double Apply(const double &in);

	// Operators
	LowPassSecondOrderFilter& operator = (const LowPassSecondOrderFilter &f);
};

#endif// LOW_PASS_ORDER2_H_