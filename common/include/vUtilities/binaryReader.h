/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  binaryReader.h
// Created:   6/8/2016
// Author:  K. Loux
// Description:  Class for assisting reading of binary files.  This object is
//               intended to provide consistency for file I/O between 32 and 64 bit
//               platforms.

#ifndef BINARY_READER_H_
#define BINARY_READER_H_

// Standard C++ headers
#include <string>
#include <fstream>
#include <vector>

// Local headers
#include "vUtilities/wheelSetStructures.h"

// Local forward delcarations
class Vector;

class BinaryReader
{
public:
	BinaryReader(std::ifstream& file);

	bool Read(std::string& v);
	bool Read(short& v);
	bool Read(int& v);
	bool Read(long& v);
	bool Read(long long& v);
	bool Read(unsigned short& v);
	bool Read(unsigned int& v);
	bool Read(unsigned long& v);
	bool Read(unsigned long long& v);
	bool Read(float& v);
	bool Read(double& v);
	bool Read(long double& v);
	bool Read(bool& v);

	bool Read(const Vector& v);

	template<typename T>
	bool Read(const CornerSet<T>& v);

	template<typename T>
	bool Read(const EndSet<T>& v);

	template<typename T>
	bool Read(std::vector<T>& v);

private:
	bool Read8Bit(char& v);
	bool Read16Bit(char& v);
	bool Read32Bit(char& v);
	bool Read64Bit(char& v);
};

#endif// BINARY_READER_H_