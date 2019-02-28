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


#include "defs.h"

/**	
 *  Function to compare REALS with a certain tolerance and presision 
 *  @param a		1rs REAL to compare
 *  @param b		2nd REAL to compare
 *  @param TOL		Tolerancia for the comparisson
 *  @param PREC		Precission for the comparisson
 *  @return bool	True if both REALS are equal for the pressicion and the tolerance.
 */
bool SonIguales ( REAL a, REAL b, REAL TOL, REAL PREC )
{
  if ( fabs ( a ) > fabs ( b ))
  {
    if (( fabs ( a - b )) > ( TOL*( fabs ( a ) + PREC )))
      return false;
  }
  else
  {
    if (( fabs ( a - b )) > ( TOL*( fabs ( b ) + PREC )))
      return false;
  }
  return true;
}

/**	
 *  Function to compare REALS with a certain tolerance and presision
 *  The tolerance and presision are with the default values.
 *  @param a		1rs REAL to compare
 *  @param b		2nd REAL to compare
 *  @return bool	True if both REALS are equal for the default presision and the tolerance.
 */

bool SonIguales ( REAL a, REAL b )
{
  return SonIguales ( a, b, TOLERANCIA, PRECISION );
}

/**
 *  Function to compare if a REAL is 0 with a certain tolerance
 *  @param a		REAL to compare with 0
 *  @return bool	True if the REALS are equal 0 for the presision and the tolerance.
 */
bool EsCero ( REAL a, REAL TOL )
{
  return SonIguales ( a, 0., TOL, PRECISION );
}

/**	
 *  Function to compare if a REAL is 0 with the default tolerance
 *  @param a		REAL to compare with 0
 *  @return bool	True if the REALS are equal 0 for the presision and the tolerance.
 */
bool EsCero ( REAL a )
{
  return SonIguales ( a, 0. );
}

/**	
 *  Clean spaces in string
 *  @param std::string::const_iterator i		Initial iterator of string
 *  @param std::string::const_iterator f		Final iterator of string
 */
std::string Trim ( std::string::const_iterator i, std::string::const_iterator f )
{
  std::string::const_iterator it;
  std::string buff;
  for ( it = i; it != f; ++it )
  {
    if ( *it != ' ' )
      buff += *it;
  }
  return buff;
}
