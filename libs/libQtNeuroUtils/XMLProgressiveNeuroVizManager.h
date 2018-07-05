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

#include <string>
#include <fstream>

#include <QDomDocument>
#include <QFile>

#include "XMLProgressiveNeuroViz.h"

using namespace std;

class XMLProgressiveNeuroVizManager
{
    QDomDocument mDomDoc;

    XMLProgressiveNeuroViz *mXMLProgressiveNeuroViz;

    QString mPathToFiles;

    void generateDomObject ( );

    QDomElement addElement ( QDomDocument &doc,
                             QDomNode &node,
                             const QString &tag,
                             const QString &value );

    void parseXMLwithDOM ( );

  public:
    XMLProgressiveNeuroVizManager ( void );

    ~XMLProgressiveNeuroVizManager ( void );

    XMLProgressiveNeuroViz *getXMLProgressiveNeuroViz ( ) const;

    QDomDocument getXMLDomDocument ( );

    void generateXMLContent ( QString pSWCFile,
                              QString pModelFileName,
                              NeuronType pNeuronType,
                              unsigned int pSomaResol,
                              unsigned int pModelVerResol,
                              unsigned int pModelHorResol
    );

    void loadXMLContent ( QString pFilePath );

    void exportDomToFile ( std::string fileName );

    void setPathToFiles ( QString pPathToFiles );

    QString getPathToFiles ( );

    void loadCriterionsValues ( );
};
