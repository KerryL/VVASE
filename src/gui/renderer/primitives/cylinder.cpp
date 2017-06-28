/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  cylinder.cpp
// Date:  5/14/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Derived from Primitive for creating cylindrical objects.

// Local headers
#include "vRenderer/primitives/cylinder.h"
#include "vRenderer/renderWindow.h"
#include "vMath/carMath.h"
#include "vUtilities/unitConverter.h"

//==========================================================================
// Class:			Cylinder
// Function:		Cylinder
//
// Description:		Constructor for the Cylinder class.
//
// Input Arguments:
//		renderWindow	= RenderWindow& pointing to the object that owns this
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Cylinder::Cylinder(RenderWindow &renderWindow) : Primitive(renderWindow)
{
	drawCaps = false;
	radius = 0.0;
	endPoint1.Set(0.0, 0.0, 0.0);
	endPoint2.Set(0.0, 0.0, 0.0);
	resolution = 4;
}

//==========================================================================
// Class:			Cylinder
// Function:		~Cylinder
//
// Description:		Destructor for the Cylinder class.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Cylinder::~Cylinder()
{
}

//==========================================================================
// Class:			Cylinder
// Function:		GenerateGeometry
//
// Description:		Creates the OpenGL instructions to create this object in
//					the scene.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Cylinder::GenerateGeometry()
{
	if (resolution < 3)
		resolution = 3;

	double halfHeight = endPoint1.Distance(endPoint2) / 2.0;

	Vector axisDirection = (endPoint2 - endPoint1).Normalize();
	Vector center = endPoint1 + axisDirection * halfHeight;
	Vector referenceDirection(1.0, 0.0, 0.0);

	// Determine the angle and axis of rotation
	Vector axisOfRotation = referenceDirection.Cross(axisDirection);
	double angle = acos(axisDirection * referenceDirection);// [rad]

	glPushMatrix();

		glTranslated(center.x, center.y, center.z);

		// Rotate the current matrix, if the rotation axis is non-zero
		if (!VVASEMath::IsZero(axisOfRotation.Length()))
			glRotated(UnitConverter::RAD_TO_DEG(angle), axisOfRotation.x, axisOfRotation.y, axisOfRotation.z);

		// Create the cylinder along the X-axis (must match the reference direction above)
		// (the openGL matrices take care of correct position/orientation in hardware)
		// We'll use a triangle strip to draw the cylinder
		glBegin(GL_TRIANGLE_STRIP);

		int i;
		Vector point(halfHeight, 0.0, 0.0);
		for (i = 0; i <= resolution; i++)
		{
			angle = (double)i * 2.0 * VVASEMath::Pi / (double)resolution;
			point.y = radius * cos(angle);
			point.z = radius * sin(angle);

			glNormal3d(0.0, point.y / radius, point.z / radius);

			glVertex3d(point.x, point.y, point.z);
			glVertex3d(-point.x, point.y, point.z);
		}

		glEnd();

		if (drawCaps)
		{
			glNormal3d(1.0, 0.0, 0.0);
			glBegin(GL_POLYGON);

			for (i = 0; i <= resolution; i++)
			{
				angle = (double)i * 2.0 * VVASEMath::Pi / (double)resolution;
				point.y = radius * cos(angle);
				point.z = radius * sin(angle);

				glVertex3d(point.x, point.y, point.z);
			}

			glEnd();

			glNormal3d(-1.0, 0.0, 0.0);
			glBegin(GL_POLYGON);

			for (i = 0; i <= resolution; i++)
			{
				angle = (double)i * 2.0 * VVASEMath::Pi / (double)resolution;
				point.y = radius * cos(angle);
				point.z = radius * sin(angle);

				glVertex3d(-point.x, point.y, point.z);
			}

			glEnd();
		}

	glPopMatrix();
}

//==========================================================================
// Class:			Cylinder
// Function:		HasValidParameters
//
// Description:		Checks to see if the information about this object is
//					valid and complete (gives permission to create the object).
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for OK to draw, false otherwise
//
//==========================================================================
bool Cylinder::HasValidParameters()
{
	if (!VVASEMath::IsZero(endPoint1.Distance(endPoint2)) && radius > 0.0)
		return true;

	return false;
}

//==========================================================================
// Class:			Cylinder
// Function:		SetResolution
//
// Description:		Sets the number of faces use to approximate the cylinder.
//
// Input Arguments:
//		resolution	= const int&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Cylinder::SetResolution(const int &resolution)
{
	this->resolution = resolution;
	modified = true;
}

//==========================================================================
// Class:			Cylinder
// Function:		SetCapping
//
// Description:		Sets the flag indicating whether or not the cylinder should be
//					capped on the base end.
//
// Input Arguments:
//		drawCaps	= const bool&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Cylinder::SetCapping(const bool &drawCaps)
{
	this->drawCaps = drawCaps;
	modified = true;
}

//==========================================================================
// Class:			Cylinder
// Function:		SetEndPoint1
//
// Description:		Sets the location of the first end point.
//
// Input Arguments:
//		endPoint1	= const Vector&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Cylinder::SetEndPoint1(const Vector &endPoint1)
{
	this->endPoint1 = endPoint1;
	modified = true;
}

//==========================================================================
// Class:			Cylinder
// Function:		SetEndPoint2
//
// Description:		Sets the location of the second end point.
//
// Input Arguments:
//		endPoint2	= const Vector&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Cylinder::SetEndPoint2(const Vector &endPoint2)
{
	this->endPoint2 = endPoint2;
	modified = true;
}

//==========================================================================
// Class:			Cylinder
// Function:		SetRadius
//
// Description:		Sets the radius at the base of the Cylinder.
//
// Input Arguments:
//		radius	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Cylinder::SetRadius(const double &radius)
{
	this->radius = radius;
	modified = true;
}

//==========================================================================
// Class:			Cylinder
// Function:		IsIntersectedBy
//
// Description:		Determines if this object is intersected by the specified
//					line.
//
// Input Arguments:
//		point		= const Vector&
//		direction	= const Vector&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool
//
//==========================================================================
bool Cylinder::IsIntersectedBy(const Vector& point, const Vector& direction) const
{
	// TODO:  Implement
	return false;
}
