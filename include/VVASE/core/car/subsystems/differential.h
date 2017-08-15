/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  differential.h
// Date:  11/6/2007
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains class declaration for DIFFERENTIAL class.

#ifndef DIFFERENTIAL_H_
#define DIFFERENTIAL_H_

namespace VVASE
{

// Local forward declarations
class BinaryReader;
class BinaryWriter;

class Differential
{
public:
	Differential() = default;
	explicit Differential(const double& biasRatio);

	// File read/write functions
	void Write(BinaryWriter& file) const;
	void Read(BinaryReader& file, const int& fileVersion);

	// Overloaded operators
	Differential& operator=(const Differential &differential);

	double biasRatio;
};

}// namespace VVASE

#endif// DIFFERENTIAL_H_
