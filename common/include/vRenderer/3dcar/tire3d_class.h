/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  tire3d_class.h
// Created:  5/23/2008
// Author:  K. Loux
// Description:  Contains class declaration for the TIRE3D class.
// History:
//	4/25/2009	- Changed Update() to include color argument in the form of wxColor, K. Loux.
//	5/17/2009	- Removed VTK dependencies, K. Loux.

#ifndef _TIRE3D_CLASS_H_
#define _TIRE3D_CLASS_H_

// VVASE forward declarations
class VECTOR;
class COLOR;
class CYLINDER;
class DISK;

class TIRE3D
{
public:
	// Constructor
	TIRE3D(RENDER_WINDOW &_Renderer);

	// Destructor
	~TIRE3D();

	// Updates the 3D representation of the tire on the screen
	void Update(const VECTOR &ContactPatch, const VECTOR &Center, VECTOR OriginalNormal,
		VECTOR TargetNormal, const double &Width, const double &InsideDiameter,
		const int &Resolution, const COLOR &Color, bool Show);

	// Returns true if the passed reference is to an actor from this object
	bool ContainsThisActor(const PRIMITIVE *Actor);

private:
	// The outer surface of the tire
	CYLINDER *OuterSurface;

	// The inner surface of the tire
	CYLINDER *InnerSurface;

	// The sidewalls
	DISK *Sidewall1;
	DISK *Sidewall2;
};

#endif// _TIRE3D_CLASS_H_