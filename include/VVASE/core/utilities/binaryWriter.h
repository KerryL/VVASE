/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  binaryWriter.h
// Date:  6/8/2016
// Auth:  K. Loux
// Licn:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Class for assisting writing of binary files.  This object is
//        intended to provide consistency for file I/O between 32 and 64 bit
//        platforms.

#ifndef BINARY_WRITER_H_
#define BINARY_WRITER_H_

// Standard C++ headers
#include <string>
#include <fstream>
#include <vector>

// Local headers
#include "vUtilities/wheelSetStructures.h"

namespace VVASE
{

class BinaryWriter
{
public:
	BinaryWriter(std::ofstream& file);

	bool Write(const std::string& v);
	bool Write(const char& v);
	bool Write(const short& v);
	bool Write(const int& v);
	bool Write(const long& v);
	bool Write(const long long& v);
	bool Write(const unsigned char& v);
	bool Write(const unsigned short& v);
	bool Write(const unsigned int& v);
	bool Write(const unsigned long& v);
	bool Write(const unsigned long long& v);
	bool Write(const float& v);
	bool Write(const double& v);
	bool Write(const bool& v);

	bool Write(const Vector& v);

	template<typename T>
	bool Write(const CornerSet<T>& v)
	{
		bool ok(true);
		ok = Write(v.leftFront) && ok;
		ok = Write(v.rightFront) && ok;
		ok = Write(v.leftRear) && ok;
		ok = Write(v.rightRear) && ok;

		return ok;
	}

	template<typename T>
	bool Write(const EndSet<T>& v)
	{
		bool ok(true);
		ok = Write(v.front) && ok;
		ok = Write(v.rear) && ok;

		return ok;
	}

	template<typename T>
	bool Write(const std::vector<T>& v)
	{
		bool ok(true);
		ok = Write((unsigned int)v.size()) && ok;

		unsigned int i;
		for (i = 0; i < v.size(); i++)
			ok = Write(v[i]) && ok;

		return ok;
	}

private:
	std::ofstream& file;

	bool Write8Bit(const char* const v);
	bool Write16Bit(const char* const v);
	bool Write32Bit(const char* const v);
	bool Write64Bit(const char* const v);
};

}// namespace VVASE

#endif// BINARY_WRITER_H_
