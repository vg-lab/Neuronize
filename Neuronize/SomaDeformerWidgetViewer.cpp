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

#include <QMouseEvent>
#include <math.h>
#include <limits.h>
#include "SomaDeformerWidgetViewer.h"

#define INCSCALE 0.0
#define SOMAID  1
#define _DELETE_RETESEL_VERTEXS_ true

// Constructor must call the base class constructor.
#if QT_VERSION < 0x040000
                                                                                                                        SomaDeformerWidgetViewer::SomaDeformerWidgetViewer(QWidget *parent, const char *name)
  : QGLViewer(parent, name)
#else
SomaDeformerWidgetViewer::SomaDeformerWidgetViewer ( QWidget *parent )
  : QGLViewer ( parent )
#endif
{
  //restoreStateFromFile();
  //help();

  camera ( )->setPosition ( qglviewer::Vec ( 0, 0, 50 ));
  camera ( )->setOrientation ( qglviewer::Quaternion ( qglviewer::Vec ( 1, 0, 0 ), qglviewer::Vec ( 0, 0, 0 )));

}

void SomaDeformerWidgetViewer::draw ( )
{
  const qglviewer::Vec cameraPos = camera ( )->position ( );
  const GLfloat pos[4] = { ( float ) cameraPos[0], ( float ) cameraPos[1], ( float ) cameraPos[2], 1.0 };
  glLightfv ( GL_LIGHT1, GL_POSITION, pos );

  glLightfv ( GL_LIGHT1, GL_SPOT_DIRECTION, camera ( )->viewDirection ( ));

  if ( meshRenderer != NULL )
  {
    if ( lDebugMode )
      glutWireSphere ( mRealSomaRadio, 12, 12 );

    mBaseMesh->getUnprotectedMesh ( )->update_normals ( );
    meshRenderer->Render ( );
    drawSelection ( );
  }

  if ( mPatternMeshRenderer != NULL )
  {
    mPatternMeshRenderer->Render ( );
  }

  if ( mSelectionMode != NONE )
  {
    drawSelectionRectangle ( );
  }

}

void SomaDeformerWidgetViewer::animate ( )
{
  if ( mDeformating )
  {
    if ( mMSDeformator != NULL )
    {
      if ( actInterpol < numInterpol )
      {
        updateInterpolation ( );
        ++actInterpol;
      }

      mMSDeformator->update ( );

      updateFramesForModel ( );

      if ( mExportOBJSecuence )
      {
        exportModel ( "SomaDeformSecuence_" + QString::number ( ++mActDeformIter ) + ".obj" );
      }
    }
  }
}

void SomaDeformerWidgetViewer::mousePressEvent ( QMouseEvent *e )
{
  mSelectionRectangle = QRect ( e->pos ( ), e->pos ( ));

  if (( e->button ( ) == Qt::LeftButton ) && ( e->modifiers ( ) == Qt::ShiftModifier ))
  {
    mSelectionMode = ADD;
  }
  else
  {
    if (( e->button ( ) == Qt::LeftButton ) && ( e->modifiers ( ) == Qt::AltModifier ))
    {
      mSelectionMode = REMOVE;
    }
    else
    {
      if ( e->modifiers ( ) == Qt::ControlModifier )
      {
        startManipulation ( );
      }
      QGLViewer::mousePressEvent ( e );
    }
  }

}

void SomaDeformerWidgetViewer::mouseMoveEvent ( QMouseEvent *e )
{
  if ( mSelectionMode != NONE )
  {
    mSelectionRectangle.setBottomRight ( e->pos ( ));
    updateGL ( );
  }
  else
  {
    QGLViewer::mouseMoveEvent ( e );
  }
}

void SomaDeformerWidgetViewer::mouseReleaseEvent ( QMouseEvent *e )
{
  if ( mSelectionMode != NONE )
  {
    // Actual selection on the rectangular area.
    // Possibly swap left/right and top/bottom to make rectangle_ valid
    mSelectionRectangle = mSelectionRectangle.normalized ( );

    // Define selection window dimensions
    setSelectRegionWidth ( mSelectionRectangle.width ( ));
    setSelectRegionHeight ( mSelectionRectangle.height ( ));
    // Compute rectangle center and perform selection
    select ( mSelectionRectangle.center ( ));
    // Update display to show new selected objects
    updateGL ( );
  }
  else
    QGLViewer::mouseReleaseEvent ( e );

}

void SomaDeformerWidgetViewer::drawWithNames ( )
{
  for ( int i = 0; i < int ( mMSDeformator->getNodeContainer ( ).size ( )); i++ )
  {
    glPushName ( i );
    glPushMatrix ( );
    glMultMatrixd ( mFrameSelectors.at ( i ).matrix ( ));
    glutSolidSphere ( mRealSomaRadio/20.0, 4, 4 );
    glPopMatrix ( );
    glPopName ( );
  }
}

void SomaDeformerWidgetViewer::endSelection ( const QPoint & )
{
  // Flush GL buffers
  glFlush ( );

  // Get the number of objects that were seen through the pick matrix frustum. Reset GL_RENDER mode.
  GLint nbHits = glRenderMode ( GL_RENDER );

  if ( nbHits > 0 )
  {
    for ( int i = 0; i < nbHits; ++i )
    {
      switch ( mSelectionMode )
      {
        case ADD    : addIdToSelection (( selectBuffer ( ))[4*i + 3] );
          break;
        case REMOVE : removeIdFromSelection (( selectBuffer ( ))[4*i + 3] );
          break;
        default : break;
      }
    }
  }
  mSelectionMode = NONE;
}

void SomaDeformerWidgetViewer::setupViewer ( )
{
  //Viewer configuration
  camera ( )->setZClippingCoefficient ( 500.0 );

  mBaseMesh = NULL;
  mMSDeformator = NULL;
  meshRenderer = NULL;
  mSWCImporter = NULL;

  mPatternMesh = NULL;
  mPatternMeshRenderer = NULL;

  mExportOBJSecuence = false;
  mRedundantVertex = true;

  mRenderMask = 0;
  mRenderMask = MeshRenderer::RENDER_SURFACE
    ^ MeshRenderer::RENDER_VERTICES
    ^ MeshRenderer::RENDER_EDGES
    ^ MeshRenderer::RENDER_NORMALS;

  mModelColor = QColor ( 128, 128, 255 );

  mSelectionMode = NONE;

  mDeformating = false;
  mNormalizeModel = true;
  mRealSomaRadio = 1;

  numInterpol = 100;
  actInterpol = 0;

  mUseSphericalProjection = true;
  mUseContourPropagation = true;
  lDebugMode = true;

  mApicalParam = 0;
  mBasalParam = 0;

  mMSDeformator = new MSObject ( );
  mXMLSomaDefManager = NULL;

  setAnimationPeriod ( 20 );
  startAnimation ( );

  updateGL ( );

  QColor mBackGroundColor = QColor ( 250, 250, 250 );
  //QColor mBackGroundColor = QColor(0,0,0);
  //setBackgroundColor(mBackGroundColor);
}

void SomaDeformerWidgetViewer::drawSelectionRectangle ( ) const
{
  startScreenCoordinatesSystem ( );
  glDisable ( GL_LIGHTING );

  glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
  glEnable ( GL_BLEND );

  glColor4f ( 0.0, 0.0, 0.3f, 0.3f );
  glBegin ( GL_QUADS );
  glVertex2i ( mSelectionRectangle.left ( ), mSelectionRectangle.top ( ));
  glVertex2i ( mSelectionRectangle.right ( ), mSelectionRectangle.top ( ));
  glVertex2i ( mSelectionRectangle.right ( ), mSelectionRectangle.bottom ( ));
  glVertex2i ( mSelectionRectangle.left ( ), mSelectionRectangle.bottom ( ));
  glEnd ( );

  glColor4f ( 0.4f, 0.4f, 0.5f, 0.5f );
  glBegin ( GL_LINE_LOOP );
  glVertex2i ( mSelectionRectangle.left ( ) - 1, mSelectionRectangle.top ( ) - 1 );
  glVertex2i ( mSelectionRectangle.right ( ) + 1, mSelectionRectangle.top ( ) - 1 );
  glVertex2i ( mSelectionRectangle.right ( ) + 1, mSelectionRectangle.bottom ( ) + 1 );
  glVertex2i ( mSelectionRectangle.left ( ) - 1, mSelectionRectangle.bottom ( ) + 1 );
  glEnd ( );

  glDisable ( GL_BLEND );
  glEnable ( GL_LIGHTING );
  stopScreenCoordinatesSystem ( );
}

void SomaDeformerWidgetViewer::startManipulation ( )
{

  qglviewer::Vec lQtVec;
  OpenMesh::Vec3f lAverPos ( 0, 0, 0 );
  MSNode *lMSNode = NULL;

  FrameConstrictor *fc = ( FrameConstrictor * ) ( manipulatedFrame ( )->constraint ( ));
  fc->clearSet ( );

  for ( std::list < int >::const_iterator it = mMSDeformator->getSelectionContainer ( ).begin ( ),
          end = mMSDeformator->getSelectionContainer ( ).end ( ); it != end; ++it )
  {
    lMSNode = &mMSDeformator->getNodeContainer ( )[*it];
    fc->addObjectToSet ( lMSNode );
    lAverPos += ( lMSNode )->getPos ( );

    // update the node position at the new frame location
    //!!!mMSDeformator->getNodeContainer()[*it]->updatePos();
  }

  lQtVec = qglviewer::Vec ( lAverPos[0], lAverPos[1], lAverPos[2] );

  if ( mMSDeformator->getSelectionContainer ( ).size ( ) > 0 )
    manipulatedFrame ( )->setPosition ( lQtVec/mMSDeformator->getSelectionContainer ( ).size ( ));
}

bool SomaDeformerWidgetViewer::isInSelection ( int pId )
{
  bool lVal = false;

  std::list < int >::const_iterator itStart = mMSDeformator->getSelectionContainer ( ).begin ( );
  std::list < int >::const_iterator itEnd = mMSDeformator->getSelectionContainer ( ).end ( );

  while (
    ( itStart != itEnd )
      && ( !lVal )
    )
  {
    if ( *itStart == pId )
    {
      lVal = true;
    }
    itStart++;
  }

  return lVal;
}

void SomaDeformerWidgetViewer::addIdToSelection ( int id )
{
  if ( !isInSelection ( id ))
  {
    mMSDeformator->getSelectionContainer ( ).push_back ( id );
    mMSDeformator->getNodeContainer ( ).at ( id ).setFix ( true );
  }
}

void SomaDeformerWidgetViewer::removeIdFromSelection ( int id )
{
  mMSDeformator->getSelectionContainer ( ).remove ( id );
  mMSDeformator->getNodeContainer ( ).at ( id ).setFix ( false );
}

void SomaDeformerWidgetViewer::generateSomaModel ( unsigned int pModelId, float pScaleFactor )
{

  if ( mBaseMesh == NULL )
  {
    mBaseMesh = new ProceduralMesh ( );
  }
  else
  {
    delete mBaseMesh;
    mBaseMesh = new ProceduralMesh ( );
  }

  mBaseMesh->JoinBaseMesh ( mSomaModelsContainers.getElementAt ( pModelId ));
  mBaseMesh->scaleBaseMesh ( pScaleFactor );
  setModelColor ( );

  if ( mMSDeformator == NULL )
  {
    mMSDeformator = new MSObject ( );
  }
  else
  {
    mMSDeformator->destroyMSBaseStructure ( );
  }

  mMSDeformator->setMesh ( mBaseMesh );
  mMSDeformator->generateMSBaseStructure ( );

  if ( meshRenderer == NULL )
  {
    meshRenderer = new MeshRenderer ( );
  }
  else
  {
    delete meshRenderer;
    meshRenderer = new MeshRenderer ( );
  }

  meshRenderer->setMeshToRender ( mBaseMesh );
  meshRenderer->setRenderOptions ( mRenderMask );

  mDeformating = false;

  generateFramesForModel ( );

  //Apply swc deformation
  setSWCDendriticConstraints ( );
}

void SomaDeformerWidgetViewer::loadSomaContainer ( QDir pDir )
{
  QStringList filters;
  filters << "*.obj" << "*.off" << "*.ply" << "*.stl";

  QStringList list = pDir.entryList ( filters, QDir::Files );

  BaseMesh *auxBaseMesh;
  for ( int i = 0; i < list.size ( ); ++i )
  {
    std::string modelName = pDir.absolutePath ( ).toStdString ( ) + "/" + list.at ( i ).toStdString ( );
    auxBaseMesh = new BaseMesh ( modelName );
    mSomaModelsContainers.addElement ( auxBaseMesh );
  }
}

void SomaDeformerWidgetViewer::loadSWCFile ( QString pSWCFile )
{
  //Store the importer for debug mode
  if ( mSWCImporter != NULL )
    delete mSWCImporter;
  mSWCImporter = new SWCImporter ( pSWCFile.toStdString ( ));

  mRealSomaRadio = mSWCImporter->getElementAt ( 1 ).radius;
}

void SomaDeformerWidgetViewer::unLoadSWCFile ( )
{
  //Store the importer for debug mode
  if ( mSWCImporter != NULL )
    delete mSWCImporter;
  mSWCImporter = NULL;

  mRealSomaRadio = 1.0;
}

void SomaDeformerWidgetViewer::loadPatternMesh ( QString pSWCFile )
{
  if ( mPatternMesh == NULL )
  {
    mPatternMesh = new BaseMesh ( );
  }
  else
  {
    delete mPatternMesh;
    mPatternMesh = new BaseMesh ( );
  }

  mPatternMesh->loadModel ( pSWCFile.toStdString ( ));
  setPatternModelColor ( );

  if ( mPatternMeshRenderer == NULL )
  {
    mPatternMeshRenderer = new MeshRenderer ( );
  }
  else
  {
    delete mPatternMeshRenderer;
    mPatternMeshRenderer = new MeshRenderer ( );
  }

  mPatternMeshRenderer->setMeshToRender ( mPatternMesh );
  mPatternMeshRenderer->setRenderOptions ( mRenderMask );

}

SWCImporter *SomaDeformerWidgetViewer::getSWCImporter ( ) const
{
  return mSWCImporter;
}

void SomaDeformerWidgetViewer::setNormalizeExportedModel ( bool pExport )
{
  mNormalizeModel = pExport;
}

void SomaDeformerWidgetViewer::setUseSphericalProjection ( bool pUseSphericalP )
{
  mUseSphericalProjection = pUseSphericalP;

  //!!!!
  if ( mXMLSomaDefManager == NULL )
    setSWCDendriticConstraints ( );
  else
    setSWCDendriticConstraintsFromXML ( );
}

void SomaDeformerWidgetViewer::setUseContourPropagation ( bool pUseContourP )
{
  mUseContourPropagation = pUseContourP;
  setSWCDendriticConstraints ( );
}

void SomaDeformerWidgetViewer::generateSomaProcedural (//OpenMesh::Vec3f center,
  float pRadio, unsigned int pHResol, unsigned int pVResol )
{
  if ( mBaseMesh == NULL )
  {
    mBaseMesh = new ProceduralMesh ( );
  }
  else
  {
    delete mBaseMesh;
    mBaseMesh = new ProceduralMesh ( );
  }

  mBaseMesh->generateSphere ( OpenMesh::Vec3f ( 0, 0, 0 ), pRadio, pHResol, pVResol );
  setModelColor ( );

  if ( mMSDeformator == NULL )
  {
    mMSDeformator = new MSObject ( );
  }
  else
  {
    mMSDeformator->destroyMSBaseStructure ( );
  }

  mMSDeformator->setMesh ( mBaseMesh );
  mMSDeformator->generateMSBaseStructure ( );

  if ( meshRenderer == NULL )
  {
    meshRenderer = new MeshRenderer ( );
  }
  else
  {
    delete meshRenderer;
    meshRenderer = new MeshRenderer ( );
  }

  meshRenderer->setMeshToRender ( mBaseMesh );
  meshRenderer->setRenderOptions ( mRenderMask );

  mDeformating = false;

  generateFramesForModel ( );

  //Apply swc deformation
  setSWCDendriticConstraints ( );
}

void SomaDeformerWidgetViewer::generateSomaFromXML ( QString pFileToGenerate, float pSomaScale )
{
  if ( mBaseMesh == NULL )
  {
    mBaseMesh = new ProceduralMesh ( );
  }
  else
  {
    delete mBaseMesh;
    mBaseMesh = new ProceduralMesh ( );
  }

  if ( mXMLSomaDefManager == NULL )
  {
    mXMLSomaDefManager = new XMLSomaDefManager ( );
  }
  else
  {
    delete mXMLSomaDefManager;
    mXMLSomaDefManager = new XMLSomaDefManager ( );
  }

  mXMLSomaDefManager->loadXMLContent ( pFileToGenerate );

  //Seteo del path de los ficheros
  QFileInfo lFileInfo ( pFileToGenerate );
  QString lpath = lFileInfo.absolutePath ( );

  QString lSWCPath ( "" );
  QString lModelPath ( "" );

  mXMLSomaDefManager->setPathToFiles ( lpath );

  mXMLSomaDefManager->loadGeodesicDistanceFilesContent ( );
  mXMLSomaDefManager->loadGeodesicVertexIdesCandFilesContent ( );

  //Cargar el SWC
  lSWCPath = lpath + "/" + mXMLSomaDefManager->getXMLSomaDef ( )->mSWCPathFile;
  loadSWCFile ( lSWCPath );

  float lRadio = mSWCImporter->getElementAt ( SOMAID ).radius*pSomaScale;
  float lHResol = mXMLSomaDefManager->getXMLSomaDef ( )->mSomaHorResol;
  float lVResol = mXMLSomaDefManager->getXMLSomaDef ( )->mSomaVerResol;

  if (( lHResol < 1 ) || ( lVResol < 1 ))
  {
    lModelPath = lpath + "/" + mXMLSomaDefManager->getXMLSomaDef ( )->mModelPathFile;
    mBaseMesh->loadModel ( lModelPath.toStdString ( ));
    mBaseMesh->scaleBaseMesh ( pSomaScale );
  }
  else
  {
    mBaseMesh->generateSphere ( OpenMesh::Vec3f ( 0, 0, 0 ), lRadio, lHResol, lVResol );
  }

  setModelColor ( );

  if ( mMSDeformator == NULL )
  {
    mMSDeformator = new MSObject ( );
  }
  else
  {
    mMSDeformator->destroyMSBaseStructure ( );
  }

  mMSDeformator->setMesh ( mBaseMesh );
  mMSDeformator->generateMSBaseStructure ( );

  if ( meshRenderer == NULL )
  {
    meshRenderer = new MeshRenderer ( );
  }
  else
  {
    delete meshRenderer;
    meshRenderer = new MeshRenderer ( );
  }

  meshRenderer->setMeshToRender ( mBaseMesh );
  meshRenderer->setRenderOptions ( mRenderMask );

  mDeformating = false;

  generateFramesForModel ( );

  //Apply swc deformation
  setSWCDendriticConstraintsFromXML ( );
}

void SomaDeformerWidgetViewer::exportModel ( QString pFile )
{
  if ( meshRenderer != NULL )
  {
    ProceduralMesh *lPBaseMesh = new ProceduralMesh ( );
    lPBaseMesh->JoinBaseMesh ( meshRenderer->getBaseMesh ( ));

    MeshDef::VertexHandle lVertexHandle;

    if ( mRedundantVertex )
    {
      for ( unsigned int i = 0; i < mVertexIdToDel.size ( ); ++i )
      {
        lVertexHandle = lPBaseMesh->getUnprotectedMesh ( )->vertex_handle ( mVertexIdToDel.at ( i ));
        lPBaseMesh->getUnprotectedMesh ( )->delete_vertex ( lVertexHandle );
      }
    }

    //Free the memory of deleted objects
    lPBaseMesh->getUnprotectedMesh ( )->garbage_collection ( );
    lPBaseMesh->updateBaseMesh ( );

    if ( mNormalizeModel )
    {
      lPBaseMesh->scaleBaseMesh ( 1.0/( mRealSomaRadio + INCSCALE ));
      lPBaseMesh->exportMesh ( pFile.toStdString ( ));
    }
    else
    {
      lPBaseMesh->exportMesh ( pFile.toStdString ( ));
    }

    delete lPBaseMesh;
  }
}

QString SomaDeformerWidgetViewer::configurationToString ( )
{
  QString result ( "" );

  result += "\nParams associated with the model";
  result += "\n--------------------------------";
  result += "\nNormalizeModel: ";
  if ( mNormalizeModel )
    result += "true";
  else
    result += "false";

  result += "\nRealSomaRadio: " + QString::number ( mRealSomaRadio );

  result += "\nUseSphericalProjection: ";
  if ( mUseSphericalProjection )
    result += "true";
  else
    result += "false";

  result += "\nUseSphericalProjection: ";
  if ( mUseSphericalProjection )
    result += "true";
  else
    result += "false";

  result += "\n\nParams associated with the Mass Spring";
  result += "\n--------------------------------";

  result += "\nDt: " + QString::number ( mMSDeformator->getParamDt ( ));
  result += "\nDumping: " + QString::number ( mMSDeformator->getParamDumping ( ));
  result += "\nStiffness: " + QString::number ( mMSDeformator->getParamStiffness ( ));
  result += "\nSystemMass: " + QString::number ( mMSDeformator->getParamSystemMass ( ));
  result += "\nNodeMass: " + QString::number ( mMSDeformator->getParamNodeMass ( ));
  result += "\nIntegrationMethod: " + QString::number ( mMSDeformator->getParamIntegrationMethod ( ));
  result += "\nUnCollapseSpringsPercent: " + QString::number ( mMSDeformator->getParamUnCollapseSpringsPercent ( ));
  result += "\nUseConstraits: ";

  if ( mMSDeformator->getParamUseConstraits ( ))
    result += "true";
  else
    result += "false";

  result += "\n\nParams associated with the XML definition";
  result += "\n--------------------------------";
  result += "\nPendiente aun.";
  //result+="\nSWCPathFile: " + mXMLSomaDefManager->getXMLDomDocument().	mSWCPathFile;
  if (mSWCImporter != NULL) {
    result += "\n\nParams associated with SWCImporter";
    result += "\n--------------------------------";
    result += "\nSWCNumDendritics: " + QString::number(mSWCImporter->getNumDendritics());
  }

  return result;
}

void SomaDeformerWidgetViewer::setModelColor ( )
{
  MeshDef::ConstVertexIter iniLimit = mBaseMesh->getMesh ( )->vertices_begin ( );
  MeshDef::ConstVertexIter finLimit = mBaseMesh->getMesh ( )->vertices_end ( );

  mBaseMesh->setVertexColor (
    iniLimit, finLimit,

    MeshDef::Color ( mModelColor.redF ( ), mModelColor.greenF ( ), mModelColor.blueF ( ), 1.0 )

  );
}

void SomaDeformerWidgetViewer::setPatternModelColor ( )
{
  MeshDef::ConstVertexIter iniLimit = mPatternMesh->getMesh ( )->vertices_begin ( );
  MeshDef::ConstVertexIter finLimit = mPatternMesh->getMesh ( )->vertices_end ( );

  mPatternMesh->setVertexColor (
    iniLimit,
    finLimit,
    MeshDef::Color ( mModelColor.redF ( )/255, mModelColor.greenF ( ), mModelColor.blueF ( )/255, 1.0 )
  );

}

void SomaDeformerWidgetViewer::startDeformation ( bool pDeform )
{
  mDeformating = pDeform;
}

void SomaDeformerWidgetViewer::singleStepDeformation ( )
{
  mDeformating = false;

  if ( mMSDeformator != NULL )
  {
    if ( actInterpol < numInterpol )
    {
      updateInterpolation ( );
      ++actInterpol;
    }

    mMSDeformator->update ( );
    updateFramesForModel ( );
  }
}

void SomaDeformerWidgetViewer::reSetAnimationPeriod ( int pPeriod )
{
  stopAnimation ( );
  setAnimationPeriod ( pPeriod );
  startAnimation ( );
}

//Mass Spring seters
void SomaDeformerWidgetViewer::setMSIntegrator ( int pMethod )
{
  if ( mMSDeformator != NULL )
    mMSDeformator->setMSIntegrator ( pMethod );
}

void SomaDeformerWidgetViewer::setMSDt ( float pDt )
{
  if ( mMSDeformator != NULL )
    mMSDeformator->setMSDt ( pDt );
}

void SomaDeformerWidgetViewer::setMSConstraints ( bool pUseConstraits )
{
  if ( mMSDeformator != NULL )
    mMSDeformator->setMSConstraints ( pUseConstraits );
}

void SomaDeformerWidgetViewer::setMSSpringParams ( float pStiffness, float pDumping )
{
  if ( mMSDeformator != NULL )
    mMSDeformator->setMSSpringParams ( pStiffness, pDumping );
}

void SomaDeformerWidgetViewer::setMSNodeParams ( float pMass, OpenMesh::Vec3f pGravity )
{
  if ( mMSDeformator != NULL )
    mMSDeformator->setMSNodeParams ( pMass, pGravity );
}

void SomaDeformerWidgetViewer::setUnCollapseSprings ( int pUnCollapseSpringsPercent )
{
  if ( mMSDeformator != NULL )
    mMSDeformator->setUnCollapseSprings ( pUnCollapseSpringsPercent );
}

void SomaDeformerWidgetViewer::setDendriticParams ( float pBasalParam, float pApicalParam )
{
  mBasalParam = pBasalParam;
  mApicalParam = pApicalParam;

  setSWCDendriticConstraints ( );
}

QString SomaDeformerWidgetViewer::helpString ( ) const
{
  QString text ( "<h2>Neuronize</h2>" );

  text +=
    "Neuronize is used for building three-dimensional models for neuronal cells from the morphological information extracted through computer-aided tracing applications (.swc and .asc).<br><br>";
  text += "Neuronize is designed to be easy to use. <br><br>";
  text +=
    "There is a standard menu at the top and 5 buttons. The first button can be used to start building a new neuron. The second button �undo�, represented with an arrow, allows going one step back, in case you want to redo the current step from scratch. The camera button, allows saving snapshots at any moment of your meshes. The help button shows this help and information about the application. The last button can be used to close the application.<br><br>";
  text +=
    "The process of building a neuron implies a few steps. Steps are designed to be easy to follow (default itinerary). Alternatively, you can choose to modify the advance options of a particular step if desired.<br><br>";
  text += "To follow the default itinerary just do the following:<br><br>";
  text +=
    "	1.	Once the application is opened, press �Load File� and examine until finding your computer-aided tracing file (.swc or .asc) within your directories (this can take a few seconds). You can find and try two example files: ExampleNeuron1.asc and ExampleNeuron2.swc.<br><br>";
  text +=
    "		Once the soma has been loaded, the message� Soma to build = ... � will change to something similar to �Soma to build = ExampleNeuron1.swc�. Then, press �Next step�.<br><br>";
  text +=
    "	2.	Now, you are at the �Soma builder� tab. Press �Build soma� to start the deformation of the soma. If you like its shape, simply press �Next step�. (Alternatively, you can press �Rebuild (advanced options)� to build the soma changing the parameter options and pressing �Build soma�. Here, you can export the soma model by pressing �Export soma mesh� if desired).<br><br>";
  text +=
    "	3.	We are now at �Dendrites/Spines builder� tab. Press �Build dendrites� to start building the dendrites (this may take a few seconds). If you like the neuron, simply press �Next step�. You can also smooth the dendrites by pressing �Smooth�. If you want to return to the original construction, simply press again �Build dendrites�.<br><br>";
  text +=
    "		(Alternatively, you can press �Rebuild (advanced options)� to build the dendrites changing the parameter options and pressing �Build dendrites�. �. Here, you can export the neuron model by pressing �Export neuron mesh� if desired).<br><br>";
  text +=
    "	4.	Now, you have the possibility of building spines. This process normally takes several minutes, so please, consider it before starting, since it will imply waiting for a while. If you do not desire to build the spines, you can simply press �Finish� to save your model in the directory of your choice. If you want to build the spines, press �Build Spines�. This will take you to the �Advanced spines options� tab, where a default density spine dataset and a default spine morphology dataset are selected (future versions will allow to include your own spine density and morphology datasets). Press the button �Build spines�. A message warning that this process takes several minutes will appear. The length will vary depending on your neuron, and can take from several minutes to a few hours. If you decide not to build the spines, press �No�, and then �Finish� to save your neuron. If you still want to continue, press �Yes� and wait for the spines to be built. Once finished, you can press �Finish� and save your neuron and your spines, in a directory of your choice.<br><br>";

  return text;
}

void SomaDeformerWidgetViewer::drawSelection ( )
{
  OpenMesh::Vec3f lPos ( 0, 0, 0 );

  for ( std::list < int >::const_iterator it = mMSDeformator->getSelectionContainer ( ).begin ( ),
          end = mMSDeformator->getSelectionContainer ( ).end ( ); it != end; ++it )
  {
    lPos = mMSDeformator->getNodeContainer ( )[*it].getPos ( );

    glPushMatrix ( );
    glColor3f ( 1, 1, 0 );
    glTranslatef ( lPos[0], lPos[1], lPos[2] );
    glutSolidSphere ( 0.1, 4, 4 );
    glPopMatrix ( );
  }
}

void SomaDeformerWidgetViewer::generateFramesForModel ( )
{
  mFrameSelectors.clear ( );
  OpenMesh::Vec3f lPos ( 0, 0, 0 );

  //recorremos los nodos y seteamos los frames
  for ( std::vector < MSNode >::iterator it = mMSDeformator->getNodeContainer ( ).begin ( ),
          end = mMSDeformator->getNodeContainer ( ).end ( ); it != end; ++it )
  {
    qglviewer::Frame lFrame;
    lPos = it->getPos ( );
    lFrame.setPosition ( lPos[0], lPos[1], lPos[2] );
    mFrameSelectors.push_back ( lFrame );
  }
}

void SomaDeformerWidgetViewer::updateFramesForModel ( )
{
  OpenMesh::Vec3f lPos ( 0, 0, 0 );

  //recorremos los nodos y seteamos los frames
  int i = 0;
  for ( std::vector < MSNode >::iterator it = mMSDeformator->getNodeContainer ( ).begin ( ),
          end = mMSDeformator->getNodeContainer ( ).end ( ); it != end; ++it, ++i )
  {
    lPos = it->getPos ( );
    mFrameSelectors.at ( i ).setPosition ( lPos[0], lPos[1], lPos[2] );
  }
}

void SomaDeformerWidgetViewer::updateModelFromFrames ( )
{

}

void SomaDeformerWidgetViewer::setSWCDendriticConstraints ( )
{
  unsigned int lSomaId = 1;

  //!!! Reset the initial interpolating steps
  actInterpol = 0;
  mInterpolIncrementsPos.clear ( );
  mInterpolFramesUsed.clear ( );

  if ( mSWCImporter != NULL )
  {

    unsigned int lNumDendritics = mSWCImporter->getNumDendritics ( );
    unsigned int idAux = 0;
    SWCNode nodeAux;

    OpenMesh::Vec3f lOMVec ( 0, 0, 0 );
    qglviewer::Vec lQtVecActSomaId ( 0, 0, 0 );
    qglviewer::Vec lQtVecDendId ( 0, 0, 0 );

    mInterpolFramesUsed.clear ( );

    //!!!Realmente es mejor hacerlo con las dendritas
    for ( int i = 0; i < ( lNumDendritics ); ++i )
    {
      idAux = mSWCImporter->getDendritics ( )[i].initialNode;
      nodeAux = mSWCImporter->getElementAt ( idAux );

      //Comparamos que frame est� m�s cerca y lo desplazamos a esa posisci�n
      lOMVec = nodeAux.position;

      //Dont change !!!
      lQtVecDendId = qglviewer::Vec ( lOMVec[0], lOMVec[1], lOMVec[2] );

      lQtVecActSomaId = mFrameSelectors.at ( 0 ).position ( );

      unsigned int lFrameIdSelected = 0;
      float lLenght = ( lQtVecDendId - lQtVecActSomaId ).norm ( );
      float lActLenght = lLenght;

      for ( unsigned int j = 0; j < mFrameSelectors.size ( ); ++j )
      {
        lQtVecActSomaId = mFrameSelectors.at ( j ).position ( );

        lActLenght = ( lQtVecDendId - lQtVecActSomaId ).norm ( );

        if (( lActLenght < lLenght )
          && ( !isIdInContainer ( j, mInterpolFramesUsed ))
          )
        {
          lFrameIdSelected = j;
          lLenght = lActLenght;
        }
      }

      addIdToSelection ( lFrameIdSelected );
      mInterpolFramesUsed.push_back ( lFrameIdSelected );

      //Calculate the increment in the interpolation
      qglviewer::Vec lQtIncrement ( 0, 0, 0 );

      //Spherical adaptation
      if ( mUseSphericalProjection )
      {
        float lDendriticIncrement = 0;

        if ( mSWCImporter->getDendritics ( )[i].type == DendriticType::BASAL )
        {
          lDendriticIncrement = (( lOMVec.length ( ) - mRealSomaRadio )*mBasalParam );
        }

        if ( mSWCImporter->getDendritics ( )[i].type == DendriticType::APICAL )
        {
          lDendriticIncrement = (( lOMVec.length ( ) - mRealSomaRadio )*mApicalParam );
        }

        lQtVecDendId.normalize ( );
        float finalRadius = ( mRealSomaRadio + lDendriticIncrement );
        lQtVecDendId *= finalRadius;
      }

      lQtIncrement = lQtVecDendId - mFrameSelectors.at ( lFrameIdSelected ).position ( );
      lQtIncrement /= ( float ) numInterpol;

      mInterpolIncrementsPos.push_back ( lQtIncrement );

      if ( mUseContourPropagation )
      {
        //Store all the incidente vertex handles
        MeshDef::VertexHandle vhandle;
        MeshDef::ConstVertexVertexIter CVVIter;
        MeshDef::VertexHandle lAuxVertexHandle;

        vhandle = mBaseMesh->getMesh ( )->vertex_handle ( lFrameIdSelected );
        unsigned int nextContourVertexId = 0;

        //Atacamos cada vertice adyacente.
        for ( CVVIter = mBaseMesh->getMesh ( )->cvv_iter ( vhandle ); CVVIter; ++CVVIter )
        {
          lAuxVertexHandle = mBaseMesh->getMesh ( )->vertex_handle ( CVVIter->idx ( ));
          nextContourVertexId = CVVIter->idx ( );

          if ( !isIdInContainer ( nextContourVertexId, mInterpolFramesUsed ))
          {
            //Lo anyadimos al contenedor
            addIdToSelection ( nextContourVertexId );
            mInterpolFramesUsed.push_back ( nextContourVertexId );

            //Calculo del punto final (sobre la dendrita)
            qglviewer::Vec lBaseVec = (( mFrameSelectors.at ( nextContourVertexId ).position ( )
              - mFrameSelectors.at ( lFrameIdSelected ).position ( )));

            lBaseVec /= lBaseVec.norm ( );
            lBaseVec *= nodeAux.radius;

            qglviewer::Vec lFinalPos = lQtVecDendId + lBaseVec;;

            //Realizaci�n de la interpolaci�n
            lQtIncrement = lFinalPos - mFrameSelectors.at ( nextContourVertexId ).position ( );
            lQtIncrement /= ( float ) numInterpol;
            mInterpolIncrementsPos.push_back ( lQtIncrement );
          }

        }

      }

    }
  }
}

void SomaDeformerWidgetViewer::setSWCDendriticConstraintsFromXML ( )
{

  unsigned int lSomaId = 1;

  //!!! Reset the initial interpolating steps
  actInterpol = 0;
  mInterpolIncrementsPos.clear ( );
  mInterpolFramesUsed.clear ( );

  if ( mSWCImporter != NULL )
  {
    unsigned int lNumDendritics = mSWCImporter->getNumDendritics ( );
    unsigned int idAux = 0;

    SWCNode nodeAux;

    OpenMesh::Vec3f lOMVec ( 0, 0, 0 );
    qglviewer::Vec lQtVecActSomaId ( 0, 0, 0 );
    qglviewer::Vec lQtVecDendId ( 0, 0, 0 );
    float lDenRadius = 0;
    float lGeoDist = 0;

    mInterpolFramesUsed.clear ( );

    //!!!Realmente es mejor hacerlo con las dendritas
    for ( int i = 0; i < ( lNumDendritics ); ++i )
    {
      //Seleccionamos el nodo inicial
      idAux = mSWCImporter->getDendritics ( )[i].initialNode;
      nodeAux = mSWCImporter->getElementAt ( idAux );

      lOMVec = nodeAux.position;

      lQtVecDendId = qglviewer::Vec ( lOMVec[0], lOMVec[1], lOMVec[2] );

      XMLSomaDefDendriticModif lXMLDendriticModif = mXMLSomaDefManager->getXMLSomaDef ( )->dendriticContainer.at ( i );

      unsigned int lIdVertex = lXMLDendriticModif.mGeoVertexIdsCand.at ( 0 );

      lQtVecActSomaId = mFrameSelectors.at ( lIdVertex ).position ( );

      unsigned int lFrameIdSelected = lIdVertex;
      float lLenght = ( lQtVecDendId - lQtVecActSomaId ).norm ( );
      float lActLenght = lLenght;

      unsigned int lNumVertexIds = lXMLDendriticModif.mGeoVertexIdsCand.size ( );
      for ( unsigned int j = 0; j < lNumVertexIds; ++j )
      {
        lIdVertex = lXMLDendriticModif.mGeoVertexIdsCand.at ( j );

        //mFrameSelectors.at(lIdVertex).getPosition(lQtVecActSomaId.x,lQtVecActSomaId.y,lQtVecActSomaId.z);
        lQtVecActSomaId = mFrameSelectors.at ( lIdVertex ).position ( );

        lActLenght = ( lQtVecDendId - lQtVecActSomaId ).norm ( );

        if (( lActLenght < lLenght )
          && ( !isIdInContainer ( lIdVertex, mInterpolFramesUsed ))
          )
        {
          lFrameIdSelected = lIdVertex;
          lLenght = lActLenght;
        }
      }

      qglviewer::Vec lQtIncrement ( 0, 0, 0 );

      //Spherical adaptation
      //if (mUseSphericalProjection)
      {
        float lDendriticIncrement = 0;
        //lDendriticIncrement = ( (lOMVec.length() - mRealSomaRadio) * mXMLSomaDefManager->getXMLSomaDef()->dendriticContainer.at(i).mParamValue1 );

        ////Avance en funci�n del radio real del soma (una forma de especificarlo igual para todas las dendritas)
        ////Es decir, avanzamos el nodo d ela dendrita tantas veces el radio
        //lDendriticIncrement = ( mRealSomaRadio * mXMLSomaDefManager->getXMLSomaDef()->dendriticContainer.at(i).mParamValue1 );

        //Valor normalizado en el que

        //(lOMVec.length() - mRealSomaRadio) = Distancia desde el borde del soma hasta el nodo de inicio d ela dendrita
        //Si esa distancia la multiplicacmos po 0 y la sumamos al radioReal nos quedamos en la superficie del soma
        //Si la multiplicamos por 1, nos quedamos en el incio de la dendrita.
        lDendriticIncrement = (( lOMVec.length ( ) - mRealSomaRadio )
          *mXMLSomaDefManager->getXMLSomaDef ( )->dendriticContainer.at ( i ).mParamValue1 );

        lQtVecDendId.normalize ( );
        float finalRadius = ( mRealSomaRadio + lDendriticIncrement );
        lQtVecDendId *= finalRadius;
      }

      //Calculo de la posici�n final, con o sin proyecci�n sofre la esfera real del soma
      lQtIncrement = lQtVecDendId - mFrameSelectors.at ( lFrameIdSelected ).position ( );
      lQtIncrement /= ( float ) numInterpol;

      for ( unsigned int j = 0; j < lXMLDendriticModif.mGeoVertexIdsCand.size ( ); ++j )
      {
        //Si la distacia geodesica es menor que el radio para el vertice actual entonces este se selecciona
        lIdVertex = lXMLDendriticModif.mGeoVertexIdsCand.at ( j );

        if ( !isIdInContainer ( lIdVertex, mInterpolFramesUsed ))
        {
          addIdToSelection ( lIdVertex );
          mInterpolFramesUsed.push_back ( lIdVertex );
          mInterpolIncrementsPos.push_back ( lQtIncrement );
        }
      }
    }
  }
}

bool SomaDeformerWidgetViewer::isIdInContainer ( unsigned int pId, std::vector < unsigned int > pVector )
{

  for ( int i = 0; i < pVector.size ( ); ++i )
  {
    if ( pVector.at ( i ) == pId )
      return true;
  }
  return false;
}

void SomaDeformerWidgetViewer::updateInterpolation ( )
{
  unsigned int lNumInterpols = mInterpolFramesUsed.size ( );
  unsigned int lIdNoteToInterpol = 0;
  qglviewer::Vec lQtVecDendId ( 0, 0, 0 );

  for ( unsigned int i = 0; i < lNumInterpols; ++i )
  {
    lIdNoteToInterpol = mInterpolFramesUsed.at ( i );
    lQtVecDendId = mFrameSelectors.at ( lIdNoteToInterpol ).position ( );

    lQtVecDendId += mInterpolIncrementsPos.at ( i );

    mFrameSelectors.at ( lIdNoteToInterpol ).setPosition ( lQtVecDendId );
    mMSDeformator->getNodeContainer ( ).at ( lIdNoteToInterpol )
                 .setPos ( OpenMesh::Vec3f ( lQtVecDendId.x, lQtVecDendId.y, lQtVecDendId.z ));
  }
}

void SomaDeformerWidgetViewer::setNodesInCustomSphere ( )
{

}

void SomaDeformerWidgetViewer::optimizateDendriticBase ( )
{
  setVertexIdsforDendritics ( );
  optimizateDendriticTesellation ( );
  optimizateDendriticRadius ( );

}

void SomaDeformerWidgetViewer::setVertexIdsforDendritics ( )
{
  mVertexIdsByDendritic.clear ( );

  if ( mSWCImporter != NULL )
  {
    unsigned int lNumDendritics = mSWCImporter->getNumDendritics ( );

    for ( int i = 0; i < lNumDendritics; ++i )
    {
      XMLSomaDefDendriticModif lXMLDendriticModif = mXMLSomaDefManager->getXMLSomaDef ( )->dendriticContainer.at ( i );
      QList < unsigned int >::Iterator lIterQLIst = lXMLDendriticModif.mGeoVertexIdsCand.begin ( );
      std::vector < unsigned int > lVect;

      for ( ; lIterQLIst != lXMLDendriticModif.mGeoVertexIdsCand.end ( ); ++lIterQLIst )
      {
        if ( !isIdInContainerofContainers ( *lIterQLIst, mVertexIdsByDendritic ))
        {
          lVect.push_back ( *lIterQLIst );
        }
      }

      mVertexIdsByDendritic.push_back ( lVect );
    }
  }
}

bool SomaDeformerWidgetViewer::isIdInContainerofContainers ( unsigned int pId,
                                                             std::vector < std::vector < unsigned int>> pVectorOfVectors )
{
  for ( int i = 0; i < pVectorOfVectors.size ( ); ++i )
  {
    for ( int j = 0; j < pVectorOfVectors.at ( i ).size ( ); ++j )
    {
      if ( pVectorOfVectors.at ( i ).at ( j ) == pId )
        return true;
    }
  }
  return false;
}

void SomaDeformerWidgetViewer::optimizateDendriticRadius ( )
{
  unsigned int lSomaId = 1;

  if ( mSWCImporter != NULL )
  {
    unsigned int lNumDendritics = mSWCImporter->getNumDendritics ( );
    unsigned int idAux = 0;

    SWCNode nodeAux;

    MeshDef::Point lTmpVertex ( 0, 0, 0 );
    OpenMesh::Vec3f lNodePos ( 0, 0, 0 );

    OpenMesh::Vec3f lNodeFinalPos ( 0, 0, 0 );
    OpenMesh::Vec3f lNodeFinalDir ( 0, 0, 0 );

    MeshDef::VertexHandle vhandle;
    unsigned int ltmpVetexId = 0;
    std::vector < unsigned int > lIdsDendVertex;

    //Recorremos cada dendrita para conocer sus v�rtices
    for ( int i = 0; i < ( lNumDendritics ); ++i )
    {
      //Seleccionamos los datos que tenemos en la dendrita actual
      idAux = mSWCImporter->getDendritics ( )[i].initialNode;
      nodeAux = mSWCImporter->getElementAt ( idAux );

      //Seleccionamos todos los datos relativos al a dendrita actual en la definicion XML
      XMLSomaDefDendriticModif lXMLDendriticModif = mXMLSomaDefManager->getXMLSomaDef ( )->dendriticContainer.at ( i );

      lIdsDendVertex.clear ( );
      for ( unsigned int j = 0; j < lXMLDendriticModif.mGeoVertexIdsCand.size ( ); ++j )
      {
        lIdsDendVertex.push_back ( lXMLDendriticModif.mGeoVertexIdsCand.at ( j ));
      }

      //Nos quedamos con la posici�n del primer nodo de la dendrita
      lNodePos = nodeAux.position;

      //Seleccionamos el primer v�rtice del grupo de candidatos
      unsigned int lIdVertex = lXMLDendriticModif.mGeoVertexIdsCand.at ( 0 );

      //Ahora compararemos con la malla directamente
      lTmpVertex = mBaseMesh->getMesh ( )->points ( )[lIdVertex];

      unsigned int lNearestIdVertexSelected = lIdVertex;
      float lLenght = ( lTmpVertex - lNodePos ).length ( );
      float lActLenght = lLenght;

      unsigned int lNumVertexIds = lXMLDendriticModif.mGeoVertexIdsCand.size ( );

      for ( unsigned int j = 0; j < lNumVertexIds; ++j )
      {
        lIdVertex = lXMLDendriticModif.mGeoVertexIdsCand.at ( j );

        lTmpVertex = mBaseMesh->getMesh ( )->points ( )[lIdVertex];

        lActLenght = ( lTmpVertex - lNodePos ).length ( );

        if (( lActLenght < lLenght )
          )
        {
          lNearestIdVertexSelected = lIdVertex;
          lLenght = lActLenght;
        }
      }

      lNodeFinalPos = mBaseMesh->getMesh ( )->points ( )[lNearestIdVertexSelected];
      vhandle = mBaseMesh->getMesh ( )->vertex_handle ( lNearestIdVertexSelected );

      MeshDef::ConstVertexVertexIter CVVIter;

      for ( CVVIter = mBaseMesh->getMesh ( )->cvv_iter ( vhandle ); CVVIter; ++CVVIter )
      {
        ltmpVetexId = mBaseMesh->getMesh ( )->vertex_handle ( CVVIter->idx ( )).idx ( );

        if ( isIdInContainer ( ltmpVetexId, lIdsDendVertex ))
        {

          lNodeFinalDir = (
            ( mBaseMesh->getMesh ( )->points ( )[ltmpVetexId] )
              - ( mBaseMesh->getMesh ( )->points ( )[lNearestIdVertexSelected] )
          );

          lNodeFinalDir.normalize ( );
          lNodeFinalDir *= nodeAux.radius;

          lNodeFinalPos = mBaseMesh->getMesh ( )->points ( )[lNearestIdVertexSelected];
          lNodeFinalPos += lNodeFinalDir;

          mMSDeformator->getNodeContainer ( ).at ( ltmpVetexId )
                       .setPos ( OpenMesh::Vec3f ( lNodeFinalPos[0], lNodeFinalPos[1], lNodeFinalPos[2] ));

        }
      }
    }
  }
}

void SomaDeformerWidgetViewer::optimizateDendriticTesellation ( )
{
  //1)Parar la simulaci�n
  startDeformation ( false );
  unsigned int lSomaId = 1;

  if ( mSWCImporter != NULL )
  {
    unsigned int lNumDendritics = mSWCImporter->getNumDendritics ( );
    unsigned int idAux = 0;

    SWCNode nodeAux;

    MeshDef::Point lTmpVertex ( 0, 0, 0 );
    OpenMesh::Vec3f lNodePos ( 0, 0, 0 );

    std::vector < unsigned int > lIdsCentralVertex;
    std::vector < unsigned int > lIdsContourVertex;
    std::vector < unsigned int > lIdsTotalVertex;

    std::vector < unsigned int > lIdsTotalFacesToDel;
    mVertexIdToDel.clear ( );

    for ( int i = 0; i < ( lNumDendritics ); ++i )
    {
      //Seleccionamos los datos que tenemos en la dendrita actual
      idAux = mSWCImporter->getDendritics ( )[i].initialNode;
      nodeAux = mSWCImporter->getElementAt ( idAux );

      //Seleccionamos todos los datos relativos al a dendrita actual en la definicion XML
      XMLSomaDefDendriticModif lXMLDendriticModif = mXMLSomaDefManager->getXMLSomaDef ( )->dendriticContainer.at ( i );

      //Nos quedamos con la posici�n del primer nodo de la dendrita
      lNodePos = nodeAux.position;

      //Seleccionamos el primer v�rtice del grupo de candidatos
      unsigned int lIdVertex = lXMLDendriticModif.mGeoVertexIdsCand.at ( 0 );

      //Ahora compararemos con la malla directamente
      lTmpVertex = mBaseMesh->getMesh ( )->points ( )[lIdVertex];

      //lQtVecActSomaId = mFrameSelectors.at(0).position();

      unsigned int lNearestIdVertexSelected = lIdVertex;
      float lLenght = ( lTmpVertex - lNodePos ).length ( );
      float lActLenght = lLenght;

      unsigned int lNumVertexIds = lXMLDendriticModif.mGeoVertexIdsCand.size ( );

      for ( unsigned int j = 0; j < lNumVertexIds; ++j )
      {
        lIdVertex = lXMLDendriticModif.mGeoVertexIdsCand.at ( j );

        //Cargamos el contenedor del total de vertices mientras lo recorremos
        lIdsTotalVertex.push_back ( lIdVertex );

        //Ahora compararemos con la malla directamente
        lTmpVertex = mBaseMesh->getMesh ( )->points ( )[lIdVertex];

        lActLenght = ( lTmpVertex - lNodePos ).length ( );

        if (( lActLenght < lLenght )
          )
        {
          lNearestIdVertexSelected = lIdVertex;
          lLenght = lActLenght;
        }
      }
      //Limpiamos los contenedores
      lIdsContourVertex.clear ( );
      lIdsCentralVertex.clear ( );

      bool lCentralVertex = true;
      unsigned int ltmpVetexId = 0;
      MeshDef::ConstVertexVertexIter CVVIter;
      MeshDef::VertexHandle vhandle;

      for ( unsigned int j = 0; j < lNumVertexIds; ++j )
      {
        lIdVertex = lXMLDendriticModif.mGeoVertexIdsCand.at ( j );
        lCentralVertex = true;

        //recorremos el contorno del vertice actual
        vhandle = mBaseMesh->getMesh ( )->vertex_handle ( lIdVertex );

        for ( CVVIter = mBaseMesh->getMesh ( )->cvv_iter ( vhandle ); CVVIter; ++CVVIter )
        {
          ltmpVetexId = mBaseMesh->getMesh ( )->vertex_handle ( CVVIter->idx ( )).idx ( );

          if (
            ( !isIdInContainer ( ltmpVetexId, mVertexIdsByDendritic.at ( i )))
            )
          {
            lCentralVertex = false;
          }
        }

        if ( lCentralVertex )
          lIdsCentralVertex.push_back ( lIdVertex );
        else
          lIdsContourVertex.push_back ( lIdVertex );

      }

      //Marcamos todos los vertices centrales excepto el seleccionado como pivote para eliminarlos
      //en la exportacion
      for ( unsigned int k = 0; k < lIdsCentralVertex.size ( ); ++k )
      {
        if ( lIdsCentralVertex.at ( k ) != lNearestIdVertexSelected )
        {
          mVertexIdToDel.push_back ( lIdsCentralVertex.at ( k ));
        }
      }

      //Borramos los triangulos adyacentes a los centrales
      MeshDef::ConstVertexFaceIter CVFIter;
      MeshDef::FaceHandle lAuxFaceHandle;

      lIdsTotalFacesToDel.clear ( );

      std::vector < MeshDef::FaceHandle > vecFaceHoleHandle;
      vecFaceHoleHandle.clear ( );

      //Seleccionamos todas las facetas a borrar
      // ->adyacentes a cada vertice central
      for ( unsigned k = 0; k < lIdsCentralVertex.size ( ); ++k )
      {
        //recorremos el controno del vertice actual
        vhandle = mBaseMesh->getMesh ( )->vertex_handle ( lIdsCentralVertex.at ( k ));

        for ( CVFIter = mBaseMesh->getMesh ( )->cvf_iter ( vhandle ); CVFIter; ++CVFIter )
        {
          lAuxFaceHandle = mBaseMesh->getMesh ( )->face_handle ( CVFIter->idx ( ));

          if ( !isIdInContainer ( lAuxFaceHandle.idx ( ), lIdsTotalFacesToDel ))
          {
            lIdsTotalFacesToDel.push_back ( lAuxFaceHandle.idx ( ));
            vecFaceHoleHandle.push_back ( lAuxFaceHandle );
          }
        }
      }

      for ( unsigned int k = 0; k < vecFaceHoleHandle.size ( ); ++k )
      {
        mBaseMesh->getUnprotectedMesh ( )->delete_face ( vecFaceHoleHandle.at ( k ), false );
      }

      //Free the memory of deleted objects
      mBaseMesh->getUnprotectedMesh ( )->garbage_collection ( );
      mBaseMesh->updateBaseMesh ( );

      std::vector < MeshDef::VertexHandle > glb_tubeFace_VHandles;

      unsigned int lIdVertexA = lIdsContourVertex.at ( 0 );
      unsigned int lIdVertexB = 0;

      std::vector < unsigned int > lUserVertex;
      lUserVertex.clear ( );

      lUserVertex.push_back ( lIdVertexA );
      for ( unsigned int k = 0; k < lIdsContourVertex.size ( ); ++k )
      {
        vhandle = mBaseMesh->getMesh ( )->vertex_handle ( lIdsContourVertex.at ( k ));

        for ( CVVIter = mBaseMesh->getMesh ( )->cvv_iter ( vhandle ); CVVIter; ++CVVIter )
        {
          ltmpVetexId = mBaseMesh->getMesh ( )->vertex_handle ( CVVIter->idx ( )).idx ( );

          //Si el actual forma parte del contorno y no ha sido usado
          if (
            ( isIdInContainer ( ltmpVetexId, lIdsContourVertex ))
            )
          {
            lIdVertexA = vhandle.idx ( );
            lIdVertexB = ltmpVetexId;

            glb_tubeFace_VHandles.clear ( );

            glb_tubeFace_VHandles.push_back ( mBaseMesh->getMesh ( )->vertex_handle ( lNearestIdVertexSelected ));
            glb_tubeFace_VHandles.push_back ( mBaseMesh->getMesh ( )->vertex_handle ( lIdVertexA ));
            glb_tubeFace_VHandles.push_back ( mBaseMesh->getMesh ( )->vertex_handle ( lIdVertexB ));

            mBaseMesh->getUnprotectedMesh ( )->add_face ( glb_tubeFace_VHandles );

            lUserVertex.push_back ( ltmpVetexId );
          }
        }
      }
      mBaseMesh->getUnprotectedMesh ( )->garbage_collection ( );
      mBaseMesh->updateBaseMesh ( );
    }
  }
}

void SomaDeformerWidgetViewer::addSomaModel(string path) {
    auto auxBaseMesh = new BaseMesh ( path );
    mSomaModelsContainers.addElement ( auxBaseMesh );
    mNormalizeModel = true;
}



