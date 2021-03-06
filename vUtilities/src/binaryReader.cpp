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

// Local headers
#include "vUtilities/binaryReader.h"

//==========================================================================
// Class:			BinaryReader
// Function:		BinaryReader
//
// Description:		Constructor for BinaryReader class.
//
// Input Arguments:
//		file	= std::ifstream&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
BinaryReader::BinaryReader(std::ifstream& file) : file(file)
{
}

//==========================================================================
// Class:			BinaryReader
// Function:		Read
//
// Description:		Reads specified object from file.
//
// Input Arguments:
//		v	= std::string&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool
//
//==========================================================================
bool BinaryReader::Read(std::string& v)
{
	bool ok(true);
	unsigned int length;
	ok = Read(length);

	char c;
	unsigned int i;
	for (i = 0; i < length; i++)
	{
		ok = Read(c) && ok;
		v.push_back(c);
	}

	return ok;
}

//==========================================================================
// Class:			BinaryReader
// Function:		Read
//
// Description:		Reads specified object from file.
//
// Input Arguments:
//		v	= char&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool
//
//==========================================================================
bool BinaryReader::Read(char& v)
{
	return Read8Bit(&v);
}

//==========================================================================
// Class:			BinaryReader
// Function:		Read
//
// Description:		Reads specified object from file.
//
// Input Arguments:
//		v	= short&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool
//
//==========================================================================
bool BinaryReader::Read(short& v)
{
	return Read16Bit(reinterpret_cast<char* const>(&v));
}

//==========================================================================
// Class:			BinaryReader
// Function:		Read
//
// Description:		Reads specified object from file.
//
// Input Arguments:
//		v	= int&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool
//
//==========================================================================
bool BinaryReader::Read(int& v)
{
	return Read32Bit(reinterpret_cast<char* const>(&v));
}

//==========================================================================
// Class:			BinaryReader
// Function:		Read
//
// Description:		Reads specified object from file.
//
// Input Arguments:
//		v	= long&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool
//
//==========================================================================
bool BinaryReader::Read(long& v)
{
	return Read32Bit(reinterpret_cast<char* const>(&v));
}

//==========================================================================
// Class:			BinaryReader
// Function:		Read
//
// Description:		Reads specified object from file.
//
// Input Arguments:
//		v	= long lont&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool
//
//==========================================================================
bool BinaryReader::Read(long long& v)
{
	return Read64Bit(reinterpret_cast<char* const>(&v));
}

//==========================================================================
// Class:			BinaryReader
// Function:		Read
//
// Description:		Reads specified object from file.
//
// Input Arguments:
//		v	= unsigned char&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool
//
//==========================================================================
bool BinaryReader::Read(unsigned char& v)
{
	return Read8Bit(reinterpret_cast<char* const>(&v));
}

//==========================================================================
// Class:			BinaryReader
// Function:		Read
//
// Description:		Reads specified object from file.
//
// Input Arguments:
//		v	= unsigned short&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool
//
//==========================================================================
bool BinaryReader::Read(unsigned short& v)
{
	return Read16Bit(reinterpret_cast<char* const>(&v));
}

//==========================================================================
// Class:			BinaryReader
// Function:		Read
//
// Description:		Reads specified object from file.
//
// Input Arguments:
//		v	= unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool
//
//==========================================================================
bool BinaryReader::Read(unsigned int& v)
{
	return Read32Bit(reinterpret_cast<char* const>(&v));
}

//==========================================================================
// Class:			BinaryReader
// Function:		Read
//
// Description:		Reads specified object from file.
//
// Input Arguments:
//		v	= unsigned long&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool
//
//==========================================================================
bool BinaryReader::Read(unsigned long& v)
{
	return Read32Bit(reinterpret_cast<char* const>(&v));
}

//==========================================================================
// Class:			BinaryReader
// Function:		Read
//
// Description:		Reads specified object from file.
//
// Input Arguments:
//		v	= unsigned long long&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool
//
//==========================================================================
bool BinaryReader::Read(unsigned long long& v)
{
	return Read64Bit(reinterpret_cast<char* const>(&v));
}

//==========================================================================
// Class:			BinaryReader
// Function:		Read
//
// Description:		Reads specified object from file.
//
// Input Arguments:
//		v	= float&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool
//
//==========================================================================
bool BinaryReader::Read(float& v)
{
	return Read32Bit(reinterpret_cast<char* const>(&v));
}

//==========================================================================
// Class:			BinaryReader
// Function:		Read
//
// Description:		Reads specified object from file.
//
// Input Arguments:
//		v	= double&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool
//
//==========================================================================
bool BinaryReader::Read(double& v)
{
	return Read64Bit(reinterpret_cast<char* const>(&v));
}

//==========================================================================
// Class:			BinaryReader
// Function:		Read
//
// Description:		Reads specified object from file.
//
// Input Arguments:
//		v	= bool&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool
//
//==========================================================================
bool BinaryReader::Read(bool& v)
{
	return Read8Bit(reinterpret_cast<char* const>(&v));
}

//==========================================================================
// Class:			BinaryReader
// Function:		Read
//
// Description:		Reads specified object from file.
//
// Input Arguments:
//		v	= Vector&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool
//
//==========================================================================
bool BinaryReader::Read(Vector& v)
{
	bool ok(true);

	ok = Read(v.x) && ok;
	ok = Read(v.y) && ok;
	ok = Read(v.z) && ok;

	return ok;
}

//==========================================================================
// Class:			BinaryReader
// Function:		Read8Bit
//
// Description:		Reads 8-bit object from file.
//
// Input Arguments:
//		v	= char* const
//
// Output Arguments:
//		None
//
// Return Value:
//		bool
//
//==========================================================================
bool BinaryReader::Read8Bit(char* const v)
{
	file.read(v, 1);
	return file.good();
}

//==========================================================================
// Class:			BinaryReader
// Function:		Read16Bit
//
// Description:		Reads 16-bit object from file.
//
// Input Arguments:
//		v	= char* const
//
// Output Arguments:
//		None
//
// Return Value:
//		bool
//
//==========================================================================
bool BinaryReader::Read16Bit(char* const v)
{
	file.read(v, 2);
	return file.good();
}

//==========================================================================
// Class:			BinaryReader
// Function:		Read32Bit
//
// Description:		32-bit object from file.
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
bool BinaryReader::Read32Bit(char* const v)
{
	file.read(v, 4);
	return file.good();
}

//==========================================================================
// Class:			BinaryReader
// Function:		Read64Bit
//
// Description:		Reads 64-bit object from file.
//
// Input Arguments:
//		v	= char* const
//
// Output Arguments:
//		None
//
// Return Value:
//		bool
//
//==========================================================================
bool BinaryReader::Read64Bit(char* const v)
{
	file.read(v, 8);
	return file.good();
}
