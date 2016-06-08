/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  tireSet.h
// Created:  3/9/2008
// Author:  K. Loux
// Description:  Contains class declaration for TireSet class.  This class contains a set
//				 of four tires.  Required to manage dynamic memory allocation of a set of tires.
// History:
//	11/22/2009	- Moved to vCar.lib, K. Loux.

#ifndef TIRESET_H_
#define TIRESET_H_

// Local forward declarations
class Tire;
class BinaryReader;
class BinaryWriter;

class TireSet
{
public:
	TireSet();
	TireSet(const TireSet &tireSet);
	~TireSet();

	// File read/write functions
	void Write(BinaryWriter& file) const;
	void Read(BinaryReader& file, const int& fileVersion);

	Tire *rightFront;
	Tire *leftFront;
	Tire *rightRear;
	Tire *leftRear;

	TireSet& operator=(const TireSet &tireSet);
};

#endif// TIRESET_H_