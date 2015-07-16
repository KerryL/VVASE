/*===================================================================================
                                    CarDesigner
                           Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  filterBase.h
// Created:  5/16/2011
// Author:  K. Loux
// Description:  Base class (abstract) for digital filters.
// History:

#ifndef FILTER_BASE_H_
#define FILTER_BASE_H_

class FilterBase
{
public:
	// NOTE:  Constructor MUST initialize the filter parameters a and b, and the input/output vectors y and u
	FilterBase(const double &sampleRate);
	FilterBase(const FilterBase &f);
	virtual ~FilterBase();

	virtual void Initialize(const double &initialValue) = 0;

	virtual double Apply(const double &in) = 0;

	double GetRawValue() const { return u[0]; };

	double GetFilteredValue() const { return y[0]; };

	// Assignment operator (avoids MSVC Warning C4512)
	FilterBase& operator = (const FilterBase &f);

protected:
	double *a;
	double *b;
	double *y;
	double *u;

	const double sampleRate;// [Hz]
};

#endif// FILTER_BASE_H_