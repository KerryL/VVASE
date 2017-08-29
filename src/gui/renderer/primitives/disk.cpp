/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  disk.cpp
// Date:  5/14/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Derived from Primitive for creating disk objects.

// GLEW headers
#include <GL/glew.h>

// Local headers
#include "VVASE/gui/renderer/primitives/disk.h"
#include "VVASE/gui/utilities/unitConverter.h"
#include "VVASE/core/utilities/carMath.h"

// LibPlot2D headers
#include <lp2d/renderer/renderWindow.h>

// Eigen headers
#include <Eigen/Geometry>

namespace VVASE
{

//==========================================================================
// Class:			Disk
// Function:		Disk
//
// Description:		Constructor for the Disk class.
//
// Input Arguments:
//		renderWindow	= RenderWindow* pointing to the object that owns this
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Disk::Disk(LibPlot2D::RenderWindow &renderWindow) : Primitive(renderWindow)
{
	mBufferInfo.resize(1);
}

//==========================================================================
// Class:			Disk
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
void Disk::GenerateGeometry()
{
	glBindVertexArray(mBufferInfo[0].GetVertexArrayIndex());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBufferInfo[0].GetIndexBufferIndex());
	glDrawElements(GL_TRIANGLES, mBufferInfo[0].indexBuffer.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	assert(!LibPlot2D::RenderWindow::GLHasError());
}

//==========================================================================
// Class:			Disk
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
bool Disk::HasValidParameters()
{
	if (outerRadius > 0.0 && !VVASE::Math::IsZero(normal.norm()))
		return true;

	return false;
}

//==========================================================================
// Class:			Disk
// Function:		SetResolution
//
// Description:		Sets the number of faces use to approximate the disk.
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
void Disk::SetResolution(const int &resolution)
{
	this->resolution = resolution;
	mModified = true;
}

//==========================================================================
// Class:			Disk
// Function:		SetOuterRadius
//
// Description:		Sets the outer radius of the disk.
//
// Input Arguments:
//		outerRadius	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Disk::SetOuterRadius(const double &outerRadius)
{
	this->outerRadius = outerRadius;
	mModified = true;
}

//==========================================================================
// Class:			Disk
// Function:		SetInnerRadius
//
// Description:		Sets the inner radius of the disk.
//
// Input Arguments:
//		innerRadius	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Disk::SetInnerRadius(const double &innerRadius)
{
	this->innerRadius = innerRadius;
	mModified = true;
}

//==========================================================================
// Class:			Disk
// Function:		SetCenter
//
// Description:		Sets the location of the center of the disk.
//
// Input Arguments:
//		center	= const Eigen::Vector3d&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Disk::SetCenter(const Eigen::Vector3d &center)
{
	this->center = center;
	mModified = true;
}

//==========================================================================
// Class:			Disk
// Function:		SetNormal
//
// Description:		Sets the disk's normal direction.
//
// Input Arguments:
//		normal	= const Eigen::Vector3d&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Disk::SetNormal(const Eigen::Vector3d &normal)
{
	this->normal = normal.normalized();
	mModified = true;
}

//==========================================================================
// Class:			Disk
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
bool Disk::IsIntersectedBy(const Eigen::Vector3d& point, const Eigen::Vector3d& direction) const
{
	// TODO:  Implement
	return false;
}

//==========================================================================
// Class:			Disk
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
void Disk::Update(const unsigned int& /*i*/)
{
	mBufferInfo[0].GetOpenGLIndices(true);

	mBufferInfo[0].vertexCount = resolution * (static_cast<int>(innerRadius > 0.0) + 1);
	mBufferInfo[0].vertexBuffer.resize(mBufferInfo[0].vertexCount
		* (mRenderWindow.GetVertexDimension() + 4));// 3D vertex, RGBA color + 3D normal? TODO
	assert(mRenderWindow.GetVertexDimension() == 4);

	const unsigned int triangleCount([this]()
	{
		if (innerRadius == 0.0)
			return resolution - 2;
		return resolution * 2;
	}());
	mBufferInfo[0].indexBuffer.resize(triangleCount * 3);

	const Eigen::Vector3d radial([this]()
	{
		Eigen::Vector3d v(normal);
		if (fabs(v.x()) < fabs(v.y()) && fabs(v.x()) < fabs(v.z()))// rotate about x
			return Eigen::AngleAxisd(0.5 * M_PI, Eigen::Vector3d::UnitX()) * v;
		else if (fabs(v.y()) < fabs(v.z()))// rotate about y
			return Eigen::AngleAxisd(0.5 * M_PI, Eigen::Vector3d::UnitY()) * v;

		return Eigen::AngleAxisd(0.5 * M_PI, Eigen::Vector3d::UnitZ()) * v;
	}().normalized() * outerRadius);

	const unsigned int colorStart([this]()
	{
		if (innerRadius > 0.0)
			return 8 * resolution;
		return 4 * resolution;
	}());

	int j;
	for (j = 0; j < resolution; ++j)
	{
		const double angle(2.0 * M_PI * j / static_cast<double>(resolution));
		const Eigen::AngleAxisd rotation(angle, normal);
		const Eigen::Vector3d v(rotation * radial);

		mBufferInfo[0].vertexBuffer[j * 4] = static_cast<float>((center + v).x());
		mBufferInfo[0].vertexBuffer[j * 4 + 1] = static_cast<float>((center + v).y());
		mBufferInfo[0].vertexBuffer[j * 4 + 2] = static_cast<float>((center + v).z());
		mBufferInfo[0].vertexBuffer[j * 4 + 3] = 1.0f;

		mBufferInfo[0].vertexBuffer[colorStart + j * 4] = static_cast<float>(mColor.GetRed());
		mBufferInfo[0].vertexBuffer[colorStart + j * 4 + 1] = static_cast<float>(mColor.GetGreen());
		mBufferInfo[0].vertexBuffer[colorStart + j * 4 + 2] = static_cast<float>(mColor.GetBlue());
		mBufferInfo[0].vertexBuffer[colorStart + j * 4 + 3] = static_cast<float>(mColor.GetAlpha());

		if (innerRadius > 0.0)
		{
			const Eigen::Vector3d u(v.normalized() * innerRadius);

			mBufferInfo[0].vertexBuffer[4 * resolution + j * 4] = static_cast<float>((center + u).x());
			mBufferInfo[0].vertexBuffer[4 * resolution + j * 4 + 1] = static_cast<float>((center + u).y());
			mBufferInfo[0].vertexBuffer[4 * resolution + j * 4 + 2] = static_cast<float>((center + u).z());
			mBufferInfo[0].vertexBuffer[4 * resolution + j * 4 + 3] = 1.0f;

			mBufferInfo[0].vertexBuffer[colorStart + 4 * resolution + j * 4] = static_cast<float>(mColor.GetRed());
			mBufferInfo[0].vertexBuffer[colorStart + 4 * resolution + j * 4 + 1] = static_cast<float>(mColor.GetGreen());
			mBufferInfo[0].vertexBuffer[colorStart + 4 * resolution + j * 4 + 2] = static_cast<float>(mColor.GetBlue());
			mBufferInfo[0].vertexBuffer[colorStart + 4 * resolution + j * 4 + 3] = static_cast<float>(mColor.GetAlpha());

			if (j == resolution - 1)// These triangles connect first vertices with last
			{
				mBufferInfo[0].indexBuffer[6 * j] = resolution - 1;
				mBufferInfo[0].indexBuffer[6 * j + 1] = 0;
				mBufferInfo[0].indexBuffer[6 * j + 2] = 2 * resolution - 1;

				mBufferInfo[0].indexBuffer[6 * j + 3] = 0;
				mBufferInfo[0].indexBuffer[6 * j + 4] = resolution;
				mBufferInfo[0].indexBuffer[6 * j + 5] = 2 * resolution - 1;
			}
			else
			{
				mBufferInfo[0].indexBuffer[6 * j] = j;
				mBufferInfo[0].indexBuffer[6 * j + 1] = j + 1;
				mBufferInfo[0].indexBuffer[6 * j + 2] = resolution + j;

				mBufferInfo[0].indexBuffer[6 * j + 3] = j + 1;
				mBufferInfo[0].indexBuffer[6 * j + 4] = resolution + j + 1;
				mBufferInfo[0].indexBuffer[6 * j + 5] = resolution + j;
			}
		}
	}

	if (innerRadius == 0.0)
	{
		for (j = 0; j < resolution - 2; ++j)
		{
			mBufferInfo[0].indexBuffer[3 * j] = 0;
			mBufferInfo[0].indexBuffer[3 * j + 1] = j + 1;
			mBufferInfo[0].indexBuffer[3 * j + 2] = j + 2;
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

	glBindVertexArray(0);

	assert(!LibPlot2D::RenderWindow::GLHasError());
}

}// namespace VVASE
