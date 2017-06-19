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

#ifndef TIRESET_H_
#define TIRESET_H_

// Local forward declarations
class Tire;
class BinaryReader;
class BinaryWriter;

class TireSet
{
public:
    // Required by RegisterableComponent
    static std::unique_ptr<TireSet> Create() { return std::make_unique<TireSet>(); }
    static std::string GetName() { return _T("Tires"); }

	// File read/write functions
	void Write(BinaryWriter& file) const override;
	void Read(BinaryReader& file, const int& fileVersion) override;

	std::unique_ptr<Tire> rightFront;
	std::unique_ptr<Tire> leftFront;
	std::unique_ptr<Tire> rightRear;
	std::unique_ptr<Tire> leftRear;
};

#endif// TIRESET_H_