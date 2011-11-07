/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  plot_curve.h
// Created:  5/23/2009
// Author:  K. Loux
// Description:  Derived from PRIMITIVE for creating plot curves objects.
// History:
//	11/9/2010	- Modified to accomodate 3D plots, K. Loux.

#ifndef _PLOT_CURVE_H_
#define _PLOT_CURVE_H_

// VVASE headers
#include "vRenderer/primitives/primitive.h"
#include "vUtilities/managed_list_class.h"

// VVASE forward declarations
class AXIS;

class PLOT_CURVE : public PRIMITIVE
{
public:
	// Constructor
	PLOT_CURVE(RENDER_WINDOW &_RenderWindow);
	PLOT_CURVE(const PLOT_CURVE &PlotCurve);

	// Destructor
	~PLOT_CURVE();

	// The structure containg the information for each point
	struct XYZPOINT
	{
		double X, Y, Z;
	};

	// Mandatory overloads from PRIMITIVE - for creating geometry and testing the
	// validity of this object's parameters
	void GenerateGeometry(void);
	bool HasValidParameters(void);

	// Remove all data from the plot
	void ClearData(void) { PlotData.Clear(); };

	// For setting up the plot
	void BindToXAxis(const AXIS *_XAxis) { XAxis = _XAxis; Modified = true; };
	void BindToYAxis(const AXIS *_YAxis) { YAxis = _YAxis; Modified = true; };
	void BindToZAxis(const AXIS *_ZAxis) { ZAxis = _ZAxis; Modified = true; };
	void AddPoint(const double &X, const double &Y);
	void AddPoint(const double &X, const double &Y, const double &Z);

	// Overloaded operators
	PLOT_CURVE& operator = (const PLOT_CURVE &PlotCurve);

private:
	// The axis with which this object is associated
	const AXIS *XAxis;
	const AXIS *YAxis;
	const AXIS *ZAxis;

	// For use when the axis scale changes
	void RescalePoint(const XYZPOINT *XYZPoint, int *Point);

	// For appearance
	static const int OffsetFromWindowEdge;

	// The data to be plotted
	MANAGED_LIST<XYZPOINT> PlotData;
};

#endif// _PLOT_CURVE_H_