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

#include "ui_neuronize.h"

#include "SomaCreatorWidget.h"
#include "SomaDeformerWidget.h"
#include "NeuroGeneratorWidget.h"
#include "batchbuilder.h"

class Neuronize: public QMainWindow
{
  Q_OBJECT

    QString mFullSWCFilePath;

    QString mInputFilePath;
    QString mOuputFilePath;

    std::vector < QString > mFilesContainer;

  public:

    Neuronize ( QWidget *parent = 0 );

    ~Neuronize ( );

  private:

    Ui::NeuronizeClass ui;

    //Widgets
    SomaCreatorWidget *mSomaCreatorWidget;

    SomaDeformerWidget *mSomaDeformerWidget;

    NeuroGeneratorWidget *mNeuroGeneratorWidget;

    BatchBuilder *mBatchBuilder;
    //NeuroGeneratorWidget	* mSpinesGeneratorWidget;

    unsigned int mActiveTab;

  public slots:

    void genetareNeuronsInBatch ( );

  private slots:
    void resetNeuronnizeInterface ( );

    void showSomaCreator ( );

    void showSomaDeformer ( );

    void showDendriteGenerator ( );

    void showSpinesGenerator ( );

    void showBatchBuilder ( );

    void takeASnapshot ( );

    void showHelp ( );

    void actionNewNeuron ( );

    void actionQuit ( );

    void actionAbout ( );

    void actionBack ( );

    void NewNeuronQuestionAndRestart ( );

};

#endif // XNEURON_H




