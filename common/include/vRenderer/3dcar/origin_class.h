/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  origin_class.h
// Created:  5/23/2008
// Author:  K. Loux
// Description:  Contains class declaration for the ORIGIN class.
// History:
//	5/17/2009	- Removed VTK dependencies, K. Loux.

#ifndef _ORIGIN_CLASS_H_
#define _ORIGIN_CLASS_H_

// VVASE forward declarations
class VECTOR3D;
class RENDER_WINDOW;

class ORIGIN
{
public:
	// Constructor
	ORIGIN(RENDER_WINDOW &_Renderer);

	// Destructor
	~ORIGIN();

	// Updates the object in the rendered scene
	void Update(const double &ShaftLength, const double &ShaftDiameter, const double &TipLength,
		const double &TipDiameter, const int &Resolution, const bool &Show);

private:
	// The objects that make up the origin
	VECTOR3D *XDirection;
	VECTOR3D *YDirection;
	VECTOR3D *ZDirection;
};

#endif// _ORIGIN_CLASS_H_