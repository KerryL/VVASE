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
	explicit Cone(LibPlot2D::RenderWindow &renderWindow);

	// Mandatory overloads from Primitive - for creating geometry and testing the
	// validity of this object's parameters
	bool IsIntersectedBy(const Eigen::Vector3d& point, const Eigen::Vector3d& direction) const;

	// Private data accessors
	void SetResolution(const int &resolution);
	void SetCapping(const bool &drawCaps);
	void SetTip(const Eigen::Vector3d &tip);
	void SetBaseCenter(const Eigen::Vector3d &baseCenter);
	void SetRadius(const double &radius);

protected:
	// Mandatory overloads from Primitive - for creating geometry and testing the
	// validity of this object's parameters
	bool HasValidParameters() override;
	void GenerateGeometry() override;
	void Update(const unsigned int& i) override;

private:
	bool drawCaps = false;

	int resolution = 4;

	Eigen::Vector3d tip = Eigen::Vector3d::Zero();
	Eigen::Vector3d baseCenter = Eigen::Vector3d::Zero();

	double radius = 0.0;
};

}// namespace VVASE

#endif// CONE_H_
