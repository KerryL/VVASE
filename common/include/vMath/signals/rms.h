/*===================================================================================
                                    CarDesigner
                           Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  rms.h
// Created:  5/3/2011
// Author:  K. Loux
// Description:  Computes root-mean-square of data.
// History:

#ifndef RMS_H_
#define RMS_H_

// Local forward declarations
class Dataset2D;

class RootMeanSquare
{
public:
	static Dataset2D ComputeTimeHistory(const Dataset2D &data);
};

#endif// RMS_H_