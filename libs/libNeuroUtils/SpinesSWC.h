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

#ifndef _SPINESSWC_H_
#define _SPINESSWC_H_

#include <neuroutils/api.h>
#include <QRandomGenerator>

#include "NeuroSWC.h"
#include "BaseMeshContainer.h"
#include "SWCSpinesDistributor.h"
#include "ASC2SWCV2.h"
#include <libs/libNeuroUtils/BBDD.h>

using namespace std;
using namespace NSNeuroSWC;
using namespace NSBaseMeshContainer;
using namespace NSSWCSpinesDistributor;

namespace NSSpinesSWC
{
  struct SpineInfo
  {
    unsigned int mIndexationNode;
    unsigned int mSpineId;
    OpenMesh::Vec3f mSpinePosition;
    OpenMesh::Vec3f mSpineDirection;
  };

  struct SpineDenNode
  {
    float SpinesPer_um;
    unsigned int DistToSoma;
    float DistToSomaNormalized;
  };

  struct VertexDistances
  {
    int lId;
    bool lUsed;
    float lDistanceToSoma;
    float lDendriticDistanceReal;
    float lDendriticDistanceNorm;
    float lDendriticRadiusFactor;
  };

  struct FacesDistances
  {
    int lId;
    float lDistanceToSoma;
  };

  class NEUROUTILS_API SpinesSWC: public BaseMesh
  {
    protected:
      NeuroSWC *NeuronMesh;

      unsigned int mNumVerticesEnSpina;

      unsigned int totalNumSpines;
      unsigned int HorResol;
      unsigned int VerResol;

      float mMinLongSpine;
      float mMaxLongSpine;

      float mMinRadio;
      float mMaxRadio;

      unsigned int spineGlobalDesp;

      unsigned int somaNumVertex;

      bool mMultipleSpinesGropus;
      float mDistributorModificator;

      //Temporary declaratios (optimize the serial code)
      OpenMesh::Vec3f glb_direction;

      //Local Spine
      std::vector <MeshDef::Point> LocalSpineVContainer;

      //Global Mesh Vertex Handle
      MeshDef::VertexHandle *glb_GlobalSpinesVHandle;
      std::vector <MeshDef::VertexHandle> glb_spineFace_VHandles;

      unsigned int spinesVHandleTotal;

      std::vector <SpineDenNode> BasalSpineDensityContainer;
      std::vector <SpineDenNode> ApicalSpineDensityContainer;

      std::vector <VertexDistances> mVertexDistancesContainer;
      std::vector <FacesDistances> mFacesDistancesContainer;

      std::vector < unsigned int > mDendriticVertexContainer;

      //BaseMeshContainer					* mSpinesContainer;
      BaseMeshContainer *mSpinesModeledContainer;

      BaseMeshContainer *mSpinesSynthContainer;

      std::vector <SpineInfo> mSpinesInfo;

      std::vector < unsigned int > mSpineDesp;

      void generateSpinesDistribution ( );

    public:
      SpinesSWC ( );

      SpinesSWC ( NeuroSWC *neuron,
                  unsigned int numSpines,
                  unsigned int HorizontalResol,
                  unsigned int VerticalResol,
                  float minLongitudSpine,
                  float maxLongitudSpine,
                  float minRadio,
                  float maxRadio );

      SpinesSWC ( NeuroSWC *neuron,
                  unsigned int numSpines,
                  unsigned int HorizontalResol,
                  unsigned int VerticalResol,
                  float minLongitudSpine,
                  float maxLongitudSpine,
                  float minRadio,
                  float maxRadio,
                  std::vector <VertexDistances> pVertexDistancesContainer,
                  std::vector <FacesDistances> pFacesDistancesContainer );

      ~SpinesSWC ( );

      void calcVertexCandDistances ( bool pCompleteMesh = true );

      void generateLocalSpine ( OpenMesh::Vec3f YDirVec, float radio );

      void distributeSpinesProcedural ( );

      void distributeSpinesModeled ( );

      unsigned int getNumVerticesEnSpina ( ) { return mNumVerticesEnSpina; };

      //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

      void distributeSpineAlongVertexs ( unsigned int pNumSpines,
                                         std::vector < unsigned int > pVertexSegmentsCandidate,
                                         float pSpinesPerVertex );

      void distributeSpineAlongDendritic ( unsigned int pIdDendritic );

      void distributeSpineAlongAllDendritics ( );

      void distributeSpinesInSegments ( float pSpinesDesp,
                                        float pBProgressionFactor,
                                        float pAProgressionFactor,
                                        float pBasalMinDistance,
                                        float pBasalCteDistance,
                                        float pApicalMinDistance,
                                        float pApicalCteDistance,
                                        BBDD::BBDD bbdd,
                                        const std::string& neuronName,
                                        const std::string& tmpPath
      );

      //void distributeSpineAlongAllDendriticsInGroups(unsigned int pNumberOfGropus);

      void setMultipleGroupsOfSpines ( bool pMultiGroup );

      void setDistributorModificator ( float pDistModificator );

      unsigned int preCalcNumSpines ( );

      //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

      void setAllParams ( unsigned int numSpines,
                          unsigned int HorizontalResol,
                          unsigned int VerticalResol,
                          float minLongitudSpine,
                          float maxLongitudSpine,
                          float minRadio,
                          float maxRadio );

      std::vector <VertexDistances> getVertexDistancesContainer ( );

      std::vector <FacesDistances> getFacesDistancesContainer ( );

      void setVertexDistancesContainer ( std::vector <VertexDistances> pContainer );

      void setFacesDistancesContainer ( std::vector <FacesDistances> pContainer );

      void exportSpinesInfo ( string fileName );

      void importSpinesInfo ( string fileName );

      //const std::vector<SpineInfo> & getSpinesInfo() const {return mSpinesInfo;}
      const std::vector <SpineInfo> getSpinesInfo ( ) { return mSpinesInfo; }


      //BaseMeshContainer * getSpinesContainer();

      //void setSpinesContainer(BaseMeshContainer * pContainer);

      //BaseMeshContainer getSpinesContainer();

      void setSpinesContainer ( BaseMeshContainer *pContainer );

      void generateSpinesFromInfo ( );

      void vectorizeSpineDesp ( );

      const std::vector < unsigned int > &getSpineDesp ( ) const { return mSpineDesp; };

      void distributeSpines(const vector<Spine> &spines,const std::string& neuronName,const OpenMesh::Vec3f& diplacement, BBDD::BBDD bbdd, const std::string& tmpPath);

      static MeshDef::Color getRandomColor();

      static const std::vector<Qt::GlobalColor > spineColors;
      static QRandomGenerator random;
  };
}

#endif //_SPINESSWC_H_