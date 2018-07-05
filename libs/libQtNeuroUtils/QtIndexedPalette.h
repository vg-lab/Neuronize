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

#pragma once

#include <vector>

#include <QImage>
#include <QColor>

using namespace std;

class QtIndexedPalette
{
    vector < QColor > mColorPalette;
    QImage baseImge;
    int mMaxLevelToInterpolate;

    const QColor &getColorLevelMod ( int pLevel ) const;
    const QColor &getColorLevelMod ( int pLevel, int pValMax, int pValMin ) const;
    const QColor &getColorLevelInterpolated ( int pLevel ) const;
    bool mInterpolateColor;

  public:
    QtIndexedPalette ( QImage pImage );
    ~QtIndexedPalette ( void );

    const QImage &getImage ( ) const;
    const vector < QColor > &getColorPalette ( ) const;

    void setMaxLevelToInterpolate ( int pMaxLevelToInterpolate ) { mMaxLevelToInterpolate = pMaxLevelToInterpolate; };
    int getMaxLevelToInterpolate ( ) { return mMaxLevelToInterpolate; };

    void setInterpolateColor ( bool pInterpolateColor ) { mInterpolateColor = pInterpolateColor; };
    bool getInterpolateColor ( ) { return mInterpolateColor; };

    const QColor &getColorByLevel ( int pLevel ) const;

};
