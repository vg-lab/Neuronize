/*
 * Copyright (c) 2018 CCS/UPM - GMRV/URJC.
 *
 * Authors: Juan Pedro Brito Méndez <juanpedro.brito@upm.es>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

/****************************************************************************
 *			Constants definitions and utilities libraries					*
 * **************************************************************************/

//#pragma once
#ifndef DEFS_H_
#define DEFS_H_

#include <math.h>

/********************************
 * 				Macros			*
 ********************************/
//From radians to degrees
#define RADTODEG( x ) (((x)*180.)/PIGPU)

//From degrees to radians 
#define DEGTORAD( x ) ( (((x)*PIGPU))/180.)

//Midpoint of two values
#define MEDIO( x, y )  ((x+y)/2.)

//Sin from Cosin
#define COSTOSIN( x ) sqrt(1.-(x*x))

//Cosin from sin
#define SINTOCOS( x ) sqrt(1.-(x*x))

/********************************
 * 	  Typedefs and constants	*
 ********************************/

//The actual values are low presision (floats)

//typedef int ENTERO;
typedef unsigned int ENTERO;
//typedef unsigned long int ENTERO;

typedef float REAL;
//typedef double REAL;

//PI value
#define PI      3.14159265358979323846264338327950288419716939937510582
#define PIGPU    3.14159265358979323846264338327950288419716939937510582
#define PRECISION  10E-10

//The tolerance is obtained from the machine presision
#define EPSI (fabs(cos(DEGTORAD(90.-PRECISION))-cos(DEGTORAD(90.+PRECISION))))
const REAL TOLERANCIA EPSI;

//Prototipado de funciones


/**	
 *  Function to compare REALS with a certain tolerance and presision 
 *  @param a		1rs REAL to compare
 *  @param b		2nd REAL to compare
 *  @param TOL		Tolerancia for the comparisson
 *  @param PREC		Precission for the comparisson
 *  @return bool	True if both REALS are equal for the pressicion and the tolerance.
 */
bool SonIguales ( REAL a, REAL b, REAL TOL, REAL PREC );

/**	
 *  Function to compare REALS with a certain tolerance and presision
 *  The tolerance and presision are with the default values.
 *  @param a		1rs REAL to compare
 *  @param b		2nd REAL to compare
 *  @return bool	True if both REALS are equal for the default presision and the tolerance.
 */
bool SonIguales ( REAL a, REAL b );

/**	
 *  Function to compare if a REAL is 0 with a certain tolerance
 *  @param a		REAL to compare with 0
 *  @return bool	True if the REALS are equal 0 for the presision and the tolerance.
 */
bool EsCero ( REAL a, REAL TOL );

/**	
 *  Function to compare if a REAL is 0 with the default tolerance
 *  @param a		REAL to compare with 0
 *  @return bool	True if the REALS are equal 0 for the presision and the tolerance.
 */
bool EsCero ( REAL a );

/********************************
 * 	  auxiliar  templates       *
 ********************************/

///####
#include <sstream>

std::string Trim ( std::string::const_iterator i, std::string::const_iterator f );

/*
 * //####
template <typename T>
std::string to_string(const T& value)
{
	std:stringstream oss;
	oss << value;
	return oss.str();
}

//template<class T>
template<typename T>
    T from_String(const std::string& s)
{
     std::istringstream stream (s);
     T t;
     stream >> t;
     return t;
}
*/

#endif /*DEFS_H_*/
