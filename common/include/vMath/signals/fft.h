/*===================================================================================
                                    CarDesigner
                           Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  fft.h
// Created:  5/3/2011
// Author:  K. Loux
// Description:  Performs Fast Fourier Transform on data.
// History:

#ifndef FFT_H_
#define FFT_H_

// Local forward declarations
class Dataset2D;

class FastFourierTransform
{
public:
	static Dataset2D Compute(const Dataset2D &data);
};

#endif// FFT_H_