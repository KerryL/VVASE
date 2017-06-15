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
	bool Read(char& v);
	bool Read(short& v);
	bool Read(int& v);
	bool Read(long& v);
	bool Read(long long& v);
	bool Read(unsigned char& v);
	bool Read(unsigned short& v);
	bool Read(unsigned int& v);
	bool Read(unsigned long& v);
	bool Read(unsigned long long& v);
	bool Read(float& v);
	bool Read(double& v);
	bool Read(bool& v);

	bool Read(Vector& v);

	template<typename T>
	bool Read(CornerSet<T>& v)
	{
		bool ok(true);
		ok = Read(v.leftFront) && ok;
		ok = Read(v.rightFront) && ok;
		ok = Read(v.leftRear) && ok;
		ok = Read(v.rightRear) && ok;

		return ok;
	}

	template<typename T>
	bool Read(EndSet<T>& v)
	{
		bool ok(true);
		ok = Read(v.front) && ok;
		ok = Read(v.rear) && ok;

		return ok;
	}

	template<typename T>
	bool Read(std::vector<T>& v)
	{
		bool ok(true);
		unsigned int size;
		ok = Read(size) && ok;
		v.resize(size);

		unsigned int i;
		for (i = 0; i < v.size(); i++)
			ok = Read(v[i]) && ok;

		return ok;
	}

private:
	std::ifstream& file;

	bool Read8Bit(char* const v);
	bool Read16Bit(char* const v);
	bool Read32Bit(char* const v);
	bool Read64Bit(char* const v);
};

#endif// BINARY_READER_H_