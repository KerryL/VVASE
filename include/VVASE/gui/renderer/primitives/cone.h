/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  cone.h
// Date:  5/14/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Derived from Primitive for creating conical objects.

#ifndef CONE_H_
#define CONE_H_

// LibPlot2D headers
#include <lp2d/renderer/primitives/primitive.h>

// Eigen headers
#include <Eigen/Eigen>

namespace VVASE
{

class Cone : public LibPlot2D::Primitive
{
public:
	Cone(RenderWindow &renderWindow);
	~Cone();

	// Mandatory overloads from Primitive - for creating geometry and testing the
	// validity of this object's parameters
	void GenerateGeometry();
	bool HasValidParameters();
	bool IsIntersectedBy(const Eigen::Vector3d& point, const Eigen::Vector3d& direction) const;

	// Private data accessors
	void SetResolution(const int &resolution);
	void SetCapping(const bool &drawCaps);
	void SetTip(const Eigen::Vector3d &tip);
	void SetBaseCenter(const Eigen::Vector3d &baseCenter);
	void SetRadius(const double &radius);

private:
	bool drawCaps;

	int resolution;

	Eigen::Vector3d tip;
	Eigen::Vector3d baseCenter;

	double radius;
};

}// namespace VVASE

#endif// CONE_H_
