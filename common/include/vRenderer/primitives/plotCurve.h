/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  plotCurve.h
// Created:  5/2/2011
// Author:  K. Loux
// Description:  Derived from Primitive for creating plot curves objects.
// History:

#ifndef PLOT_CURVE_H_
#define PLOT_CURVE_H_

// Local headers
#include "vRenderer/primitives/primitive.h"
#include "vUtilities/managedList.h"

// Local forward declarations
class Axis;
class Dataset2D;

class PlotCurve : public Primitive
{
public:
	PlotCurve(RenderWindow &renderWindow);
	PlotCurve(const PlotCurve &plotCurve);

	~PlotCurve();

	// Mandatory overloads from Primitive - for creating geometry and testing the
	// validity of this object's parameters
	void GenerateGeometry();
	bool HasValidParameters();

	void SetModified() { modified = true; }

	void SetSize(const unsigned int &size) { this->size = size; modified = true; }

	// Remove all data from the plot
	void SetData(const Dataset2D *data);
	void ClearData() { data = NULL; }

	// For setting up the plot
	void BindToXAxis(Axis *xAxis) { this->xAxis = xAxis; modified = true; }
	void BindToYAxis(Axis *yAxis) { this->yAxis = yAxis; modified = true; }

	Axis *GetYAxis() { return yAxis; }

	// Gets the Y-value that corresponds to the specified X value
	bool GetYAt(double &value);

	// Overloaded operators
	PlotCurve& operator=(const PlotCurve &plotCurve);

private:
	// The axes with which this object is associated
	Axis *xAxis;
	Axis *yAxis;

	unsigned int size;

	// For use when the axis scale changes
	void RescalePoint(const double *xyPoint, int *point);

	const Dataset2D *data;
};

#endif// PLOT_CURVE_H_