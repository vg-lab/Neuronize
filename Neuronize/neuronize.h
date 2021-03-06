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

#ifndef NEURONIZE_H
#define NEURONIZE_H

//#include <GL/glew.h>
//#include <GL/glut.h>

#include <QMainWindow>
#include <QTemporaryDir>
#include <QtWidgets/QProgressDialog>

#include "ui_neuronize.h"

#include "SomaCreatorWidget.h"
#include "SomaDeformerWidget.h"
#include "NeuroGeneratorWidget.h"
#include "RepairWidget.h"
#include "CompareMeshesWidget.h"
#include <libs/libNeuroUtils/BBDD.h>


class Neuronize: public QMainWindow
{
  Q_OBJECT

    QString mFullSWCFilePath;

    QString mInputFilePath;
    QString mOuputFilePath;

    std::vector < std::tuple<QString,skelgenerator::Neuron* >> mFilesContainer;
    QTemporaryDir tempDir;

  public:

    Neuronize ( QWidget *parent = 0 );

    ~Neuronize ( );

    static BBDD::BBDD bbdd;
    static QString tmpPath;
    static QString configPath;
    static QString envPath;
    static QString outPath;
    static bool hasPython;

  private:

    Ui::NeuronizeClass ui;

    //Widgets
    SomaCreatorWidget *mSomaCreatorWidget;

    SomaDeformerWidget *mSomaDeformerWidget;

    NeuroGeneratorWidget *mNeuroGeneratorWidget;

    RepairWidget *mRepairWidget;

    CompareMeshesWidget *mCompareMeshesWidget;



    //NeuroGeneratorWidget	* mSpinesGeneratorWidget;

    unsigned int mActiveTab;

  public slots:

    void genetareNeuronsInBatch ( QString inputFilePath,QString outputFilePath,int subdivisions,QString baseName );

  private slots:
    void resetNeuronnizeInterface ( );

    void showSomaCreator ( );

    void showSomaDeformer ( );

    void showDendriteGenerator ( );

    void showSpinesGenerator ( );

    void takeASnapshot ( );

    void showHelp ( );

    void actionNewNeuron ( );

    void actionQuit ( );

    void actionAbout ( );

    void actionBack ( );

    void NewNeuronQuestionAndRestart ( );

    void onSomaBuildFinish();

    void showExportDialog();

    void deleteDatabase();

    void createDatabase();

    void exportDatabase();

    void resetPythonEnv();

    void initPythonEnv();
};

#endif // XNEURON_H




