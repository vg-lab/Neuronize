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

#include "randomUtils.h"

void initrand ( )
{
  srand ( time ( NULL ));
  //srand((unsigned)(time(0)));
}

int randint ( )
{
  return rand ( );
}

int randint ( int max )
{
  return int ( max*rand ( )/( RAND_MAX + 1.0 ));
}

int randint ( int min, int max )
{
  if ( min > max )
    return max + int (( min - max + 1 )*rand ( )/( RAND_MAX + 1.0 ));
  else
    return min + int (( max - min + 1 )*rand ( )/( RAND_MAX + 1.0 ));
}

float randfloat ( )
{
  return rand ( )/( float ( RAND_MAX ) + 1 );
}

float randfloat ( float max )
{
  return randfloat ( )*max;
}

float randfloat ( float min, float max )
{
  if ( min > max )
    return randfloat ( )*( min - max ) + max;
  else
    return randfloat ( )*( max - min ) + min;
}

double randdouble ( )
{
  return rand ( )/( double ( RAND_MAX ) + 1 );
}

double randdouble ( double max )
{
  return randdouble ( )*max;
}

double randdouble ( double min, double max )
{
  if ( min > max )
    return randdouble ( )*( min - max ) + max;
  else
    return randdouble ( )*( max - min ) + min;
}

char *randstr ( char *str, unsigned long length )
{
  //make we were passed a valid pointer
  if ( !str )
    return 0;

  //make sure the string is supposed to contain something
  if ( !length )
    return 0;

  //put random characters into the string, give both
  //upper and lower case numbers an equal chance at
  //being used
  unsigned long x = 0;
  for ( x = 0; x < length - 1; x++ )
  {
    if ( !randint ( 1 ))
      str[x] = ( char ) randint ( 65, 90 );
    else
      str[x] = ( char ) randint ( 97, 122 );
  }

  //null terminate the string
  str[x] = 0;

  return str;
}

char *randstr ( char *str, unsigned long length, unsigned long flags )
{
  //make we were passed a valid pointer
  if ( !str )
    return 0;

  //make sure the string is supposed to contain something
  if ( !length )
    return 0;

  //if none of the flags were used then we set flags to use just upper and lower case
  if (( !flags & 0x00000001 )
    && (( !flags ) & 0x00000002 )
    && (( !flags ) & 0x00000004 )
    && (( !flags ) & 0x00000008 )
    && (( !flags ) & 0x00000010 )
    && (( !flags ) & 0x00000020 )
    && (( !flags ) & 0x00000040 )
    && (( !flags ) & 0x00000080 )
    && (( !flags ) & 0x00000100 ))
  {
    flags = RAND_STR_LOWER_CASE_LETTERS | RAND_STR_CAPITOL_LETTERS;
  }

  char t[9] = { 0 };
  int i = 0;

  //each pass of the loop the flags are checked and for each flag that is used
  //then a random character in the flags range is added to a temporary array of
  //characters, after all the flags are checked a random character from the
  //temporary array is chosen to be inserted into the string
  unsigned long x = 0;
  for ( x = 0; x < length - 1; x++ )
  {
    i = 0;

    if ( flags & 0x1 )
    {
      t[i] = randint ( 65, 90 );
      i++;
    }

    if ( flags & 0x2 )
    {
      t[i] = randint ( 97, 122 );
      i++;
    }

    if ( flags & 0x4 )
    {
      t[i] = randint ( 48, 57 );
      i++;
    }

    if ( flags & 0x8 )
    {
      t[i] = randint ( 32, 47 );
      i++;
    }

    if ( flags & 0x10 )
    {
      t[i] = randint ( 58, 64 );
      i++;
    }

    if ( flags & 0x20 )
    {
      t[i] = randint ( 91, 96 );
      i++;
    }

    if ( flags & 0x40 )
    {
      t[i] = randint ( 123, 126 );
      i++;
    }

    if ( flags & 0x80 )
    {
      if ( !randint ( 32 ))
      {
        t[i] = 127;
        i++;
      }
      else
      {
        t[i] = randint ( 1, 31 );
        i++;
      }
    }

    if ( flags & 0x100 )
    {
      t[i] = randint ( 127, 255 );
      i++;
    }

    str[x] = t[randint ( i - 1 )];
  }

  //null terminate the string
  str[x] = 0;

  return str;
}
