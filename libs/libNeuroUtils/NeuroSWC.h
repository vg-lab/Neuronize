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

#ifndef _NEUROSWC_H_
#define _NEUROSWC_H_

#include <neuroutils/api.h>

#include <string>
#include <vector>
#include <iosfwd>

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>

#include <boost/math/quaternion.hpp>

#include "SWCImporter.h"
#include "BaseMesh.h"
#include "BaseMeshContainer.h"
#include <libs/libSysNeuroUtils/MathUtils.h>

using namespace std;
using namespace NSBaseMesh;
using namespace NSSWCImporter;
using namespace NSBaseMeshContainer;

namespace NSNeuroSWC
{
  class NEUROUTILS_API NeuroSWC: public BaseMesh
  {
    protected:

      int HResolTube;
      int VResolTube;
      int HResolSoma;
      int VResolSoma;

      int mSomaId;
      unsigned int mSomaNumFaces;

      string fileName;

      SWCImporter *importer;

      //Temporary declaratios (optimize the serial code)
      OpenMesh::Vec3f glb_forward;

      float glb_modulo;
      float glb_incremet;
      float glb_incRadio;
      float glb_angle;
      int glb_nVerticesTube;
      int glb_nVerticesSoma;

      bool mPiramidalSoma;

      //Local VertexHandle
      MeshDef::VertexHandle *glb_tubeVHandle;

      //Global Mesh Vertex Handle
      MeshDef::VertexHandle *glb_meshVHandle;
      unsigned int meshVHandleTotal;

      std::vector <MeshDef::FaceHandle> mFusionCandidateFace_VHandles;

      std::vector <MeshDef::VertexHandle> glb_tubeFace_VHandles;

      boost::numeric::ublas::matrix < float > glb_mat;

      boost::numeric::ublas::vector < float > glb_forwardVec;
      boost::numeric::ublas::vector < float > glb_DespVec;
      boost::numeric::ublas::vector < float > glb_AuxVec;

      //////////////////////////////////////////////////////
      //Elementos de rotaci�n del primer slice de los tubos
      boost::numeric::ublas::matrix < float > glb_mat_1Slice;

      boost::numeric::ublas::vector < float > glb_forwardVec_1Slice;
      boost::numeric::ublas::vector < float > glb_TransVectorAux_1Slice;
      boost::numeric::ublas::vector < float > glb_RightVector_1Slice;
      boost::numeric::ublas::vector < float > glb_UpVector_1Slice;
      boost::numeric::ublas::vector < float > glb_AuxDir_1Slice;

      //////////////////////////////////////////////////////

      //Preproces angles
      boost::numeric::ublas::vector < float > CoSinAngles;
      boost::numeric::ublas::vector < float > SinAngles;

      boost::numeric::ublas::vector < float > XVector;
      boost::numeric::ublas::vector < float > YVector;
      boost::numeric::ublas::vector < float > glb_ZeroVec;

      //Auxiliar forwardvector
      boost::numeric::ublas::vector < float > glb_TransVectorAux;

      //Right vector
      boost::numeric::ublas::vector < float > glb_RightVector;

      //Up vector
      boost::numeric::ublas::vector < float > glb_UpVector;

      BaseMeshContainer *mSomaContainer;

      bool mSharpEnd;
      bool mTessellControlSew;

      float SignedArea ( OpenMesh::Vec3f pPoint1, OpenMesh::Vec3f pPoint2, OpenMesh::Vec3f pPoint3 );

      float Determinant3x3 ( OpenMesh::Vec3f pPoint1, OpenMesh::Vec3f pPoint2 );

    public:
      NeuroSWC ( );

      NeuroSWC ( string SWCName, int horResolTube, int verResolTube, int horResolSoma, int verResolSoma );

      ~NeuroSWC ( );

      void generateNeuron ( string SWCName,
                            int horResolTube,
                            int verResolTube,
                            int horResolSoma,
                            int verResolSoma,
                            unsigned int pSomaModel = -1 );

      //Add a spheric object to model
      void generateSphere ( const OpenMesh::Vec3f &center, float radio, int width, int height );

      //Generate sections of the dendrities
      void generateLinearTube ( const OpenMesh::Vec3f &ini,
                                const OpenMesh::Vec3f &fin,
                                float radioIni,
                                float radioFin,
                                int nodeId,
                                int nodeParent );
      void generateLinearTubeWithControlSews ( const OpenMesh::Vec3f &ini,
                                               const OpenMesh::Vec3f &fin,
                                               float radioIni,
                                               float radioFin,
                                               int nodeId,
                                               int nodeParent );

      //Generate the dendritic terminations
      void generateDendriticTerminations ( );

      void connectConsecutiveTubes ( );

      void connectNoConsecutiveTubes ( );

      void connectNoConsecutiveTubes2 ( );

      void connectSoma ( );

      void connectPiramidalSoma ( );

      unsigned int getSomaNumVertex ( );

      unsigned int getTubeNumVertex ( );

      unsigned int getTubeHReol ( ) { return HResolTube; };

      unsigned int getTubeVReol ( ) { return VResolTube; };

      void setAllParams ( int horResolTube, int verResolTube, int horResolSoma, int verResolSoma );

      void JoinSpines ( const BaseMesh *pBaseMesh );

      void addFaceFusionCandidate ( MeshDef::FaceHandle pFaceCandidate );

      void freeFaceFusionCandidates ( );

      void setColorsFromSWC ( const SWCImporter *pImporter );

      bool isFaceHandleInContainer ( MeshDef::FaceHandle pFHandle,
                                     std::vector <MeshDef::FaceHandle> pFaceHandleContainer );

      bool isVertexHandleInContainer ( MeshDef::VertexHandle pVHandle,
                                       std::vector <MeshDef::VertexHandle> pVertexHandleContainer );

      void applyNoiseToSoma ( float range, bool pCteNoise );

      void applyNoiseToDendritics ( float range, bool pCteNoise );

      unsigned int getSomaNumFaces ( );

      SWCImporter *getImporter ( );

      void setImporter ( SWCImporter *pImporter );

      void setSomaContainer ( BaseMeshContainer *pContainer );

      void setPiramidalSoma ( bool pPiramidalSoma );

      void calcPreciseBifurcation ( );

  };
}
#endif //_NEUROSWC_H_