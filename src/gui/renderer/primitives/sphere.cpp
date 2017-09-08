/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  sphere.cpp
// Date:  5/14/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Derived from Primitive for creating spherical objects.

// GLEW headers
#include <GL/glew.h>

// Local headers
#include "VVASE/gui/renderer/primitives/sphere.h"
#include "VVASE/gui/utilities/unitConverter.h"
#include "VVASE/core/utilities/carMath.h"
#include "VVASE/gui/renderer/carRenderer.h"

// LibPlot2D headers
#include <lp2d/renderer/renderWindow.h>

// Standard C++ headers
#include <cassert>

namespace VVASE
{

//==========================================================================
// Class:			Sphere
// Function:		Static members
//
// Description:		Static members of the Sphere class.
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
bool Sphere::mInitialized(false);
unsigned int Sphere::mProgram;
const std::string Sphere::mResolutionName("resolution");
const std::string Sphere::mRadiusName("radius");

//=============================================================================
// Class:			Sphere
// Function:		mPassThroughVertexShader
//
// Description:		Vertex shader which does nothing.
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
//=============================================================================
const std::string Sphere::mPassThroughVertexShader(
	"#version 400\n"
	"\n"
	/*"uniform mat4 modelviewMatrix;\n"
	"uniform mat4 projectionMatrix;\n"*/
	"\n"
	"layout(location = 0) in vec4 position;\n"
	"layout(location = 1) in vec4 color;\n"
	"layout(location = 2) in float radius;\n"
	"\n"
	"out SphereInfo\n"
	"{\n"
	"    vec4 color;\n"
	"    float radius;\n"
	"} sphereInfo;\n"
	"\n"
	"void main()\n"
	"{\n"
	"    sphereInfo.color = color;\n"
	"    sphereInfo.radius = radius;\n"
	//"    gl_Position = projectionMatrix * modelviewMatrix * position;\n"
	"    gl_Position = position;\n"
	"}\n"
);

//==========================================================================
// Class:			Sphere
// Function:		mSphereGeometryShader
//
// Description:		Geometry shader for spheres.  Uses on-GPU recursive subdivision.
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
const std::string Sphere::mSphereGeometryShader(
	"#version 400\n"
	"\n"
	"uniform mat4 modelviewMatrix;\n"
	"uniform mat4 projectionMatrix;\n"
	"uniform mat3 normalMatrix;\n"
	"uniform int resolution;\n"
	"\n"
	"layout (points) in;\n"
	"layout (triangle_strip, max_vertices = 3) out;\n"
	"\n"
	"in SphereInfo\n"
	"{\n"
	"    vec4 color;\n"
	"    float radius;\n"
	"} sphereInfo[];\n"
	"\n"
	"out fragmentData\n"
	"{\n"
	"    vec4 color;\n"
	"    vec3 normal;\n"
	"    vec3 position;\n"
	"} f;\n"
	"\n"
	"void DoSubdivision(vec3 corner1, vec3 corner2, vec3 corner3)\n"
	"{\n"
	"    int i;\n"
	"    for (i = resolution; i > 1; --i)\n"
	"    {\n"
	// Compute the three points that divide this triangle into four sub-triangles
	// The division works like this:
	/*-----------------------
	      Corner 1
	         /\
	        /  \
	       /    \
	   MP1/\----/\MP2
	     /  \  /  \
	    /    \/MP3 \
	    ------------
	   Corner 2    Corner 3
	-----------------------*/
	// TODO:  This is wrong!
	"        vec3 midPoint1 = normalize((corner1 + corner2) * 0.5) * sphereInfo[0].radius;\n"
	"        vec3 midPoint2 = normalize((corner1 + corner3) * 0.5) * sphereInfo[0].radius;\n"
	"        vec3 midPoint3 = normalize((corner2 + corner3) * 0.5) * sphereInfo[0].radius;\n"
	"\n"
	// Call this method for each of the four sub-triangles, with one less level
	// Note that the order in which the points are supplied is important to make
	// sure that the triangles are created in a consistent manner (clock-wise).
	/*"        RecursiveSubdivision(corner1, midPoint2, midPoint1, level);\n"
	"        RecursiveSubdivision(corner2, midPoint1, midPoint3, level);\n"
	"        RecursiveSubdivision(corner3, midPoint3, midPoint2, level);\n"
	"        RecursiveSubdivision(midPoint1, midPoint2, midPoint3, level);\n"*/
	"    }\n"
	"\n"
	// If level is less than 1, add the triangle to the scene instead of
	// continuing with the sub-division
	//"    if (level < 1)\n"
	//"    {\n"
	"        vec4 corner1Projected = projectionMatrix * modelviewMatrix * vec4(corner1, 1.0);\n"
	"        vec4 corner2Projected = projectionMatrix * modelviewMatrix * vec4(corner2, 1.0);\n"
	"        vec4 corner3Projected = projectionMatrix * modelviewMatrix * vec4(corner3, 1.0);\n"
	/*"        vec3 vector1 = vec3(corner2Projected - corner1Projected);\n"
	"        vec3 vector2 = vec3(corner3Projected - corner1Projected);\n"*/
	"        vec3 vector1 = vec3(corner2 - corner1);\n"
	"        vec3 vector2 = vec3(corner3 - corner1);\n"
	"        vec3 localNormal = normalMatrix * normalize(cross(vector1, vector2));\n"
	"\n"
	"        f.color = sphereInfo[0].color;\n"
	"        f.normal = localNormal;\n"
	"        f.position = vec3(corner1Projected);\n"
	"        gl_Position = corner1Projected;\n"
	"        EmitVertex();\n"
	"\n"
	"        f.color = sphereInfo[0].color;\n"
	"        f.normal = localNormal;\n"
	"        f.position = vec3(corner2Projected);\n"
	"        gl_Position = corner2Projected;\n"
	"        EmitVertex();\n"
	"\n"
	"        f.color = sphereInfo[0].color;\n"
	"        f.normal = localNormal;\n"
	"        f.position = vec3(corner3Projected);\n"
	"        gl_Position = corner3Projected;\n"
	"        EmitVertex();\n"
	"\n"
	"        EndPrimitive();\n"
	"        return;\n"
	//"    }\n"
	"}\n"
	"\n"
	"void main()\n"
	"{\n"
	/*"    if (resolution * sphereInfo[0].radius > 10000.0)\n"
	"    { return; }\n"
	"    vec4 center = gl_in[0].gl_Position;\n"
	"center.w = 0;\n"
	"float r = sphereInfo[0].radius;\n"
	"    vec4 p1 = modelviewMatrix * (vec4(r * vec3(10.0, 0.0, 0.0), 1.0) + center);\n"
	"    vec4 p2 = modelviewMatrix * (vec4(r * vec3(-5.0, 8.66, 0.0), 1.0) + center);\n"
	"    vec4 p3 = modelviewMatrix * (vec4(r * vec3(-5.0, -8.66, 0.0), 1.0) + center);\n"
	"    vec3 vector1 = vec3(p2 - p1);\n"
	"    vec3 vector2 = vec3(p3 - p1);\n"
	"    vec3 localNormal = normalMatrix * normalize(cross(vector1, vector2));\n"
	"    gl_Position = projectionMatrix * p1;\n"
	"    f.color = sphereInfo[0].color;\n"
	"    f.normal = localNormal;\n"
	"    f.position = vec3(p1);\n"
	"    EmitVertex();\n"
	"\n"
	"    gl_Position = projectionMatrix * p2;\n"
	"    f.color = sphereInfo[0].color;\n"
	"    f.normal = localNormal;\n"
	"    f.position = vec3(p2);\n"
	"    EmitVertex();\n"
	"\n"
	"    gl_Position = projectionMatrix * p3;\n"
	"    f.color = sphereInfo[0].color;\n"
	"    f.normal = localNormal;\n"
	"    f.position = vec3(p3);\n"
	"    EmitVertex();\n"
	"\n"
	"    EndPrimitive();\n"*/
	"    vec3 center = gl_in[0].gl_Position.xyz;\n"
	"    float t = (1.0 + sqrt(5.0)) / 2.0;\n"
	"    float s = sqrt(1.0 + t * t) * sphereInfo[0].radius;\n"
	"    vec3 vertex[12];\n"
	"    vertex[0] = vec3(t, 1.0, 0.0) / s + center;\n"
	"    vertex[1] = vec3(-t, 1.0, 0.0) / s + center;\n"
	"    vertex[2] = vec3(t, -1.0, 0.0) / s + center;\n"
	"    vertex[3] = vec3(-t, -1.0, 0.0) / s + center;\n"
	"    vertex[4] = vec3(1.0, 0.0, t) / s + center;\n"
	"    vertex[5] = vec3(1.0, 0.0, -t) / s + center;\n"
	"    vertex[6] = vec3(-1, 0.0, t) / s + center;\n"
	"    vertex[7] = vec3(-1.0, 0.0, -t) / s + center;\n"
	"    vertex[8] = vec3(0.0, t, 1.0) / s + center;\n"
	"    vertex[9] = vec3(0.0, -t, 1.0) / s + center;\n"
	"    vertex[10] = vec3(0.0, t, -1.0) / s + center;\n"
	"    vertex[11] = vec3(0.0, -t, -1.0) / s + center;\n"
	"\n"
	"    DoSubdivision(vertex[0], vertex[8], vertex[4]);\n"
	"    DoSubdivision(vertex[0], vertex[5], vertex[10]);\n"
	"    DoSubdivision(vertex[2], vertex[4], vertex[9]);\n"
	"    DoSubdivision(vertex[2], vertex[11], vertex[5]);\n"
	"    DoSubdivision(vertex[1], vertex[6], vertex[8]);\n"
	"\n"
	"    DoSubdivision(vertex[1], vertex[10], vertex[7]);\n"
	"    DoSubdivision(vertex[3], vertex[9], vertex[6]);\n"
	"    DoSubdivision(vertex[3], vertex[7], vertex[11]);\n"
	"    DoSubdivision(vertex[0], vertex[10], vertex[8]);\n"
	"    DoSubdivision(vertex[1], vertex[8], vertex[10]);\n"
	"\n"
	"    DoSubdivision(vertex[2], vertex[9], vertex[11]);\n"
	"    DoSubdivision(vertex[3], vertex[11], vertex[9]);\n"
	"    DoSubdivision(vertex[4], vertex[2], vertex[0]);\n"
	"    DoSubdivision(vertex[5], vertex[0], vertex[2]);\n"
	"    DoSubdivision(vertex[6], vertex[1], vertex[3]);\n"
	"\n"
	"    DoSubdivision(vertex[7], vertex[3], vertex[1]);\n"
	"    DoSubdivision(vertex[8], vertex[6], vertex[4]);\n"
	"    DoSubdivision(vertex[9], vertex[4], vertex[6]);\n"
	"    DoSubdivision(vertex[10], vertex[5], vertex[7]);\n"
	"    DoSubdivision(vertex[11], vertex[7], vertex[5]);\n"
	"}\n"
);

//==========================================================================
// Class:			Sphere
// Function:		Sphere
//
// Description:		Constructor for the Sphere class.
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
Sphere::Sphere(LibPlot2D::RenderWindow &renderWindow) : Primitive(renderWindow)
{
	mBufferInfo.resize(1);
}

//==========================================================================
// Class:			Sphere
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
void Sphere::GenerateGeometry()
{
	mRenderWindow.UseProgram(mProgram);

	if (mResolutionChanged)
	{
		glUniform1i(mRenderWindow.GetActiveProgramInfo().uniformLocations.find(mResolutionName)->second, resolution);
		mResolutionChanged = false;
	}

	glBindVertexArray(mBufferInfo[0].GetVertexArrayIndex());
	glDrawArrays(GL_POINTS, 0, mBufferInfo[0].vertexCount);
	glBindVertexArray(0);

	mRenderWindow.UseDefaultProgram();

	assert(!LibPlot2D::RenderWindow::GLHasError());
}

//==========================================================================
// Class:			Sphere
// Function:		DoGLInitialization
//
// Description:		Initialized required OpenGL objects.
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
void Sphere::DoGLInitialization()
{
	if (mInitialized)
		return;

	std::vector<GLuint> shaderList;
	shaderList.push_back(mRenderWindow.CreateShader(GL_VERTEX_SHADER, mPassThroughVertexShader));
	//shaderList.push_back(mRenderWindow.CreateDefaultVertexShader());
	shaderList.push_back(mRenderWindow.CreateShader(GL_GEOMETRY_SHADER, mSphereGeometryShader));
	shaderList.push_back(mRenderWindow.CreateDefaultFragmentShader());

	LibPlot2D::RenderWindow::ShaderInfo s;
	s.programId = mRenderWindow.CreateProgram(shaderList);
	mRenderWindow.AssignDefaultLocations(s);
	s.uniformLocations[mResolutionName] = glGetUniformLocation(s.programId, mResolutionName.c_str());
	s.attributeLocations[mRadiusName] = glGetAttribLocation(s.programId, mRadiusName.c_str());
	mProgram = mRenderWindow.AddShader(s);

	mInitialized = true;
}

//==========================================================================
// Class:			Sphere
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
bool Sphere::HasValidParameters()
{
	// Spheres must have a positive radius
	if (radius > 0.0)
		return true;

	// Otherwise return false
	return false;
}

//==========================================================================
// Class:			Sphere
// Function:		SetResolution
//
// Description:		Sets the number of faces use to approximate the sphere.
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
void Sphere::SetResolution(const int &resolution)
{
	this->resolution = resolution;
	mModified = true;
	mResolutionChanged = true;
}

//==========================================================================
// Class:			Sphere
// Function:		SetCenter
//
// Description:		Sets the location of the center of the sphere.
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
void Sphere::SetCenter(const Eigen::Vector3d &center)
{
	this->center = center;
	mModified = true;
}

//==========================================================================
// Class:			Sphere
// Function:		SetRadius
//
// Description:		Sets the radius at the base of the sphere.
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
void Sphere::SetRadius(const double &radius)
{
	this->radius = radius;
	mModified = true;
}

//==========================================================================
// Class:			Sphere
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
bool Sphere::IsIntersectedBy(const Eigen::Vector3d& point, const Eigen::Vector3d& direction) const
{
	assert(VVASE::Math::IsZero(1.0 - direction.norm()));

	if (pow((point - center).norm(), 2) > pow(direction.dot(point - center), 2) + radius * radius)
		return false;

	return true;
}

//==========================================================================
// Class:			Sphere
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
void Sphere::Update(const unsigned int& /*i*/)
{
	DoGLInitialization();

	if (resolution < 0)// Don't need to update mResolutionChanged here because it will already have been changed
		resolution = 0;
	else if (resolution > 2)
		resolution = 2;

	mBufferInfo[0].GetOpenGLIndices(false);

	mBufferInfo[0].vertexCount = 1;
	mBufferInfo[0].vertexBuffer.resize(mBufferInfo[0].vertexCount
		* (mRenderWindow.GetVertexDimension() + 5));// 4D vertex, RGBA color, radius
	assert(mRenderWindow.GetVertexDimension() == 4);

	mBufferInfo[0].vertexBuffer[0] = static_cast<float>(center.x());
	mBufferInfo[0].vertexBuffer[1] = static_cast<float>(center.y());
	mBufferInfo[0].vertexBuffer[2] = static_cast<float>(center.z());
	mBufferInfo[0].vertexBuffer[3] = 1.0f;

	mBufferInfo[0].vertexBuffer[4] = static_cast<float>(mColor.GetRed());
	mBufferInfo[0].vertexBuffer[5] = static_cast<float>(mColor.GetGreen());
	mBufferInfo[0].vertexBuffer[6] = static_cast<float>(mColor.GetBlue());
	mBufferInfo[0].vertexBuffer[7] = static_cast<float>(mColor.GetAlpha());

	mBufferInfo[0].vertexBuffer[8] = static_cast<float>(radius);

	glBindVertexArray(mBufferInfo[0].GetVertexArrayIndex());

	glBindBuffer(GL_ARRAY_BUFFER, mBufferInfo[0].GetVertexBufferIndex());
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * mBufferInfo[0].vertexBuffer.size(),
		mBufferInfo[0].vertexBuffer.data(), GL_DYNAMIC_DRAW);

	const auto& attributeMap(mRenderWindow.GetProgramInfo(mProgram).attributeLocations);
	glEnableVertexAttribArray(attributeMap.find(LibPlot2D::RenderWindow::mPositionName)->second);
	glVertexAttribPointer(attributeMap.find(LibPlot2D::RenderWindow::mPositionName)->second, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(attributeMap.find(LibPlot2D::RenderWindow::mColorName)->second);
	glVertexAttribPointer(attributeMap.find(LibPlot2D::RenderWindow::mColorName)->second, 4, GL_FLOAT, GL_FALSE, 0,
		(void*)(sizeof(GLfloat) * mRenderWindow.GetVertexDimension() * mBufferInfo[0].vertexCount));

	glEnableVertexAttribArray(attributeMap.find(mRadiusName)->second);
	glVertexAttribPointer(attributeMap.find(mRadiusName)->second, 1, GL_FLOAT, GL_FALSE, 0,
		(void*)(sizeof(GLfloat) * (mRenderWindow.GetVertexDimension() + 4) * mBufferInfo[0].vertexCount));

	glBindVertexArray(0);

	assert(!LibPlot2D::RenderWindow::GLHasError());

	mBufferInfo[0].vertexCountModified = false;
}

}// namespace VVASE
