/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  cylinder.cpp
// Date:  5/14/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Derived from Primitive for creating cylindrical objects.

// GLEW headers
#include <GL/glew.h>

// Local headers
#include "VVASE/gui/renderer/primitives/cylinder.h"
#include "VVASE/gui/utilities/unitConverter.h"
#include "VVASE/core/utilities/carMath.h"

// LibPlot2D headers
#include <lp2d/renderer/renderWindow.h>

// Eigen headers
#include <Eigen/Geometry>

namespace VVASE
{

//==========================================================================
// Class:			Cylinder
// Function:		Cylinder
//
// Description:		Constructor for the Cylinder class.
//
// Input Arguments:
//		renderWindow	= LibPlot2D::RenderWindow& pointing to the object that owns this
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Cylinder::Cylinder(LibPlot2D::RenderWindow &renderWindow) : Primitive(renderWindow)
{
	mBufferInfo.resize(1);
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
	glBindVertexArray(mBufferInfo[0].GetVertexArrayIndex());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBufferInfo[0].GetIndexBufferIndex());
	glDrawElements(GL_TRIANGLES, mBufferInfo[0].indexBuffer.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	assert(!LibPlot2D::RenderWindow::GLHasError());
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
	if (!VVASE::Math::IsZero((endPoint1 - endPoint2).norm()) && radius > 0.0)
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
	mModified = true;
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
	mModified = true;
}

//==========================================================================
// Class:			Cylinder
// Function:		SetEndPoint1
//
// Description:		Sets the location of the first end point.
//
// Input Arguments:
//		endPoint1	= const Eigen::Vector3d&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Cylinder::SetEndPoint1(const Eigen::Vector3d &endPoint1)
{
	this->endPoint1 = endPoint1;
	mModified = true;
}

//==========================================================================
// Class:			Cylinder
// Function:		SetEndPoint2
//
// Description:		Sets the location of the second end point.
//
// Input Arguments:
//		endPoint2	= const Eigen::Vector3d&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Cylinder::SetEndPoint2(const Eigen::Vector3d &endPoint2)
{
	this->endPoint2 = endPoint2;
	mModified = true;
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
	mModified = true;
}

//==========================================================================
// Class:			Cylinder
// Function:		IsIntersectedBy
//
// Description:		Determines if this object is intersected by the specified
//					line.
//
// Input Arguments:
//		point		= const Eigen::Vector3d&
//		direction	= const Eigen::Vector3d&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool
//
//==========================================================================
bool Cylinder::IsIntersectedBy(const Eigen::Vector3d& point, const Eigen::Vector3d& direction) const
{
	// TODO:  Implement
	return false;
}

//==========================================================================
// Class:			Cylinder
// Function:		Update
//
// Description:		Updates the GL buffers associated with this object.
//
// Input Arguments:
//		i	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Cylinder::Update(const unsigned int& /*i*/)
{
	if (resolution < 3)
		resolution = 3;

	mBufferInfo[0].GetOpenGLIndices(true);

	mBufferInfo[0].vertexCount = 2 * resolution;
	mBufferInfo[0].vertexBuffer.resize(mBufferInfo[0].vertexCount
		* (mRenderWindow.GetVertexDimension() + 4));// 4D vertex, RGBA color
	assert(mRenderWindow.GetVertexDimension() == 4);

	const unsigned int triangleCount([this]()
	{
		unsigned int count(2 * resolution);
		if (drawCaps)
			count += 2 * (resolution - 2);
		return count;
	}());
	mBufferInfo[0].indexBuffer.resize(triangleCount * 3);

	const Eigen::Vector3d axisDirection((endPoint2 - endPoint1).normalized());
	const Eigen::Vector3d radial([axisDirection]()
	{
		Eigen::Vector3d v(axisDirection);
		if (fabs(v.x()) < fabs(v.y()) && fabs(v.x()) < fabs(v.z()))// rotate about x
			return Eigen::AngleAxisd(0.5 * M_PI, Eigen::Vector3d::UnitX()) * v;
		else if (fabs(v.y()) < fabs(v.z()))// rotate about y
			return Eigen::AngleAxisd(0.5 * M_PI, Eigen::Vector3d::UnitY()) * v;

		return Eigen::AngleAxisd(0.5 * M_PI, Eigen::Vector3d::UnitZ()) * v;
	}().normalized() * radius);

	int j;
	for (j = 0; j < resolution; ++j)
	{
		const double angle(2.0 * M_PI * j / static_cast<double>(resolution));
		const Eigen::AngleAxisd rotation(angle, axisDirection);
		const Eigen::Vector3d v(rotation * radial);

		mBufferInfo[0].vertexBuffer[j * 4] = static_cast<float>((endPoint1 + v).x());
		mBufferInfo[0].vertexBuffer[j * 4 + 1] = static_cast<float>((endPoint1 + v).y());
		mBufferInfo[0].vertexBuffer[j * 4 + 2] = static_cast<float>((endPoint1 + v).z());
		mBufferInfo[0].vertexBuffer[j * 4 + 3] = 1.0f;

		mBufferInfo[0].vertexBuffer[resolution * 4 + j * 4] = static_cast<float>((endPoint2 + v).x());
		mBufferInfo[0].vertexBuffer[resolution * 4 + j * 4 + 1] = static_cast<float>((endPoint2 + v).y());
		mBufferInfo[0].vertexBuffer[resolution * 4 + j * 4 + 2] = static_cast<float>((endPoint2 + v).z());
		mBufferInfo[0].vertexBuffer[resolution * 4 + j * 4 + 3] = 1.0f;

		mBufferInfo[0].vertexBuffer[8 * resolution + j * 4] = static_cast<float>(mColor.GetRed());
		mBufferInfo[0].vertexBuffer[8 * resolution + j * 4 + 1] = static_cast<float>(mColor.GetGreen());
		mBufferInfo[0].vertexBuffer[8 * resolution + j * 4 + 2] = static_cast<float>(mColor.GetBlue());
		mBufferInfo[0].vertexBuffer[8 * resolution + j * 4 + 3] = static_cast<float>(mColor.GetAlpha());

		mBufferInfo[0].vertexBuffer[12 * resolution + j * 4] = static_cast<float>(mColor.GetRed());
		mBufferInfo[0].vertexBuffer[12 * resolution + j * 4 + 1] = static_cast<float>(mColor.GetGreen());
		mBufferInfo[0].vertexBuffer[12 * resolution + j * 4 + 2] = static_cast<float>(mColor.GetBlue());
		mBufferInfo[0].vertexBuffer[12 * resolution + j * 4 + 3] = static_cast<float>(mColor.GetAlpha());

		if (j == resolution - 1)// These triangles connect first vertices with last
		{
			mBufferInfo[0].indexBuffer[j * 6] = resolution - 1;
			mBufferInfo[0].indexBuffer[j * 6 + 1] = 0;
			mBufferInfo[0].indexBuffer[j * 6 + 2] = 2 * resolution - 1;

			mBufferInfo[0].indexBuffer[j * 6 + 3] = 0;
			mBufferInfo[0].indexBuffer[j * 6 + 4] = resolution;
			mBufferInfo[0].indexBuffer[j * 6 + 5] = 2 * resolution - 1;
		}
		else
		{
			mBufferInfo[0].indexBuffer[j * 6] = j;
			mBufferInfo[0].indexBuffer[j * 6 + 1] = j + 1;
			mBufferInfo[0].indexBuffer[j * 6 + 2] = resolution + j;

			mBufferInfo[0].indexBuffer[j * 6 + 3] = j + 1;
			mBufferInfo[0].indexBuffer[j * 6 + 4] = resolution + j + 1;
			mBufferInfo[0].indexBuffer[j * 6 + 5] = resolution + j;
		}
	}

	if (drawCaps)
	{
		for (j = 0; j < resolution - 2; ++j)
		{
			mBufferInfo[0].indexBuffer[6 * resolution + j * 3] = 0;
			mBufferInfo[0].indexBuffer[6 * resolution + j * 3 + 1] = j + 1;
			mBufferInfo[0].indexBuffer[6 * resolution + j * 3 + 2] = j + 2;

			mBufferInfo[0].indexBuffer[9 * resolution - 6 + j * 3] = resolution;
			mBufferInfo[0].indexBuffer[9 * resolution - 5 + j * 3] = resolution + j + 1;
			mBufferInfo[0].indexBuffer[9 * resolution - 4 + j * 3] = resolution + j + 2;
		}
	}

	glBindVertexArray(mBufferInfo[0].GetVertexArrayIndex());

	glBindBuffer(GL_ARRAY_BUFFER, mBufferInfo[0].GetVertexBufferIndex());
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * mBufferInfo[0].vertexBuffer.size(),
		mBufferInfo[0].vertexBuffer.data(), GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(mRenderWindow.GetDefaultPositionLocation());
	glVertexAttribPointer(mRenderWindow.GetDefaultPositionLocation(), 4, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(mRenderWindow.GetDefaultColorLocation());
	glVertexAttribPointer(mRenderWindow.GetDefaultColorLocation(), 4, GL_FLOAT, GL_FALSE, 0,
		(void*)(sizeof(GLfloat) * mRenderWindow.GetVertexDimension() * mBufferInfo[0].vertexCount));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBufferInfo[0].GetIndexBufferIndex());
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * mBufferInfo[0].indexBuffer.size(),
		mBufferInfo[0].indexBuffer.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	assert(!LibPlot2D::RenderWindow::GLHasError());

	mBufferInfo[0].vertexCountModified = false;
}

}// namespace VVASE
