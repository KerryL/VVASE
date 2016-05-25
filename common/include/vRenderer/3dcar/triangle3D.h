/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  triangle3D.h
// Created:  5/23/2008
// Author:  K. Loux
// Description:  Contains class declaration for the Triangle3D class.  This is used for
//				 bell cranks, uprights, etc.
// History:
//	4/25/2009	- Changed Update() to include color argument in the form of wxColor and
//				  removed PURPOSE enumeration, K. Loux.
//	5/17/2009	- Removed VTK dependencies, K. Loux.

#ifndef TRIANGLE3D_H_
#define TRIANGLE3D_H_

// VVASE forward declarations
class Vector;
class Color;
class Triangle;

class Triangle3D
{
public:
	Triangle3D(RenderWindow &renderer);
	~Triangle3D();

	// Updates the 3D representation of the tire on the screen
	void Update(const Vector &node1, const Vector &node2, const Vector &node3,
		const Color &color, bool show);

	bool ContainsThisActor(const Primitive *actor);

private:
	Triangle *triangle;
};

#endif// TRIANGLE3D_H_