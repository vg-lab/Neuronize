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

#ifndef QTSDW
#define QTSDW

#include <libs/libGLNeuroUtils/config.h>

#include <QVector3D>
#include <QColor>
#include <QDir>
#include <QGLViewer/qglviewer.h>

#include <libs/libNeuroUtils/SWCImporter.h>
#include <libs/libGLNeuroUtils/MeshRenderer.h>
#include <libs/libNeuroUtils/BaseMeshContainer.h>
#include <libs/libNeuroUtils/MSObject.h>
#include <libs/libQtNeuroUtils/XMLSomaDefManager.h>

#include "FrameConstrictor.h"
#include "manipulatedFrameSetConstraint.h"
#include <QGLViewer/manipulatedFrame.h>

using namespace NSSWCImporter;
using namespace NSMeshRenderer;
using namespace NSBaseMeshContainer;
using namespace NSMSObject;

class SomaDeformerWidgetViewer: public QGLViewer
{
  public :
#if QT_VERSION < 0x040000
    SomaDeformerWidgetViewer(QWidget *parent, const char *name);
#else
    SomaDeformerWidgetViewer ( QWidget *parent );
#endif

    void setupViewer ( );

    void generateSomaModel ( unsigned int pModelId, float pScaleFactor );

    void generateSomaProcedural (//OpenMesh::Vec3f center,
      float pRadio, unsigned int pHResol, unsigned int pVResol );

    void generateSomaFromXML ( QString pFileToGenerate, float pSomaScale );

    void loadSomaContainer ( QDir pDir );

    void loadSWCFile ( QString pSWCFile );

    void unLoadSWCFile ( );

    void loadPatternMesh ( QString pSWCFile );

    void exportModel ( QString pFile );

    QString configurationToString ( );

    void startDeformation ( bool pDeform );

    void singleStepDeformation ( );

    void reSetAnimationPeriod ( int pPeriod );

    //Mass Spring seters
    void setMSIntegrator ( int pMethod );
    void setMSDt ( float pDt );
    void setMSConstraints ( bool pUseConstraits );
    void setMSSpringParams ( float pStiffness, float pDumping );
    void setMSNodeParams ( float pMass, OpenMesh::Vec3f pGravity );
    void setUnCollapseSprings ( int pUnCollapseSpringsPercent );

    void setDendriticParams ( float pBasalParam, float pApicalParam );

    SWCImporter *getSWCImporter ( ) const;

    void setNormalizeExportedModel ( bool pExport );

    void setUseSphericalProjection ( bool pUseSphericalP );

    void setUseContourPropagation ( bool pUseContourP );

    void optimizateDendriticBase ( );

    void optimizateDendriticTesellation ( );

    void optimizateDendriticTesellation2 ( );

    void optimizateDendriticRadius ( );

    void setExportOBJSecuence ( bool pExportOBJSecuence ) { mExportOBJSecuence = pExportOBJSecuence; };

    void setDeleteRedundantVertex ( bool pDeleteRedundantVertexs ) { mRedundantVertex = pDeleteRedundantVertexs; };

    void setVertexIdsforDendritics ( );

    bool isDeformating ( ) { return mDeformating; }

    //Redefinido
    virtual void animate ( );

    void addSomaModel(std::string path);

protected :

    virtual void draw ( );

    //Mouse events functions
    virtual void mousePressEvent ( QMouseEvent *e );

    virtual void mouseMoveEvent ( QMouseEvent *e );

    virtual void mouseReleaseEvent ( QMouseEvent *e );

    //Selection functions, libqgl need both to select objects
    virtual void drawWithNames ( );

    virtual void endSelection ( const QPoint & );

    virtual QString helpString ( ) const;

  private:

    ProceduralMesh *mBaseMesh;
public:
    ProceduralMesh *getMBaseMesh() const;

private:
    MSObject *mMSDeformator;
    MeshRenderer *meshRenderer;
    SWCImporter *mSWCImporter;

    //!!!
    XMLSomaDefManager *mXMLSomaDefManager;

    BaseMesh *mPatternMesh;
    MeshRenderer *mPatternMeshRenderer;

    unsigned int mRenderMask;
    QColor mModelColor;

    BaseMeshContainer mSomaModelsContainers;

    bool mDeformating;

    bool mNormalizeModel;
    float mRealSomaRadio;

    bool mUseSphericalProjection;
    bool mUseContourPropagation;

    bool lDebugMode;

    float mApicalParam;
    float mBasalParam;

    void setModelColor ( );

    void setPatternModelColor ( );

    //Manipulation control
    void drawSelectionRectangle ( ) const;

    void startManipulation ( );

    void addIdToSelection ( int id );

    void removeIdFromSelection ( int id );

    // Different selection modes
    enum SelectionMode { NONE, ADD, REMOVE };
    SelectionMode mSelectionMode;

    //Current rectangular selection
    QRect mSelectionRectangle;

    std::vector < qglviewer::Frame > mFrameSelectors;

    bool isInSelection ( int pId );

    void drawSelection ( );

    void generateFramesForModel ( );

    void updateFramesForModel ( );

    void updateModelFromFrames ( );

    void setSWCDendriticConstraints ( );

    void setSWCDendriticConstraintsFromXML ( );

    bool isIdInContainer ( unsigned int pId, std::vector < unsigned int > pVector );
    bool isIdInContainerofContainers ( unsigned int pId, std::vector < std::vector < unsigned int>> pVectorOfVectors );

    //Initial interpol values
    unsigned int numInterpol;
    unsigned int actInterpol;

    std::vector < qglviewer::Vec > mInterpolIncrementsPos;
    std::vector < unsigned int > mInterpolFramesUsed;

    std::vector < unsigned int > mVertexIdToDel;

    std::vector < std::vector < unsigned int>> mVertexIdsByDendritic;

    void updateInterpolation ( );

    void setNodesInCustomSphere ( );

    bool mExportOBJSecuence;
    bool mRedundantVertex;
    int mActDeformIter;
};
#endif //
