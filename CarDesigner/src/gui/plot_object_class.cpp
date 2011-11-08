/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  plot_object_class.cpp
// Created:  1/22/2009
// Author:  K. Loux
// Description:  Intermediate class for creating plots from arrays of data.
// History:
//	2/22/2009	- Fixed bug in Update() that overwrote data when plotting the same
//				  parameter for multiple car sources, K. Loux.
//	5/23/2009	- Re-wrote to remove VTK dependencies, K. Loux.
//	10/27/2010	- Added IsZero() to check for very small numbers - improved stability
//				  when attempting to create plots with range of zero, K. Loux.
//	11/9/2010	- Renamed from PLOT2D, K. Loux.
//	11/16/2010	- Fixed bug in auto-limit code (incorrect limit determination in
//				  special cases), K. Loux.

// CarDesigner headers
#include "gui/plot_object_class.h"
#include "gui/iteration_class.h"
#include "gui/components/main_frame_class.h"
#include "gui/renderer/plot_renderer_class.h"
#include "vUtilities/debug_class.h"
#include "vUtilities/convert_class.h"
#include "vRenderer/primitives/plot_curve.h"
#include "vRenderer/primitives/axis.h"
#include "vRenderer/primitives/text_class.h"
#include "vMath/car_math.h"

// FTGL headers
#include <FTGL/ftgl.h>

//==========================================================================
// Class:			PLOT_OBJECT
// Function:		PLOT_OBJECT
//
// Description:		Constructor for PLOT_OBJECT class.
//
// Input Arguments:
//		_Renderer	= PLOT_RENDERER& reference to the object that handles the
//					  drawing operations
//		_DataSource	= const ITERATION& reference to the associated iteration object
//		_debugger	= const Debugger& reference to the application's debug printing
//					  utility
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
PLOT_OBJECT::PLOT_OBJECT(PLOT_RENDERER &_Renderer, const ITERATION &_DataSource,
						 const Debugger &_debugger) : Debugger(_debugger),
						 DataSource(_DataSource), Renderer(_Renderer)
{
	// Create the actors
	AxisTop = new AXIS(Renderer);
	AxisBottom = new AXIS(Renderer);
	AxisLeft = new AXIS(Renderer);
	AxisRight = new AXIS(Renderer);
	TitleObject = new TEXT_RENDERING(Renderer);

	// Find the location of the fonts directory
	wxString FontFile;
#ifdef __WXMSW__
	FontFile = wxGetOSDirectory() + _T("\\fonts\\arial.ttf");
#elif defined __WXGTK__
	// FIXME:  This probably isn't very portable...
	FontFile = _T("/usr/share/fonts/dejavu/DejaVuSans.ttf");
#else
	// Unknown platfrom - warn the user
#	warning "Unrecognized platform - unable to locate font files!"
	FontFile = wxEmptyString;
#endif

	// Create the fonts
	AxisFont = new FTGLTextureFont(FontFile.c_str());
	TitleFont = new FTGLTextureFont(FontFile.c_str());

	// Make sure the fonts loaded OK
	if (AxisFont->Error() || TitleFont->Error())
	{
		delete AxisFont;
		AxisFont = NULL;

		delete TitleFont;
		TitleFont = NULL;

		// Warn the user as well
		Debugger.Print(_T("Warning:  Could not load font file!"), Debugger::PriorityLow);
	}
	else
	{
		AxisFont->FaceSize(12);
		AxisFont->CharMap(FT_ENCODING_UNICODE);

		TitleFont->FaceSize(18);
		TitleFont->CharMap(FT_ENCODING_UNICODE);
	}

	// Initialize auto-scaling to true
	ResetAutoScaling();
}

//==========================================================================
// Class:			PLOT_OBJECT
// Function:		~PLOT_OBJECT
//
// Description:		Destructor for PLOT_OBJECT class.
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
PLOT_OBJECT::~PLOT_OBJECT()
{
	// Delete the font objects
	delete AxisFont;
	AxisFont = NULL;
	delete TitleFont;
	TitleFont = NULL;
}

//==========================================================================
// Class:			PLOT_OBJECT
// Function:		Update
//
// Description:		Updates the data in the plot and re-sets the fonts, sizes
//					and positions.
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
void PLOT_OBJECT::Update(void)
{
	// Clear out the existing plots
	RemoveExistingPlots();

	// Clear the Z label variable
	if (DataSource.GetAutoGenerateZLabel())
		ZLabel = wxEmptyString;
	else
		ZLabel = DataSource.GetZLabel();

	// Make sure we know what kind of data we're dealing with
	int i;

	// Assign a local pointer to ease readability
	int CurrentCar;

	// Initialize the list plots
	for (i = 0; i < ITERATION::NumberOfPlots; i++)
	{
		// See if this plot is active
		if (DataSource.GetActivePlot((ITERATION::PLOT_ID)i))
		{
			// Set up the z-axis label
			// If units for all plots are the same, use the same units
			// If the curves are the same property but for different corners, put the
			// property name as the label with the corner in the legend
			// FIXME:  Still need a legend very badly!
			if (DataSource.GetAutoGenerateZLabel())
			{
				if (ZLabel.IsEmpty())
					ZLabel = DataSource.GetPlotName((ITERATION::PLOT_ID)i) + " [" +
						DataSource.GetPlotUnits((ITERATION::PLOT_ID)i) + "]";
				else
					ZLabel = _T("Multiple Variables");
			}

			// Do this once for each active car
			for (CurrentCar = 0; CurrentCar < DataSource.GetAssociatedCarCount(); CurrentCar++)
			{
				// Create the objects
				PLOT_CURVE *NewPlot = new PLOT_CURVE(Renderer);

				// Bind the new curve to the left hand axis (FIXME:  Allow more options with better logic)
				NewPlot->BindToXAxis(AxisBottom);
				NewPlot->BindToZAxis(AxisLeft);

				// Add the objects to the lists
				PlotList.Add(NewPlot);
			}
		}
	}

	// Add elements to the X-axis and Z-axis data
	int CurrentPoint;
	double XData(0.0), ZData;
	for (CurrentPoint = 0; CurrentPoint < DataSource.GetNumberOfPoints(); CurrentPoint++)
	{
		// Determine the X data based on the currently selected X-axis type
		// We can hard-code the zero here instead of CurrentCar because the X-axis for
		// all cars is the same.
		switch (DataSource.GetXAxisType())
		{
		case ITERATION::AxisTypePitch:
			XData = DataSource.GetDataValue(0, CurrentPoint, ITERATION::Pitch);
			break;

		case ITERATION::AxisTypeRoll:
			XData = DataSource.GetDataValue(0, CurrentPoint, ITERATION::Roll);
			break;

		case ITERATION::AxisTypeHeave:
			XData = DataSource.GetDataValue(0, CurrentPoint, ITERATION::Heave);
			break;

		case ITERATION::AxisTypeRackTravel:
			XData = DataSource.GetDataValue(0, CurrentPoint, ITERATION::RackTravel);
			break;

		case ITERATION::AxisTypeUnused:
			// No plots will be generated, but we need to continue so we can draw an empty set of axes
			break;

		default:
			assert(0);
			break;
		}

		// Initialize the axis limits, if necessary
		if (CurrentPoint == 0)
		{
			XMinOriginal = XData;
			XMaxOriginal = XData;
		}
		// Check to see if this X is the new X min or max
		else if (XData < XMinOriginal)
			XMinOriginal = XData;
		else if (XData > XMaxOriginal)
			XMaxOriginal = XData;

		// Add the data for each plot
		// This bit can be slightly confusing:
		//    CurrentPlot refers to the actual curves being plotted (CurrentPlot is always
		//      less than PlotList.GetCount())
		//    i refers to the list of possible plots that we can have (it is always less
		//      than ITERATION::NumberOfPlots)
		int CurrentPlot = 0;
		for (i = 0; i < ITERATION::NumberOfPlots; i++)
		{
			// See if the plot with this object is active
			if (DataSource.GetActivePlot((ITERATION::PLOT_ID)i))
			{
				// Do this once for each active car
				for (CurrentCar = 0; CurrentCar < DataSource.GetAssociatedCarCount(); CurrentCar++)
				{
					// Get the Z data
					ZData = DataSource.GetDataValue(CurrentCar, CurrentPoint, (ITERATION::PLOT_ID)i);

					// Initialize the Z mins and maxes, if necessary
					if (CurrentPlot == 0 && CurrentPoint == 0)
					{
						ZMinOriginal = ZData;
						ZMaxOriginal = ZData;
					}
					// Check to see if the Z data is a new max or min
					else if (ZData < ZMinOriginal)
						ZMinOriginal = ZData;
					else if (ZData > ZMaxOriginal)
						ZMaxOriginal = ZData;

					// Perform the adding of the data
					PlotList[CurrentPlot]->AddPoint(XData, ZData);

					// Increment the current plot counter
					CurrentPlot++;
				}
			}
		}
	}

	// Format the plot
	FormatPlot();

	// Render the plot
	Renderer.Render();

	return;
}

//==========================================================================
// Class:			PLOT_OBJECT
// Function:		RemoveExistingPlots
//
// Description:		Deletes the existing plots.
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
void PLOT_OBJECT::RemoveExistingPlots(void)
{
	// Remove all existing plots from the list
	while (PlotList.GetCount() > 0)
	{
		// Remove the object from the renderer object
		Renderer.RemoveActor(PlotList[0]);

		// Remove it from the local list
		PlotList.Remove(0);
	}
}

//==========================================================================
// Class:			PLOT_OBJECT
// Function:		FormatPlot
//
// Description:		Formats the plot actor.
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
void PLOT_OBJECT::FormatPlot(void)
{
	// Set the background color
	Renderer.SetBackgroundColor(COLOR::ColorWhite);

	// If the axes mins and maxes are equal, stretch the range to make the plot display
	// We use IsZero to check for equality to avoid problems with very small numbers
	if (VVASEMath::IsZero(XMaxOriginal - XMinOriginal))
	{
		if (VVASEMath::IsZero(XMinOriginal))
		{
			XMinOriginal = -1.0;
			XMaxOriginal = 1.0;
		}
		else
		{
			XMinOriginal -= XMinOriginal * 0.1;
			XMaxOriginal += XMaxOriginal * 0.1;
		}
	}

	if (VVASEMath::IsZero(YMaxOriginal - YMinOriginal))
	{
		if (VVASEMath::IsZero(YMinOriginal))
		{
			YMinOriginal = -1.0;
			YMaxOriginal = 1.0;
		}
		else
		{
			YMinOriginal -= YMinOriginal * 0.1;
			YMaxOriginal += YMaxOriginal * 0.1;
		}
	}

	if (VVASEMath::IsZero(ZMaxOriginal - ZMinOriginal))
	{
		if (VVASEMath::IsZero(ZMinOriginal))
		{
			ZMinOriginal = -1.0;
			ZMaxOriginal = 1.0;
		}
		else
		{
			ZMinOriginal -= ZMinOriginal * 0.1;
			ZMaxOriginal += ZMaxOriginal * 0.1;
		}
	}

	// If we want to auto-scale the plot, set the range limits to the original values
	if (AutoScaleX)
	{
		XMin = XMinOriginal;
		XMax = XMaxOriginal;
	}

	if (AutoScaleY)
	{
		YMin = YMinOriginal;
		YMax = YMaxOriginal;
	}

	if (AutoScaleZ)
	{
		ZMin = ZMinOriginal;
		ZMax = ZMaxOriginal;
	}

	// Set up the axes resolution (and at the same time tweak the max and min)
	// FIXME:  Make maximum number of ticks dependant on plot size and width of
	// number (i.e. 1 2 3 fits better than 0.001 0.002 0.003)
	double XMajorResolution = AutoScaleAxis(XMin, XMax, 7, !AutoScaleX);
	double XMinorResolution = XMajorResolution;
	double YMajorResolution = AutoScaleAxis(YMin, YMax, 10, !AutoScaleY);
	double YMinorResolution = YMajorResolution;
	double ZMajorResolution = AutoScaleAxis(ZMin, ZMax, 10, !AutoScaleZ);
	double ZMinorResolution = ZMajorResolution;

	// Make sure the auto-scaled values are numbers
	// If they're not numbers, set them to +/- 1 and recalculate the tick spacing
	// (with inputs of +/- 1, they will always give valid results)
	if (VVASEMath::IsNaN(XMin) || VVASEMath::IsNaN(XMax))
	{
		XMin = -1.0;
		XMax = 1.0;
		XMajorResolution = AutoScaleAxis(XMin, XMax, 7, !AutoScaleX);
		XMinorResolution = XMajorResolution;
	}

	if (VVASEMath::IsNaN(YMin) || VVASEMath::IsNaN(YMax))
	{
		YMin = -1.0;
		YMax = 1.0;
		YMajorResolution = AutoScaleAxis(YMin, YMax, 7, !AutoScaleY);
		YMinorResolution = YMajorResolution;
	}

	if (VVASEMath::IsNaN(ZMin) || VVASEMath::IsNaN(ZMax))
	{
		ZMin = -1.0;
		ZMax = 1.0;
		ZMajorResolution = AutoScaleAxis(ZMin, ZMax, 7, !AutoScaleZ);
		ZMinorResolution = ZMajorResolution;
	}

	// If we're auto-scaling, update the "original values" because chances are they
	// have been tweaked to make the number prettier
	if (AutoScaleX)
	{
		XMinOriginal = XMin;
		XMaxOriginal = XMax;
	}

	if (AutoScaleY)
	{
		YMinOriginal = YMin;
		YMaxOriginal = YMax;
	}

	if (AutoScaleZ)
	{
		ZMinOriginal = ZMin;
		ZMaxOriginal = ZMax;
	}

	// Apply the desired properties to each axis
	AxisBottom->SetOrientation(AXIS::OrientationBottom);
	AxisBottom->SetMinimum(XMin);
	AxisBottom->SetMaximum(XMax);
	AxisBottom->SetMinorResolution(XMinorResolution);
	AxisBottom->SetMajorResolution(XMajorResolution);
	AxisBottom->SetFont(AxisFont);
	AxisBottom->SetTickStyle(AXIS::TickStyleInside);
	AxisBottom->SetGrid(DataSource.GetShowGridLines());

	AxisLeft->SetOrientation(AXIS::OrientationLeft);
	AxisLeft->SetMinimum(ZMin);
	AxisLeft->SetMaximum(ZMax);
	AxisLeft->SetMinorResolution(ZMinorResolution);
	AxisLeft->SetMajorResolution(ZMajorResolution);
	AxisLeft->SetFont(AxisFont);
	AxisLeft->SetLabel(ZLabel);
	AxisLeft->SetTickStyle(AXIS::TickStyleInside);
	AxisLeft->SetGrid(DataSource.GetShowGridLines());

	AxisTop->SetOrientation(AXIS::OrientationTop);
	AxisTop->SetMinimum(XMin);
	AxisTop->SetMaximum(XMax);
	AxisTop->SetMinorResolution(XMinorResolution);
	AxisTop->SetMajorResolution(XMajorResolution);
	AxisTop->SetTickStyle(AXIS::TickStyleInside);

	AxisRight->SetOrientation(AXIS::OrientationRight);
	AxisRight->SetMinimum(ZMin);
	AxisRight->SetMaximum(ZMax);
	AxisRight->SetMinorResolution(ZMinorResolution);
	AxisRight->SetMajorResolution(ZMajorResolution);
	AxisRight->SetTickStyle(AXIS::TickStyleInside);

	// Set the title properties
	TitleObject->SetFont(TitleFont);
	TitleObject->SetCentered(true);
	if (DataSource.GetAutoGenerateTitle())
		// Use file name
		TitleObject->SetText(DataSource.GetCleanName());
	else
		// Use user-specified name
		TitleObject->SetText(DataSource.GetTitle());
	TitleObject->SetPosition(Renderer.GetSize().GetWidth() / 2.0,
		Renderer.GetSize().GetHeight() - 75.0 / 2.0);// 75.0 is from OffsetFromWindowEdge in axis.cpp

	// Go through all of the curves and assign them different colors
	int i;
	COLOR Color;
	for (i = 0; i < PlotList.GetCount(); i++)
	{
		// Select the plot color
		// FIXME:  Choose the color with a better method!!!
		if (i == 0)
			Color.Set(1.0, 0.0, 0.0);
		else if (i == 1)
			Color.Set(0.0, 1.0, 0.0);
		else if (i == 2)
			Color.Set(0.0, 0.0, 1.0);
		else if (i == 3)
			Color.Set(1.0, 0.0, 1.0);
		else if (i == 4)
			Color.Set(0.0, 1.0, 1.0);
		else
			Color.Set(0.0, 0.0, 0.0);

		// Assign the color to the plot
		PlotList[i]->SetColor(Color);
	}

	// Add the X axis label
	wxString AxisLabel("ERROR");
	if (DataSource.GetAutoGenerateXLabel())
	{
		switch (DataSource.GetXAxisType())
		{
		case ITERATION::AxisTypePitch:
			AxisLabel.Printf("Pitch [%s]", DataSource.GetMainFrame().
				GetConverter().GetUnitType(Convert::UNIT_TYPE_ANGLE).c_str());
			break;

		case ITERATION::AxisTypeRoll:
			AxisLabel.Printf("Roll [%s]", DataSource.GetMainFrame().
				GetConverter().GetUnitType(Convert::UNIT_TYPE_ANGLE).c_str());
			break;

		case ITERATION::AxisTypeHeave:
			AxisLabel.Printf("Heave [%s]", DataSource.GetMainFrame().
				GetConverter().GetUnitType(Convert::UNIT_TYPE_DISTANCE).c_str());
			break;

		case ITERATION::AxisTypeRackTravel:
			AxisLabel.Printf("Rack Travel [%s]", DataSource.GetMainFrame().
				GetConverter().GetUnitType(Convert::UNIT_TYPE_DISTANCE).c_str());
			break;

		case ITERATION::AxisTypeUnused:
			// Don't print an X-axis label
			AxisLabel = wxEmptyString;
			break;

		default:
			assert(0);
			break;
		}
	}
	else
		AxisLabel = DataSource.GetXLabel();

	// Assign the string to the label
	AxisBottom->SetLabel(AxisLabel.c_str());

	// Set the axis colors
	Color.Set(0.0, 0.0, 0.0);
	AxisBottom->SetColor(Color);
	AxisTop->SetColor(Color);
	AxisLeft->SetColor(Color);
	AxisRight->SetColor(Color);

	return;
}

//==========================================================================
// Class:			PLOT_OBJECT
// Function:		AutoScaleAxis
//
// Description:		Calculates the number of tick marks to use to span the range
//					and make the plot look nice.
//
// Input Arguments:
//		Min			= double& specifying the minimum value for the axis (required for input and output)
//		Max			= double& specifying the maximum value for the axis (required for input and output)
//		MaxTicks	= int specifying the maximum number of ticks to use
//		ForceLimits	= const bool& specifying whether or not to preserve the specified limits
//
// Output Arguments:
//		Min			= double& specifying the minimum value for the axis (required for input and output)
//		Max			= double& specifying the maximum value for the axis (required for input and output)
//
// Return Value:
//		double, spacing between each tick mark for the axis (MajorResolution)
//
//==========================================================================
double PLOT_OBJECT::AutoScaleAxis(double &Min, double &Max, int MaxTicks, const bool &ForceLimits)
{
	// Get the order of magnitude of the axes to decide how to scale them
	double Range = Max - Min;
	int OrderOfMagnitude = (int)log10(Range);
	double TickSpacing = Range / MaxTicks;

	// Acceptable resolution steps are:
	//	Ones,
	//	Twos (even numbers), and
	//	Fives (multiples of five),
	// each within the order of magnitude (i.e. [37, 38, 39], [8.5, 9.0, 9.5], and [20, 40, 60] are all acceptable)

	// Determine which method will result in the least whitespace before and after the actual range,
	// and will get us closest to the maximum number of ticks.

	// Scale the tick spacing so it is between 0.1 and 10.0
	double ScaledSpacing = TickSpacing / pow(10.0, OrderOfMagnitude - 1);

	// Choose the maximum spacing value that fits our criteria
	if (ScaledSpacing > 5.0)
		ScaledSpacing = 10.0;
	else if (ScaledSpacing > 2.0)
		ScaledSpacing = 5.0;
	else if (ScaledSpacing > 1.0)
		ScaledSpacing = 2.0;
	else if (ScaledSpacing > 0.5)
		ScaledSpacing = 1.0;
	else if (ScaledSpacing > 0.2)
		ScaledSpacing = 0.5;
	else if (ScaledSpacing > 0.1)
		ScaledSpacing = 0.2;
	else
		ScaledSpacing = 0.1;

	// Re-scale back to the correct order of magnitude
	TickSpacing = ScaledSpacing * pow(10.0, OrderOfMagnitude - 1);

	// Round the min and max down and up, respectively, so the plot fits within the range [Min Max]
	if (!ForceLimits)
	{
		if (fmod(Min, TickSpacing) != 0)
		{
			if (Min < 0)
			{
				Min -= fmod(Min, TickSpacing);
				Min -= TickSpacing;
			}
			else
				Min -= fmod(Min, TickSpacing);
		}
		if (fmod(Max, TickSpacing) != 0)
		{
			if (Max > 0)
			{
				Max -= fmod(Max, TickSpacing);
				Max += TickSpacing;
			}
			else
				Max -= fmod(Max, TickSpacing);
		}
	}

	// If numerical processing leads to ugly numbers, clean them up a bit
	if (VVASEMath::IsZero(Min))
		Min = 0.0;
	if (VVASEMath::IsZero(Max))
		Max = 0.0;

	return TickSpacing;
}

//==========================================================================
// Class:			PLOT_OBJECT
// Function:		SetXMin
//
// Description:		Sets the lower X limit.
//
// Input Arguments:
//		_XMin	= const double& describing desired minimum X limit
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PLOT_OBJECT::SetXMin(const double &_XMin)
{
	// Make sure the limit is within the bounds of the original
	if (_XMin <= XMinOriginal)
	{
		XMin = XMinOriginal;

		// If the opposite limit is also at the original value, enable auto-scaling again
		if (XMax == XMaxOriginal)
			AutoScaleX = true;

		return;
	}

	// Make the assignment and disable auto-scaling
	XMin = _XMin;
	AutoScaleX = false;

	return;
}

//==========================================================================
// Class:			PLOT_OBJECT
// Function:		SetXMax
//
// Description:		Sets the upper X limit.
//
// Input Arguments:
//		_XMax	= const double& describing desired maximum X limit
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PLOT_OBJECT::SetXMax(const double &_XMax)
{
	// Make sure the limit is within the bounds of the original
	if (_XMax >= XMaxOriginal)
	{
		XMax = XMaxOriginal;

		// If the opposite limit is also at the original value, enable auto-scaling again
		if (XMin == XMinOriginal)
			AutoScaleX = true;

		return;
	}

	// Make the assignment and disable auto-scaling
	XMax = _XMax;
	AutoScaleX = false;

	return;
}

//==========================================================================
// Class:			PLOT_OBJECT
// Function:		SetYMin
//
// Description:		Sets the lower Y limit.
//
// Input Arguments:
//		_YMin	= const double& describing desired minimum Y limit
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PLOT_OBJECT::SetYMin(const double &_YMin)
{
	// Make sure the limit is within the bounds of the original
	if (_YMin <= YMinOriginal)
	{
		YMin = YMinOriginal;

		// If the opposite limit is also at the original value, enable auto-scaling again
		if (YMax == YMaxOriginal)
			AutoScaleY = true;

		return;
	}

	// Make the assignment and disable auto-scaling
	YMin = _YMin;
	AutoScaleY = false;

	return;
}

//==========================================================================
// Class:			PLOT_OBJECT
// Function:		SetYMax
//
// Description:		Sets the upper Y limit.
//
// Input Arguments:
//		_YMax	= const double& describing desired maximum Y limit
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PLOT_OBJECT::SetYMax(const double &_YMax)
{
	// Make sure the limit is within the bounds of the original
	if (_YMax >= YMaxOriginal)
	{
		YMax = YMaxOriginal;

		// If the opposite limit is also at the original value, enable auto-scaling again
		if (YMin == YMinOriginal)
			AutoScaleY = true;

		return;
	}

	// Make the assignment and disable auto-scaling
	YMax = _YMax;
	AutoScaleY = false;

	return;
}

//==========================================================================
// Class:			PLOT_OBJECT
// Function:		SetZMin
//
// Description:		Sets the lower Z limit.
//
// Input Arguments:
//		_ZMin	= const double& describing desired minimum Z limit
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PLOT_OBJECT::SetZMin(const double &_ZMin)
{
	// Make sure the limit is within the bounds of the original
	if (_ZMin <= ZMinOriginal)
	{
		ZMin = ZMinOriginal;

		// If the opposite limit is also at the original value, enable auto-scaling again
		if (ZMax == ZMaxOriginal)
			AutoScaleZ = true;

		return;
	}

	// Make the assignment and disable auto-scaling
	ZMin = _ZMin;
	AutoScaleZ = false;

	return;
}

//==========================================================================
// Class:			PLOT_OBJECT
// Function:		SetZMax
//
// Description:		Sets the upper Z limit.
//
// Input Arguments:
//		_ZMax	= const double& describing desired maximum Z limit
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PLOT_OBJECT::SetZMax(const double &_ZMax)
{
	// Make sure the limit is within the bounds of the original
	if (_ZMax >= ZMaxOriginal)
	{
		ZMax = ZMaxOriginal;

		// If the opposite limit is also at the original value, enable auto-scaling again
		if (ZMin == ZMinOriginal)
			AutoScaleZ = true;

		return;
	}

	// Make the assignment and disable auto-scaling
	ZMax = _ZMax;
	AutoScaleZ = false;

	return;
}

//==========================================================================
// Class:			PLOT_OBJECT
// Function:		ResetAutoScaling
//
// Description:		Resets auto-scaling for all axes.
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
void PLOT_OBJECT::ResetAutoScaling(void)
{
	// Enable auto-scaling for all axes
	AutoScaleX = true;
	AutoScaleY = true;
	AutoScaleZ = true;

	return;
}