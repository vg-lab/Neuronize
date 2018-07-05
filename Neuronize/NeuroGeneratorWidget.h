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

#ifndef NGW_H
#define NGW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>

#include "ui_NeuroGeneratorWidget.h"
#include "NeuroGeneratorWidgetViewer.h"

class NeuroGeneratorWidget: public QWidget, public Ui::NeuroGeneratorWidget
{
  Q_OBJECT

    QTimer *mMsgTimer;
    unsigned int mMiSecsSimulation;

    QMessageBox *msgBox;

  public slots:

    void destroyAllGroupsSpines ( ) { viewer->destroyAllGroupsSpines ( ); }

    //void showControls();
    void generateDendrites ( );

    void exportNeuron ( QString fileName );

    void loadNeuronDefinitionAndGenerateMesh ( );

  public:
    NeuroGeneratorWidget ( QWidget *parent = 0 );

    ~NeuroGeneratorWidget ( );

    Ui::NeuroGeneratorWidget getUI ( ) { return ui; };

    void ReLoadSomaModelsFromPath ( QString pPathToSomas );

    void setMorphologyFile ( QString lSWCFile ) { mSWCFleName = lSWCFile; };

    void loadMorphologyFile ( QString lSWCFile );

    void loadSTDSoma ( );

    void showDendirtesTab ( );

    void showSpinesTab ( );

    void batchSpinesGeneration ( );

    void exportSpinesInmediatly ( QString fileName );

    void loadSpinesModelFromPath ( QString pPath );

    NeuroGeneratorWidgetViewer *getViewer ( ) { return viewer; }

    void applySmooth ( unsigned int pIterations,
                       unsigned int pMethod,
                       unsigned int pTechnique,
                       unsigned int pContinuity );

  signals:

    void finishNeuronSurfaceAndRestart ( );

    void finishNeuronSurfaceAndGoToSpines ( );

    void finishSpinesAndRestart ( );

    void resetToSomaState ( );

  private:
    Ui::NeuroGeneratorWidget ui;

    QString mSWCFleName;

    NeuroGeneratorWidgetViewer *viewer;

  private slots:

    void loadNeuronModel ( );

    void loadMorphologyFile ( );

    void ReLoadSomaModels ( );

    void loadSpinesModel ( );

    QString loadSWCFile ( );

    void generateNeuron ( );

    void generateNeuronMeshFromFile ( QString fileName );

    void generateNeuronMeshFromInternalInfo ( );

    void generateNeuronLines ( );

    void generateNeuronSphereNodes ( );

    void generateSpines ( );

    void attachNewSpinesGroup ( );

    void applyNoise ( );

    void applySmooth ( );

    void applySubdivide ( );

    void applyDecimate ( );

    void exportNeuron ( );

    void exportNeuronAndSpines ( );

    void exportSpines ( );

    void joinSpines ( );

    void ControlStateRender ( );

    void setNormalScale ( );

    void setPixelScale ( );

    void ControlLights ( );

    void SetDebugMode ( );

    void LoadSomaModels ( );

    void setSomaModel ( );

    void LoadSpinesModels ( );

    void setSpinesTypes ( );

    void setSomasScale ( );

    void setSpinesScale ( );

    void setMultipleGroupsOfSpines ( );

    void setColorFromSWC ( );

    void setSpinesDistributionParams ( );

    void setPiramidalSoma ( );

    void setNeuronType ( );

    void emitFinishNeuronSurfaceAndRestart ( );

    void emitFinishNeuronSurfaceAndGoToSpines ( );

    void emitFinishSpinesAndRestart ( );

    void RebuildWithAdvancedOptions ( );

    void goAdvencedSpinesOptions ( );

    void showMsjDendritesGeneration ( );

    void exportSpinesInfo ( );

    void importSpinesInfo ( );

};

#endif // QTNEUROSYNTH_H
