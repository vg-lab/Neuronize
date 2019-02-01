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

#ifndef QTSOMADEFORMER_H
#define QTSOMADEFORMER_H

#include <GL/glew.h>

#include <QMainWindow>
#include <QFileDialog>

#include "ui_SomaDeformerWidget.h"

#include "SomaDeformerWidgetViewer.h"
#include "SomaCreatorWidget.h"
#include <libs/libSysNeuroUtils/SystemUtils.h>

class SomaDeformerWidget: public QWidget, public Ui::SomaDeformerWidget
{
  Q_OBJECT

    QTimer *mMsgTimer;
    unsigned int mMiSecsSimulation;

  public:
    SomaDeformerWidget ( QWidget *parent = 0);
    ~SomaDeformerWidget ( );

    void deformDuringNSteps ( int pSteps );
    void finalSomaOptimization ( );
    void stopDeformation ( );
    void setModeledSoma(std::string path);
    void setSWCFile (std::string path);

    //const
    SomaDeformerWidgetViewer *getViewer ( ) { return viewer; }

  signals:

    void finishSoma ( );

  private:

    Ui::SomaDeformerWidget ui;

    SomaCreatorWidget* somaCreator;
public:
    void setSomaCreator(SomaCreatorWidget *somaCreator);

private:

    SomaDeformerWidgetViewer *viewer;

    QString mXMLFile;

  public slots:
    void startDeformation ( );
    void optimizateDendriticBase ( );
    void exportModelWithSTDName ( );

    void loadPredefinedXMLSomaDefAndGOAdvancedOptions ( );
    void loadPredefinedXMLSomaDef ( );
    void loadPredefinedXMLSomaDefAndSimulate ( );

    void finalizeSoma ( );
    

  private slots:

    void loadSomaModels ( );
    void loadSWCFile ( );
    void loadPatternMesh ( );
    void loadXMLSomaDef ( );

    void generateSoma ( );
    void singleStepDeformation ( );
    void useSphericalSoma ();

    void setAnimationPeriod ( );

    void setMSIntegrator ( );
    void setMSDt ( );
    void setMSConstraints ( );
    void setMSSpringParams ( );
    void setMSNodeParams ( );
    void setUnCollapseSprings ( );

    void exportModel ( );

    void setNormalizeExport ( );

    void setDendriticParams ( );

    void setUseSphericalProjection ( );

    void setUseContourPropagation ( );

    void setExportOBJSecuence ( );

    void setDeleteRedundantVertex ( );

    void emitFinishSoma ( );

    void restoreDefaultValues ( );

    void showContinueMsg ( );

};

#endif // QTSOMADEFORMER_H
