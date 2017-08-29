/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  cone.cpp
// Date:  5/14/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Derived from Primitive for creating conical objects.

// GLEW headers
#include <GL/glew.h>

// Local headers
#include "VVASE/gui/renderer/primitives/cone.h"
#include "VVASE/gui/utilities/unitConverter.h"
#include "VVASE/core/utilities/carMath.h"

// LibPlot2D headers
#include <lp2d/renderer/renderWindow.h>

// Eigen headers
#include <Eigen/Geometry>

namespace VVASE
{

//==========================================================================
// Class:			Cone
// Function:		Cone
//
// Description:		Constructor for the Cone class.
//
// Input Arguments:
//		renderWindow	= LibPlot2D::RenderWindow& reference to the object that owns this
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Cone::Cone(LibPlot2D::RenderWindow &renderWindow) : Primitive(renderWindow)
{
	mBufferInfo.resize(1);
}

//==========================================================================
// Class:			Cone
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
void Cone::GenerateGeometry()
{
	glBindVertexArray(mBufferInfo[0].GetVertexArrayIndex());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBufferInfo[0].GetIndexBufferIndex());
	glDrawElements(GL_TRIANGLES, mBufferInfo[0].indexBuffer.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	assert(!LibPlot2D::RenderWindow::GLHasError());
}

//==========================================================================
// Class:			Cone
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
bool Cone::HasValidParameters()
{
	// Cones must have a non-zero distance tip-to-base, and must have a positive radius
	if (!VVASE::Math::IsZero((tip - baseCenter).norm()) && radius > 0.0)
		return true;

	// Otherwise return false
	return false;
}

//==========================================================================
// Class:			Cone
// Function:		SetResolution
//
// Description:		Sets the number of faces use to approximate the cone.
//
// Input Arguments:
//		_resolution	= const int&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Cone::SetResolution(const int &resolution)
{
	this->resolution = resolution;
	mModified = true;
}

//==========================================================================
// Class:			Cone
// Function:		SetCapping
//
// Description:		Sets the flag indicating whether or not the cone should be
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
void Cone::SetCapping(const bool &drawCaps)
{
	this->drawCaps = drawCaps;
	mModified = true;
}

//==========================================================================
// Class:			Cone
// Function:		SetTip
//
// Description:		Sets the location of the tip of the cone.
//
// Input Arguments:
//		_tip	= const Eigen::Vector3d&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Cone::SetTip(const Eigen::Vector3d &tip)
{
	this->tip = tip;
	mModified = true;
}

//==========================================================================
// Class:			Cone
// Function:		SetBaseCenter
//
// Description:		Sets the location of the center of the cone's base.
//
// Input Arguments:
//		_baseCenter	= const Eigen::Vector3d&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Cone::SetBaseCenter(const Eigen::Vector3d &baseCenter)
{
	this->baseCenter = baseCenter;
	mModified = true;
}

//==========================================================================
// Class:			Cone
// Function:		SetRadius
//
// Description:		Sets the radius at the base of the cone.
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
void Cone::SetRadius(const double &radius)
{
	this->radius = radius;
	mModified = true;
}

//==========================================================================
// Class:			Cone
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
bool Cone::IsIntersectedBy(const Eigen::Vector3d& point, const Eigen::Vector3d& direction) const
{
	// TODO:  Implement
	return false;
}

//==========================================================================
// Class:			Cone
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
void Cone::Update(const unsigned int& /*i*/)
{
	if (resolution < 3)
		resolution = 3;

	mBufferInfo[0].GetOpenGLIndices(true);

	mBufferInfo[0].vertexCount = resolution + 1;
	mBufferInfo[0].vertexBuffer.resize(mBufferInfo[0].vertexCount
		* (mRenderWindow.GetVertexDimension() + 4));// 3D vertex, RGBA color + 3D normal? TODO
	assert(mRenderWindow.GetVertexDimension() == 4);

	const unsigned int triangleCount(resolution + (resolution - 2) * static_cast<int>(drawCaps));
	mBufferInfo[0].indexBuffer.resize(triangleCount * 3);

	const Eigen::Vector3d axisDirection((tip - baseCenter).normalized());
	const Eigen::Vector3d radial([axisDirection]()
	{
		Eigen::Vector3d v(axisDirection);
		if (fabs(v.x()) < fabs(v.y()) && fabs(v.x()) < fabs(v.z()))// rotate about x
			return Eigen::AngleAxisd(0.5 * M_PI, Eigen::Vector3d::UnitX()) * v;
		else if (fabs(v.y()) < fabs(v.z()))// rotate about y
			return Eigen::AngleAxisd(0.5 * M_PI, Eigen::Vector3d::UnitY()) * v;

		return Eigen::AngleAxisd(0.5 * M_PI, Eigen::Vector3d::UnitZ()) * v;
	}().normalized() * radius);

	const unsigned int colorStart(4 * (resolution + 1));

	int j;
	for (j = 0; j < resolution; ++j)
	{
		const double angle(2.0 * M_PI * j / static_cast<double>(resolution));
		const Eigen::AngleAxisd rotation(angle, axisDirection);
		const Eigen::Vector3d v(rotation * radial);

		mBufferInfo[0].vertexBuffer[j * 4] = static_cast<float>((baseCenter + v).x());
		mBufferInfo[0].vertexBuffer[j * 4 + 1] = static_cast<float>((baseCenter + v).y());
		mBufferInfo[0].vertexBuffer[j * 4 + 2] = static_cast<float>((baseCenter + v).z());
		mBufferInfo[0].vertexBuffer[j * 4 + 3] = 1.0f;

		mBufferInfo[0].vertexBuffer[colorStart + j * 4] = static_cast<float>(mColor.GetRed());
		mBufferInfo[0].vertexBuffer[colorStart + j * 4 + 1] = static_cast<float>(mColor.GetGreen());
		mBufferInfo[0].vertexBuffer[colorStart + j * 4 + 2] = static_cast<float>(mColor.GetBlue());
		mBufferInfo[0].vertexBuffer[colorStart + j * 4 + 3] = static_cast<float>(mColor.GetAlpha());

		mBufferInfo[0].indexBuffer[j * 3] = resolution;
		mBufferInfo[0].indexBuffer[j * 3 + 1] = j;
		mBufferInfo[0].indexBuffer[j * 3 + 2] = (j + 1) % resolution;
	}

	mBufferInfo[0].vertexBuffer[4 * resolution] = static_cast<float>(tip.x());
	mBufferInfo[0].vertexBuffer[4 * resolution + 1] = static_cast<float>(tip.y());
	mBufferInfo[0].vertexBuffer[4 * resolution + 2] = static_cast<float>(tip.z());
	mBufferInfo[0].vertexBuffer[4 * resolution + 3] = 1.0f;

	mBufferInfo[0].vertexBuffer[colorStart + 4 * resolution] = static_cast<float>(mColor.GetRed());
	mBufferInfo[0].vertexBuffer[colorStart + 4 * resolution + 1] = static_cast<float>(mColor.GetGreen());
	mBufferInfo[0].vertexBuffer[colorStart + 4 * resolution + 2] = static_cast<float>(mColor.GetBlue());
	mBufferInfo[0].vertexBuffer[colorStart + 4 * resolution + 3] = static_cast<float>(mColor.GetAlpha());

	if (drawCaps)
	{
		for (j = 0; j < resolution - 2; ++j)
		{
			mBufferInfo[0].indexBuffer[3 * resolution + j * 3] = 0;
			mBufferInfo[0].indexBuffer[3 * resolution + j * 3 + 1] = j + 1;
			mBufferInfo[0].indexBuffer[3 * resolution + j * 3 + 2] = j + 2;
		}
	}

	glBindVertexArray(mBufferInfo[0].GetVertexArrayIndex());

	glBindBuffer(GL_ARRAY_BUFFER, mBufferInfo[0].GetVertexBufferIndex());
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(GLfloat) * mBufferInfo[0].vertexCount * (mRenderWindow.GetVertexDimension() + 4),// TODO:  Normals?
		mBufferInfo[0].vertexBuffer.data(), GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(mRenderWindow.GetPositionLocation());
	glVertexAttribPointer(mRenderWindow.GetPositionLocation(), 4, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(mRenderWindow.GetColorLocation());
	glVertexAttribPointer(mRenderWindow.GetColorLocation(), 4, GL_FLOAT, GL_FALSE, 0,
		(void*)(sizeof(GLfloat) * mRenderWindow.GetVertexDimension() * mBufferInfo[0].vertexCount));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBufferInfo[0].GetIndexBufferIndex());
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * mBufferInfo[0].indexBuffer.size(),
		mBufferInfo[0].indexBuffer.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	assert(!LibPlot2D::RenderWindow::GLHasError());
}

}// namespace VVASE
