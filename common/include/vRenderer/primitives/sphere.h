/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  sphere.h
// Created:  5/14/2009
// Author:  K. Loux
// Description:  Derived from PRIMITIVE for creating spherical objects.
// History:

#ifndef _SPHERE_H_
#define _SPHERE_H_

// Local headers
#include "vRenderer/primitives/primitive.h"
#include "vMath/vector_class.h"

class SPHERE : public PRIMITIVE
{
public:
	// Constructor
	SPHERE(RENDER_WINDOW &_RenderWindow);

	// Destructor
	~SPHERE();

	// Mandatory overloads from PRIMITIVE - for creating geometry and testing the
	// validity of this object's parameters
	void GenerateGeometry(void);
	bool HasValidParameters(void);

	// Private data accessors
	void SetResolution(const int &_Resolution);
	void SetCenter(const VECTOR &_Center);
	void SetRadius(const double &_Radius);

private:
	// The resolution
	int Resolution;

	// The sphere's center point
	VECTOR Center;

	// The sphere's radius
	double Radius;

	// Performs subdivision of the triangles to achieve a "rounder" sphere
	void RecursiveSubdivision(const VECTOR &Corner1, const VECTOR &Corner2,
		const VECTOR &Corner3, int Level);

	// Adds a vertex to the glList
	void AddVertex(const VECTOR &Vertex);
};

#endif// _SPHERE_H_