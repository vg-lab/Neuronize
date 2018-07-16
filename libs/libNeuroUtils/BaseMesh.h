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

#ifndef _BASE_MESH_H_
#define _BASE_MESH_H_

#include <string>
#include <vector>
#include <iosfwd>

#include <iostream>
#include <algorithm>

#include "MeshDef.hpp"

#include <libs/libSysNeuroUtils/randomUtils.h>
#include <libs/libSysNeuroUtils/MathUtils.h>

#include <OpenMesh/Tools/Subdivider/Uniform/SubdividerT.hh>

#include <OpenMesh/Tools/Subdivider/Uniform/CompositeLoopT.hh>
#include <OpenMesh/Tools/Subdivider/Uniform/CompositeSqrt3T.hh>
#include <OpenMesh/Tools/Subdivider/Uniform/LoopT.hh>
#include <OpenMesh/Tools/Subdivider/Uniform/Sqrt3T.hh>
#include <OpenMesh/Tools/Subdivider/Uniform/Sqrt3InterpolatingSubdividerLabsikGreinerT.hh>
#include <OpenMesh/Tools/Subdivider/Uniform/ModifiedButterFlyT.hh>

/*
#include <OpenMesh/Tools/Decimater/DecimaterT.hh>
#include <OpenMesh/Tools/Decimater/ModNormalFlippingT.hh>
#include <OpenMesh/Tools/Decimater/ModQuadricT.hh>
*/

#include <OpenMesh/Tools/Smoother/JacobiLaplaceSmootherT.hh>

#include <SOIL/SOIL.h>

using namespace std;

class BSphere;
class AABB;

namespace NSBaseMesh
{
  enum SOPType
  {
    //SOP_UniformCompositeLoop,
    //SOP_UniformCompositeSqrt3,
      SOP_UniformLoop,
    SOP_UniformSqrt3,
    SOP_UniformInterpolatingSqrt3,
    SOP_ModifiedButterfly,
    SOP_Undefined
  };

  class BaseMesh
  {
    protected:
      //Mesh to store data
      MeshDef *Mesh;

      BSphere *mBSphere;
      AABB *mAABB;

      unsigned char *mTexture;
      int mTextureWidth;
      int mTextureHeight;

      unsigned int mNumVertex;
      unsigned int mNumFaces;

      uint n_tris;              // number of triangles in current mesh
      uint n_quads;              // number of quads in current mesh

      std::vector < unsigned int > triIndices;    // Indices of displayed triangles for current mesh
      std::vector < unsigned int > quadIndices;    // Indices of displayed quads for current mesh

      std::vector <MeshDef::Point> mFacesCentroid;          // Faces centroids

      //Si el cambio de color de los vertices falla con los vertex handlers
      std::vector <MeshDef::ConstVertexIter> mVertexIterators;    // Vertex Iterators

      ////Information about the mesh
      std::ostream *info;        // Information output stream.
      std::ostream *warn;        // Warning output stream.
      std::ostream *err;        // Error output stream.

      int vertexPointByteDist;    // Distance between sequential vertex points in memory
      int vertexColorByteDist;        // Distance between sequential vertex colors in memory
      int vertexNormalByteDist;       // Distance between sequential vertex normals in memory
      int vertexTexCoordByteDist;     // Distance between sequential vertex texture coordinates in memory

      bool readyMesh;

      //typedef OpenMesh::Subdivider::Uniform::CompositeTraits			CTraits;
      //typedef OpenMesh::TriMesh_ArrayKernelT<CTraits>					CMesh;
      //typedef OpenMesh::Subdivider::Uniform::SubdividerT< MeshDef >		CSubdivider;

      typedef OpenMesh::Subdivider::Uniform::SubdividerT <MeshDef> Subdivider;
      typedef std::map < SOPType, Subdivider * > SubdividerPool;
      SubdividerPool subdivider_;

/*
			typedef OpenMesh::Decimater::DecimaterT<MeshDef>				decimater_t;  
			typedef OpenMesh::Decimater::ModQuadricT< decimater_t >			mod_quadric_t;
			typedef OpenMesh::Decimater::ModNormalFlippingT< decimater_t >	mod_nf_t;

			// object types
			typedef std::auto_ptr< decimater_t   >   decimater_o;
			typedef std::auto_ptr< mod_quadric_t >   mod_quadric_o;
			typedef std::auto_ptr< mod_nf_t      >   mod_nf_o;
*/

      //Smooth params
      OpenMesh::Smoother::SmootherT < MeshDef >::Continuity continuity;  // = SmootherT<MyMesh>::C1;
      OpenMesh::Smoother::SmootherT < MeshDef >::Component component;  //  = SmootherT<MyMesh>::Tangential_and_Normal;

    public:

      //Cuidado, esto no deber�a ser publico
      void updateBaseMesh ( );

      BaseMesh ( );

      BaseMesh ( string meshName );

      ~BaseMesh ( );

      // Get the mesh to render.
      const MeshDef *getMesh ( ) const;

      MeshDef *getUnprotectedMesh ( );

      const std::vector <MeshDef::ConstVertexIter> &getVertexIterators ( ) const { return mVertexIterators; };

      //Importation method to use wiht AssImp
      void ImportFromAsset ( string model );

      void applyNoiseToAll ( float range, bool pCteNoise );

      void applyNoiseToIntervall ( float range, bool pCteNoise, unsigned int pInitialDesp, unsigned int pFinalDesp );

      void applyControlledNoise ( float range, bool pCteNoise, unsigned int pLocalLimit, unsigned int pMinDesp );

      void applyMatrixTransform ( const boost::numeric::ublas::matrix < float > &pMatrixTransform,
                                  unsigned int pDimMatrix );

      //Smooth mesh by differnete subdivisi�n methods
      void SmoothMesh ( uint smoothMethod, uint pTechnique, uint pContinuity, uint pIterations );

      //Join the vertex and the faces to the current mesh
      void JoinBaseMesh ( const BaseMesh *pBaseMesh );

      //Subdivide Mesh
      void Subdivide ( SOPType pSOPType );

      //Decimate
      //void Decimate(unsigned int pNumSteps);

      /*Control where informational, warning and error messages are sent.
      */

      // Set the output stream for information, can be set to NULL to disable messages.
      void setInfoStream ( std::ostream *o );

      // Set the output stream for warnings, can be set to NULL to disable messages.
      void setWarningStream ( std::ostream *o );

      // Set the output stream for errors, Can be set to NULL to disable messages.
      void setErrorStream ( std::ostream *o );

      // Set all streams at once.  Convienience function, can be set to NULL to disable messages.
      void setStreams ( std::ostream *o );

      unsigned int getNumVertex ( );

      unsigned int getNumFaces ( );

      unsigned int calcNumVertex ( );

      void calcFacesCentroid ( );

      MeshDef::Point getFaceCentroidById ( unsigned int pIndex );

      uint calcNumFaces ( );

      uint calcNumTri ( );

      uint calcNumQuads ( );

      uint getNumTri ( );

      uint getNumQuads ( );

      void calcTriIndices ( );

      void calcQuadIndices ( );

      void calcVertexIterators ( );

      std::vector < unsigned > getTriIndices ( );

      std::vector < unsigned > getQuadIndices ( );

      void prepareGeometry ( );

      // Calculate the byte distances between consecutive elements in memory.
      void calcByteDistances ( );

      // Distance between sequential vertex points in memory
      int getVertexPointDistance ( );

      // Distance between sequential vertex colors in memory
      int getVertexColorDistance ( );

      // Distance between sequential vertex normals in memory
      int getVertexNormalDistance ( );

      // Distance between sequential vertex texture coordinates in memory
      int getVertexTexCoordDistance ( );

      //Load a model from file
      void loadModel ( string meshName );

      //Load a testure from file
      void loadTexture ( string pTextureName );

      unsigned char *getTexture ( );

      int getTextureWidth ( );

      int getTextureHeight ( );

      //Return true if the mesh are ready
      bool MeshIsReady ( );

      void exportMesh ( string name );

      void setVertexColor ( const MeshDef::ConstVertexIter begin,
                            const MeshDef::ConstVertexIter end,
                            MeshDef::Color pColor );

      //void setVertexColor(int begin, int end, MeshDef::Color pColor);
      void setVertexColor ( const MeshDef::VertexHandle begin, int end, MeshDef::Color pColor );

      void setVertexColor ( const MeshDef::VertexHandle begin, MeshDef::Color pColor );

      void setFacesColor ( const MeshDef::ConstFaceIter begin,
                           const MeshDef::ConstFaceIter end,
                           MeshDef::Color pColor );

      ////////////////////////////////////////////////////////////////////////////////////////
      /*Methods to set/get the source params:
        *	vertex positions
        *	vertex parametric positions of the texture
        *	vertex normals
        *	vertex colors

        If not specified or invalid, the property is set to the default
        * the built-in vertex point coordinates, that is mesh->point(v).
        * the built-in vertex texture coordinates, that is mesh->texcoord2D(v)
        * the built-in vertex normals coordinates, that is mesh.normal(v).
        * the built-in vertex colors, that is, mesh->color(v)
      */

      /*
      void setVertexPositionSource(const VertexPositionSource& custom = VertexPositionSource());
      const VertexPositionSource& getVertexPositionSource() const;

      void setVertexParamSource(const VertexParamSource& custom = VertexParamSource());
      const VertexParamSource& getVertexParamSource() const;

      void setVertexNormalSource(const VertexNormalSource& custom = VertexNormalSource());
      const VertexNormalSource& getVertexNormalSource() const;

      void setVertexColorSource(const VertexColorSource& custom = VertexColorSource());
      const VertexColorSource& getVertexColorSource() const;
      */

      void scaleBaseMesh ( float pScale );

      void buildTrisFromBuffers ( float *pVertexs, int pNumVertex, int *faces, int pNumFaces );

      void buildQuadsFromBuffers ( float *pVertexs, int pNumVertex, int *faces, int pNumFaces );
  };
}
#endif //_BASE_MESH_H_
