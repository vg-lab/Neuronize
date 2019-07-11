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

#ifndef NGWV_H
#define NGWV_H

#include <GL/glew.h>

#include <QDir>
#include <QGLViewer/qglviewer.h>

#include <libs/libNeuroUtils/BaseMesh.h>
#include <libs/libNeuroUtils/BaseMeshContainer.h>
#include <libs/libNeuroUtils/NeuroSWC.h>
#include <libs/libNeuroUtils/SpinesSWC.h>
#include <libs/libNeuroUtils/SWCImporter.h>

#include <libs/libQtNeuroUtils/XMLProgressiveNeuroVizManager.h>

#include <libs/libGLNeuroUtils/MeshRenderer.h>
#include <libs/libGLNeuroUtils/LightManager.h>

#include <SkelGeneratorUtil/Neuron.h>

using namespace NSBaseMesh;
using namespace NSBaseMeshContainer;
using namespace NSNeuroSWC;
using namespace NSSpinesSWC;
using namespace NSMeshRenderer;
using namespace NSLightManager;

using namespace NSSWCImporter;

#include "ui_NeuroGeneratorWidget.h"
#include <libs/libNeuroUtils/AS2SWCV2.h>

class NeuroGeneratorWidgetViewer: public QGLViewer
{
  public :
#if QT_VERSION < 0x040000
    NeuroGeneratorWidgetViewer(QWidget *parent, const char *name);
#else
    NeuroGeneratorWidgetViewer ( QWidget *parent );
#endif

    void setupViewer ( );

    void loadNeuronModel ( QString pModel );

    void loadSpinesModel ( QString pModel );

    void loadSWCFile ( QString pSWCFile );

    void loadSomaContainer ( QDir pDir );

    void loadSpinesContainer ( QDir pDir );

    BaseMeshContainer getSomaModelsContainer ( ) const;

    BaseMeshContainer getSpinesModelsContainer ( ) const;

    unsigned int getNumModelsSpines ( );

    unsigned int getSpinesSynthContainersNumEles ( );

    void generateNeuronMeshFromFile ( QString pSWCFile,
                                      unsigned int pTubesHResol,
                                      unsigned int pTubesVResol,
                                      unsigned int pSomaHResol,
                                      unsigned int pSomaVResol );

    void generateNeuronMeshFromInternalInfo ( unsigned int pTubesHResol,
                                              unsigned int pTubesVResol,
                                              unsigned int pSomaHResol,
                                              unsigned int pSomaVResol );

    void generateNeuronLines ( bool pGenLines );

    void generateNeuronSphereNodes ( bool pSphereNodes );

    void destroyNeuronMesh ( );

    bool hasSpines ( );

    /*
      Genera tantos grupos de espinas como se especifique en la intefaz, cada uno con tantas
      espinas como se especifique.
      Tipo de espinas generadas: Procedurales
    */
    void generateSpines ( unsigned int pNumSpines,
                          unsigned int pHorResol,
                          unsigned int pVerResol,
                          float pMinLongSpine,
                          float pMaxLongSpine,
                          float pMinRadio,
                          float pMaxRadio );

    void destroyAllGroupsSpines ( );

    /*
      Genera tantos grupos de espinas como se especifique en la intefaz, cada uno con tantas
      espinas como se especifique.
      Tipo de espinas generadas: Modeladas
    */
    void generateSomeSpinesGroup ( unsigned int pNumSpinesByGroup,
                                   unsigned int pNumOfGroups,
                                   unsigned int pHorResol,
                                   unsigned int pVerResol,
                                   float pMinLongSpine,
                                   float pMaxLongSpine,
                                   float pMinRadio,
                                   float pMaxRadio );

    /*
      Calcula autom�ticamente el n�mero de espinas que deber�a llevar la neurona (n�mero real de espinas), y calcula el
      n�mero de grupos que se crear�n en funci�n dle n�mero de espinas por grupo que s eespecifica en la interfaz, y
      disemeina estas espinas de manera aleatoria por la neruona (por esto es semireal)
    */
    void generateSemiRealSpinesGroup ( unsigned int pNumSpinesByGroup,
                                       unsigned int pHorResol,
                                       unsigned int pVerResol,
                                       float pMinLongSpine,
                                       float pMaxLongSpine,
                                       float pMinRadio,
                                       float pMaxRadio );

    /*
      Igual que el anterior pero reparte las espinas seg�na las gr�ficas de distribuci�n de Ruth, la distribuci�n
      y el n�mero de espinas es real .. pero nos e pueden situar de manera correcta sobre la malla debido a la limitaci�n
      de la resoluci�n.
    */
    void generateRealSpinesGroup ( unsigned int pNumSpines,
                                   unsigned int pHorResol,
                                   unsigned int pVerResol,
                                   float pMinLongSpine,
                                   float pMaxLongSpine,
                                   float pMinRadio,
                                   float pMaxRadio );

    /*
      Se realiza una distribuci�n de espinas seg�n los par�mtros que ha dado Ruth
        Incremental desde una distancia inical hasta una final y constante a aprtir de ah�
        Se han de diferenciar entre apical y basal
    */
    void generateSpinesInSegment ( unsigned int pNumSpinesByGroup,
                                   unsigned int pHorResol,
                                   unsigned int pVerResol,
                                   float pMinLongSpine,
                                   float pMaxLongSpine,
                                   float pMinRadio,
                                   float pMaxRadio );

    void setSpinesDistributionParams ( float pSpinesDesp,
                                       float pBProgressionFactor,
                                       float pAProgressionFactor,
                                       float pBasalMinDistance,
                                       float pBasalCteDistance,
                                       float pApicalMinDistance,
                                       float pApicalCteDistance );

    void attachNewSpines ( unsigned int pNumSpines,
                           unsigned int pHorResol,
                           unsigned int pVerResol,
                           float pMinLongSpine,
                           float pMaxLongSpine,
                           float pMinRadio,
                           float pMaxRadio );

    void applyNoiseToNeuron ( float pNoiseInterval, bool pCteNoise );

    void applyNoiseToSoma ( float pNoiseInterval, bool pCteNoise );

    void applyNoiseToDendritics ( float pNoiseInterval, bool pCteNoise );

    void applyNoiseToSpines ( float pNoiseInterval, bool pCteNoise, unsigned int pLocalLimit, unsigned int pMinDesp );

    void applySmoothToNeuron ( unsigned int pSmoothMethod,
                               unsigned int pTechnique,
                               unsigned int pContinuity,
                               unsigned int pIterations );

    void applySmoothToSpines ( unsigned int pSmoothMethod,
                               unsigned int pTechnique,
                               unsigned int pContinuity,
                               unsigned int pIterations );

    void applySubdivide ( unsigned int pSubdivType );

    void applyDecimate ( unsigned int pSteps );

    void update ( );

    void exportNeuron ( QString pFile );

    void exportSpines ( QString pFile );

    void exportGroupOfSpines ( QString pFile );

    void joinSpines ( );

    NeuroSWC *getNeuroSWC ( ) { return mesh; };

    SpinesSWC *getSpinesSWC ( ) { return meshSpines; };

    void ControlStateRender ( bool pRendSolid, bool pRendEdges, bool pRendNormals, bool pRendVertex );

    void setNormalScale ( float pScale );

    void setPixelScale ( float pScale );

    void ControlLights ( int control );

    void ShowTextData ( );

    void SetDebugMode ( bool pVal );

    void ConfigureDebug ( unsigned int pDebugRangeIni,
                          unsigned int pDebugRangeFin,
                          MeshRenderer::DebugMode pDebugType );

    void SetSomaModel ( unsigned int pSomaId );

    void SetSpinesType ( unsigned int pSpinesTypes );

    void setSomaScaleFactor ( float pScaleFactor );

    void setSpineScaleFactor ( float pScaleFactor );

    void setMultipleGroupsOfSpines ( bool pMultiGroup );

    void setColorFromSWC ( QString pFile );

    void setPiramidalSoma ( bool pPiramSoma );

    void setNeuronType ( NeuronType pNeuronType );

    void adaptSomaMOdelWithSWCInfo ( );

    BaseMesh *getNeuronMesh ( ) { return static_cast<BaseMesh *>(mesh); }

    void setNeuronColor ( MeshDef::Color pColor );

    void exportSpinesInfo ( QString fileName );

    void exportLastSpinesInfo ( QString fileName );

    void importSpinesInfo ( QString fileName );

    void generateSpinesVrml(skelgenerator::Neuron *neuron,const std::string& tempPath);


    void generateSpinesASC(std::vector<Spine>& spines,unsigned int pHorResol, unsigned int pVerResol, float pMinLongSpine,
                           float pMaxLongDistance, float pMinRadio, float pMaxRadio);

protected:

    virtual void draw ( );

    virtual QString helpString ( ) const;

  private:
    unsigned int mNumVertesSpineIndexation;

    QString mSWCFile;

    NeuroSWC *mesh;

    SpinesSWC *meshSpines;

    MeshRenderer *meshRend;

    MeshRenderer *spineMeshRend;

    XMLProgressiveNeuroVizManager *mXMLProgressiveNeuroVizManager;

    std::vector < SpineInfo > mLastSpinesInfo;

    unsigned int renderMask;

    unsigned int mSegmentList;

    unsigned int mSphereList;
    unsigned int mSphereSlices;
    unsigned int mSphereStacks;

    bool mDebugMode;
    unsigned int mDebugRangeIni;
    unsigned int mDebugRangeFin;
    MeshRenderer::DebugMode mDebugType;

    bool mSchematicLines;
    bool mSchematicSpheres;

    //Lighs configuration
    LightManager *lights;

    unsigned int mSomaModelId;

    unsigned int mSpinesTypes;

    SWCImporter *mSWCImporter;

    BaseMeshContainer mSomaModelsContainers;

    BaseMeshContainer mSpinesModelsContainers;

    BaseMeshContainer mSpinesSynthContainers;

    float mSomaScaleFactor;

    float mSpineScaleFactor;

    bool mPiramidalSoma;

    //Spines distribution params
    float mSpinesDesp;
    float mBProgressionFactor;
    float mAProgressionFactor;
    float mBasalMinDistance;
    float mBasalCteDistance;
    float mApicalMinDistance;
    float mApicalCteDistance;

    NeuronType mNeuronType;

    void InitLight ( );

    void generateSegmentList ( );

    void generateSphereList ( );

    void drawSegmentList ( );

    void drawSphereList ( );

private slots:

    SpinesSWC* fusionAllSpines(vector<SpinesSWC *> &spineMeshes);

    SpinesSWC* fusionSpines(SpinesSWC* mesh1, SpinesSWC* mesh2);
};

#endif
