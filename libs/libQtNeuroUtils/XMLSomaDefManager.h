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

#include <qtneuroutils/api.h>

#include <string>
#include <fstream>

#include <QDomDocument>
#include <QFile>

#include "XMLSomaDef.h"

using namespace std;

class QTNEUROUTILS_API XMLSomaDefManager
{
    QDomDocument mDomDoc;

    XMLSomaDef *mXMLSomaDef;

    QString mPathToFiles;

    void generateDomObject ( );

    QDomElement addElement ( QDomDocument &doc,
                             QDomNode &node,
                             const QString &tag,
                             const QString &value );

    void parseXMLwithDOM ( );

  public:
    XMLSomaDefManager ( void );
    ~XMLSomaDefManager ( void );

    XMLSomaDef *getXMLSomaDef ( );
    QDomDocument getXMLDomDocument ( );

    void generateXMLContent ( QString pSWCFile,
                              QString pModelFileName,
                              QString pDefaultGeodesicFileName,
                              QString pDefaultGeodesicVertsIdsFileName,
                              unsigned int pSomaHorResol,
                              unsigned int pSomaVerResol,
                              unsigned int pNumDendritics,
                              float pDefaultaDendriticeValue1,
                              float pDefaultaDendriticeValue2
    );

    void loadXMLContent ( QString pFilePath );

    void exportDomToFile ( std::string fileContent );

    void setPathToFiles ( QString pPathToFiles );

    QString getPathToFiles ( );

    void loadGeodesicDistanceFilesContent ( );

    void loadGeodesicVertexIdesCandFilesContent ( );

};
