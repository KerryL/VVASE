/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  swaybar3d_class.h
// Created:  1/11/2009
// Author:  K. Loux
// Description:  Contains class declaration for the DAMPER3D class.
// History:
//	4/25/2009	- Changed Update() to include color argument in the form of wxColor, K. Loux.
//	5/17/2009	- Removed VTK dependencies, K. Loux.

#ifndef _SWAYBAR3D_CLASS_H_
#define _SWAYBAR3D_CLASS_H_

// VVASE headers
#include "vCar/suspension_class.h"

// VVASE forward declarations
class VECTOR;
class COLOR;
class CYLINDER;

class SWAYBAR3D
{
public:
	// Constructor
	SWAYBAR3D(RENDER_WINDOW &_Renderer);

	// Destructor
	~SWAYBAR3D();

	// Updates the 3D representation of the sway bar on the screen
	void Update(const VECTOR &RightLink, const VECTOR &LeftLink, const VECTOR &TorsionMemberTopRight,
		const VECTOR &TorsionMemberBottomLeft, const SUSPENSION::BAR_STYLE &BarStyle,
		const double &Dimension, const int &Resolution, const COLOR &Color, bool Show);

	// Returns true if the passed reference is to an actor from this object
	bool ContainsThisActor(const PRIMITIVE *Actor);

private:
	// The two torque arms
	CYLINDER *TorqueArm1;
	CYLINDER *TorqueArm2;

	// The torsion member
	CYLINDER *TorsionMember;
};

#endif// _SWAYBAR3D_CLASS_H_