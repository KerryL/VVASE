/*===================================================================================
                                    CarDesigner
                           Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  integral.h
// Created:  5/3/2011
// Author:  K. Loux
// Description:  Computes discrete-time integral of data.
// History:

#ifndef INTEGRAL_H_
#define INTEGRAL_H_

// Local forward declarations
class Dataset2D;

class DiscreteIntegral
{
public:
	static Dataset2D ComputeTimeHistory(const Dataset2D &data);
};

#endif// INTEGRAL_H_