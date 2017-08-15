/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  binaryReader.h
// Date:  6/8/2016
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Class for assisting reading of binary files.  This object is
//        intended to provide consistency for file I/O between 32 and 64 bit
//        platforms.

#ifndef BINARY_READER_H_
#define BINARY_READER_H_

// Eigen headers
#include <Eigen/Eigen>

// Standard C++ headers
#include <string>
#include <fstream>
#include <vector>

// Local headers
#include "VVASE/core/utilities/wheelSetStructures.h"
#include "VVASE/core/utilities/vvaseString.h"

namespace VVASE
{

class BinaryReader
{
public:
	BinaryReader(vvaseInFileStream& file);

	bool Read(vvaseString& v);
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

	template <typename _Scalar, int _Rows, int _Cols>
	bool Read(Eigen::Matrix<_Scalar, _Rows, _Cols>& v)
	{
		bool ok(true);
		int rows, cols;
		ok = Read(rows) && ok;
		ok = Read(cols) && ok;
		assert(rows == _Rows || _Rows == Eigen::Dynamic);
		assert(cols == _Cols || _Cols == Eigen::Dynamic);

		if (rows == Eigen::Dynamic || cols == Eigen::Dynamic)
			v.resize(rows, cols);

		int i;
		for (i = 0; i < _Rows * _Cols; ++i)
			ok = Read(v.data()[i]) && ok;
		return ok;
	}

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
	vvaseInFileStream& file;

	bool Read8Bit(char* const v);
	bool Read16Bit(char* const v);
	bool Read32Bit(char* const v);
	bool Read64Bit(char* const v);
};

}// namespace VVASE

#endif// BINARY_READER_H_
