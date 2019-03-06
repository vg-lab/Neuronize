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

#ifndef XMLDENDMODIF
#define XMLDENDMODIF

#include <qtneuroutils/api.h>

#include <iostream>
#include <fstream>

using namespace std;

#include <QString>
#include <QList>

struct DenModifInfo
{
  unsigned int id;
  float distance;
};

class QTNEUROUTILS_API XMLSomaDefDendriticModif
{

  public:

    unsigned int mId;
    QString mDenType;
    QString mGeodeVertexDistanceFile;
    QString mGeodeVertexIdsCandByDisFile;

    float mParamValue1;
    float mParamValue2;

    QList < DenModifInfo > mGeoDistanceContainer;
    QList < unsigned int > mGeoVertexIdsCand;

    XMLSomaDefDendriticModif ( void );
    ~XMLSomaDefDendriticModif ( void );

    void loadGeoDistanceFileContainerFromPath ( QString pPathFile );

    void loadGeoVertexIdsCandFileContainerFromPath ( QString pPathFile );

};
#endif
