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

#ifndef SOMACREATORWIDGET
#define SOMACREATORWIDGET

#include <QMainWindow>
#include <QDomDocument>

#include <SkelGenerator/SkelGeneratorUtil/Neuron.h>

#include <libs/libQtNeuroUtils/XMLSomaDef.h>
#include <libs/libQtNeuroUtils/XMLSomaDefManager.h>

#include <libs/libNeuroUtils/SWCImporter.h>
#include <libs/libNeuroUtils/BaseMesh.h>

#include "ui_SomaCreatorWidget.h"

//!!!#include <engine.h>
//---#include <mex.h>
//---#include <GeoCalc.h>

using namespace NSSWCImporter;
using namespace NSBaseMesh;

class SomaCreatorWidget: public QWidget, public Ui::SomaCreatorWidget
{
  Q_OBJECT

    //!!!Engine *m_pEngine;


  public slots:

    //XML methods
    void generateXMLContent ( );

    void loadXMLContent ( );

    void setSWCFile ( );

    QString getSWCFile ( );

    QString getFullPathToSWCFile ( );

    //GeodesicMethods
    void loadSWCFile ( );

    void loadModelName ( );

    void loadModel ( );

    void loadModel ( QString fileName );

    void loadGeodesicDendriticDistanceFile ( );

    void calcAndExportNearestVertexToSWCDendritics ( );

    void calcAndExportIdVertexToDendriticsDeformation ( );

    void setGeoDistanceFromSWCImporter ( );

    //Methods for magazine.
    void setExitDirectory ( );

    void generateXMLSoma ( );

    void generateXMLSoma ( QString fileName );

    void calcNearestVertex ( );

    //void generateMatLabScritp();

    void resetInterface ( );

  signals:

    void somaCreated ( );

  public:
    SomaCreatorWidget ( QWidget *parent = 0 );

    ~SomaCreatorWidget ( );

    void deleteTreeViewer ( );

    void loadSWCFile ( QString pFileToLoad );

    skelgenerator::Neuron *getNeuron() const;

    void setNeuron(skelgenerator::Neuron *neuron);


private:

    Ui::SomaCreatorWidget ui;

    XMLSomaDefManager *mXMLSomaDefManager;

    SWCImporter *mSWCImporter;

    BaseMesh *mBaseMesh;

    QString mDefaultVidsFileName;
    QString mDefaultGeoDistFileName;
    QString mToolBoxDir;
    QString mSWCFileName;
    QString mFullPathSWCFileName;
    QString mMehsFileName;
    QString mExitDirectory;

    std::vector < unsigned int > mNearestVertex;
    skelgenerator::Neuron* neuron;

private:

    bool isIdInContainer ( unsigned int pId, std::vector < unsigned int > pVector );

    void stringToFile ( QString pFileContent, QString pFilePath );
};

#endif // QTXMLDENDRITICMODIFICATOR_H
