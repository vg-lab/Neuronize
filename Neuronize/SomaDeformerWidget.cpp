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

#include "SomaDeformerWidget.h"

#include <QMessageBox>

SomaDeformerWidget::SomaDeformerWidget ( QWidget *parent )
  : QWidget ( parent )
{
  ui.setupUi ( this );

  viewer = new SomaDeformerWidgetViewer ( this );

  mXMLFile = "tmp/Definition.xml";

  //Add the viewer component to the interface
  ui.verticalLayout_Viewer->addWidget ( viewer );

  //Configure the compnent
  viewer->setupViewer ( );

  //Add the integrator method selector
  ui.comboBox_MS_Integrator_Method->addItem ( "Euler" );
  ui.comboBox_MS_Integrator_Method->addItem ( "RK2" );
  ui.comboBox_MS_Integrator_Method->addItem ( "Verlet" );

  QObject::connect ( ui.radioButton_ModelledSomas, SIGNAL( clicked ( )), this, SLOT( loadSomaModels ( )) );
  QObject::connect ( ui.radioButton_XMLSoma, SIGNAL( clicked ( )), this, SLOT( loadXMLSomaDef ( )) );

  QObject::connect ( ui.spinBox_animationPeriod, SIGNAL( editingFinished ( )), this, SLOT( setAnimationPeriod ( )) );
  QObject::connect ( ui.checkBox_useSWCFile, SIGNAL( clicked ( )), this, SLOT( loadSWCFile ( )) );

  QObject::connect ( ui.pushButton_GenerateFromOption,
                     SIGNAL( clicked ( )),
                     this,
                     SLOT( loadPredefinedXMLSomaDefAndSimulate ( )) );

  QObject::connect ( ui.pushButton_ExportModel, SIGNAL( clicked ( )), this, SLOT( exportModel ( )) );
  QObject::connect ( ui.pushButton_ImportSWC, SIGNAL( clicked ( )), this, SLOT( loadSWCFile ( )) );

  //Mass Spring conections
  QObject::connect ( ui.pushButton_MS_StartSimulation, SIGNAL( clicked ( )), this, SLOT( startDeformation ( )) );
  QObject::connect ( ui.pushButton_MS_SingleStepSimulation,
                     SIGNAL( clicked ( )),
                     this,
                     SLOT( singleStepDeformation ( )) );
  QObject::connect ( ui.pushButton_MS_StopSimulation, SIGNAL( clicked ( )), this, SLOT( stopDeformation ( )) );

  QObject::connect ( ui.comboBox_MS_Integrator_Method,
                     SIGNAL( currentIndexChanged ( int )),
                     this,
                     SLOT( setMSIntegrator ( )) );
  QObject::connect ( ui.doubleSpinBox_MS_Integrator_dt, SIGNAL( editingFinished ( )), this, SLOT( setMSDt ( )) );
  QObject::connect ( ui.checkBox_MS_Constraints, SIGNAL( clicked ( )), this, SLOT( setMSConstraints ( )) );
  QObject::connect ( ui.doubleSpinBox_MS_Stiffness, SIGNAL( editingFinished ( )), this, SLOT( setMSSpringParams ( )) );
  QObject::connect ( ui.doubleSpinBox_MS_Dumping, SIGNAL( editingFinished ( )), this, SLOT( setMSSpringParams ( )) );

  QObject::connect ( ui.doubleSpinBox_NodeMass, SIGNAL( editingFinished ( )), this, SLOT( setMSNodeParams ( )) );
  QObject::connect ( ui.doubleSpinBox_gravityForce_X, SIGNAL( editingFinished ( )), this, SLOT( setMSNodeParams ( )) );
  QObject::connect ( ui.doubleSpinBox_gravityForce_Y, SIGNAL( editingFinished ( )), this, SLOT( setMSNodeParams ( )) );
  QObject::connect ( ui.doubleSpinBox_gravityForce_Z, SIGNAL( editingFinished ( )), this, SLOT( setMSNodeParams ( )) );
  QObject::connect ( ui.spinBox_UnCollapseSprigns,
                     SIGNAL( editingFinished ( )),
                     this,
                     SLOT( setUnCollapseSprings ( )) );

  QObject::connect ( ui.pushButton_UsePatternMesh, SIGNAL( clicked ( )), this, SLOT( loadPatternMesh ( )) );
  QObject::connect ( ui.checkBox_NormalizeModel, SIGNAL( clicked ( )), this, SLOT( setNormalizeExport ( )) );

  QObject::connect ( ui.doubleSpinBox_ApicalParam, SIGNAL( editingFinished ( )), this, SLOT( setDendriticParams ( )) );
  QObject::connect ( ui.doubleSpinBox_BasalParam, SIGNAL( editingFinished ( )), this, SLOT( setDendriticParams ( )) );

  QObject::connect ( ui.checkBox_UseSphericalProjection,
                     SIGNAL( clicked ( )),
                     this,
                     SLOT( setUseSphericalProjection ( )) );
  QObject::connect ( ui.checkBox_UseContourPropagation,
                     SIGNAL( clicked ( )),
                     this,
                     SLOT( setUseContourPropagation ( )) );

  QObject::connect ( ui.pushButton_ReteselDendriticBase,
                     SIGNAL( clicked ( )),
                     this,
                     SLOT( optimizateDendriticBase ( )) );

  QObject::connect ( ui.checkBox_ExportOBJSecuence, SIGNAL( clicked ( )), this, SLOT( setExportOBJSecuence ( )) );
  QObject::connect ( ui.checkBox_DeleteRedundantVertex,
                     SIGNAL( clicked ( )),
                     this,
                     SLOT( setDeleteRedundantVertex ( )) );

  QObject::connect ( ui.pushButton_GenerateSoma, SIGNAL( clicked ( )), this, SLOT( startDeformation ( )) );

  QObject::connect ( ui.pushButton_RebuildSoma,
                     SIGNAL( clicked ( )),
                     this,
                     SLOT( loadPredefinedXMLSomaDefAndGOAdvancedOptions ( )) );

  QObject::connect ( ui.pushButton_FinalizeSoma, SIGNAL( clicked ( )), this, SLOT( finalizeSoma ( )) );
  QObject::connect ( ui.pushButton_NextStep, SIGNAL( clicked ( )), this, SLOT( finalizeSoma ( )) );
  QObject::connect ( ui.pushButton_restoreDefaults, SIGNAL( clicked ( )), this, SLOT( restoreDefaultValues ( )) );

  mMsgTimer = NULL;
  mMiSecsSimulation = 5000;
}

SomaDeformerWidget::~SomaDeformerWidget ( )
{
  delete viewer;
}

void SomaDeformerWidget::loadSomaModels ( )
{
  /* select a directory using file dialog */
  if ( ui.comboBox_ModelledSomas->count ( ) == 0 )
  {
    QDir directory;
    QString path = QFileDialog::getExistingDirectory ( this, tr ( "Directory" ), directory.path ( ));
    if ( path.isNull ( ) == false )
    {
      directory.setPath ( path );
      viewer->loadSomaContainer ( directory );

      //Load the compo options
      QStringList filters;
      filters << "*.obj" << "*.off" << "*.ply" << "*.stl";

      QStringList list = directory.entryList ( filters, QDir::Files );
      ui.comboBox_ModelledSomas->addItems ( list );
    }
  }
}

void SomaDeformerWidget::loadSWCFile ( )
{
  if ( ui.checkBox_useSWCFile->isChecked ( ))
  {

    QString fileName = QFileDialog::getOpenFileName ( this, tr ( "Open File" ), "./", tr ( "NeuroMorpho(*.swc)" ));

    if ( !fileName.isNull ( ))
      viewer->loadSWCFile ( fileName );
    else
    {
      ui.checkBox_useSWCFile->setCheckState ( Qt::CheckState::Unchecked );
    }
  }
  else
  {
    viewer->unLoadSWCFile ( );
  }
}

void SomaDeformerWidget::loadPatternMesh ( )
{
  QString fileName = QFileDialog::getOpenFileName ( this, tr ( "Open File" ), "./", tr ( "Mesh(*.obj)" ));

  if ( !fileName.isNull ( ))
    viewer->loadPatternMesh ( fileName );
}

void SomaDeformerWidget::loadXMLSomaDef ( )
{
  QString fileName = QFileDialog::getOpenFileName ( this, tr ( "Open File" ), "./", tr ( "Soma Def(*.xml)" ));

  if ( fileName.isNull ( ))
  {
    ui.radioButton_ProceduralSomas->setChecked ( true );
    mXMLFile = "";
  }
  else
  {
    mXMLFile = fileName;
    generateSoma ( );
  }

}

void SomaDeformerWidget::loadPredefinedXMLSomaDef ( )
{
  generateSoma ( );

  ui.pushButton_FinalizeSoma->setEnabled ( false );
  ui.pushButton_NextStep->setEnabled ( false );
}

void SomaDeformerWidget::loadPredefinedXMLSomaDefAndGOAdvancedOptions ( )
{
  loadPredefinedXMLSomaDef ( );
  ui.tabWidget_Controls->setCurrentIndex ( 1 );
}

void SomaDeformerWidget::loadPredefinedXMLSomaDefAndSimulate ( )
{
  loadPredefinedXMLSomaDef ( );
  startDeformation ( );
}

void SomaDeformerWidget::generateSoma ( )
{
  unsigned int lHResol = ui.spinBox_SomaHorResol->value ( );
  unsigned int lVResol = ui.spinBox_SomaVerResol->value ( );
  float lRadius = ui.doubleSpinBox_ScaleFactor->value ( );
  float lScale = ui.doubleSpinBox_ScaleFactor->value ( );

  if ( ui.checkBox_useSWCFile->isChecked ( ))
  {
    //Get the soma radius and the scale this
    lRadius = viewer->getSWCImporter ( )->getElementAt ( 1 ).radius*lScale;
  }

  //Procedural soma
  if ( ui.radioButton_ProceduralSomas->isChecked ( ))
  {
    viewer->generateSomaProcedural ( lRadius, lHResol, lVResol );
  }
  else
  {
    //Modeled soma
    if (( ui.comboBox_ModelledSomas->count ( ) >= 0 )
      && ( ui.radioButton_ModelledSomas->isChecked ( ))
      )
    {
      viewer->generateSomaModel ( ui.comboBox_ModelledSomas->currentIndex ( ), lRadius );
    }
    else
    {
      if ( ui.radioButton_XMLSoma->isChecked ( ))
      {
        viewer->generateSomaFromXML ( mXMLFile, lScale );
      }
    }
  }
}

void SomaDeformerWidget::startDeformation ( )
{

  if ( !viewer->isDeformating ( ))
  {
    if ( mMsgTimer != NULL )
      delete mMsgTimer;
    mMsgTimer = new QTimer ( this );
    connect ( mMsgTimer, SIGNAL( timeout ( )), this, SLOT( showContinueMsg ( )) );
    mMsgTimer->start ( mMiSecsSimulation );
  }

  viewer->startDeformation ( true );
}

void SomaDeformerWidget::singleStepDeformation ( )
{
  viewer->singleStepDeformation ( );
}

void SomaDeformerWidget::stopDeformation ( )
{
  viewer->startDeformation ( false );
}

void SomaDeformerWidget::setAnimationPeriod ( )
{
  viewer->reSetAnimationPeriod ( ui.spinBox_animationPeriod->value ( ));
}

void SomaDeformerWidget::setMSIntegrator ( )
{
  viewer->setMSIntegrator ( ui.comboBox_MS_Integrator_Method->currentIndex ( ));
}

void SomaDeformerWidget::setMSDt ( )
{
  viewer->setMSDt ( ui.doubleSpinBox_MS_Integrator_dt->value ( ));
}

void SomaDeformerWidget::setMSConstraints ( )
{
  viewer->setMSConstraints ( ui.checkBox_MS_Constraints->isChecked ( ));
}

void SomaDeformerWidget::setMSSpringParams ( )
{
  viewer->setMSSpringParams ( ui.doubleSpinBox_MS_Stiffness->value ( ), ui.doubleSpinBox_MS_Dumping->value ( ));
}

void SomaDeformerWidget::setMSNodeParams ( )
{
  float lMass = ui.doubleSpinBox_NodeMass->value ( );
  OpenMesh::Vec3f lVecGravity ( ui.doubleSpinBox_gravityForce_X->value ( ),
                                ui.doubleSpinBox_gravityForce_Y->value ( ),
                                ui.doubleSpinBox_gravityForce_Z->value ( )
  );

  viewer->setMSNodeParams ( lMass, lVecGravity );
}

void SomaDeformerWidget::setUnCollapseSprings ( )
{
  viewer->setUnCollapseSprings ( ui.spinBox_UnCollapseSprigns->value ( ));
}

void SomaDeformerWidget::setDendriticParams ( )
{
  viewer->setDendriticParams (
    ui.doubleSpinBox_BasalParam->value ( ), ui.doubleSpinBox_ApicalParam->value ( )
  );
}

void SomaDeformerWidget::exportModel ( )
{

  QString fileName = QFileDialog::getSaveFileName ( this );
  if ( fileName.isEmpty ( ))
  {
    return;
  }
  else
  {
    viewer->setNormalizeExportedModel ( ui.checkBox_NormalizeModel->isChecked ( ));
    viewer->exportModel ( fileName );

    //Export configuration name
    QString result = viewer->configurationToString ( );
    strToFile ( result.toStdString ( ), ( fileName.toStdString ( ) + ".cfg" ));
  }
}

void SomaDeformerWidget::exportModelWithSTDName ( )
{
  QString fileName = QDir::currentPath ( ) + "/tmp/SomaGenerated/SomaDeformed.obj";
  viewer->exportModel ( fileName );

  //Export configuration name
  QString result = viewer->configurationToString ( );
  strToFile ( result.toStdString ( ), ( fileName.toStdString ( ) + ".cfg" ));
}

void SomaDeformerWidget::setNormalizeExport ( )
{
  bool lNorm = ui.checkBox_NormalizeModel->isChecked ( );
  viewer->setNormalizeExportedModel ( lNorm );
}

void SomaDeformerWidget::setUseSphericalProjection ( )
{
  bool lParam = ui.checkBox_UseSphericalProjection->isChecked ( );
  viewer->setUseSphericalProjection ( lParam );
}

void SomaDeformerWidget::setUseContourPropagation ( )
{
  bool lParam = ui.checkBox_UseContourPropagation->isChecked ( );
  viewer->setUseContourPropagation ( lParam );
}

void SomaDeformerWidget::optimizateDendriticBase ( )
{
  viewer->optimizateDendriticBase ( );
  startDeformation ( );
  mMsgTimer->stop ( );
}

void SomaDeformerWidget::setExportOBJSecuence ( )
{
  viewer->setExportOBJSecuence ( ui.checkBox_ExportOBJSecuence->isChecked ( ));
}

void SomaDeformerWidget::setDeleteRedundantVertex ( )
{
  viewer->setDeleteRedundantVertex ( ui.checkBox_DeleteRedundantVertex->isChecked ( ));
}

void SomaDeformerWidget::emitFinishSoma ( )
{
  emit finishSoma ( );
}

void SomaDeformerWidget::deformDuringNSteps ( int pSteps )
{
  for ( int i = 0; i < pSteps; ++i )
    viewer->animate ( );
}

void SomaDeformerWidget::finalSomaOptimization ( )
{
  if ( viewer->isDeformating ( ))
  {
    optimizateDendriticBase ( );
    deformDuringNSteps ( 25 );
    viewer->setDeleteRedundantVertex ( true );
  }
  else
  {
    optimizateDendriticBase ( );
    deformDuringNSteps ( 2 );
    viewer->setDeleteRedundantVertex ( true );
  }

}

void SomaDeformerWidget::finalizeSoma ( )
{
  exportModelWithSTDName();
  emit finishSoma ( );
}

void SomaDeformerWidget::restoreDefaultValues ( )
{
  ui.doubleSpinBox_ScaleFactor->setValue ( 1.0 );
  ui.spinBox_UnCollapseSprigns->setValue ( 25 );
  ui.doubleSpinBox_NodeMass->setValue ( 0.64 );
  ui.doubleSpinBox_MS_Stiffness->setValue ( 20.0 );
  ui.doubleSpinBox_MS_Dumping->setValue ( 0.05 );
  ui.doubleSpinBox_MS_Integrator_dt->setValue ( 0.005 );

  setMSDt ( );
  setMSSpringParams ( );
  setMSNodeParams ( );
  setUnCollapseSprings ( );

  stopDeformation ( );
  loadPredefinedXMLSomaDef ( );
}

void SomaDeformerWidget::showContinueMsg ( )
{
  mMsgTimer->stop ( );

  QMessageBox Msgbox;
  QString lMsge = "The soma is ready, if you like its shape, go to \"Next step\".\n";
  lMsge += "Otherwise, press \"Rebuild (advanced options)\".";

  Msgbox.setWindowTitle ( "Neuronize" );
  Msgbox.setText ( lMsge );
  Msgbox.exec ( );

  ui.pushButton_FinalizeSoma->setEnabled ( true );
  ui.pushButton_NextStep->setEnabled ( true );
}


