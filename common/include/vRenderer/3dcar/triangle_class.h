/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

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
class VECTOR;
class COLOR;
class TRIANGLE;

class TRIANGLE3D
{
public:
	// Constructor
	TRIANGLE3D(RENDER_WINDOW &_Renderer);

	// Destructor
	~TRIANGLE3D();

	// Updates the 3D representation of the tire on the screen
	void Update(const VECTOR &Node1, const VECTOR &Node2, const VECTOR &Node3,
		const COLOR &Color, bool Show);

	// Returns true if the passed reference is to an actor from this object
	bool ContainsThisActor(const PRIMITIVE *Actor);

private:
	// The triangle actor
	TRIANGLE *Triangle;
};

#endif// _TRIANGLE_CLASS_H_