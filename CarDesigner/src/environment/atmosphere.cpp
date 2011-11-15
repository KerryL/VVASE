/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  atmosphere.cpp
// Created:  12/12/2008
// Author:  K. Loux
// Description:  Contains class definitions for ATMOSPHERE class.  This is intended
//				 for use with aerodynamic models and engine models (through SAE
//				 standard conversion from HP to SAE HP).  Although for most sims,
//				 a single number describing the air density and maybe another for
//				 temperature will suffice, if someone wanted to simulate a Pike's
//				 Peak hillclimb, an atmosphere model could add to the fidelity of
//				 the simulation.  This model should be valid between Death Valley
//				 and Pike's Peak for Cold Days, Hot Days, and Standard Days, as well
//				 as reasonable custom values entered by the user.
// History:

// CarDesigner headers
#include "environment/atmosphere.h"

//==========================================================================
// Class:			ATMOSPHERE
// Function:		ATMOSPHERE
//
// Description:		Constructor for ATMOSPHERE class.
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
ATMOSPHERE::ATMOSPHERE()
{
	// Initialize today's conditions to Standard Day conditions
	CurrentDay = StandardDay;
}

//==========================================================================
// Class:			ATMOSPHERE
// Function:		~ATMOSPHERE
//
// Description:		Destructor for ATMOSPHERE class.
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
ATMOSPHERE::~ATMOSPHERE()
{
}

//==========================================================================
// Class:			ATMOSPHERE
// Function:		Constant Definitions
//
// Description:		Defines class level constants for ATMOSPHERE class.  All
//					of these are defined by ???? at ???? and describe
//					conditions at sea level.
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
const ATMOSPHERE::CONDITIONS ATMOSPHERE::ColdDay =
{
	0.0,	// [slug/in^3]  FIXME!!!
	0.0,	// [psi]
	0.0,	// [deg R]
	0.0		// [psi-sec]
};

const ATMOSPHERE::CONDITIONS ATMOSPHERE::PolarDay =
{
	0.0,	// [slug/in^3]  FIXME!!!
	0.0,	// [psi]
	0.0,	// [deg R]
	0.0		// [psi-sec]
};

const ATMOSPHERE::CONDITIONS ATMOSPHERE::StandardDay =
{
	1.376e-6,	// [slug/in^3]
	14.696,		// [psi]
	518.67,		// [deg R]
	2.514e-9	// [psi-sec]
};

const ATMOSPHERE::CONDITIONS ATMOSPHERE::TropicDay =
{
	0.0,	// [slug/in^3]  FIXME!!!
	0.0,	// [psi]
	0.0,	// [deg R]
	0.0		// [psi-sec]
};

const ATMOSPHERE::CONDITIONS ATMOSPHERE::HotDay =
{
	0.0,	// [slug/in^3]  FIXME!!!
	0.0,	// [psi]
	0.0,	// [deg R]
	0.0		// [psi-sec]
};

//==========================================================================
// Class:			ATMOSPHERE
// Function:		SetConditions
//
// Description:		Sets today's conditions according to the passed argument.
//
// Input Arguments:
//		_CurrentDay	= CONDITIONS describing the current day's conditions
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void ATMOSPHERE::SetConditions(ATMOSPHERE::CONDITIONS _CurrentDay)
{
	// Set the current conditions
	CurrentDay = _CurrentDay;
}

//==========================================================================
// Class:			ATMOSPHERE
// Function:		GetConditions
//
// Description:		Gets the conditions for the current day, adjusted for
//					non-sea-level altitudes.
//
// Input Arguments:
//		Altitude	= double describing current height above sea level [ft]
//
// Output Arguments:
//		None
//
// Return Value:
//		CONDITIONS describing the conditions at the current altitude
//
//==========================================================================
ATMOSPHERE::CONDITIONS ATMOSPHERE::GetConditions(double Altitude)
{
	// FIXME:  implement the atmosphere!!!
	// For now, just return the current day's conditions at sea level
	return CurrentDay;
}

/* FORTRAN CODE FOR STANDARD ATMOSPHERE
From:  http://www.pdas.com/atmos.htm
!+
SUBROUTINE Atmosphere(alt, sigma, delta, theta)
!   -------------------------------------------------------------------------
! PURPOSE - Compute the properties of the 1976 standard atmosphere to 86 km.
! AUTHOR - Ralph Carmichael, Public Domain Aeronautical Software
! NOTE - If alt > 86, the values returned will not be correct, but they will
!   not be too far removed from the correct values for density.
!   The reference document does not use the terms pressure and temperature
!   above 86 km.
  IMPLICIT NONE
!============================================================================
!     A R G U M E N T S                                                     |
!============================================================================
  REAL,INTENT(IN)::  alt        ! geometric altitude, km.
  REAL,INTENT(OUT):: sigma      ! density/sea-level standard density
  REAL,INTENT(OUT):: delta      ! pressure/sea-level standard pressure
  REAL,INTENT(OUT):: theta      ! temperature/sea-level standard temperature
!============================================================================
!     L O C A L   C O N S T A N T S                                         |
!============================================================================
  REAL,PARAMETER:: REARTH = 6369.0                 ! radius of the Earth (km)
  REAL,PARAMETER:: GMR = 34.163195                     ! hydrostatic constant
  INTEGER,PARAMETER:: NTAB=8       ! number of entries in the defining tables
!============================================================================
!     L O C A L   V A R I A B L E S                                         |
!============================================================================
  INTEGER:: i,j,k                                                  ! counters
  REAL:: h                                       ! geopotential altitude (km)
  REAL:: tgrad, tbase      ! temperature gradient and base temp of this layer
  REAL:: tlocal                                           ! local temperature
  REAL:: deltah                             ! height above base of this layer
!============================================================================
!     L O C A L   A R R A Y S   ( 1 9 7 6   S T D.  A T M O S P H E R E )   |
!============================================================================
  REAL,DIMENSION(NTAB),PARAMETER:: htab= &
                          (/0.0, 11.0, 20.0, 32.0, 47.0, 51.0, 71.0, 84.852/)
  REAL,DIMENSION(NTAB),PARAMETER:: ttab= &
          (/288.15, 216.65, 216.65, 228.65, 270.65, 270.65, 214.65, 186.946/)
  REAL,DIMENSION(NTAB),PARAMETER:: ptab= &
               (/1.0, 2.233611E-1, 5.403295E-2, 8.5666784E-3, 1.0945601E-3, &
                                     6.6063531E-4, 3.9046834E-5, 3.68501E-6/)
  REAL,DIMENSION(NTAB),PARAMETER:: gtab= &
                                (/-6.5, 0.0, 1.0, 2.8, 0.0, -2.8, -2.0, 0.0/)
!----------------------------------------------------------------------------
  h=alt*REARTH/(alt+REARTH)      ! convert geometric to geopotential altitude

  i=1
  j=NTAB                                       ! setting up for binary search
  DO
    k=(i+j)/2                                              ! integer division
    IF (h < htab(k)) THEN
      j=k
    ELSE
      i=k
    END IF
    IF (j <= i+1) EXIT
  END DO

  tgrad=gtab(i)                                     ! i will be in 1...NTAB-1
  tbase=ttab(i)
  deltah=h-htab(i)
  tlocal=tbase+tgrad*deltah
  theta=tlocal/ttab(1)                                    ! temperature ratio

  IF (tgrad == 0.0) THEN                                     ! pressure ratio
    delta=ptab(i)*EXP(-GMR*deltah/tbase)
  ELSE
    delta=ptab(i)*(tbase/tlocal)**(GMR/tgrad)
  END IF

  sigma=delta/theta                                           ! density ratio
  RETURN
END Subroutine Atmosphere   ! -----------------------------------------------
*/