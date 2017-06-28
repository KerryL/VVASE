/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  spring.h
// Date:  1/3/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains class declaration for SPRING class.  This class contains
//        the information and methods required to model a non-linear spring.

#ifndef SPRING_H_
#define SPRING_H_

namespace VVASE
{

class Spring
{
public:
	Spring();
	~Spring();

	double rate;// [lb/in]

private:
};

}// namespace VVASE

#endif// SPRING_H_
