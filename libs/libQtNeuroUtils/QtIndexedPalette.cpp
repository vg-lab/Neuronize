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

#include "QtIndexedPalette.h"

QtIndexedPalette::QtIndexedPalette ( QImage pImage )
{
  baseImge = pImage;

  int lSize = baseImge.size ( ).height ( )*baseImge.size ( ).width ( );
  for ( int i = 0; i < lSize; ++i )
  {
    mColorPalette.push_back ( QColor ( baseImge.pixel ( i, 0 )));
  }

  mMaxLevelToInterpolate = mColorPalette.size ( );
  mInterpolateColor = false;
}

QtIndexedPalette::~QtIndexedPalette ( void )
{
  mColorPalette.clear ( );
}

const QImage &QtIndexedPalette::getImage ( ) const
{
  return baseImge;
}

const vector < QColor > &QtIndexedPalette::getColorPalette ( ) const
{
  return mColorPalette;
}

const QColor &QtIndexedPalette::getColorLevelMod ( int pLevel ) const
{
  return mColorPalette.at ( pLevel%( mColorPalette.size ( )));
}

const QColor &QtIndexedPalette::getColorLevelMod ( int pLevel, int pValMax, int pValMin ) const
{
  int lLevel = pLevel%( mColorPalette.size ( ));

  return mColorPalette.at ( lLevel );
}

const QColor &QtIndexedPalette::getColorLevelInterpolated ( int pLevel ) const
{
  /*
  MaxLevel	->	MaxPaletta
  pLevel		->	X
    =>>	X	=	MaxPaleta*pLevel / maxLevel
  */

  int lLevel = ( pLevel*( mColorPalette.size ( ) - 1 ))/( mMaxLevelToInterpolate );
  //Dasmos la vuleta por si acaso nos pasamos
  //lLevel	%=	( mColorPalette.size() );
  return mColorPalette.at ( lLevel );

  //int lLevel = pLevel % ( mColorPalette.size() );
  //return mColorPalette.at ( lLevel );
}

const QColor &QtIndexedPalette::getColorByLevel ( int pLevel ) const
{
  if ( mInterpolateColor )
  {
    return getColorLevelInterpolated ( pLevel );
  }
  else
  {
    return getColorLevelMod ( pLevel );
  }
}



