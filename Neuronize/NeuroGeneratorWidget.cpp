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

#include "NeuroGeneratorWidget.h"
#include "neuronize.h"

#include <libs/libQtNeuroUtils/QtThreadsManager.hpp>

//#include <QtGui>

NeuroGeneratorWidget::NeuroGeneratorWidget (const QString &tmpDir, QWidget *parent )
  : QWidget ( parent )
{
  ui.setupUi ( this );

  viewer = new NeuroGeneratorWidgetViewer ( this );

  //Add the viewer component to the interface
  ui.verticalLayout_Viewer->addWidget ( viewer );

  //Configure the compnent
  viewer->setupViewer ( );

  mTempDir = tmpDir;
  mSWCFleName = "";

    hideAdvancedOptions();

  setSpinesDistributionParams ( );

  QObject::connect ( ui.pushButton_OpenNeuron, SIGNAL( clicked ( )), this, SLOT( loadNeuronModel ( )) );
  QObject::connect ( ui.pushButton_ColorFromSWC, SIGNAL( clicked ( )), this, SLOT( setColorFromSWC ( )) );

  QObject::connect ( ui.pushButton_OpenSpines, SIGNAL( clicked ( )), this, SLOT( loadSpinesModel ( )) );

  QObject::connect ( ui.pushButton_LoadNeuronDefinition, SIGNAL( clicked ( )), this, SLOT( loadMorphologyFile ( )) );

  QObject::connect ( ui.pushButton_GenerateNeuronMesh, SIGNAL( clicked ( )), this, SLOT( generateDendrites ( )) );

  QObject::connect ( ui.pushButton_ApplyNoise, SIGNAL( clicked ( )), this, SLOT( applyNoise ( )) );
  QObject::connect ( ui.pushButton_ExportModel, SIGNAL( clicked ( )), this, SLOT( exportNeuron ( )) );
  QObject::connect ( ui.pushButton_ApplySmooth, SIGNAL( clicked ( )), this, SLOT( applySmooth ( )) );

  QObject::connect ( ui.pushButton_SmoothDendrites, SIGNAL( clicked ( )), this, SLOT( applySmooth ( )) );

  QObject::connect ( ui.pushButton_generateSpines, SIGNAL( clicked ( )), this, SLOT( generateSpines ( )) );
  QObject::connect ( ui.pushButton_ExportSpines, SIGNAL( clicked ( )), this, SLOT( exportSpines ( )) );

  QObject::connect ( ui.checkBox_RenderSolid, SIGNAL( clicked ( )), this, SLOT( ControlStateRender ( )) );
  QObject::connect ( ui.checkBox_RenderEdges, SIGNAL( clicked ( )), this, SLOT( ControlStateRender ( )) );
  QObject::connect ( ui.checkBox_RenderNormals, SIGNAL( clicked ( )), this, SLOT( ControlStateRender ( )) );
  QObject::connect ( ui.checkBox_RenderVertexs, SIGNAL( clicked ( )), this, SLOT( ControlStateRender ( )) );

  QObject::connect ( ui.doubleSpinBox_NormalLong, SIGNAL( editingFinished ( )), this, SLOT( setNormalScale ( )) );
  QObject::connect ( ui.doubleSpinBox_VertexLong, SIGNAL( editingFinished ( )), this, SLOT( setPixelScale ( )) );

  QObject::connect ( ui.spinBox_LightType, SIGNAL( editingFinished ( )), this, SLOT( ControlLights ( )) );

  //Debug configurations
  QObject::connect ( ui.checkBox_ActiveDebug, SIGNAL( clicked ( )), this, SLOT( SetDebugMode ( )) );
  QObject::connect ( ui.radioButton_DebugSegments, SIGNAL( clicked ( )), this, SLOT( SetDebugMode ( )) );
  QObject::connect ( ui.radioButton_DebugDendriticDistanceNorm, SIGNAL( clicked ( )), this, SLOT( SetDebugMode ( )) );
  QObject::connect ( ui.radioButton_DebugDendriticDistanceReal, SIGNAL( clicked ( )), this, SLOT( SetDebugMode ( )) );
  QObject::connect ( ui.radioButton_DendriticDistanceSoma, SIGNAL( clicked ( )), this, SLOT( SetDebugMode ( )) );
  QObject::connect ( ui.radioButton_DebugVertex, SIGNAL( clicked ( )), this, SLOT( SetDebugMode ( )) );
  QObject::connect ( ui.radioButton_DebugFaces, SIGNAL( clicked ( )), this, SLOT( SetDebugMode ( )) );
  QObject::connect ( ui.spinBox_DebugRangeIni, SIGNAL( editingFinished ( )), this, SLOT( SetDebugMode ( )) );
  QObject::connect ( ui.spinBox_DebugRangeFin, SIGNAL( editingFinished ( )), this, SLOT( SetDebugMode ( )) );

  QObject::connect ( ui.pushButton_OnlyImportSWC, SIGNAL( clicked ( )), this, SLOT( loadSWCFile ( )) );

  QObject::connect ( ui.radioButton_ModeledSoma, SIGNAL( clicked ( )), this, SLOT( LoadSomaModels ( )) );
  QObject::connect ( ui.radioButton_ProceduralSoma, SIGNAL( clicked ( )), this, SLOT( LoadSomaModels ( )) );
  QObject::connect ( ui.pushButton_BrowseSomaModels, SIGNAL( clicked ( )), this, SLOT( ReLoadSomaModels ( )) );

  QObject::connect ( ui.comboBox_SomaModels, SIGNAL( currentIndexChanged ( int )), this, SLOT( setSomaModel ( )) );

  QObject::connect ( ui.radioButton_ModelSpines, SIGNAL( clicked ( )), this, SLOT( LoadSpinesModels ( )) );
  QObject::connect ( ui.radioButton_ProceduralSpines, SIGNAL( clicked ( )), this, SLOT( setSpinesTypes ( )) );
  QObject::connect ( ui.radioButton_RealSpines, SIGNAL( clicked ( )), this, SLOT( LoadSpinesModels ( )) );
  QObject::connect ( ui.radioButton_SemiRealSpines, SIGNAL( clicked ( )), this, SLOT( LoadSpinesModels ( )) );
  QObject::connect ( ui.radioButton_SegmentSpines, SIGNAL( clicked ( )), this, SLOT( LoadSpinesModels ( )) );

  QObject::connect ( ui.doubleSpinBox_SomaScale, SIGNAL( editingFinished ( )), this, SLOT( setSomasScale ( )) );
  QObject::connect ( ui.doubleSpinBox_SpinesScale, SIGNAL( editingFinished ( )), this, SLOT( setSpinesScale ( )) );

  QObject::connect ( ui.doubleSpinBox_SpineSeparation,
                     SIGNAL( editingFinished ( )),
                     this,
                     SLOT( setSpinesDistributionParams ( )) );
  QObject::connect ( ui.doubleSpinBox_BProgFactor,
                     SIGNAL( editingFinished ( )),
                     this,
                     SLOT( setSpinesDistributionParams ( )) );
  QObject::connect ( ui.doubleSpinBox_AProgFactor,
                     SIGNAL( editingFinished ( )),
                     this,
                     SLOT( setSpinesDistributionParams ( )) );

  QObject::connect ( ui.doubleSpinBox_basalMinDistance,
                     SIGNAL( editingFinished ( )),
                     this,
                     SLOT( setSpinesDistributionParams ( )) );
  QObject::connect ( ui.doubleSpinBox_BasalCteDIstance,
                     SIGNAL( editingFinished ( )),
                     this,
                     SLOT( setSpinesDistributionParams ( )) );
  QObject::connect ( ui.doubleSpinBox_apicalMinDistance,
                     SIGNAL( editingFinished ( )),
                     this,
                     SLOT( setSpinesDistributionParams ( )) );
  QObject::connect ( ui.doubleSpinBox_apicalCteDistance,
                     SIGNAL( editingFinished ( )),
                     this,
                     SLOT( setSpinesDistributionParams ( )) );

  QObject::connect ( ui.checkBox_piramidalSoma, SIGNAL( clicked ( )), this, SLOT( setPiramidalSoma ( )) );

  QObject::connect ( ui.radioButton_NeuronTypeInter, SIGNAL( clicked ( )), this, SLOT( setNeuronType ( )) );
  QObject::connect ( ui.radioButton_NeuronTypePiram, SIGNAL( clicked ( )), this, SLOT( setNeuronType ( )) );

  QObject::connect ( ui.checkBox_GenerateMesh,
                     SIGNAL( clicked ( )),
                     this,
                     SLOT( generateNeuronMeshFromInternalInfo ( )) );
  QObject::connect ( ui.checkBox_GenerateSegments, SIGNAL( clicked ( )), this, SLOT( generateNeuronLines ( )) );
  QObject::connect ( ui.checkBox_SphereNodes, SIGNAL( clicked ( )), this, SLOT( generateNeuronSphereNodes ( )) );

  QObject::connect ( ui.pushButton_Subdivide, SIGNAL( clicked ( )), this, SLOT( applySubdivide ( )) );
  QObject::connect ( ui.pushButton_Decimate, SIGNAL( clicked ( )), this, SLOT( applyDecimate ( )) );

  QObject::connect ( ui.pushButton_GenerateDendrites, SIGNAL( clicked ( )), this, SLOT( generateDendrites ( )) );

  QObject::connect ( ui.pushButton_GoToGenerateSpines,
                     SIGNAL( clicked ( )),
                     this,
                     SLOT( emitFinishNeuronSurfaceAndGoToSpines ( )) );
  QObject::connect ( ui.pushButton_NextStep,
                     SIGNAL( clicked ( )),
                     this,
                     SLOT( emitFinishNeuronSurfaceAndGoToSpines ( )) );

  QObject::connect ( ui.pushButton_FinishNeuronAndRestart,
                     SIGNAL( clicked ( )),
                     this,
                     SLOT( emitFinishNeuronSurfaceAndRestart ( )) );

  QObject::connect ( ui.pushButton_RebuildWithAdvancedOptions,
                     SIGNAL( clicked ( )),
                     this,
                     SLOT( RebuildWithAdvancedOptions ( )) );

  QObject::connect ( ui.pushButton_FinishNeuron, SIGNAL( clicked ( )), this, SLOT( emitFinishSpinesAndRestart ( )) );

  //Modificaciones Presentaci�n -> Javier de DeFelipe
  ui.checkBox_piramidalSoma->setCheckState (( Qt::Checked ));
  setPiramidalSoma ( );

  ui.checkBox_RenderNormals->setCheckState (( Qt::Unchecked ));
  ui.checkBox_RenderVertexs->setCheckState (( Qt::Unchecked ));
  ControlStateRender ( );

  ui.comboBox_SmoothContinuity->addItem ( "C0" );
  ui.comboBox_SmoothContinuity->addItem ( "C1" );
  ui.comboBox_SmoothContinuity->addItem ( "C2" );

  //ui.comboBox_SubdividerType->addItem("None");
  ui.comboBox_SubdividerType->addItem ( "U. Loop" );
  ui.comboBox_SubdividerType->addItem ( "U. Sqrt3" );
  ui.comboBox_SubdividerType->addItem ( "U. I. Sqrt3" );
  ui.comboBox_SubdividerType->addItem ( "M. Butterfly" );

    for (int i = 0; i < 6; ++i)
    ui.tabWidget_RenderControl->removeTab ( 0 );


  QObject::connect ( ui.pushButton_SpinesCtrl_Rebuild, SIGNAL( clicked ( )), this, SLOT( destroyAllGroupsSpines ( )) );
  QObject::connect ( ui.pushButton_SpinesCtrl_Save, SIGNAL( clicked ( )), this, SLOT( exportNeuronAndSpines ( )) );
  QObject::connect ( ui.pushButton_SaveNeuronSurface, SIGNAL( clicked ( )), this, SLOT( exportNeuron ( )) );
  QObject::connect ( ui.pushButton_GoSpinesAdvancedOptions,
                     SIGNAL( clicked ( )),
                     this,
                     SLOT( goAdvencedSpinesOptions ( )) );

  mMsgTimer = NULL;
  mMiSecsSimulation = 100;

  msgBox = NULL;

  setSpinesDistributionParams ( );

  //Escalar las espinas al doble
  ui.doubleSpinBox_SpinesScale->setValue ( 3.0 );
  setSpinesScale ( );

  QObject::connect ( ui.pushButton_exportSpinesInfo, SIGNAL( clicked ( )), this, SLOT( exportSpinesInfo ( )) );
  QObject::connect ( ui.pushButton_importSpinesInfo, SIGNAL( clicked ( )), this, SLOT( importSpinesInfo ( )) );

}

void NeuroGeneratorWidget::loadMorphologyFile ( )
{
  mSWCFleName = loadSWCFile ( );
  generateNeuron ( );
}

void NeuroGeneratorWidget::loadMorphologyFile ( QString lSWCFile )
{
  mSWCFleName = lSWCFile;
  viewer->loadSWCFile ( mSWCFleName );
  generateNeuron ( );

  ui.pushButton_GoToGenerateSpines->setEnabled ( true );
  ui.pushButton_FinishNeuronAndRestart->setEnabled ( true );
  ui.pushButton_SmoothDendrites->setEnabled ( true );
}

void NeuroGeneratorWidget::generateDendrites ( )
{
  if ( msgBox != NULL )
    delete msgBox;
  msgBox = new QMessageBox ( NULL );

  QString lMsge = "The dendrites will be build now,\n";
  lMsge += "please wait until finish.";// in a few secods.";

  msgBox->setWindowTitle ( "Neuronize" );
  msgBox->setText ( lMsge );

  msgBox->open ( );

  if ( mMsgTimer != NULL )
    delete mMsgTimer;
  mMsgTimer = new QTimer ( this );
  connect ( mMsgTimer, SIGNAL( timeout ( )), this, SLOT( loadNeuronDefinitionAndGenerateMesh ( )) );
  mMsgTimer->start ( mMiSecsSimulation );
}

void NeuroGeneratorWidget::loadNeuronDefinitionAndGenerateMesh ( )
{
  viewer->loadSWCFile ( mSWCFleName );

  generateNeuron ( );

  viewer->setNeuronColor ( MeshDef::Color ( 0.5, 0.5, 1.0, 0.5 ));

  ui.pushButton_GoToGenerateSpines->setEnabled ( true );

  ui.pushButton_GoToGenerateSpines->setEnabled ( true );
  ui.pushButton_NextStep->setEnabled ( true );
  ui.pushButton_SmoothDendrites->setEnabled ( true );


  QFileInfo fi (mSWCFleName);
  bool neuronAdded = Neuronize::bbdd.addNeuron(fi.baseName().toStdString(),mSWCFleName.toStdString());

  if (!neuronAdded) {
      MeshVCG somaMesh(mTempDir.toStdString() + "/RealSize.obj");
      Neuronize::bbdd.addSoma(fi.baseName().toStdString(), somaMesh, BBDD::Spring_Mass, this->contours);

      SWCImporter *importer = this->viewer->getNeuroSWC()->getImporter();
      for (const auto &dendritic : importer->getDendritics()) {
          Neuronize::bbdd.addDendrite(fi.baseName().toStdString(), dendritic.initialNode, dendritic.finalNode,
                                      dendritic.type);
      }
  }


  viewer->updateGL ( );

  if ( mMsgTimer != NULL )
    mMsgTimer->stop ( );
}

NeuroGeneratorWidget::~NeuroGeneratorWidget ( )
{
  delete viewer;
  if ( mMsgTimer != NULL )
    delete mMsgTimer;
}

void NeuroGeneratorWidget::loadNeuronModel ( )
{

  QString fileName = QFileDialog::getOpenFileName ( this, tr ( "Open File" ), "./", tr ( "Mesh(*.obj)" ));

  if ( !fileName.isNull ( ))
  {
    viewer->loadNeuronModel ( fileName );

    unsigned int lTHorResol = ui.spinBox_TubesHorResolution->value ( );
    unsigned int lTVerResol = ui.spinBox_TubesVerResolution->value ( );
    unsigned int lSHorResol = ui.spinBox_SomaHorResolution->value ( );
    unsigned int lSVerResol = ui.spinBox_SomaHorResolution->value ( );

    viewer->getNeuroSWC ( )->setAllParams ( lTHorResol, lTVerResol, lSHorResol, lSVerResol );
  }
}

void NeuroGeneratorWidget::loadSTDSoma(bool scale)
{
  viewer->loadNeuronModel ( mTempDir + "/SomaGenerated/SomaDeformed.obj" );


  viewer->loadSWCFile ( mSWCFleName );
  if (scale) {
    viewer->adaptSomaMOdelWithSWCInfo();
  }
}

void NeuroGeneratorWidget::loadSpinesModel ( )
{

  QString fileName = QFileDialog::getOpenFileName ( this, tr ( "Open File" ), "./", tr ( "Mesh(*.obj)" ));
  if ( !fileName.isNull ( ))
  {
    viewer->loadSpinesModel ( fileName );

    unsigned int lNumSpines = ui.spinBox_NumSpines->value ( );
    unsigned int lHorizontalResol = ui.spinBox_SpinesHorResolution->value ( );
    unsigned int lVerticalResol = ui.spinBox_SpinesVerResolution->value ( );
    float lMinLongitudSpine = ui.doubleSpinBox_MinLongSpine->value ( );
    float lMaxLongitudSpine = ui.doubleSpinBox_MaxLongSpine->value ( );
    float lMinRadio = ui.doubleSpinBox_MinRadioSpine->value ( );
    float lMaxRadio = ui.doubleSpinBox_MaxRadioSpine->value ( );

    viewer->getSpinesSWC ( )->setAllParams ( lNumSpines,
                                             lHorizontalResol,
                                             lVerticalResol,
                                             lMinLongitudSpine,
                                             lMaxLongitudSpine,
                                             lMinRadio,
                                             lMaxRadio );
  }
}

void NeuroGeneratorWidget::loadSpinesModelFromPath ( QString pPath )
{
  if ( viewer->getNumModelsSpines ( ) == 0 )
  {
    QDir directory;
    QString path = pPath;
    if ( path.isNull ( ) == false )
    {
      directory.setPath ( path );
      viewer->loadSpinesContainer ( directory );
    }
  }

  if ( ui.radioButton_ModelSpines->isChecked ( ))
    viewer->SetSpinesType ( 1 );
  if ( ui.radioButton_RealSpines->isChecked ( ))
    viewer->SetSpinesType ( 2 );
  if ( ui.radioButton_SemiRealSpines->isChecked ( ))
    viewer->SetSpinesType ( 3 );
  if ( ui.radioButton_SegmentSpines->isChecked ( ))
    viewer->SetSpinesType ( 4 );
  else
    viewer->SetSpinesType ( 0 );

}

QString NeuroGeneratorWidget::loadSWCFile ( )
{
  QString fileName = QFileDialog::getOpenFileName ( this, tr ( "Open File" ), "./", tr ( "NeuroMorpho(*.swc)" ));

  if ( !fileName.isNull ( ))
  {
    viewer->loadSWCFile ( fileName );
  }

  return fileName;
}

void NeuroGeneratorWidget::generateNeuron ( )
{
  if ( mSWCFleName != "" )
  {
    generateNeuronMeshFromFile ( mSWCFleName );
    generateNeuronLines ( );
    generateNeuronSphereNodes ( );
  }
}

void NeuroGeneratorWidget::generateNeuronMeshFromFile ( QString fileName )
{
  unsigned int lTHorResol = ui.spinBox_TubesHorResolution->value ( );
  unsigned int lTVerResol = ui.spinBox_TubesVerResolution->value ( );
  unsigned int lSHorResol = ui.spinBox_SomaHorResolution->value ( );
  unsigned int lSVerResol = ui.spinBox_SomaHorResolution->value ( );

  if ( ui.checkBox_GenerateMesh->isChecked ( ))
  {
    if ( !fileName.isNull ( ))
    {
      viewer->generateNeuronMeshFromFile ( fileName, lTHorResol, lTVerResol, lSHorResol, lSVerResol );
    }
  }
  else
  {
    viewer->destroyNeuronMesh ( );
  }
}

void NeuroGeneratorWidget::generateNeuronMeshFromInternalInfo ( )
{
  unsigned int lTHorResol = ui.spinBox_TubesHorResolution->value ( );
  unsigned int lTVerResol = ui.spinBox_TubesVerResolution->value ( );
  unsigned int lSHorResol = ui.spinBox_SomaHorResolution->value ( );
  unsigned int lSVerResol = ui.spinBox_SomaHorResolution->value ( );

  if ( ui.checkBox_GenerateMesh->isChecked ( ))
  {
    viewer->generateNeuronMeshFromInternalInfo ( lTHorResol, lTVerResol, lSHorResol, lSVerResol );
  }
  else
  {
    viewer->destroyNeuronMesh ( );
  }
}

void NeuroGeneratorWidget::generateNeuronLines ( )
{
  bool lGenLines = ui.checkBox_GenerateSegments->isChecked ( );

  viewer->generateNeuronLines ( lGenLines );
}

void NeuroGeneratorWidget::generateNeuronSphereNodes ( )
{
  bool lGenSpheres = ui.checkBox_SphereNodes->isChecked ( );

  viewer->generateNeuronSphereNodes ( lGenSpheres );
}

void NeuroGeneratorWidget::generateSpines ( )
{
  QString text = QObject::tr (
    "This proces can take several minutes.\n Do you still want to continue?" );
  QMessageBox msgBox ( QObject::tr ( "Build spines" ),
                       text,
                       QMessageBox::Warning,
                       QMessageBox::Yes | QMessageBox::Default,
                       QMessageBox::No | QMessageBox::Escape,
                       QMessageBox::NoButton );

  if ( msgBox.exec ( ) == QMessageBox::No )
  {
    ui.tabWidget_RenderControl->setCurrentIndex ( 0 );
    return;
  }
  unsigned int lNumSpines = ui.spinBox_NumSpines->value ( );
  unsigned int lHorResol = ui.spinBox_SpinesHorResolution->value ( );
  unsigned int lVerResol = ui.spinBox_SpinesVerResolution->value ( );

  float lMinLongSpine = ui.doubleSpinBox_MinLongSpine->value ( );
  float lMaxLongSpine = ui.doubleSpinBox_MaxLongSpine->value ( );

  float lMinRadio = ui.doubleSpinBox_MinRadioSpine->value ( );
  float lMaxRadio = ui.doubleSpinBox_MaxRadioSpine->value ( );

  unsigned int lNumOfGroups = ui.spinBox_NumOfGroupsModeledSpines->value ( );
  unsigned int lGenerateOption = 0;

  if ( ui.radioButton_ProceduralSpines->isChecked ( ))
    lGenerateOption = 0;
  else if ( ui.radioButton_ModelSpines->isChecked ( ))
    lGenerateOption = 1;
  else if ( ui.radioButton_SemiRealSpines->isChecked ( ))
    lGenerateOption = 2;
  else if ( ui.radioButton_RealSpines->isChecked ( ))
    lGenerateOption = 3;
  else if ( ui.radioButton_SegmentSpines->isChecked ( ))
    lGenerateOption = 4;
  else if (ui.radioButton_VrmlSpines->isChecked ( ))
    lGenerateOption = 5;
  else if (ui.radioButton_RealAscPos->isChecked ( ))
    lGenerateOption = 6;
  else if (ui.radioButton_ImarisSpines->isChecked ( ))
      lGenerateOption = 7;
  else if (ui.radioButton_RepairedImarisSpines->isChecked())
      lGenerateOption = 8;



//  lGenerateOption = 0;
//  viewer->SetSpinesType(lGenerateOption);
 std::cout<<"-->> Opcion elegida:"<<lGenerateOption<<std::endl;
  switch ( lGenerateOption )
  {
    //Rand Procedurales
    case 0: viewer->SetSpinesType ( 0 );
      viewer->generateSomeSpinesGroup ( lNumSpines,
                                        lNumOfGroups,
                                        lHorResol,
                                        lVerResol,
                                        lMinLongSpine,
                                        lMaxLongSpine,
                                        lMinRadio,
                                        lMaxRadio );
      break;

      //Rand Modeled
    case 1: viewer->SetSpinesType ( 1 );
      viewer->generateSomeSpinesGroup ( lNumSpines,
                                        lNumOfGroups,
                                        lHorResol,
                                        lVerResol,
                                        lMinLongSpine,
                                        lMaxLongSpine,
                                        lMinRadio,
                                        lMaxRadio );
      break;

    case 2: viewer->generateSemiRealSpinesGroup ( lNumSpines,
                                                  lHorResol,
                                                  lVerResol,
                                                  lMinLongSpine,
                                                  lMaxLongSpine,
                                                  lMinRadio,
                                                  lMaxRadio );
      break;

      //Real distrig, using the distribution functiones
    case 3: viewer->generateRealSpinesGroup ( lNumSpines,
                                              lHorResol,
                                              lVerResol,
                                              lMinLongSpine,
                                              lMaxLongSpine,
                                              lMinRadio,
                                              lMaxRadio );
      break;

      //Simply real distribution (gived by Ruth)
    case 4: viewer->generateSpinesInSegment ( lNumSpines,
                                              lHorResol,
                                              lVerResol,
                                              lMinLongSpine,
                                              lMaxLongSpine,
                                              lMinRadio,
                                              lMaxRadio );
      break;
      case 5:
          viewer->generateSpinesVrml(this->neuron, mTempDir.toStdString());
          break;
      case 6:
      viewer->generateSpinesASC(this->spines,lHorResol,
                                lVerResol,
                                lMinLongSpine,
                                lMaxLongSpine,
                                lMinRadio,
                                lMaxRadio );
      break;
      case 7:
          viewer->generateSpinesImaris(this->neuron, mTempDir.toStdString());
          break;
      case 8:
          viewer->generateRepairedImarisSpines(this->neuron, mTempDir.toStdString());
  }
}

void NeuroGeneratorWidget::batchSpinesGeneration(skelgenerator::Neuron *pNeuron, vector<Spine> spines) {
  unsigned int lNumSpines = ui.spinBox_NumSpines->value();
  unsigned int lHorResol = ui.spinBox_SpinesHorResolution->value();
  unsigned int lVerResol = ui.spinBox_SpinesVerResolution->value();

  float lMinLongSpine = ui.doubleSpinBox_MinLongSpine->value();
  float lMaxLongSpine = ui.doubleSpinBox_MaxLongSpine->value();

  float lMinRadio = ui.doubleSpinBox_MinRadioSpine->value();
  float lMaxRadio = ui.doubleSpinBox_MaxRadioSpine->value();

  unsigned int lNumOfGroups = ui.spinBox_NumOfGroupsModeledSpines->value();
  unsigned int lGenerateOption = 0;

  if (pNeuron != nullptr) {
    std::cout<<"---------------------------------------------->> Opcion elegida: 5"<<"----------------------------------" <<std::endl << std::flush;
      viewer->generateSpinesVrml(pNeuron,mTempDir.toStdString());
  } else if (!spines.empty()) {
    std::cout<<"---------------------------------------------->> Opcion elegida: 6"<<"----------------------------------" <<std::endl << std::flush;
    viewer->generateSpinesASC(spines, lHorResol,
                              lVerResol,
                              lMinLongSpine,
                              lMaxLongSpine,
                              lMinRadio,
                              lMaxRadio);
  } else {

      if (ui.radioButton_ProceduralSpines->isChecked())
          lGenerateOption = 0;
      else if (ui.radioButton_ModelSpines->isChecked())
          lGenerateOption = 1;
      else if (ui.radioButton_SemiRealSpines->isChecked())
          lGenerateOption = 2;
      else if (ui.radioButton_RealSpines->isChecked())
          lGenerateOption = 3;
      else if (ui.radioButton_SegmentSpines->isChecked())
          lGenerateOption = 4;


      std::cout << "---------------------------------------------->> Opcion elegida:" << lGenerateOption
                << "<<----------------------------------" << std::endl << std::flush;

      switch (lGenerateOption) {
          //Rand Procedurales
          case 0:
              viewer->SetSpinesType(0);
              viewer->generateSomeSpinesGroup(lNumSpines,
                                              lNumOfGroups,
                                              lHorResol,
                                              lVerResol,
                                              lMinLongSpine,
                                              lMaxLongSpine,
                                              lMinRadio,
                                              lMaxRadio);
              break;

              //Rand Modeled
          case 1:
              viewer->SetSpinesType(1);
              viewer->generateSomeSpinesGroup(lNumSpines,
                                              lNumOfGroups,
                                              lHorResol,
                                              lVerResol,
                                              lMinLongSpine,
                                              lMaxLongSpine,
                                              lMinRadio,
                                              lMaxRadio);
              break;

              //???
          case 2:
              viewer->generateSemiRealSpinesGroup(lNumSpines,
                                                  lHorResol,
                                                  lVerResol,
                                                  lMinLongSpine,
                                                  lMaxLongSpine,
                                                  lMinRadio,
                                                  lMaxRadio);
              break;

              //Real distrig, using the distribution functiones
          case 3:
              viewer->generateRealSpinesGroup(lNumSpines,
                                              lHorResol,
                                              lVerResol,
                                              lMinLongSpine,
                                              lMaxLongSpine,
                                              lMinRadio,
                                              lMaxRadio);
              break;

              //Simply real distribution (gived by Ruth)
          case 4:
              viewer->generateSpinesInSegment(lNumSpines,
                                              lHorResol,
                                              lVerResol,
                                              lMinLongSpine,
                                              lMaxLongSpine,
                                              lMinRadio,
                                              lMaxRadio);
              break;


      }
  }
}

void NeuroGeneratorWidget::attachNewSpinesGroup ( )
{
  unsigned int lNumSpines = ui.spinBox_NumSpines->value ( );
  unsigned int lHorResol = ui.spinBox_SpinesHorResolution->value ( );
  unsigned int lVerResol = ui.spinBox_SpinesVerResolution->value ( );

  float lMinLongSpine = ui.doubleSpinBox_MinLongSpine->value ( );
  float lMaxLongSpine = ui.doubleSpinBox_MaxLongSpine->value ( );

  float lMinRadio = ui.doubleSpinBox_MinRadioSpine->value ( );
  float lMaxRadio = ui.doubleSpinBox_MaxRadioSpine->value ( );

  viewer->attachNewSpines ( lNumSpines, lHorResol, lVerResol, lMinLongSpine, lMaxLongSpine, lMinRadio, lMaxRadio );
}

void NeuroGeneratorWidget::applyNoise ( )
{
  float lNoiseVal = ( float ) ui.doubleSpinBox_MaxNoise->value ( );
  bool lVal = ui.radioButton_ConstantNoise->isChecked ( );

  if ( ui.checkBox_NoiseNeuron->isChecked ( ))
    viewer->applyNoiseToNeuron ( lNoiseVal, lVal );

  if ( ui.checkBox_NoiseSoma->isChecked ( ))
    viewer->applyNoiseToSoma ( lNoiseVal, lVal );

  if ( ui.checkBox_NoiseDendritic->isChecked ( ))
    viewer->applyNoiseToDendritics ( lNoiseVal, lVal );

  if ( ui.checkBox_NoiseSpines->isChecked ( ))
  {
    unsigned int lHorResol = ui.spinBox_SpinesHorResolution->value ( );
    unsigned int lVerResol = ui.spinBox_SpinesVerResolution->value ( );

    unsigned int lLocalLimit = ( lHorResol*lVerResol ) + 1;
    unsigned int lMinDesp = lHorResol;

    viewer->applyNoiseToSpines ( lNoiseVal, lVal, lLocalLimit, lMinDesp );
  }

  viewer->update ( );
}

void NeuroGeneratorWidget::applySmooth ( )
{
  unsigned int lMethod = 0;
  unsigned int lTechnique = 0;
  unsigned int lIterations = ui.spinBox_SmoothIterations->value ( );

  if ( ui.radioButton_SmoothTangential->isChecked ( ))
  {
    lMethod = 1;
  }

  if ( ui.radioButton_SmoothTechnique_Laplacian->isChecked ( ))
  {
    lTechnique = 1;
  }

  unsigned int lContinuity = ui.comboBox_SmoothContinuity->currentIndex ( );

  if ( ui.checkBox_SmoothNeuron->isChecked ( ))
    viewer->applySmoothToNeuron ( lMethod, lTechnique, lContinuity, lIterations );
  if ( ui.checkBox_SmoothSpines->isChecked ( ))
    viewer->applySmoothToSpines ( lMethod, lTechnique, lContinuity, lIterations );

  viewer->update ( );
}

void NeuroGeneratorWidget::applySmooth ( unsigned int pIterations,
                                         unsigned int pMethod,
                                         unsigned int pTechnique,
                                         unsigned int pContinuity )
{
  unsigned int lMethod = pMethod;
  unsigned int lTechnique = pTechnique;
  unsigned int lIterations = pIterations;
  unsigned int lContinuity = pContinuity;

  if ( ui.checkBox_SmoothNeuron->isChecked ( ))
    viewer->applySmoothToNeuron ( lMethod, lTechnique, lContinuity, lIterations );
  if ( ui.checkBox_SmoothSpines->isChecked ( ))
    viewer->applySmoothToSpines ( lMethod, lTechnique, lContinuity, lIterations );

  viewer->update ( );
}

void NeuroGeneratorWidget::applySubdivide ( )
{
  viewer->applySubdivide ( ui.comboBox_SubdividerType->currentIndex ( ));
}

void NeuroGeneratorWidget::applyDecimate ( )
{
  viewer->applyDecimate ( ui.spinBox_DecimateSteps->value ( ));
}

void NeuroGeneratorWidget::exportNeuron ( )
{
  QString fileName = QFileDialog::getSaveFileName ( this, "Save neuron surface as:" );
  if ( fileName.isEmpty ( ))
  {
    return;
  }
  else
  {
    viewer->exportNeuron ( fileName + ".obj" );
  }
}

void NeuroGeneratorWidget::exportNeuron ( QString fileName )
{
  if ( fileName.isEmpty ( ))
  {
    return;
  }
  else
  {
    viewer->exportNeuron ( fileName + ".obj" );
  }
}

void NeuroGeneratorWidget::exportSpines ( )
{

  QString fileName = QFileDialog::getSaveFileName ( this, "Save spines as:" );

  if ( fileName.isEmpty ( ))
  {
    return;
  }
  else
  {
    unsigned int lNumMeshesToExport = viewer->getSpinesSynthContainersNumEles ( );

    if ( lNumMeshesToExport > 0 )
    {
      viewer->exportGroupOfSpines ( fileName );
    }
    else
    {
      viewer->exportSpines ( fileName );
    }
    viewer->exportSpinesInfo ( fileName );
  }
}

void NeuroGeneratorWidget::exportSpinesInmediatly ( QString fileName )
{
  unsigned int lNumMeshesToExport = viewer->getSpinesSynthContainersNumEles ( );

  viewer->exportSpines ( fileName );
  viewer->exportSpinesInfo ( fileName );
}

void NeuroGeneratorWidget::exportNeuronAndSpines ( )
{
  exportNeuron ( );

  if ( viewer->hasSpines ( ))
  {
    exportSpines ( );
  }
}

void NeuroGeneratorWidget::joinSpines ( )
{
  viewer->joinSpines ( );
}

void NeuroGeneratorWidget::ControlStateRender ( )
{
  viewer->ControlStateRender ( ui.checkBox_RenderSolid->isChecked ( ),
                               ui.checkBox_RenderEdges->isChecked ( ),
                               ui.checkBox_RenderNormals->isChecked ( ),
                               ui.checkBox_RenderVertexs->isChecked ( )
  );
}

void NeuroGeneratorWidget::setNormalScale ( )
{
  viewer->setNormalScale ( ui.doubleSpinBox_NormalLong->value ( ));
}

void NeuroGeneratorWidget::setPixelScale ( )
{
  viewer->setPixelScale ( ui.doubleSpinBox_VertexLong->value ( ));
}

void NeuroGeneratorWidget::ControlLights ( )
{
  viewer->ControlLights ( ui.spinBox_LightType->value ( ));
}

void NeuroGeneratorWidget::SetDebugMode ( )
{
  viewer->SetDebugMode ( ui.checkBox_ActiveDebug->isChecked ( ));

  MeshRenderer::DebugMode lAuxDebugMode;
  if ( ui.radioButton_DebugSegments->isChecked ( ))
    lAuxDebugMode = MeshRenderer::DebugMode::DEBUG_SEGMENTS;
  if ( ui.radioButton_DebugVertex->isChecked ( ))
    lAuxDebugMode = MeshRenderer::DebugMode::DEBUG_VERTEXS;
  if ( ui.radioButton_DebugFaces->isChecked ( ))
    lAuxDebugMode = MeshRenderer::DebugMode::DEBUG_FACES;
  if ( ui.radioButton_DebugDendriticDistanceNorm->isChecked ( ))
    lAuxDebugMode = MeshRenderer::DebugMode::DEN_DISTANCE_NORM;
  if ( ui.radioButton_DebugDendriticDistanceReal->isChecked ( ))
    lAuxDebugMode = MeshRenderer::DebugMode::DEN_DISTANCE_REAL;
  if ( ui.radioButton_DendriticDistanceSoma->isChecked ( ))
    lAuxDebugMode = MeshRenderer::DebugMode::DEN_DISTANCE_TO_SOMA;

  viewer->ConfigureDebug ( ui.spinBox_DebugRangeIni->value ( ), ui.spinBox_DebugRangeFin->value ( ), lAuxDebugMode );
}

void NeuroGeneratorWidget::ReLoadSomaModels ( )
{
  QDir directory;
  QString path = QFileDialog::getExistingDirectory ( this, tr ( "Directory" ), directory.path ( ));
  if ( path.isNull ( ) == false )
  {
    directory.setPath ( path );
    viewer->loadSomaContainer ( directory );

    QStringList filters;
    filters << "*.obj" << "*.off" << "*.ply" << "*.stl";

    QStringList list = directory.entryList ( filters, QDir::Files );
    ui.comboBox_SomaModels->addItems ( list );
  }

  setSomaModel ( );
}

void NeuroGeneratorWidget::ReLoadSomaModelsFromPath ( QString pPathToSomas )
{
  QDir directory;
  QString path = pPathToSomas;
  if ( path.isNull ( ) == false )
  {
    directory.setPath ( path );
    viewer->loadSomaContainer ( directory );

    //Load the compo options
    QStringList filters;
    filters << "*.obj" << "*.off" << "*.ply" << "*.stl";

    QStringList list = directory.entryList ( filters, QDir::Files );
    ui.comboBox_SomaModels->addItems ( list );

    ui.radioButton_ModeledSoma->setChecked ( true );
  }

  setSomaModel ( );
}

void NeuroGeneratorWidget::LoadSomaModels ( )
{
  if ( ui.radioButton_ModeledSoma->isChecked ( ))
  {
    if ( ui.comboBox_SomaModels->count ( ) == 0 )
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
        ui.comboBox_SomaModels->addItems ( list );
      }
    }
    setSomaModel ( );
  }
  else
  {
    viewer->SetSomaModel ( -1 );
  }
}

void NeuroGeneratorWidget::setSomaModel ( )
{
  viewer->SetSomaModel ( ui.comboBox_SomaModels->currentIndex ( ));
}

void NeuroGeneratorWidget::LoadSpinesModels ( )
{
  if (( ui.radioButton_ModelSpines->isChecked ( ))
    || ( ui.radioButton_RealSpines->isChecked ( ))
    || ( ui.radioButton_SemiRealSpines->isChecked ( ))
    || ( ui.radioButton_SegmentSpines->isChecked ( ))
    )
  {
    if ( viewer->getNumModelsSpines ( ) == 0 )
    {
      QDir directory;
      QString path = QFileDialog::getExistingDirectory ( this, tr ( "Directory" ), directory.path ( ));
      if ( path.isNull ( ) == false )
      {
        directory.setPath ( path );
        viewer->loadSpinesContainer ( directory );
      }
    }
    if ( ui.radioButton_ModelSpines->isChecked ( ))
      viewer->SetSpinesType ( 1 );
    if ( ui.radioButton_RealSpines->isChecked ( ))
      viewer->SetSpinesType ( 2 );
    if ( ui.radioButton_SemiRealSpines->isChecked ( ))
      viewer->SetSpinesType ( 3 );
    if ( ui.radioButton_SegmentSpines->isChecked ( ))
      viewer->SetSpinesType ( 4 );
  }
  else
  {
    viewer->SetSpinesType ( 0 );
  }
}

void NeuroGeneratorWidget::setSpinesTypes ( )
{
  if ( ui.radioButton_ProceduralSpines->isChecked ( ))
    viewer->SetSpinesType ( 0 );
  if ( ui.radioButton_ModelSpines->isChecked ( ))
    viewer->SetSpinesType ( 1 );
  if ( ui.radioButton_RealSpines->isChecked ( ))
    viewer->SetSpinesType ( 2 );
  if ( ui.radioButton_SemiRealSpines->isChecked ( ))
    viewer->SetSpinesType ( 3 );
}

void NeuroGeneratorWidget::setSomasScale ( )
{
  viewer->setSomaScaleFactor ( ui.doubleSpinBox_SomaScale->value ( ));
}

void NeuroGeneratorWidget::setSpinesScale ( )
{
  viewer->setSpineScaleFactor ( ui.doubleSpinBox_SpinesScale->value ( ));
}

void NeuroGeneratorWidget::setMultipleGroupsOfSpines ( )
{
  viewer->setMultipleGroupsOfSpines ( ui.checkBox_MultiplesGroupsOfSpines->isChecked ( ));
}

void NeuroGeneratorWidget::setColorFromSWC ( )
{
  QString fileName = QFileDialog::getOpenFileName ( this, tr ( "Open File" ), "./", tr ( "Meuro Def(*.swc)" ));

  if ( !fileName.isNull ( ))
  {
    viewer->setColorFromSWC ( fileName );
  }
}

void NeuroGeneratorWidget::setSpinesDistributionParams ( )
{
  float lSpinesDesp = ui.doubleSpinBox_SpineSeparation->value ( );
  float lBProgressionFactor = ui.doubleSpinBox_BProgFactor->value ( );
  float lAProgressionFactor = ui.doubleSpinBox_AProgFactor->value ( );

  float lBasalMinDistance = ui.doubleSpinBox_basalMinDistance->value ( );
  float lBasalCteDistance = ui.doubleSpinBox_BasalCteDIstance->value ( );
  float lApicalMinDistance = ui.doubleSpinBox_apicalMinDistance->value ( );
  float lApicalCteDistance = ui.doubleSpinBox_apicalCteDistance->value ( );

  viewer->setSpinesDistributionParams ( lSpinesDesp,
                                        lBProgressionFactor,
                                        lAProgressionFactor,
                                        lBasalMinDistance,
                                        lBasalCteDistance,
                                        lApicalMinDistance,
                                        lApicalCteDistance );

}

void NeuroGeneratorWidget::setPiramidalSoma ( )
{
  viewer->setPiramidalSoma ( ui.checkBox_piramidalSoma->isChecked ( ));
}

void NeuroGeneratorWidget::setNeuronType ( )
{
  NeuronType lNeuronType;
  if ( ui.radioButton_NeuronTypeInter->isChecked ( ))
  {
    lNeuronType = NeuronType::INTER;
  }
  else
  {
    if ( ui.radioButton_NeuronTypePiram->isChecked ( ))
    {
      lNeuronType = NeuronType::PIRAM;
    }
  }

  viewer->setNeuronType ( lNeuronType );

}

void NeuroGeneratorWidget::emitFinishNeuronSurfaceAndRestart ( )
{
  QString fileName = QFileDialog::getSaveFileName ( this );

  if ( !fileName.isNull ( ))
  {
    viewer->exportNeuron ( fileName );
  }

  emit finishNeuronSurfaceAndRestart ( );
}

void NeuroGeneratorWidget::emitFinishNeuronSurfaceAndGoToSpines ( )
{
  emit finishNeuronSurfaceAndGoToSpines ( );
}

void NeuroGeneratorWidget::emitFinishSpinesAndRestart ( )
{
  exportNeuron ( );

  if ( viewer->hasSpines ( ))
    exportSpines ( );

  emit finishSpinesAndRestart ( );
}

void NeuroGeneratorWidget::showDendirtesTab ( )
{
    if (ui.tabWidget_RenderControl->count() >= 2)
  {
    ui.tabWidget_RenderControl->removeTab ( 0 );
    ui.tabWidget_RenderControl->removeTab ( 0 );
  }

  ui.tabWidget_RenderControl->insertTab ( 0, ui.tab_DendritesCtrl, "Dendrites builder." );
  ui.tabWidget_RenderControl->insertTab ( 1, ui.tab_AdvanceDendritesCtrl, "Advanced dendrites options." );
    //ui.tabWidget_RenderControl->insertTab ( 2, ui.tab_ModifiersCtrl, "Modifiers Ctrl." );
}

void NeuroGeneratorWidget::showSpinesTab ( )
{

    if (ui.tabWidget_RenderControl->count() >= 2)
  {
    ui.tabWidget_RenderControl->removeTab ( 0 );
    ui.tabWidget_RenderControl->removeTab ( 0 );
  }

  ui.tabWidget_RenderControl->insertTab ( 0, ui.tab_SpinesCtrl, "Spines builder." );
  ui.tabWidget_RenderControl->insertTab ( 1, ui.tab_AdvanceSpinesCtrl, "Advanced spines options." );
    //ui.tabWidget_RenderControl->insertTab ( 2, ui.tab_MoreAdvanceSpinesCtrl, "More Advanced spines options." );
}

void NeuroGeneratorWidget::RebuildWithAdvancedOptions ( )
{
  loadSTDSoma(false);

  ui.tabWidget_RenderControl->setCurrentIndex ( 1 );

  ui.pushButton_GoToGenerateSpines->setEnabled ( false );
  ui.pushButton_NextStep->setEnabled ( false );
  ui.pushButton_SmoothDendrites->setEnabled ( false );
}

void NeuroGeneratorWidget::goAdvencedSpinesOptions ( )
{
  if (!this->spines.empty()) {
    ui.radioButton_RealAscPos->setEnabled(true);
    ui.radioButton_RealAscPos->setChecked(true);
  }

  if (this->neuron != nullptr) {
    ui.radioButton_VrmlSpines->setEnabled(true);
    ui.radioButton_VrmlSpines->setChecked(true);
  }

  if (this->neuron->hasImarisSpines()) {
      ui.radioButton_ImarisSpines->setEnabled(true);
      ui.radioButton_ImarisSpines->setChecked(true);
      if (Neuronize::hasPython){
          ui.radioButton_RepairedImarisSpines->setEnabled(true);
      }
  }

  ui.tabWidget_RenderControl->setCurrentIndex(1);
}


void NeuroGeneratorWidget::showMsjDendritesGeneration ( )
{

  QMessageBox Msgbox;
  QString lMsge = "The dendrites are being build,\n";
  lMsge += "please wait until finish in a few secods.";

  Msgbox.setWindowTitle ( "Neuronize" );
  Msgbox.setText ( lMsge );
  Msgbox.setWindowModality ( Qt::NonModal );

  Msgbox.exec ( );

  mMsgTimer->stop ( );
}

void NeuroGeneratorWidget::exportSpinesInfo ( )
{
  QString fileName = QFileDialog::getSaveFileName ( this, "Save spines info as:" );
  if ( fileName.isEmpty ( ))
  {
    return;
  }
  else
  {
    viewer->exportSpinesInfo ( fileName );
  }

}

void NeuroGeneratorWidget::importSpinesInfo ( )
{

  QString fileName = QFileDialog::getOpenFileName ( this, tr ( "Open File" ), "./", tr ( "Spines Info(*.SpinesInfo)" ));

  if ( !fileName.isNull ( ))
  {
    viewer->importSpinesInfo ( fileName );
  }
}

void NeuroGeneratorWidget::setNeuron(skelgenerator::Neuron *neuron) {
  NeuroGeneratorWidget::neuron = neuron;
}

void NeuroGeneratorWidget::setSpines(const vector<Spine> &spines) {
  NeuroGeneratorWidget::spines = spines;
}

void NeuroGeneratorWidget::setContours(const vector<vector<OpenMesh::Vec3d>> &contours) {
    NeuroGeneratorWidget::contours = contours;
}

void NeuroGeneratorWidget::hideAdvancedOptions() {
    //Spine Section
    ui.pushButton_exportSpinesInfo->hide();
    ui.pushButton_importSpinesInfo->hide();
    ui.groupBox_SpinesCtrlHide->hide();
    ui.groupBox_AdvanceSpinesCtrl_Shape->hide();
    ui.radioButton_AdvanceSpinesCtrl_LoadPositionsFromFile->hide();
    // Dendrite section
    ui.groupBox_ModifiersCtrlHide->hide();
    ui.groupBox_NeuronCtrlHide->hide();
    ui.groupBox_SpinesCtrlHide->hide();
    ui.groupBox_RenderCtrlHide->hide();
}
