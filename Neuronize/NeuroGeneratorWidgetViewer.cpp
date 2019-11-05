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

#include "NeuroGeneratorWidgetViewer.h"
#include "neuronize.h"
#include <math.h>
#include <SkelGeneratorUtil/Neuron.h>
#include <QtCore/QDirIterator>
#include <libs/libSysNeuroUtils/MathUtils.h>
#include <QtConcurrent/QtConcurrent>
#include <queue>
#include <libs/libGLNeuroUtils/MeshRenderer.h>
//#include <QtGui>

// Constructor must call the base class constructor.
#if QT_VERSION < 0x040000
                                                                                                                        NeuroGeneratorWidgetViewer::NeuroGeneratorWidgetViewer(QWidget *parent, const char *name)
  : QGLViewer(parent, name)
#else
NeuroGeneratorWidgetViewer::NeuroGeneratorWidgetViewer ( QWidget *parent )
  : QGLViewer ( parent )
#endif
{
  //restoreStateFromFile();
  //help();

  //camera()->setPosition( qglviewer::Vec(0,0,500) );
  //camera()->setOrientation( qglviewer::Quaternion( qglviewer::Vec(1,0,0), qglviewer::Vec(0,0,0) ) );
}

void NeuroGeneratorWidgetViewer::setupViewer ( )
{
  //Viewer configuration
  camera ( )->setZClippingCoefficient ( 500.0 );

  mesh = NULL;
  meshRend = NULL;

  meshSpines = NULL;
  spineMeshRend = NULL;

  mSWCImporter = NULL;

  mXMLProgressiveNeuroVizManager = NULL;

  mDebugMode = false;
  mSomaModelId = -1;
  mSpinesTypes = 0;

  mPiramidalSoma = false;
  mNeuronType = NeuronType::INTER;

  mSomaScaleFactor = 1.0;
  mSpineScaleFactor = 1.0;

  mSegmentList = 1;
  mSphereList = 2;

  mSphereSlices = 8;
  mSphereStacks = 8;

  mSchematicLines = true;
  mSchematicSpheres = true;

  renderMask = 0;
  renderMask = MeshRenderer::RENDER_SURFACE
    ^ MeshRenderer::RENDER_VERTICES
    ^ MeshRenderer::RENDER_EDGES
    ^ MeshRenderer::RENDER_NORMALS;

  InitLight ( );

  updateGL ( );

  QColor mBackGroundColor = QColor ( 250, 250, 250 );
  //QColor mBackGroundColor = QColor(0,0,0);
  //setBackgroundColor(mBackGroundColor);
}

void NeuroGeneratorWidgetViewer::draw ( )
{
  // Place light at camera position
  const qglviewer::Vec cameraPos = camera ( )->position ( );
  const GLfloat pos[4] = { ( float ) cameraPos[0], ( float ) cameraPos[1], ( float ) cameraPos[2], 1.0 };
  glLightfv ( GL_LIGHT1, GL_POSITION, pos );

  // Orientate light along view direction
  glLightfv ( GL_LIGHT1, GL_SPOT_DIRECTION, camera ( )->viewDirection ( ));

  if ( meshRend != NULL )
    meshRend->Render ( );
  if ( spineMeshRend != NULL )
    spineMeshRend->Render ( );

  if ( mSpinesSynthContainers.getContainer ( ).size ( ) > 0 )
  {
    //for (int i=1;i<mSpinesSynthContainers.getContainer().size();++i)
    for ( int i = 0; i < mSpinesSynthContainers.getContainer ( ).size ( ); ++i )
    {
      spineMeshRend->setMeshToRender ( mSpinesSynthContainers.getElementAt ( i ));
      spineMeshRend->Render ( );
    }
    spineMeshRend->setMeshToRender ( mSpinesSynthContainers.getElementAt ( 0 ));
  }

  if ( mSchematicLines )
  {
    glColor3f ( 0.8, 0.0, 0 );
    drawSegmentList ( );
  }

  if ( mSchematicSpheres )
  {
    glColor3f ( 0, 0.25, 0.5 );
    drawSphereList ( );
  }

  if ( mDebugMode )
  {
    ShowTextData ( );
  }
}

void NeuroGeneratorWidgetViewer::loadNeuronModel ( QString pModel )
{

  if ( mesh == NULL )
  {
    mesh = new NeuroSWC ( );
  }
  else
  {
    delete mesh;
    mesh = new NeuroSWC ( );
  }

  mesh->loadModel ( pModel.toStdString ( ));

  mesh->setVertexColor ( mesh->getMesh ( )->vertices_begin ( ),
                         mesh->getMesh ( )->vertices_end ( ),
                         MeshDef::Color ( 0.5, 0.5, 1.0, 1.0 ));


  if ( meshRend == NULL )
  {
    meshRend = new MeshRenderer ( );
  }
  else
  {
    delete meshRend;
    meshRend = new MeshRenderer ( );
  }

  meshRend->setMeshToRender ( mesh );

  meshRend->setRenderOptions ( renderMask );

  //Delete previus data
  if ( mSWCImporter != NULL )
    delete mSWCImporter;
  mSWCImporter = NULL;

  updateGL ( );
}

void NeuroGeneratorWidgetViewer::loadSpinesModel ( QString pModel )
{

  if ( meshSpines == NULL )
  {
    meshSpines = new SpinesSWC ( );
  }
  else
  {
    delete meshSpines;
    meshSpines = new SpinesSWC ( );
  }

  meshSpines->loadModel ( pModel.toStdString ( ));

  if ( spineMeshRend == NULL )
  {
    spineMeshRend = new MeshRenderer ( );
  }
  else
  {
    delete spineMeshRend;
    spineMeshRend = new MeshRenderer ( );
  }

  spineMeshRend->setMeshToRender ( meshSpines );

  spineMeshRend->setRenderOptions ( renderMask );

  updateGL ( );
}

void NeuroGeneratorWidgetViewer::loadSWCFile ( QString pSWCFile )
{
  mSWCFile = pSWCFile;
  if ( mSWCImporter != NULL )
    delete mSWCImporter;
  mSWCImporter = new SWCImporter ( pSWCFile.toStdString ( ));
}

void NeuroGeneratorWidgetViewer::adaptSomaMOdelWithSWCInfo ( )
{
  mesh->scaleBaseMesh ( mSWCImporter->getElementAt ( 1 ).radius );
}

void NeuroGeneratorWidgetViewer::loadSomaContainer ( QDir pDir )
{
  QStringList filters;
  filters << "*.obj" << "*.off" << "*.ply" << "*.stl";

  QStringList list = pDir.entryList ( filters, QDir::Files );

  BaseMesh *auxBaseMesh;

  mSomaModelsContainers.destroyAllElementsInContainer ( );

  for ( int i = 0; i < list.size ( ); ++i )
  {
    std::string modelName = pDir.absolutePath ( ).toStdString ( ) + "/" + list.at ( i ).toStdString ( );
    auxBaseMesh = new BaseMesh ( modelName );
    mSomaModelsContainers.addElement ( auxBaseMesh );
  }

  if ( mSomaScaleFactor != 1.0 )
  {
    mSomaModelsContainers.scaleContainer ( mSomaScaleFactor );
  }
}

void NeuroGeneratorWidgetViewer::loadSpinesContainer ( QDir pDir )
{
  QStringList filters;
  filters << "*.obj" << "*.off" << "*.ply" << "*.stl";

  QStringList list = pDir.entryList ( filters, QDir::Files );

  BaseMesh *auxBaseMesh;
  for ( int i = 0; i < list.size ( ); ++i )
  {
    std::string modelName = pDir.absolutePath ( ).toStdString ( ) + "/" + list.at ( i ).toStdString ( );
    auxBaseMesh = new BaseMesh ( modelName );
    mSpinesModelsContainers.addElement ( auxBaseMesh );
  }

  if ( mSpineScaleFactor != 1.0 )
  {
    mSpinesModelsContainers.scaleContainer ( mSpineScaleFactor );
  }
}

BaseMeshContainer NeuroGeneratorWidgetViewer::getSomaModelsContainer ( ) const
{
  return mSomaModelsContainers;
}

BaseMeshContainer NeuroGeneratorWidgetViewer::getSpinesModelsContainer ( ) const
{
  return mSpinesModelsContainers;
}

unsigned int NeuroGeneratorWidgetViewer::getNumModelsSpines ( )
{
  return mSpinesModelsContainers.getContainer ( ).size ( );
}

unsigned int NeuroGeneratorWidgetViewer::getSpinesSynthContainersNumEles ( )
{
  return mSpinesSynthContainers.getNumElements ( );
}

void NeuroGeneratorWidgetViewer::SetSomaModel ( unsigned int pSomaId )
{
  mSomaModelId = pSomaId;
}

void NeuroGeneratorWidgetViewer::SetSpinesType ( unsigned int pSpinesTypes )
{
  mSpinesTypes = pSpinesTypes;
}

void NeuroGeneratorWidgetViewer::generateNeuronMeshFromFile ( QString pSWCFile,
                                                              unsigned int pTubesHResol,
                                                              unsigned int pTubesVResol,
                                                              unsigned int pSomaHResol,
                                                              unsigned int pSomaVResol )
{
  mSWCFile = pSWCFile;

  if ( mesh == NULL )
  {
    mesh = new NeuroSWC ( );
  }
  else
  {
    delete mesh;
    mesh = new NeuroSWC ( );
  }

  mesh->setSomaContainer ( &mSomaModelsContainers );

  mesh->setPiramidalSoma ( mPiramidalSoma );

  mesh->generateNeuron ( pSWCFile.toStdString ( ), pTubesHResol, pTubesVResol, pSomaHResol, pSomaVResol, mSomaModelId );

  if ( meshRend == NULL )
  {
    meshRend = new MeshRenderer ( );
  }
  else
  {
    delete meshRend;
    meshRend = new MeshRenderer ( );
  }

  meshRend->setMeshToRender ( mesh );

  meshRend->setRenderOptions ( renderMask );

  //Store the importer for debug mode
  if ( mSWCImporter != NULL )
    delete mSWCImporter;
  mSWCImporter = new SWCImporter ( pSWCFile.toStdString ( ));

  camera ( )->setPosition ( qglviewer::Vec ( 0, 0, 100 ));
  camera ( )->setOrientation ( qglviewer::Quaternion ( qglviewer::Vec ( 1, 0, 0 ), qglviewer::Vec ( 0, 0, 0 )));

  updateGL ( );
}

void NeuroGeneratorWidgetViewer::generateNeuronMeshFromInternalInfo ( unsigned int pTubesHResol,
                                                                      unsigned int pTubesVResol,
                                                                      unsigned int pSomaHResol,
                                                                      unsigned int pSomaVResol )
{
  if ( mSWCFile.length ( ) > 0 )
  {
    generateNeuronMeshFromFile ( mSWCFile, pTubesHResol, pTubesVResol, pSomaHResol, pSomaVResol );
  }
}

void NeuroGeneratorWidgetViewer::generateNeuronLines ( bool pGenLines )
{
  if (( mSWCImporter != NULL ) && ( pGenLines ))
  {
    generateSegmentList ( );
    updateGL ( );
  }

  mSchematicLines = pGenLines;
}

void NeuroGeneratorWidgetViewer::generateNeuronSphereNodes ( bool pSphereNodes )
{
  if (( mSWCImporter != NULL ) && ( pSphereNodes ))
  {
    generateSphereList ( );
    updateGL ( );
  }
  mSchematicSpheres = pSphereNodes;
}

void NeuroGeneratorWidgetViewer::destroyNeuronMesh ( )
{
  if ( mesh != NULL )
  {
    delete mesh;
    mesh = NULL;
  }

  if ( meshRend != NULL )
  {
    delete meshRend;
    meshRend = NULL;
  }
}

void NeuroGeneratorWidgetViewer::generateSpines ( unsigned int pNumSpines,
                                                  unsigned int pHorResol,
                                                  unsigned int pVerResol,
                                                  float pMinLongSpine,
                                                  float pMaxLongSpine,
                                                  float pMinRadio,
                                                  float pMaxRadio )
{
  if ( mesh == NULL )
    return;

  unsigned int lNumMeshes = mSpinesSynthContainers.getContainer ( ).size ( );
  if ( lNumMeshes != 0 )
  {
    for ( int i = 0; i < lNumMeshes; ++i )
    {
      delete mSpinesSynthContainers.getElementAt ( i );
    }

    mSpinesSynthContainers.getContainer ( ).clear ( );
  }

  if ( meshSpines == NULL )
  {
    meshSpines =
      new SpinesSWC ( mesh, pNumSpines, pHorResol, pVerResol, pMinLongSpine, pMaxLongSpine, pMinRadio, pMaxRadio );
  }
  else
  {
    delete meshSpines;
    meshSpines =
      new SpinesSWC ( mesh, pNumSpines, pHorResol, pVerResol, pMinLongSpine, pMaxLongSpine, pMinRadio, pMaxRadio );
  }

  meshSpines->calcVertexCandDistances ( false );

  meshSpines->setSpinesContainer ( &mSpinesModelsContainers );

  //!!!
  if ( mSpinesTypes == 0 )
  {
    meshSpines->distributeSpinesProcedural ( );
  }
  if ( mSpinesTypes == 1 )
  {
    meshSpines->distributeSpinesModeled ( );
  }
  if ( mSpinesTypes == 2 )
  {
    meshSpines->distributeSpineAlongAllDendritics ( );
  }

  if ( spineMeshRend == NULL )
  {
    spineMeshRend = new MeshRenderer ( );
  }
  else
  {
    delete spineMeshRend;
    spineMeshRend = new MeshRenderer ( );
  }

  spineMeshRend->setMeshToRender ( meshSpines );

  spineMeshRend->setRenderOptions ( renderMask );

  updateGL ( );
}

void NeuroGeneratorWidgetViewer::generateSomeSpinesGroup ( unsigned int pNumSpinesByGroup,
                                                           unsigned int pNumOfGroups,
                                                           unsigned int pHorResol,
                                                           unsigned int pVerResol,
                                                           float pMinLongSpine,
                                                           float pMaxLongSpine,
                                                           float pMinRadio,
                                                           float pMaxRadio )
{

  unsigned int lNumMeshes = mSpinesSynthContainers.getContainer ( ).size ( );
  unsigned int lTotalNumSpines = 0;
  float lDendriticModificator = 0;
  float lNumGroupsOfSpines = 0;
  BaseMesh *tmpMesh = NULL;

  if ( mesh == NULL )
    return;

  if ( meshSpines != NULL )
  {
    delete meshSpines;
    meshSpines == NULL;
  }

  if ( lNumMeshes != 0 )
  {
    for ( int i = 0; i < lNumMeshes; ++i )
    {
      delete mSpinesSynthContainers.getElementAt ( i );
    }

    mSpinesSynthContainers.getContainer ( ).clear ( );
  }

  meshSpines =
    new SpinesSWC ( mesh, pNumSpinesByGroup, pHorResol, pVerResol, pMinLongSpine, pMaxLongSpine, pMinRadio, pMaxRadio );

  meshSpines->calcVertexCandDistances ( false );

  lNumGroupsOfSpines = pNumOfGroups;

  std::vector < VertexDistances > lVertexDistancesContainer = meshSpines->getVertexDistancesContainer ( );
  std::vector < FacesDistances > lFacesDistancesContainer = meshSpines->getFacesDistancesContainer ( );

  for ( int i = 0; i < lNumGroupsOfSpines; ++i )
  {
    delete meshSpines;

    meshSpines = new SpinesSWC ( mesh,
                                 pNumSpinesByGroup,
                                 pHorResol,
                                 pVerResol,
                                 pMinLongSpine,
                                 pMaxLongSpine,
                                 pMinRadio,
                                 pMaxRadio,
                                 lVertexDistancesContainer,
                                 lFacesDistancesContainer );

    meshSpines->setSpinesContainer ( &mSpinesModelsContainers );

    if ( mSpinesTypes == 0 )
      meshSpines->distributeSpinesProcedural ( );
    if ( mSpinesTypes == 1 )
      meshSpines->distributeSpinesModeled ( );

    mSpinesSynthContainers.addElement ( new BaseMesh ( ));
    std::cout << "-->> Antes del join" << std::endl;
    std::cout << "-->> Numero de espinas en el synth container:" << mSpinesSynthContainers.getContainer ( ).size ( )
              << std::endl;

    mSpinesSynthContainers.getElementAt ( i )->JoinBaseMesh ( meshSpines );

    //---std::cout<<"-->> Antes de anyadir elementos"<<std::endl;
    MeshDef::ConstVertexIter iniLimit = mSpinesSynthContainers.getElementAt ( i )->getMesh ( )->vertices_begin ( );
    MeshDef::ConstVertexIter finLimit = mSpinesSynthContainers.getElementAt ( i )->getMesh ( )->vertices_end ( );

    MeshDef::Color lColor ( randfloat ( ), randfloat ( ), randfloat ( ), 1.0 );

    //---std::cout<<"-->> Despues de anyadir elementos"<<std::endl;
    mSpinesSynthContainers.getElementAt ( i )->setVertexColor ( iniLimit, finLimit, lColor
    );
    //Restore the ne
    lVertexDistancesContainer = meshSpines->getVertexDistancesContainer ( );
    lFacesDistancesContainer = meshSpines->getFacesDistancesContainer ( );
  }

  delete meshSpines;
  meshSpines = NULL;

  if ( spineMeshRend != NULL )
  {
    delete spineMeshRend;
  }

  spineMeshRend = new MeshRenderer ( );

  spineMeshRend->setMeshToRender ( mSpinesSynthContainers.getElementAt ( 0 ));

  spineMeshRend->setRenderOptions ( renderMask );

  updateGL ( );
}

void NeuroGeneratorWidgetViewer::generateSemiRealSpinesGroup ( unsigned int pNumSpinesByGroup,
                                                               unsigned int pHorResol,
                                                               unsigned int pVerResol,
                                                               float pMinLongSpine,
                                                               float pMaxLongSpine,
                                                               float pMinRadio,
                                                               float pMaxRadio )
{

  unsigned int lNumMeshes = mSpinesSynthContainers.getContainer ( ).size ( );
  unsigned int lTotalNumSpines = 0;
  float lDendriticModificator = 0;
  float lNumGroupsOfSpines = 0;
  BaseMesh *tmpMesh = NULL;

  if ( mesh == NULL )
    return;

  if ( meshSpines != NULL )
  {
    delete meshSpines;
    meshSpines == NULL;
  }

  if ( lNumMeshes != 0 )
  {
    for ( int i = 0; i < lNumMeshes; ++i )
    {
      delete mSpinesSynthContainers.getElementAt ( i );
    }

    mSpinesSynthContainers.getContainer ( ).clear ( );
  }

  meshSpines =
    new SpinesSWC ( mesh, pNumSpinesByGroup, pHorResol, pVerResol, pMinLongSpine, pMaxLongSpine, pMinRadio, pMaxRadio );

  meshSpines->calcVertexCandDistances ( false );

  std::vector < VertexDistances > lVertexDistancesContainer = meshSpines->getVertexDistancesContainer ( );
  std::vector < FacesDistances > lFacesDistancesContainer = meshSpines->getFacesDistancesContainer ( );

  lTotalNumSpines = ( lVertexDistancesContainer.size ( ));
  lNumGroupsOfSpines = lTotalNumSpines/pNumSpinesByGroup + 1;
  lDendriticModificator = 1.0/lNumGroupsOfSpines;

  for ( int i = 0; i < lNumGroupsOfSpines; ++i )
  {
    delete meshSpines;

    meshSpines = new SpinesSWC ( mesh,
                                 pNumSpinesByGroup,
                                 pHorResol,
                                 pVerResol,
                                 pMinLongSpine,
                                 pMaxLongSpine,
                                 pMinRadio,
                                 pMaxRadio,
                                 lVertexDistancesContainer,
                                 lFacesDistancesContainer );

    meshSpines->setDistributorModificator ( lDendriticModificator );

    meshSpines->setSpinesContainer ( &mSpinesModelsContainers );

    meshSpines->distributeSpinesModeled ( );

    mSpinesSynthContainers.addElement ( new BaseMesh ( ));
    mSpinesSynthContainers.getElementAt ( i )->JoinBaseMesh ( meshSpines );

    MeshDef::ConstVertexIter iniLimit = mSpinesSynthContainers.getElementAt ( i )->getMesh ( )->vertices_begin ( );
    MeshDef::ConstVertexIter finLimit = mSpinesSynthContainers.getElementAt ( i )->getMesh ( )->vertices_end ( );
    MeshDef::Color lColor ( randfloat ( ), randfloat ( ), randfloat ( ), 1.0 );

    mSpinesSynthContainers.getElementAt ( i )->setVertexColor ( iniLimit, finLimit, lColor
    );
    //Restore the ne
    lVertexDistancesContainer = meshSpines->getVertexDistancesContainer ( );
    lFacesDistancesContainer = meshSpines->getFacesDistancesContainer ( );
  }

  delete meshSpines;
  meshSpines = NULL;

  if ( spineMeshRend != NULL )
  {
    delete spineMeshRend;
  }

  spineMeshRend = new MeshRenderer ( );

  spineMeshRend->setMeshToRender ( mSpinesSynthContainers.getElementAt ( 0 ));

  spineMeshRend->setRenderOptions ( renderMask );

  updateGL ( );
}

void NeuroGeneratorWidgetViewer::generateRealSpinesGroup ( unsigned int pNumSpinesByGroup,
                                                           unsigned int pHorResol,
                                                           unsigned int pVerResol,
                                                           float pMinLongSpine,
                                                           float pMaxLongSpine,
                                                           float pMinRadio,
                                                           float pMaxRadio )
{

  unsigned int lNumMeshes = mSpinesSynthContainers.getContainer ( ).size ( );
  unsigned int lTotalNumSpines = 0;
  float lDendriticModificator = 0;
  float lNumGroupsOfSpines = 0;
  BaseMesh *tmpMesh = NULL;

  if ( mesh == NULL )
    return;

  if ( meshSpines != NULL )
  {
    delete meshSpines;
    meshSpines == NULL;
  }

  if ( lNumMeshes != 0 )
  {
    for ( int i = 0; i < lNumMeshes; ++i )
    {
      delete mSpinesSynthContainers.getElementAt ( i );
    }

    mSpinesSynthContainers.getContainer ( ).clear ( );
  }

  meshSpines =
    new SpinesSWC ( mesh, pNumSpinesByGroup, pHorResol, pVerResol, pMinLongSpine, pMaxLongSpine, pMinRadio, pMaxRadio );

  meshSpines->calcVertexCandDistances ( true );

  lTotalNumSpines = meshSpines->preCalcNumSpines ( );
  lNumGroupsOfSpines = ( float ) lTotalNumSpines/pNumSpinesByGroup;
  lDendriticModificator = 1.0/lNumGroupsOfSpines;

  std::vector < VertexDistances > lVertexDistancesContainer = meshSpines->getVertexDistancesContainer ( );
  std::vector < FacesDistances > lFacesDistancesContainer = meshSpines->getFacesDistancesContainer ( );

  for ( int i = 0; i < lNumGroupsOfSpines; ++i )
  {
    delete meshSpines;

    meshSpines = new SpinesSWC ( mesh,
                                 pNumSpinesByGroup,
                                 pHorResol,
                                 pVerResol,
                                 pMinLongSpine,
                                 pMaxLongSpine,
                                 pMinRadio,
                                 pMaxRadio,
                                 lVertexDistancesContainer,
                                 lFacesDistancesContainer );

    meshSpines->setDistributorModificator ( lDendriticModificator );

    meshSpines->setSpinesContainer ( &mSpinesModelsContainers );

    meshSpines->distributeSpineAlongAllDendritics ( );

    mSpinesSynthContainers.addElement ( new BaseMesh ( ));
    mSpinesSynthContainers.getElementAt ( i )->JoinBaseMesh ( meshSpines );

    MeshDef::ConstVertexIter iniLimit = mSpinesSynthContainers.getElementAt ( i )->getMesh ( )->vertices_begin ( );
    MeshDef::ConstVertexIter finLimit = mSpinesSynthContainers.getElementAt ( i )->getMesh ( )->vertices_end ( );
    MeshDef::Color lColor ( randfloat ( ), randfloat ( ), randfloat ( ), 1.0 );

    mSpinesSynthContainers.getElementAt ( i )->setVertexColor ( iniLimit, finLimit, lColor
    );
    //Restore the ne
    lVertexDistancesContainer = meshSpines->getVertexDistancesContainer ( );
    lFacesDistancesContainer = meshSpines->getFacesDistancesContainer ( );
  }

  delete meshSpines;
  meshSpines = NULL;

  if ( spineMeshRend != NULL )
  {
    delete spineMeshRend;
  }

  spineMeshRend = new MeshRenderer ( );

  spineMeshRend->setMeshToRender ( mSpinesSynthContainers.getElementAt ( 0 ));

  spineMeshRend->setRenderOptions ( renderMask );

  updateGL ( );
}

void NeuroGeneratorWidgetViewer::destroyAllGroupsSpines ( )
{
  if ( meshSpines != NULL )
  {
    delete meshSpines;
    meshSpines = nullptr;
  }

  unsigned int lNumMeshes = mSpinesSynthContainers.getContainer ( ).size ( );

  if ( lNumMeshes != 0 )
  {
    for ( int i = 0; i < lNumMeshes; ++i )
    {
      delete mSpinesSynthContainers.getElementAt ( i );
    }

    mSpinesSynthContainers.getContainer ( ).clear ( );
  }

  if ( spineMeshRend != NULL )
  {
    delete spineMeshRend;
  }
  spineMeshRend = NULL;

  updateGL ( );
}

void NeuroGeneratorWidgetViewer::generateSpinesInSegment ( unsigned int pNumSpinesByGroup,
                                                           unsigned int pHorResol,
                                                           unsigned int pVerResol,
                                                           float pMinLongSpine,
                                                           float pMaxLongSpine,
                                                           float pMinRadio,
                                                           float pMaxRadio )
{
  unsigned int lNumMeshes = mSpinesSynthContainers.getContainer ( ).size ( );
  unsigned int lTotalNumSpines = 0;
  float lDendriticModificator = 0;
  float lNumGroupsOfSpines = 0;
  BaseMesh *tmpMesh = NULL;

  if ( mesh == NULL )
    return;

  if ( meshSpines != NULL )
  {
    delete meshSpines;
    meshSpines == NULL;
  }

  if ( spineMeshRend != NULL )
  {
    delete spineMeshRend;
  }

  spineMeshRend = new MeshRenderer ( );

  if ( lNumMeshes != 0 )
  {
    for ( int i = 0; i < lNumMeshes; ++i )
    {
      delete mSpinesSynthContainers.getElementAt ( i );
    }

    mSpinesSynthContainers.getContainer ( ).clear ( );
  }

  meshSpines =
    new SpinesSWC ( mesh, pNumSpinesByGroup, pHorResol, pVerResol, pMinLongSpine, pMaxLongSpine, pMinRadio, pMaxRadio );

  meshSpines->setSpinesContainer ( &mSpinesModelsContainers );

  QFileInfo fi(mSWCFile);

  meshSpines->distributeSpinesInSegments ( mSpinesDesp,
                                           mBProgressionFactor,
                                           mAProgressionFactor,
                                           mBasalMinDistance,
                                           mBasalCteDistance,
                                           mApicalMinDistance,
                                           mApicalCteDistance,
                                           Neuronize::bbdd,
                                           fi.baseName().toStdString(),
                                           Neuronize::tmpPath.toStdString()


  );

  int i = 0;

  mSpinesSynthContainers.addElement ( new BaseMesh ( ));
  mSpinesSynthContainers.getElementAt ( i )->JoinBaseMesh ( meshSpines );


  MeshDef::ConstVertexIter iniLimit = mSpinesSynthContainers.getElementAt ( i )->getMesh ( )->vertices_begin ( );
  MeshDef::ConstVertexIter finLimit = mSpinesSynthContainers.getElementAt ( i )->getMesh ( )->vertices_end ( );

  mSpinesSynthContainers.getElementAt ( i )->setVertexColor ( iniLimit, finLimit, MeshDef::Color ( 0.6, 0.0, 0.0, 1.0 )
  );

  mLastSpinesInfo.clear ( );
  mLastSpinesInfo = meshSpines->getSpinesInfo ( );
  mNumVertesSpineIndexation = meshSpines->getNumVerticesEnSpina ( );

  delete meshSpines;
  meshSpines = NULL;

  if ( spineMeshRend != NULL )
  {
    delete spineMeshRend;
  }

  spineMeshRend = new MeshRenderer ( );

  spineMeshRend->setMeshToRender ( mSpinesSynthContainers.getElementAt ( 0 ));

  spineMeshRend->setRenderOptions ( renderMask );
  updateGL ( );
}

void NeuroGeneratorWidgetViewer::generateSegmentList ( )
{
  glNewList ( mSegmentList, GL_COMPILE );

  OpenMesh::Vec3f pos;
  OpenMesh::Vec3f prevPos;
  int lParent = 0;

  glDisable ( GL_LIGHTING );

  for ( unsigned int i = 1; i < mSWCImporter->getNumNodes ( ); ++i )
  {
    pos = mSWCImporter->getElementAt ( i ).position;
    lParent = mSWCImporter->getElementAt ( i ).parent;

    if ( lParent != -1 )
    {
      prevPos = mSWCImporter->getElementAt ( lParent ).position;

      glBegin ( GL_LINES );
      glVertex3f ( pos[0], pos[1], pos[2] );
      glVertex3f ( prevPos[0], prevPos[1], prevPos[2] );
      glEnd ( );
    }
  }
  glEnable ( GL_LIGHTING );

  glEndList ( );
}

void NeuroGeneratorWidgetViewer::generateSphereList ( )
{
  glNewList ( mSphereList, GL_COMPILE );

  OpenMesh::Vec3f pos;

  for ( int i = 0; i < mSWCImporter->getNumNodes ( ); ++i )
  {
    glPushMatrix ( );

    pos = mSWCImporter->getElementAt ( i ).position;

    glTranslatef ( pos[0], pos[1], pos[2] );

    glutSolidSphere ( mSWCImporter->getElementAt ( i ).radius, mSphereSlices, mSphereStacks );

    glPopMatrix ( );
  }
  glEndList ( );
}

void NeuroGeneratorWidgetViewer::drawSegmentList ( )
{
  glCallList ( mSegmentList );
}

void NeuroGeneratorWidgetViewer::drawSphereList ( )
{
  glCallList ( mSphereList );
}

void NeuroGeneratorWidgetViewer::setSpinesDistributionParams ( float pSpinesDesp,
                                                               float pBProgressionFactor,
                                                               float pAProgressionFactor,
                                                               float pBasalMinDistance,
                                                               float pBasalCteDistance,
                                                               float pApicalMinDistance,
                                                               float pApicalCteDistance )
{

  mSpinesDesp = pSpinesDesp;
  mBProgressionFactor = pBProgressionFactor;
  mAProgressionFactor = pAProgressionFactor;

  mBasalMinDistance = pBasalMinDistance;
  mBasalCteDistance = pBasalCteDistance;
  mApicalMinDistance = pApicalMinDistance;
  mApicalCteDistance = pApicalCteDistance;
}

void NeuroGeneratorWidgetViewer::attachNewSpines ( unsigned int pNumSpines,
                                                   unsigned int pHorResol,
                                                   unsigned int pVerResol,
                                                   float pMinLongSpine,
                                                   float pMaxLongSpine,
                                                   float pMinRadio,
                                                   float pMaxRadio )
{
  if ( mesh == NULL )
    return;

  if ( meshSpines == NULL )
  {
    return;
  }
  else
  {

    std::vector < VertexDistances > lVertexDistancesContainer = meshSpines->getVertexDistancesContainer ( );
    std::vector < FacesDistances > lFacesDistancesContainer = meshSpines->getFacesDistancesContainer ( );

    delete meshSpines;
    meshSpines = new SpinesSWC ( mesh,
                                 pNumSpines,
                                 pHorResol,
                                 pVerResol,
                                 pMinLongSpine,
                                 pMaxLongSpine,
                                 pMinRadio,
                                 pMaxRadio,
                                 lVertexDistancesContainer,
                                 lFacesDistancesContainer );
  }

  meshSpines->setSpinesContainer ( &mSpinesModelsContainers );

  if ( mSpinesTypes == 0 )
    meshSpines->distributeSpinesProcedural ( );
  else
    meshSpines->distributeSpinesModeled ( );

  if ( spineMeshRend == NULL )
  {
    spineMeshRend = new MeshRenderer ( );
  }
  else
  {
    delete spineMeshRend;
    spineMeshRend = new MeshRenderer ( );
  }

  spineMeshRend->setMeshToRender ( meshSpines );

  spineMeshRend->setRenderOptions ( renderMask );
}

void NeuroGeneratorWidgetViewer::applyNoiseToNeuron ( float pNoiseInterval, bool pCteNoise )
{
  if ( mesh != NULL )
    mesh->applyNoiseToAll ( pNoiseInterval, pCteNoise );

  updateGL ( );
}

void NeuroGeneratorWidgetViewer::applyNoiseToSoma ( float pNoiseInterval, bool pCteNoise )
{
  if ( mesh != NULL )
    mesh->applyNoiseToSoma ( pNoiseInterval, pCteNoise );

  updateGL ( );
}

void NeuroGeneratorWidgetViewer::applyNoiseToDendritics ( float pNoiseInterval, bool pCteNoise )
{
  if ( mesh != NULL )
    mesh->applyNoiseToDendritics ( pNoiseInterval, pCteNoise );

  updateGL ( );
}

void NeuroGeneratorWidgetViewer::applyNoiseToSpines ( float pNoiseInterval,
                                                      bool pCteNoise,
                                                      unsigned int pLocalLimit,
                                                      unsigned int pMinDesp )
{
  if ( spineMeshRend != NULL )
  {
    if ( mSpinesSynthContainers.getContainer ( ).size ( ) > 0 )
    {
      for ( int i = 0; i < mSpinesSynthContainers.getContainer ( ).size ( ); ++i )
      {
        mSpinesSynthContainers.getElementAt ( i )
                              ->applyControlledNoise ( pNoiseInterval, pCteNoise, pLocalLimit, pMinDesp );;
      }
    }
  }

  updateGL ( );
}

void NeuroGeneratorWidgetViewer::applySmoothToNeuron ( unsigned int pSmoothMethod,
                                                       unsigned int pTechnique,
                                                       unsigned int pContinuity,
                                                       unsigned int pIterations )
{
  if ( mesh != NULL )
    mesh->SmoothMesh ( pSmoothMethod, pTechnique, pContinuity, pIterations );

  updateGL ( );
}

void NeuroGeneratorWidgetViewer::applySmoothToSpines ( unsigned int pSmoothMethod,
                                                       unsigned int pTechnique,
                                                       unsigned int pContinuity,
                                                       unsigned int pIterations )
{
  if ( spineMeshRend != NULL )
  {
    if ( mSpinesSynthContainers.getContainer ( ).size ( ) > 0 )
    {
      for ( int i = 0; i < mSpinesSynthContainers.getContainer ( ).size ( ); ++i )
      {
        mSpinesSynthContainers.getElementAt ( i )->SmoothMesh ( pSmoothMethod, pTechnique, pContinuity, pIterations );
      }
    }
  }

  updateGL ( );
}

void NeuroGeneratorWidgetViewer::applySubdivide ( unsigned int pSubdivType )
{
  if ( mSpinesSynthContainers.getContainer ( ).size ( ) > 0 )
  {
    for ( int i = 0; i < mSpinesSynthContainers.getContainer ( ).size ( ); ++i )
    {
      mSpinesSynthContainers.getElementAt ( i )->Subdivide (( NSBaseMesh::SOPType ) pSubdivType );
      mSpinesSynthContainers.getElementAt ( i )->setVertexColor ( mSpinesSynthContainers.getElementAt ( i )->getMesh ( )
                                                                                        ->vertices_begin ( ),
                                                                  mSpinesSynthContainers.getElementAt ( i )->getMesh ( )
                                                                                        ->vertices_end ( ),
                                                                  MeshDef::Color ( 0.5, 0.5, 1.0, 1.0 ));
    }
  }
  else
  {
    if ( mesh != NULL )
    {
      mesh->Subdivide (( NSBaseMesh::SOPType ) pSubdivType );
      mesh->setVertexColor ( mesh->getMesh ( )->vertices_begin ( ),
                             mesh->getMesh ( )->vertices_end ( ),
                             MeshDef::Color ( 0.5, 0.5, 1.0, 1.0 ));
    }
  }
  updateGL ( );
}

void NeuroGeneratorWidgetViewer::applyDecimate ( unsigned int pSteps )
{
  if ( mesh != NULL )
  {
    updateGL ( );
  }
}

void NeuroGeneratorWidgetViewer::update ( )
{
  draw ( );
}

void NeuroGeneratorWidgetViewer::exportNeuron ( QString pFile )
{
  //Export the mesh
  meshRend->getBaseMesh ( )->exportMesh ( pFile.toStdString ( ));

  if ( mXMLProgressiveNeuroVizManager != NULL )
  {
    delete mXMLProgressiveNeuroVizManager;
  }

  mXMLProgressiveNeuroVizManager = new XMLProgressiveNeuroVizManager ( );
  mXMLProgressiveNeuroVizManager->generateXMLContent ( mSWCFile,
                                                       pFile,
                                                       mNeuronType,
                                                       mesh->getSomaNumVertex ( ),
                                                       mesh->getTubeHReol ( ),
                                                       mesh->getTubeVReol ( )
  );

  mXMLProgressiveNeuroVizManager->exportDomToFile ( pFile.toStdString ( ));

}

void NeuroGeneratorWidgetViewer::exportSpines ( QString pFile )
{
  spineMeshRend->getBaseMesh ( )->exportMesh ( pFile.toStdString ( ));
}

void NeuroGeneratorWidgetViewer::exportGroupOfSpines ( QString pFile )
{
  unsigned int lNumberOfSpinesGroup = mSpinesSynthContainers.getContainer ( ).size ( );
  QString lTmpNameFile;

  BaseMesh *lPointer = NULL;

  for ( int i = 0; i < lNumberOfSpinesGroup; ++i )
  {
    lTmpNameFile = pFile;
    lTmpNameFile += ".obj";
    lPointer = mSpinesSynthContainers.getElementAt ( i );
    lPointer->exportMesh ( lTmpNameFile.toStdString ( ));
  }
}

void NeuroGeneratorWidgetViewer::joinSpines ( )
{
  if ( mesh == NULL )
    return;
  if ( meshSpines == NULL )
    return;

  mesh->JoinSpines (( BaseMesh * ) meshSpines );

  delete meshSpines;
  meshSpines = NULL;

  delete spineMeshRend;
  spineMeshRend = NULL;
}

void NeuroGeneratorWidgetViewer::ControlStateRender ( bool pRendSolid,
                                                      bool pRendEdges,
                                                      bool pRendNormals,
                                                      bool pRendVertex )
{

  renderMask = 0;
  if ( pRendSolid )
    renderMask ^= MeshRenderer::RENDER_SURFACE;
  if ( pRendEdges )
    renderMask ^= MeshRenderer::RENDER_EDGES;
  if ( pRendNormals )
    renderMask ^= MeshRenderer::RENDER_NORMALS;
  if ( pRendVertex )
    renderMask ^= MeshRenderer::RENDER_VERTICES;

  if ( meshRend != NULL )
    meshRend->setRenderOptions ( renderMask );
  if ( spineMeshRend != NULL )
    spineMeshRend->setRenderOptions ( renderMask );

  updateGL ( );
}

void NeuroGeneratorWidgetViewer::setNormalScale ( float pScale )
{
  if ( meshRend != NULL )
    meshRend->setNormalScale ( pScale );
  if ( spineMeshRend != NULL )
    spineMeshRend->setNormalScale ( pScale );

  updateGL ( );
}

void NeuroGeneratorWidgetViewer::setPixelScale ( float pScale )
{
  if ( meshRend != NULL )
    meshRend->setPixelScale ( pScale );
  if ( spineMeshRend != NULL )
    spineMeshRend->setPixelScale ( pScale );

  updateGL ( );
}

void NeuroGeneratorWidgetViewer::setSomaScaleFactor ( float pScaleFactor )
{
  mSomaScaleFactor = pScaleFactor;

  mSomaModelsContainers.scaleContainer ( mSomaScaleFactor );
}

void NeuroGeneratorWidgetViewer::setSpineScaleFactor ( float pScaleFactor )
{
  mSpineScaleFactor = pScaleFactor;

  mSpinesModelsContainers.destroyAllElementsInContainer ( );
  loadSpinesContainer ( QDir ( QCoreApplication::applicationDirPath() + "/Content/Spines/Low" ));
}

void NeuroGeneratorWidgetViewer::setMultipleGroupsOfSpines ( bool pMultiGroup )
{

}

void NeuroGeneratorWidgetViewer::ControlLights ( int control )
{
  lights->SetLightType ( control );
  updateGL ( );
}

void NeuroGeneratorWidgetViewer::SetDebugMode ( bool pVal )
{
  mDebugMode = pVal;
  updateGL ( );
}

void NeuroGeneratorWidgetViewer::ConfigureDebug ( unsigned int pDebugRangeIni,
                                                  unsigned int pDebugRangeFin,
                                                  MeshRenderer::DebugMode pDebugType )
{
  mDebugRangeIni = pDebugRangeIni;
  mDebugRangeFin = pDebugRangeFin;
  mDebugType = pDebugType;

  if ( mDebugType == MeshRenderer::DebugMode::DEBUG_FACES )
  {
    meshRend->getBaseMesh ( )->calcFacesCentroid ( );
  }
  updateGL ( );
}

void NeuroGeneratorWidgetViewer::ShowTextData ( )
{
  qglviewer::Vec pos;
  MeshDef::Point lAuxPoint ( 0, 0, 0 );
  static qglviewer::Vec proj;
  unsigned int lIterIni = 0;
  unsigned int lIterFin = 0;

  //Debug Segments
  switch ( mDebugType )
  {
    case MeshRenderer::DebugMode::DEBUG_SEGMENTS:
      if ( mSWCImporter != NULL )
      {
        lIterFin = mSWCImporter->getNumNodes ( );

        if (( mDebugRangeIni != 0 )
          || ( mDebugRangeFin != 0 ))
        {
          if ( mDebugRangeIni < lIterFin )
            lIterIni = mDebugRangeIni;
          if ( mDebugRangeFin < lIterFin )
            lIterFin = mDebugRangeFin;
        }

        SWCNode lNodeAux;

        static qglviewer::Vec proj;

        glColor3f ( 0, 0, 0 );

        startScreenCoordinatesSystem ( );
        for ( int i = lIterIni; i < lIterFin; ++i )
        {
          lNodeAux = mSWCImporter->getElementAt ( i );
          pos.x = lNodeAux.position[0];
          pos.y = lNodeAux.position[1];
          pos.z = lNodeAux.position[2];

          proj = camera ( )->projectedCoordinatesOf ( pos );

          drawText ( proj.x, proj.y, QString::number ( i ));
        }
        stopScreenCoordinatesSystem ( );
      }
      break;

    case MeshRenderer::DebugMode::DEN_DISTANCE_NORM:
      if ( mSWCImporter != NULL )
      {
        lIterFin = mSWCImporter->getNumNodes ( );

        if (( mDebugRangeIni != 0 )
          || ( mDebugRangeFin != 0 ))
        {
          if ( mDebugRangeIni < lIterFin )
            lIterIni = mDebugRangeIni;
          if ( mDebugRangeFin < lIterFin )
            lIterFin = mDebugRangeFin;
        }

        SWCNode lNodeAux;

        static qglviewer::Vec proj;

        glColor3f ( 0, 0, 0 );

        startScreenCoordinatesSystem ( );
        for ( int i = lIterIni; i < lIterFin; ++i )
        {
          lNodeAux = mSWCImporter->getElementAt ( i );
          pos.x = lNodeAux.position[0];
          pos.y = lNodeAux.position[1];
          pos.z = lNodeAux.position[2];

          proj = camera ( )->projectedCoordinatesOf ( pos );

          drawText ( proj.x, proj.y, QString::number ( lNodeAux.mDendriticDistanceNorm ));
        }
        stopScreenCoordinatesSystem ( );
      }
      break;

    case MeshRenderer::DebugMode::DEN_DISTANCE_REAL:
      if ( mSWCImporter != NULL )
      {
        lIterFin = mSWCImporter->getNumNodes ( );

        if (( mDebugRangeIni != 0 )
          || ( mDebugRangeFin != 0 ))
        {
          if ( mDebugRangeIni < lIterFin )
            lIterIni = mDebugRangeIni;
          if ( mDebugRangeFin < lIterFin )
            lIterFin = mDebugRangeFin;
        }

        SWCNode lNodeAux;

        static qglviewer::Vec proj;
        glColor3f ( 0, 0, 0 );

        startScreenCoordinatesSystem ( );
        for ( int i = lIterIni; i < lIterFin; ++i )
        {
          lNodeAux = mSWCImporter->getElementAt ( i );
          pos.x = lNodeAux.position[0];
          pos.y = lNodeAux.position[1];
          pos.z = lNodeAux.position[2];

          proj = camera ( )->projectedCoordinatesOf ( pos );

          drawText ( proj.x, proj.y, QString::number ( lNodeAux.mDendriticDistanceReal ));
        }
        stopScreenCoordinatesSystem ( );
      }
      break;

    case MeshRenderer::DebugMode::DEN_DISTANCE_TO_SOMA:
      if ( mSWCImporter != NULL )
      {
        lIterFin = mSWCImporter->getNumNodes ( );

        if (( mDebugRangeIni != 0 )
          || ( mDebugRangeFin != 0 ))
        {
          if ( mDebugRangeIni < lIterFin )
            lIterIni = mDebugRangeIni;
          if ( mDebugRangeFin < lIterFin )
            lIterFin = mDebugRangeFin;
        }

        SWCNode lNodeAux;

        static qglviewer::Vec proj;

        glColor3f ( 0, 0, 0 );

        startScreenCoordinatesSystem ( );
        for ( int i = lIterIni; i < lIterFin; ++i )
        {
          lNodeAux = mSWCImporter->getElementAt ( i );
          pos.x = lNodeAux.position[0];
          pos.y = lNodeAux.position[1];
          pos.z = lNodeAux.position[2];

          proj = camera ( )->projectedCoordinatesOf ( pos );

          drawText ( proj.x, proj.y, QString::number ( lNodeAux.mDistanceToSoma ));

        }
        stopScreenCoordinatesSystem ( );
      }
      break;

    case MeshRenderer::DebugMode::DEBUG_VERTEXS: lIterFin = getNeuroSWC ( )->getNumVertex ( );

      if (( mDebugRangeIni != 0 )
        || ( mDebugRangeFin != 0 ))
      {
        if ( mDebugRangeIni < lIterFin )
          lIterIni = mDebugRangeIni;
        if ( mDebugRangeFin < lIterFin )
          lIterFin = mDebugRangeFin;
      }

      glColor3f ( 0, 0, 0 );

      startScreenCoordinatesSystem ( );
      for ( int i = lIterIni; i < lIterFin; ++i )
      {
        lAuxPoint = meshRend->getBaseMesh ( )->getMesh ( )->points ( )[i];

        pos.x = lAuxPoint[0];
        pos.y = lAuxPoint[1];
        pos.z = lAuxPoint[2];

        proj = camera ( )->projectedCoordinatesOf ( pos );

        drawText ( proj.x, proj.y, QString::number ( i ));
      }
      stopScreenCoordinatesSystem ( );
      break;

    case MeshRenderer::DebugMode::DEBUG_FACES: lIterFin = getNeuroSWC ( )->getNumFaces ( );

      if (( mDebugRangeIni != 0 )
        || ( mDebugRangeFin != 0 ))
      {
        if ( mDebugRangeIni < lIterFin )
          lIterIni = mDebugRangeIni;
        if ( mDebugRangeFin < lIterFin )
          lIterFin = mDebugRangeFin;
      }

      glColor3f ( 0, 0, 0 );

      startScreenCoordinatesSystem ( );
      for ( int i = lIterIni; i < lIterFin; ++i )
      {
        lAuxPoint = meshRend->getBaseMesh ( )->getFaceCentroidById ( i );

        pos.x = lAuxPoint[0];
        pos.y = lAuxPoint[1];
        pos.z = lAuxPoint[2];

        proj = camera ( )->projectedCoordinatesOf ( pos );

        drawText ( proj.x, proj.y, QString::number ( i ));
      }
      stopScreenCoordinatesSystem ( );
      break;

    default: break;
  }

}

void NeuroGeneratorWidgetViewer::InitLight ( )
{
  glDisable ( GL_LIGHT0 );

  glShadeModel ( GL_SMOOTH );              // Enable Smooth Shading

  lights = new LightManager ( );
  lights->ActiveLighting ( );

  lights->SetLightPosition ( 0.0f, 0.0f, 0.0f );
  lights->ActiveLight ( 1, true );

  lights->SetLightType ( 0 );

}

QString NeuroGeneratorWidgetViewer::helpString ( ) const
{
  QString text ( "<h2>NeuronizeV2</h2>" );
  text += "<p>NeuronizeV2 is a tool focused on improving interoperability between two commonly used applications in the"
          " field of neuroscience, Imaris and Neurolucida®. Both tools use proprietary formats to store the information,"
          " which causes the information extracted using one tool cannot be used in the other. To solve this problem, "
          "NeuronizeV2 offers the possibility of generating tracing files for Neurolucida® from the Imaris Filament Tracer files.</p>";

  text += "<p>On the other hand, the meshes exported by Imaris can contain morphological errors (holes, faces that "
          "intersect, etc.). These errors are propagated to the metrics computed on these meshes which implies a certain"
          " error. NeuronizeV2 can repair these meshes and then compute more accurate metrics. In addition, the tool can"
          " export the meshes (the original and the repaired one) to widely used standard formats, which allows that "
          "these meshes can be easily used in other tools or shared.</p>";

  text += "<p>Apart from the functions related to interoperability, the tool is also capable of generating a "
          "three-dimensional representation of the neuron. The tool can use the spine information included in the new "
          "version of the Neurolucida® tracing files to place the dendritic spines in their actual positions and "
          "orientations. Likewise, the tool can take advantage of the information contained in the Imaris Filament "
          "Tracer files for, in addition to placing the spines in their real positions, using the real geometry present "
          "in the input file.</p>";

  text += "<p>Additionally, Neurolucida® files can also include detailed information about the soma "
          "(defined as a set of 2D contours). This information can also be used by the tool to generate a highly "
          "accurate soma mesh.</p>";

  text += "<p>Finally, to improve data sharing among neuroscientists, the tool incorporates a small database. This database"
          " stores all the information related to the generated neuron, and the repaired spines, in standard formats. "
          "Additionally, the tool can export the neuron metrics to a CSV file to be easily processed.</p>";

  text += "<p>The way to interact with the tool to achieve all these functions is detailed below.</p>";

  text += "<h3>Generation of a three-dimensional representation from morphological information.</h3>";

  text += "<p>To generate three-dimensional representations form the morphological information of the tracing files the "
          "tool offers two ways: to generate the three-dimensional representation from a single file or generate "
          "representations of several neurons all at once.</p>";

  text += "<p>First, the process of generating a three-dimensional representation of a single file is detailed, "
          "for which the “One Neuron” button must be selected.</p>";

  text += "<ol>";

  text += "<li>The first step is to select the input files to generate the neuron. There are two options: to select "
          "tracing files (SWC or ASC) with the “tracing file” button, or to load a neuron from Imaris Filament Tracer "
          "(VRML) files. To do this, the “VRML Files” button must be selected; then, the file or files containing the "
          "basal dendrites are selected and, if necessary, the file containing the apical dendrite is selected. Finally,"
          " if the user wants to keep the tracing file generated by the tool, he/she clicks on the “Save Tracing” button"
          " and inserts the name for the output file. Note that, the name that will be given to the neuron by the "
          "application will be the name of the input file if it is a tracing file or the name of the folder where the "
          "basal dendrites and axon are found in the case of Imaris Filament Tracer VRML input files.</li>";

  text += "<li>Next, the user goes to the “Soma Builder” tab. If the input file contains soma information, it will be "
         "displayed. If this information is incorrect or does not seem realistic, the user can ignore it by clicking on "
         "the “Use Spherical Soma” button to use an initial spherical soma. Once the initial soma has been decided, "
         "clicking on the “Build Soma” button starts the deformation process. If the user is satisfied with the result, "
         "he/she will click on the “Go to Neurite Generation” button to proceed to the next step. If the result is not "
         "satisfactory, the user can click on the “Rebuild (advanced options)” button to change the parameters of the "
         "deformation process. Finally, if the user wants to export the deformed soma model, he/she will click on the "
         "“Export Soma” button.</li>";

  text +="<li>The next step is done from the “Neurites/Spines Builder” tab, clicking on the “Build Neurites” button to "
         "generate the neurites. Once built, the geometry can be smoothed using the “Smooth” button. Finally, click on "
         "the  “Go to Spine Generation” button to go to the next step.</li>";

  text += "<li>At this point, the user must decide whether to export the neuron in the current state (without spines) by"
          " clicking on the “Export without spines” button or if he/she wants to add the spines by clicking on the "
          "“Build Spines” button.</li>";

  text += "<li>If the user has decided to add the spines, he/she need to choose between the different options available. "
          "Note that, by default, the application selects the most accurate spines that it can place depending on the "
          "input file. Once the spine addition process is over (it may take some time), the neuron mesh can be exported "
          "as well as a mesh containing all the generated spines.</li>";

  text += "</ol>";

  text += "<p>If the user wants to generate more than one three-dimensional representation, he/She can click on the "
          "“Set of Neurons” button, and then select the input directory and the output directory. Note that the input "
          "directories have particular requirements (see manual). Finally, there are two configuration options: one "
          "allows choosing how many times the smoothing process is applied on a neuron and the other establishes a base "
          "name for the folders generated by the tool. The process of automatic generation of tracings from geometry "
          "files is immersed within the neuron generation steps. To generate several tracings automatically in an "
          "unsupervised way from Imaris Filament Tracer files, several neurons will be generated at once and the files "
          "of the generated tracings will be found within the output folder of each corresponding neuron.</p>";

  text += "<h3>Mesh Repair</h3>";

  text += "<p>To repair the meshes it is necessary to have Python3 installed ("
          "<a href=\"https://www.python.org/ftp/python/3.7.4/python-3.7.4-amd64.exe\">https://www.python.org/ftp/python/3.7.4/python-3.7.4-amd64.exe</a>) . "
          "If Python3 is installed in the system, the “Repair Mesh” tab will be active.</p>";

  text += "<p>As in the previous case, the user can process a single file (VRML o IMX) with the “File” button, or "
          "process at once a set of files in a folder by clicking on the “Folder” button. In both cases, he/she needs to"
          " select the input and the output (be them a file or a folder). Finally, by clicking on “Advanced Options”, a "
          "series of advanced options are shown (by default, these options are configured to repair dendritic spines). "
          "When the user places the mouse over the name of these options, a dialog will appear indicating its "
          "functionality.</p>";

  text += "<h3>Mesh Comparison</h3>";

  text += "<p>The tool also allows comparing different meshes and observing the differences between them. This tool is "
          "very useful for comparing the original and repaired meshes.</p>";

  text += "<p>To compare two meshes, first, the files must be loaded (the application supports OBJ, OFF, PLY and STL "
          "formats) by clicking on the “Select File” buttons. Once the two files are selected, the two meshes will be "
          "loaded. The color of the different parts of the meshes will indicate the distance between the meshes (warm "
          "colors imply a greater distance). The application also shows a correspondence between colors and distance, "
          "and a series of metrics (such max distance, min distance and mean distance) related to this distance.</p>";

  text += "<p>On the other hand, the application also allows interacting with the views that are coordinated in such a "
          "way that the camera movements done in one of them are replicated in the other.</p>";

  text += "<h3>Exportation of the generated neurons</h3>";

  text += "<p>The application has a small local database that stores all the neurons processed with the application and "
          "all the spines.</p>";

  text += "<p>To export the information contained in this database, the user clicks on the “File -> Export Neuron Info” "
          "menu, Next, a dialog will open that allows selecting the neurons to be exported. Once those neurons are "
          "selected, the user clicks on the “OK” button and selects the directory where the information will be "
          "exported. This exported information consists of one folder for each neuron that contains two CSV files, "
          "one with the soma information and the other with all its spines information.</p>";
  return text;
}

void NeuroGeneratorWidgetViewer::setColorFromSWC ( QString pFile )
{

  if ( mSWCImporter != NULL )
    delete mSWCImporter;
  mSWCImporter = new SWCImporter ( pFile.toStdString ( ));

  mesh->setColorsFromSWC ( mSWCImporter );
  mesh->setImporter ( mSWCImporter );

  updateGL ( );
}

void NeuroGeneratorWidgetViewer::setPiramidalSoma ( bool pPiramSoma )
{
  mPiramidalSoma = pPiramSoma;
}

void NeuroGeneratorWidgetViewer::setNeuronType ( NeuronType pNeuronType )
{
  mNeuronType = pNeuronType;
}

bool NeuroGeneratorWidgetViewer::hasSpines ( )
{
  if ( meshSpines != NULL )
  {
    return true;
  }

  if (
    ( mSpinesSynthContainers.getContainer ( ).size ( ) > 0 ))
  {
    return true;
  }

  return false;
}

void NeuroGeneratorWidgetViewer::setNeuronColor ( MeshDef::Color pColor )
{
  mesh->setVertexColor ( mesh->getMesh ( )->vertices_begin ( ), mesh->getMesh ( )->vertices_end ( ), pColor );

  updateGL ( );
}

void NeuroGeneratorWidgetViewer::exportSpinesInfo ( QString fileName )
{

  if ( mLastSpinesInfo.size ( ) > 0 )
  {
    exportLastSpinesInfo ( fileName );
  }
}

void NeuroGeneratorWidgetViewer::exportLastSpinesInfo ( QString fileName )
{
  std::ofstream outputFileTXT;
  string fileTXT = fileName.toStdString ( ) + ".SpinesInfo";

  //Clean files
  std::ofstream ( fileTXT.c_str ( ));

  //Open files to add data
  outputFileTXT.open ( fileTXT.c_str ( ), std::ios::app );

  //Head of the file
  //File format version
  outputFileTXT << "#Spines of " << fileTXT.c_str ( ) << endl;

  //No version
  //outputFileTXT  << 0.01 <<endl;

  //Numero de vertices de cada espina, s�lo la primera para indexar en la GPU
  outputFileTXT << " " << mNumVertesSpineIndexation << endl;

  //Total of neurons
  outputFileTXT << " " << mLastSpinesInfo.size ( ) << endl;

  SpineInfo lSpineInfo;
  float lAuxPosX, lAuxPosY, lAuxPosZ;

  for ( int i = 0; i < mLastSpinesInfo.size ( ); ++i )
  {
    lSpineInfo = mLastSpinesInfo.at ( i );

    //Text exit
    outputFileTXT << lSpineInfo.mSpinePosition[0] << " " << lSpineInfo.mSpinePosition[1] << " "
                  << lSpineInfo.mSpinePosition[2] << " "
                  << lSpineInfo.mSpineDirection[0] << " " << lSpineInfo.mSpineDirection[1] << " "
                  << lSpineInfo.mSpineDirection[2] << " "
                  << lSpineInfo.mSpineId << " " << lSpineInfo.mIndexationNode << " "
                  << endl;

  }
  outputFileTXT.close ( );
}

void NeuroGeneratorWidgetViewer::importSpinesInfo ( QString fileName )
{

  if ( meshSpines != NULL )
  {
    delete meshSpines;
    meshSpines = NULL;
  }

  unsigned int lNumMeshes = mSpinesSynthContainers.getContainer ( ).size ( );;

  if ( lNumMeshes != 0 )
  {
    for ( int i = 0; i < lNumMeshes; ++i )
    {
      delete mSpinesSynthContainers.getElementAt ( i );
    }

    mSpinesSynthContainers.getContainer ( ).clear ( );
  }

  meshSpines = new SpinesSWC ( mesh, 500, 6, 5, 0.08, 0.25, 0.08, 0.25 );

  meshSpines->setSpinesContainer ( &mSpinesModelsContainers );
  meshSpines->importSpinesInfo ( fileName.toStdString ( ));
  meshSpines->generateSpinesFromInfo ( );

  mLastSpinesInfo.clear ( );
  mLastSpinesInfo = meshSpines->getSpinesInfo ( );
  mNumVertesSpineIndexation = meshSpines->getNumVerticesEnSpina ( );

  //Add the new SPines to container
  int i = 0;

  mSpinesSynthContainers.addElement ( new BaseMesh ( ));
  mSpinesSynthContainers.getElementAt ( i )->JoinBaseMesh ( meshSpines );

  MeshDef::ConstVertexIter iniLimit = mSpinesSynthContainers.getElementAt ( i )->getMesh ( )->vertices_begin ( );
  MeshDef::ConstVertexIter finLimit = mSpinesSynthContainers.getElementAt ( i )->getMesh ( )->vertices_end ( );

  mSpinesSynthContainers.getElementAt ( i )->setVertexColor ( iniLimit, finLimit, MeshDef::Color ( 0.5, 0.5, 1.0, 1.0 )
  );

  delete meshSpines;
  meshSpines = NULL;

  if ( spineMeshRend != NULL )
  {
    delete spineMeshRend;
  }

  spineMeshRend = new MeshRenderer ( );

  spineMeshRend->setMeshToRender ( mSpinesSynthContainers.getElementAt ( 0 ));

  spineMeshRend->setRenderOptions ( renderMask );

  updateGL ( );

}

void NeuroGeneratorWidgetViewer::generateSpinesVrml(skelgenerator::Neuron *neuron,const std::string& tempPath) {
    if ( mesh == NULL )
        return;

    if ( meshSpines != NULL )
    {
        delete meshSpines;
        meshSpines = NULL;
    }
    std::string dirPath = tempPath + "/tmpSpines";
    QDir().mkdir(QString::fromStdString(dirPath));

    boost::numeric::ublas::matrix<float> translationMatrix (4,4);
    boost::numeric::ublas::matrix<float> scaleMatrix (4,4);

    auto displacement = mSWCImporter->getDisplacement();

    generateSquareTraslationMatrix(translationMatrix,-displacement[0],-displacement[1],-displacement[2]);
    int total_files = neuron->getSpines().size();
    std::vector<SpinesSWC*> spinesMeshes(static_cast<size_t>(total_files), nullptr);
    QFileInfo fi(mSWCFile);
    bool haveSpinesNeuron = Neuronize::bbdd.haveSpinesNeuron(fi.baseName().toStdString());
    Neuronize::bbdd.openTransaction();
    int i = 0;
    for (const auto& spine:neuron->getSpines()) {
        std::string filename = spine->to_obj_without_base(dirPath,i);
        if (!haveSpinesNeuron) {
            Neuronize::bbdd.addSpineVRML(spine, filename, fi.baseName().toStdString(), tempPath, displacement);
        }
        SpinesSWC* auxMesh = new SpinesSWC();
        auxMesh->loadModel(filename);
        auxMesh->applyMatrixTransform(translationMatrix,4);
        auxMesh->updateBaseMesh();
        spinesMeshes[i] = auxMesh;
        i++;
    }
    Neuronize::bbdd.closeTransaction();

    meshSpines = fusionAllSpines(spinesMeshes);
    for(int i = 0; i < spinesMeshes.size();i++) {
        if (spinesMeshes[i] != meshSpines) {
            delete spinesMeshes[i];
        } else {
            std::cout << i << ":"<< spinesMeshes.size() <<std::endl;
        }
    }


    mSpinesSynthContainers.addElement(new BaseMesh());
    mSpinesSynthContainers.getElementAt(0)->JoinBaseMesh(meshSpines);


    MeshDef::ConstVertexIter iniLimit = mSpinesSynthContainers.getElementAt(0)->getMesh()->vertices_begin();
    MeshDef::ConstVertexIter finLimit = mSpinesSynthContainers.getElementAt(0)->getMesh()->vertices_end();

    mSpinesSynthContainers.getElementAt(0)->setVertexColor(iniLimit, finLimit, MeshDef::Color(0.6, 0.0, 0.0, 1.0)
    );


    if ( spineMeshRend != NULL )
    {
        delete spineMeshRend;
    }

    spineMeshRend = new MeshRenderer ( );

    spineMeshRend->setMeshToRender(mSpinesSynthContainers.getElementAt(0));

    spineMeshRend->setRenderOptions ( renderMask );

    updateGL();
}

SpinesSWC* NeuroGeneratorWidgetViewer::fusionAllSpines(vector<SpinesSWC *> &spineMeshes) {
  QThreadPool pool;
  std::queue<QFuture<SpinesSWC *>> futures;
  for (int i = 1; i < spineMeshes.size(); i += 2) {
    QFuture<SpinesSWC *> future = QtConcurrent::run(&pool, [=]() {
        return fusionSpines(spineMeshes[i - 1], spineMeshes[i]);
    });

    futures.push(future);
  }

  if (spineMeshes.size() % 2 == 1) {
    auto future = QtConcurrent::run(&pool, [=]() { return spineMeshes[spineMeshes.size() - 1]; });
    futures.push(future);
  }

  while (futures.size() > 1) {
    auto mesh1 = futures.front().result();
    futures.pop();
    auto mesh2 = futures.front().result();
    futures.pop();
    auto future = QtConcurrent::run(&pool, [=]() { return fusionSpines(mesh1, mesh2); });
    futures.push(future);
  }
  return futures.front().result();
}

SpinesSWC* NeuroGeneratorWidgetViewer::fusionSpines(SpinesSWC* mesh1, SpinesSWC* mesh2) {
    mesh1->JoinBaseMesh(mesh2);
    mesh1->updateBaseMesh();
    return mesh1;
}

void
NeuroGeneratorWidgetViewer::generateSpinesASC(std::vector<Spine>& spines,unsigned int pHorResol, unsigned int pVerResol,
                                              float pMinLongSpine, float pMaxLongSpine, float pMinRadio,
                                              float pMaxRadio) {
    unsigned int lNumMeshes = mSpinesSynthContainers.getContainer ( ).size ( );
    unsigned int lTotalNumSpines = 0;
    float lDendriticModificator = 0;
    float lNumGroupsOfSpines = 0;
    BaseMesh *tmpMesh = NULL;

    if ( mesh == NULL )
        return;

    if ( meshSpines != NULL )
    {
        delete meshSpines;
        meshSpines = nullptr;
    }

    if ( spineMeshRend != NULL )
    {
        delete spineMeshRend;
    }

    spineMeshRend = new MeshRenderer ( );

    if ( lNumMeshes != 0 )
    {
        for ( int i = 0; i < lNumMeshes; ++i )
        {
            delete mSpinesSynthContainers.getElementAt ( i );
        }

        mSpinesSynthContainers.getContainer ( ).clear ( );
    }

    meshSpines =
            new SpinesSWC (mesh, static_cast<unsigned int>(spines.size()), pHorResol, pVerResol, pMinLongSpine, pMaxLongSpine, pMinRadio, pMaxRadio );

    meshSpines->setSpinesContainer ( &mSpinesModelsContainers );

    auto displacement = mSWCImporter->getDisplacement();
    QFileInfo fi(mSWCFile);
    meshSpines->distributeSpines (spines,fi.baseName().toStdString(),displacement,Neuronize::bbdd,Neuronize::tmpPath.toStdString());

    int i = 0;

    mSpinesSynthContainers.addElement ( new BaseMesh ( ));
    mSpinesSynthContainers.getElementAt ( i )->JoinBaseMesh ( meshSpines );


    MeshDef::ConstVertexIter iniLimit = mSpinesSynthContainers.getElementAt ( i )->getMesh ( )->vertices_begin ( );
    MeshDef::ConstVertexIter finLimit = mSpinesSynthContainers.getElementAt ( i )->getMesh ( )->vertices_end ( );

    mSpinesSynthContainers.getElementAt ( i )->setVertexColor ( iniLimit, finLimit, MeshDef::Color ( 0.6, 0.0, 0.0, 1.0 )
    );

    mLastSpinesInfo.clear ( );
    mLastSpinesInfo = meshSpines->getSpinesInfo ( );
    mNumVertesSpineIndexation = meshSpines->getNumVerticesEnSpina ( );

    delete meshSpines;
    meshSpines = NULL;

    if ( spineMeshRend != NULL )
    {
        delete spineMeshRend;
    }

    spineMeshRend = new MeshRenderer ( );

    spineMeshRend->setMeshToRender ( mSpinesSynthContainers.getElementAt ( 0 ));

    spineMeshRend->setRenderOptions ( renderMask );

    boost::numeric::ublas::matrix<float> translationMatrix (4,4);
    generateSquareTraslationMatrix(translationMatrix,-displacement[0],-displacement[1],-displacement[2]);
    spineMeshRend->getBaseMesh()->applyMatrixTransform(translationMatrix,4);

    updateGL ( );
}







