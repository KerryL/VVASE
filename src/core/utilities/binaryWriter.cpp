/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  binaryWriter.cpp
// Date:   6/8/2016
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Class for assisting writing of binary files.  This object is
//        intended to provide consistency for file I/O between 32 and 64 bit
//        platforms.

// Local headers
#include "VVASE/core/utilities/binaryWriter.h"

namespace VVASE
{

//==========================================================================
// Class:			BinaryWriter
// Function:		BinaryWriter
//
// Description:		Constructor for BinaryWriter class.
//
// Input Arguments:
//		file	= std::ofstream&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
BinaryWriter::BinaryWriter(std::ofstream& file) : file(file)
{
}

//==========================================================================
// Class:			BinaryWriter
// Function:		Write
//
// Description:		Writes specified object to file.
//
// Input Arguments:
//		v	= const std::string&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool
//
//==========================================================================
bool BinaryWriter::Write(const std::string& v)
{
	bool ok(true);
	ok = Write((unsigned int)v.length());

	unsigned int i;
	for (i = 0; i < v.length(); i++)
		ok = Write(v[i]) && ok;

	return ok;
}

//==========================================================================
// Class:			BinaryWriter
// Function:		Write
//
// Description:		Writes specified object to file.
//
// Input Arguments:
//		v	= const char&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool
//
//==========================================================================
bool BinaryWriter::Write(const char& v)
{
	return Write8Bit(&v);
}

//==========================================================================
// Class:			BinaryWriter
// Function:		Write
//
// Description:		Writes specified object to file.
//
// Input Arguments:
//		v	= const short&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool
//
//==========================================================================
bool BinaryWriter::Write(const short& v)
{
	return Write16Bit(reinterpret_cast<const char* const>(&v));
}

//==========================================================================
// Class:			BinaryWriter
// Function:		Write
//
// Description:		Writes specified object to file.
//
// Input Arguments:
//		v	= const int&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool
//
//==========================================================================
bool BinaryWriter::Write(const int& v)
{
	return Write32Bit(reinterpret_cast<const char* const>(&v));
}

//==========================================================================
// Class:			BinaryWriter
// Function:		Write
//
// Description:		Writes specified object to file.
//
// Input Arguments:
//		v	= const long&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool
//
//==========================================================================
bool BinaryWriter::Write(const long& v)
{
	return Write32Bit(reinterpret_cast<const char* const>(&v));
}

//==========================================================================
// Class:			BinaryWriter
// Function:		Write
//
// Description:		Writes specified object to file.
//
// Input Arguments:
//		v	= const long lont&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool
//
//==========================================================================
bool BinaryWriter::Write(const long long& v)
{
	return Write64Bit(reinterpret_cast<const char* const>(&v));
}

//==========================================================================
// Class:			BinaryWriter
// Function:		Write
//
// Description:		Writes specified object to file.
//
// Input Arguments:
//		v	= const unsigned char&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool
//
//==========================================================================
bool BinaryWriter::Write(const unsigned char& v)
{
	return Write8Bit(reinterpret_cast<const char* const>(&v));
}

//==========================================================================
// Class:			BinaryWriter
// Function:		Write
//
// Description:		Writes specified object to file.
//
// Input Arguments:
//		v	= const unsigned short&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool
//
//==========================================================================
bool BinaryWriter::Write(const unsigned short& v)
{
	return Write16Bit(reinterpret_cast<const char* const>(&v));
}

//==========================================================================
// Class:			BinaryWriter
// Function:		Write
//
// Description:		Writes specified object to file.
//
// Input Arguments:
//		v	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool
//
//==========================================================================
bool BinaryWriter::Write(const unsigned int& v)
{
	return Write32Bit(reinterpret_cast<const char* const>(&v));
}

//==========================================================================
// Class:			BinaryWriter
// Function:		Write
//
// Description:		Writes specified object to file.
//
// Input Arguments:
//		v	= const unsigned long&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool
//
//==========================================================================
bool BinaryWriter::Write(const unsigned long& v)
{
	return Write32Bit(reinterpret_cast<const char* const>(&v));
}

//==========================================================================
// Class:			BinaryWriter
// Function:		Write
//
// Description:		Writes specified object to file.
//
// Input Arguments:
//		v	= const unsigned long long&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool
//
//==========================================================================
bool BinaryWriter::Write(const unsigned long long& v)
{
	return Write64Bit(reinterpret_cast<const char* const>(&v));
}

//==========================================================================
// Class:			BinaryWriter
// Function:		Write
//
// Description:		Writes specified object to file.
//
// Input Arguments:
//		v	= const float&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool
//
//==========================================================================
bool BinaryWriter::Write(const float& v)
{
	return Write32Bit(reinterpret_cast<const char* const>(&v));
}

//==========================================================================
// Class:			BinaryWriter
// Function:		Write
//
// Description:		Writes specified object to file.
//
// Input Arguments:
//		v	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool
//
//==========================================================================
bool BinaryWriter::Write(const double& v)
{
	return Write64Bit(reinterpret_cast<const char* const>(&v));
}

//==========================================================================
// Class:			BinaryWriter
// Function:		Write
//
// Description:		Writes specified object to file.
//
// Input Arguments:
//		v	= const bool&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool
//
//==========================================================================
bool BinaryWriter::Write(const bool& v)
{
	return Write8Bit(reinterpret_cast<const char* const>(&v));
}

//==========================================================================
// Class:			BinaryWriter
// Function:		Write
//
// Description:		Writes specified object to file.
//
// Input Arguments:
//		v	= const Eigen::VectorXd&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool
//
//==========================================================================
bool BinaryWriter::Write(const Eigen::VectorXd& v)
{
	bool ok(true);

	ok = Write(v.size()) && ok;
	int i;
	for (i = 0; i < v.size(); ++i)
		ok = Write(v[i]) && ok;

	return ok;
}

//==========================================================================
// Class:			BinaryWriter
// Function:		Write8Bit
//
// Description:		Writes 8-bit object to file.
//
// Input Arguments:
//		v	= const char* const
//
// Output Arguments:
//		None
//
// Return Value:
//		bool
//
//==========================================================================
bool BinaryWriter::Write8Bit(const char* const v)
{
	file.write(v, 1);
	return file.good();
}

//==========================================================================
// Class:			BinaryWriter
// Function:		Write16Bit
//
// Description:		Writes 16-bit object to file.
//
// Input Arguments:
//		v	= const char* const
//
// Output Arguments:
//		None
//
// Return Value:
//		bool
//
//==========================================================================
bool BinaryWriter::Write16Bit(const char* const v)
{
	file.write(v, 2);
	return file.good();
}

//==========================================================================
// Class:			BinaryWriter
// Function:		Write32Bit
//
// Description:		Writes 32-bit object to file.
//
// Input Arguments:
//		v	= const char* const
//
// Output Arguments:
//		None
//
// Return Value:
//		bool
//
//==========================================================================
bool BinaryWriter::Write32Bit(const char* const v)
{
	file.write(v, 4);
	return file.good();
}

//==========================================================================
// Class:			BinaryWriter
// Function:		Write64Bit
//
// Description:		Writes 64-bit object to file.
//
// Input Arguments:
//		v	= const char* const
//
// Output Arguments:
//		None
//
// Return Value:
//		bool
//
//==========================================================================
bool BinaryWriter::Write64Bit(const char* const v)
{
	file.write(v, 8);
	return file.good();
}

}// namespace VVASE
