/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  triangle_class.h
// Created:  5/23/2008
// Author:  K. Loux
// Description:  Contains class declaration for the TRIANGLE3D class.  This is used for
//				 bell cranks, uprights, etc.
// History:
//	4/25/2009	- Changed Update() to include color argument in the form of wxColor and
//				  removed PURPOSE enumeration, K. Loux.
//	5/17/2009	- Removed VTK dependencies, K. Loux.

#ifndef _TRIANGLE_CLASS_H_
#define _TRIANGLE_CLASS_H_

// VVASE forward declarations
class Vector;
class Color;
class TRIANGLE;

class TRIANGLE3D
{
public:
	// Constructor
	TRIANGLE3D(RenderWindow &_Renderer);

	// Destructor
	~TRIANGLE3D();

	// Updates the 3D representation of the tire on the screen
	void Update(const Vector &Node1, const Vector &Node2, const Vector &Node3,
		const Color &color, bool Show);

	// Returns true if the passed reference is to an actor from this object
	bool ContainsThisActor(const Primitive *Actor);

private:
	// The triangle actor
	TRIANGLE *Triangle;
};

#endif// _TRIANGLE_CLASS_H_