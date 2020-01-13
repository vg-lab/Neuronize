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

#include <SkelGeneratorUtil/Neuron.h>

#include <libs/libQtNeuroUtils/XMLSomaDef.h>
#include <libs/libQtNeuroUtils/XMLSomaDefManager.h>

#include <libs/libNeuroUtils/SWCImporter.h>
#include <libs/libNeuroUtils/BaseMesh.h>
#include <libs/libNeuroUtils/ASC2SWCV2.h>
#include <QtWidgets/QProgressDialog>
#include <QFutureWatcher>
#include <QLineEdit>

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

    void generateXMLSoma ( QString fileName, bool useSoma );

    void calcNearestVertex ( );

    //void generateMatLabScritp();

    void resetInterface ( );

    void openSaveFileDialog(QLineEdit *target, const QString &title, const QString &types);

    void openDirectory(QLineEdit* target, const QString& title);

    void openSelectFileDialog(QLineEdit *target, const QString &title, const QString &types, bool multiFile);

    void openDir(QLineEdit* des,QString test);

    void onGenerateNeurons();

    void onGenerateOneNeuron();

  signals:

    void somaCreated ( );

    void generateNeurons(QString inputDirectory,QString outputDirectory, int subdivisions,QString baseName);

  public:
    SomaCreatorWidget (const QString &tempDir,QWidget *parent = 0);

    ~SomaCreatorWidget ( );

    void deleteTreeViewer ( );

    void loadSWCFile ( QString pFileToLoad );

    skelgenerator::Neuron *getNeuron() const;

    void setNeuron(skelgenerator::Neuron *neuron);

    bool isSomaContours();

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
    QString mInputFile;
public:
    const QString &getInputFile() const;

private:

    std::vector < unsigned int > mNearestVertex;
    skelgenerator::Neuron* neuron;
    bool somaContours;
    std::vector<Spine*> spines;
    std::vector<std::vector<OpenMesh::Vec3d>> contours;
    ASC2SWCV2* ASCparser;
public:
    const vector<Spine*> &getSpines() const;
    const vector<vector<OpenMesh::Vec3d>> &getContours() const;

    ASC2SWCV2 *getASCparser() const;

    SWCImporter *getMswcImporter() const;

private:

    bool isIdInContainer ( unsigned int pId, std::vector < unsigned int > pVector );

    void stringToFile ( QString pFileContent, QString pFilePath );
};

#endif // QTXMLDENDRITICMODIFICATOR_H
