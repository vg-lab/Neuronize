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

// WAR for Windows building
#ifdef _MSC_VER
#pragma warning( disable : 4244 )
#endif

#include "BaseMesh.h"

#include "BSphere.h"
#include "AABB.h"
#include <fstream>
#include <boost/algorithm/string.hpp>

namespace NSBaseMesh
{

  BaseMesh::BaseMesh ( )
  {
    info = NULL;
    warn = NULL;
    err = NULL;

    mTexture = NULL;
    mTextureWidth
      = mTextureHeight
      = 0;

    mBSphere = NULL;
    mAABB = NULL;

    Mesh = new MeshDef ( );

    Mesh->request_face_normals ( );
    Mesh->request_face_colors ( );
    Mesh->request_vertex_normals ( );
    Mesh->request_vertex_colors ( );
    Mesh->request_vertex_texcoords2D ( );
    Mesh->request_edge_status ( );
    Mesh->request_face_status ( );
    Mesh->request_vertex_status ( );

    readyMesh = false;
    mNumVertex = 0;
    mNumFaces = 0;

    //subdivider_[SOP_UniformCompositeLoop]      = new OpenMesh::Subdivider::Uniform::CompositeLoopT<CMesh>;
    //subdivider_[SOP_UniformCompositeSqrt3]     = new OpenMesh::Subdivider::Uniform::CompositeSqrt3T<CMesh>;

    subdivider_[SOP_UniformLoop] = new OpenMesh::Subdivider::Uniform::LoopT < MeshDef >;

    subdivider_[SOP_ModifiedButterfly] = new OpenMesh::Subdivider::Uniform::ModifiedButterflyT < MeshDef >;

    subdivider_[SOP_UniformSqrt3] = new OpenMesh::Subdivider::Uniform::Sqrt3T < MeshDef >;
    subdivider_[SOP_UniformInterpolatingSqrt3] = new OpenMesh::Subdivider::Uniform::InterpolatingSqrt3LGT < MeshDef >;

  }

  BaseMesh::BaseMesh ( string meshName )
  {
    info = NULL;
    warn = NULL;
    err = NULL;

    mTexture = NULL;
    mTextureWidth
      = mTextureHeight
      = 0;

    mBSphere = NULL;
    mAABB = NULL;

    Mesh = new MeshDef ( );

    Mesh->request_face_normals ( );
    Mesh->request_face_colors ( );
    Mesh->request_vertex_normals ( );
    Mesh->request_vertex_colors ( );
    Mesh->request_vertex_texcoords2D ( );
    Mesh->request_edge_status ( );
    Mesh->request_face_status ( );
    Mesh->request_vertex_status ( );

    mNumVertex = 0;
    mNumFaces = 0;

    //subdivider_[SOP_UniformCompositeLoop]      = new OpenMesh::Subdivider::Uniform::CompositeLoopT<CMesh>;
    //subdivider_[SOP_UniformCompositeSqrt3]     = new OpenMesh::Subdivider::Uniform::CompositeSqrt3T<CMesh>;

    subdivider_[SOP_UniformLoop] = new OpenMesh::Subdivider::Uniform::LoopT < MeshDef >;
    subdivider_[SOP_ModifiedButterfly] = new OpenMesh::Subdivider::Uniform::ModifiedButterflyT < MeshDef >;

    subdivider_[SOP_UniformSqrt3] = new OpenMesh::Subdivider::Uniform::Sqrt3T < MeshDef >;
    subdivider_[SOP_UniformInterpolatingSqrt3] = new OpenMesh::Subdivider::Uniform::InterpolatingSqrt3LGT < MeshDef >;

    loadModel ( meshName );
  }

  BaseMesh::~BaseMesh ( )
  {
    delete Mesh;

    //delete subdivider_[SOP_UniformCompositeLoop];
    //delete subdivider_[SOP_UniformCompositeSqrt3];
    delete subdivider_[SOP_UniformLoop];
    delete subdivider_[SOP_UniformSqrt3];
    delete subdivider_[SOP_UniformInterpolatingSqrt3];
    delete subdivider_[SOP_ModifiedButterfly];
    //delete subdivider_[SOP_Undefined];

    if ( mTexture != NULL )
    {
      SOIL_free_image_data ( mTexture );
    }

    if ( mBSphere != NULL )
      delete mBSphere;
    if ( mAABB != NULL )
      delete mAABB;

  }

  void BaseMesh::updateBaseMesh ( )
  {
    Mesh->update_normals ( );
    prepareGeometry ( );
  }

  void BaseMesh::setVertexColor ( const MeshDef::ConstVertexIter begin,
                                  const MeshDef::ConstVertexIter end,
                                  MeshDef::Color pColor )
  {
    assignDefaultColor ( Mesh, begin, end, pColor );
  }

  void BaseMesh::setVertexColor ( const MeshDef::VertexHandle begin, int end, MeshDef::Color pColor )
  {
    assignDefaultColor ( Mesh, begin, end, pColor );
  }

  void BaseMesh::setVertexColor ( const MeshDef::VertexHandle begin, MeshDef::Color pColor )
  {
    assignDefaultColor ( Mesh, begin, pColor );
  }

  void BaseMesh::setFacesColor ( const MeshDef::ConstFaceIter begin,
                                 const MeshDef::ConstFaceIter end,
                                 MeshDef::Color pColor )
  {
    assignDefaultColor ( Mesh, begin, end, pColor );
  }

  //Load a model from file
  void BaseMesh::loadModel ( string meshName )
  {
    OpenMesh::IO::Options opt;
    opt += OpenMesh::IO::Options::VertexColor;
      if ( !OpenMesh::IO::read_mesh ( *Mesh, meshName, opt ))
    {
      std::cerr << "Error Reading File\n";
      delete Mesh;
      throw;
    }

    updateBaseMesh ( );

    opt.clear ( );
  }

  void BaseMesh::loadTexture ( string pTextureName )
  {
    if ( mTexture != NULL )
    {
      SOIL_free_image_data ( mTexture );
    }

    mTexture = SOIL_load_image ( pTextureName.c_str ( ), &mTextureWidth, &mTextureHeight, 0, SOIL_LOAD_RGB );
  }

  unsigned char *BaseMesh::getTexture ( )
  {
    return mTexture;
  }

  int BaseMesh::getTextureWidth ( )
  {
    return mTextureWidth;
  }

  int BaseMesh::getTextureHeight ( )
  {
    return mTextureHeight;
  }

  //Return true if the mesh are ready
  bool BaseMesh::MeshIsReady ( )
  {
    return readyMesh;
  }

  // Get the mesh to render.
  const MeshDef *BaseMesh::getMesh ( ) const
  {
    return Mesh;
  }

  // Get the mesh to render without protection (for modify)
  MeshDef *BaseMesh::getUnprotectedMesh ( )
  {
    return Mesh;
  }

  //Importation method to use wiht AssImp
  void BaseMesh::ImportFromAsset ( string model )
  {
    //Not implemented yet!.
  }

  void BaseMesh::applyNoiseToAll ( float range, bool pCteNoise )
  {
    //Random must be initialized before
    //initrand();

    float value = 0;
    value = range;

    //Move all vertices one unit length along it's normal direction
    for ( MeshDef::VertexIter v_it = Mesh->vertices_begin ( );
          v_it != Mesh->vertices_end ( );
          ++v_it )
    {
      if ( !pCteNoise )
      {
        value = randfloat ( range );
      }

      Mesh->set_point ( *v_it, Mesh->point ( *v_it ) + ( Mesh->normal ( *v_it )*value ));
    }
  }

  void BaseMesh::applyControlledNoise ( float range, bool pCteNoise, unsigned int pLocalLimit, unsigned int pMinDesp )
  {
    //Random must be initialized before
    //initrand();


    float value = 0;
    value = range;

    //Move all vertices one unit length along it's normal direction
    for ( MeshDef::VertexIter v_it = Mesh->vertices_begin ( );
          v_it != Mesh->vertices_end ( );
          ++v_it )
    {
      if (( v_it->idx ( )%pLocalLimit ) >= pMinDesp )
      {
        if ( !pCteNoise )
        {
          value = randfloat ( range );
        }

        Mesh->set_point ( *v_it, Mesh->point ( *v_it ) + ( Mesh->normal ( *v_it )*value ));
      }
    }
  }

  void BaseMesh::applyNoiseToIntervall ( float range,
                                         bool pCteNoise,
                                         unsigned int pInitialDesp,
                                         unsigned int pFinalDesp )
  {
    //Random must be initialized before
    //initrand();


    float value = 0;
    value = range;
    MeshDef::VertexIter v_it = Mesh->vertices_begin ( );
    unsigned int lDespCounter = 0;

    for ( ; lDespCounter < pInitialDesp; ++lDespCounter )
    {
      ++v_it;
    }

    //Move all vertices one unit length along it's normal direction
    for ( ;
      v_it != Mesh->vertices_end ( )
        && lDespCounter < pFinalDesp;
      ++v_it, ++lDespCounter )
    {
      if ( !pCteNoise )
      {
        value = randfloat ( range );
      }

      Mesh->set_point ( *v_it, Mesh->point ( *v_it ) + ( Mesh->normal ( *v_it )*value ));
    }
  }

  void BaseMesh::applyMatrixTransform ( const boost::numeric::ublas::matrix < float > &pMatrixTransform,
                                        unsigned int pDimMatrix )
  {
    namespace ublas      = boost::numeric::ublas;

    unsigned int lNumVertex = 0;
    MeshDef::Point lTmpPoint;
    ublas::vector < float > lTmpVertex ( pDimMatrix );

    for ( MeshDef::VertexIter v_it = Mesh->vertices_begin ( );
          v_it != Mesh->vertices_end ( );
          ++v_it )
    {

      lTmpPoint = Mesh->point ( *v_it );

      lTmpVertex[0] = lTmpPoint[0];
      lTmpVertex[1] = lTmpPoint[1];
      lTmpVertex[2] = lTmpPoint[2];
      lTmpVertex[3] = 1;

      lTmpVertex = prod ( pMatrixTransform, lTmpVertex );

      lTmpPoint[0] = lTmpVertex[0];
      lTmpPoint[1] = lTmpVertex[1];
      lTmpPoint[2] = lTmpVertex[2];

      Mesh->set_point ( *v_it, lTmpPoint );
    }
  }

  //Smooth mesh by differnete subdivisi�n methods
  void BaseMesh::SmoothMesh ( uint smoothMethod, uint pTechnique, uint pContinuity, uint pIterations )
  {
    switch ( pContinuity )
    {
      case 0 : continuity = OpenMesh::Smoother::SmootherT < MeshDef >::C0;
        break;
      case 1 : continuity = OpenMesh::Smoother::SmootherT < MeshDef >::C1;
        break;
      case 2 : continuity = OpenMesh::Smoother::SmootherT < MeshDef >::C2;
        break;
    }

    if ( smoothMethod == 0 )
    {
      component = OpenMesh::Smoother::SmootherT < MeshDef >::Tangential_and_Normal;
    }
    else
    {
      component = OpenMesh::Smoother::SmootherT < MeshDef >::Tangential;
    }

    if ( pTechnique == 0 )
    {
      OpenMesh::Smoother::JacobiLaplaceSmootherT <MeshDef> smoother ( *Mesh );
      smoother.initialize ( component, continuity );
      smoother.smooth ( pIterations );
    }
    //else
    //{
    //	OpenMesh::Smoother::LaplaceSmootherT<MeshDef> smoother( *Mesh );
    //	smoother.initialize(component,continuity);
    //	smoother.smooth(pIterations);
    //}
  }

  //Join the vertex and the faces to the current mesh
  void BaseMesh::JoinBaseMesh ( const BaseMesh *pBaseMesh )
  {
    MeshDef::Point lAuxVec ( 0, 0, 0 );
    MeshDef::Color lAuxColor (0,0,0,0);

    //Capturar los vhandler a medida que se crean para ir controlandolos luego


    //A�adir los v�rtices al final
    for ( MeshDef::ConstVertexIter v_it = pBaseMesh->getMesh ( )->vertices_begin ( );
          v_it != pBaseMesh->getMesh ( )->vertices_end ( );
          ++v_it )
    {
      lAuxVec = pBaseMesh->getMesh ( )->point ( *v_it );
      lAuxColor =pBaseMesh->getMesh()->color(v_it);
      auto newVertex =Mesh->add_vertex ( MeshDef::Point ( lAuxVec[0], lAuxVec[1], lAuxVec[2]
                         )
      );
      Mesh->set_color(newVertex,{lAuxColor[0],lAuxColor[1],lAuxColor[2],lAuxColor[3]});
    }

    //Anyadir las facetas al final contemplando el desplazamiento del numero de vertices inicial
    MeshDef::ConstFaceIter lFBItBM ( pBaseMesh->getMesh ( )->faces_begin ( )),
      lFEItBM ( pBaseMesh->getMesh ( )->faces_end ( ));

    MeshDef::ConstFaceVertexIter fvIt;
    std::vector <MeshDef::VertexHandle> Face_VHandles;
    //MeshDef::VertexHandle					lAuxVHandle;

    unsigned int index = 0;

    for ( ; lFBItBM != lFEItBM; ++lFBItBM )
    {

      Face_VHandles.clear ( );

      //fvIt = pBaseMesh->getMesh()->cfv_iter(*lFBItBM);

      //index =	fvIt->idx();
      //index += mNumVertex;
      //Face_VHandles.push_back(Mesh->vertex_handle(index));

      //++fvIt;
      //index = fvIt->idx();
      //index += mNumVertex;
      //Face_VHandles.push_back(Mesh->vertex_handle(index));

      //++fvIt;
      //index = fvIt->idx();
      //index += mNumVertex;
      //Face_VHandles.push_back(Mesh->vertex_handle(index));

      //Paramallas de cualquier resoluci�n
      for ( fvIt = pBaseMesh->getMesh ( )->cfv_iter ( *lFBItBM ); fvIt.is_valid ( ); ++fvIt )
      {
        index = fvIt->idx ( );
        index += mNumVertex;
        Face_VHandles.push_back ( Mesh->vertex_handle ( index ));
      }

      Mesh->add_face ( Face_VHandles );
    }

    updateBaseMesh ( );
  }

  //Subdivide Mesh
  void BaseMesh::Subdivide ( SOPType pSOPType )
  {

    subdivider_[pSOPType]->attach ( *Mesh );
    //subdivider_.attach(Mesh);
    ( *subdivider_[pSOPType] ) ( 1 );

    subdivider_[pSOPType]->detach ( );

    //Mesh->update_normals();
    updateBaseMesh ( );

    MeshDef::ConstVertexIter iniLimit = Mesh->vertices_begin ( );
    MeshDef::ConstVertexIter finLimit = Mesh->vertices_end ( );

    //Pintar vertices soma
    setVertexColor ( iniLimit, finLimit, MeshDef::Color ( 1.0, 0, 0, 0 ));
  }

/*
	void BaseMesh::Decimate(unsigned int pNumSteps)
	{
		int rc=0;

		decimater_o       decimater_;  
		mod_quadric_o     mod_quadric_;
		mod_nf_o          mod_nf_;

		// to be compatible with gcc 2.95.3
		{
			decimater_o  tmp( new decimater_t ( *Mesh ) );
			decimater_ = tmp;
		}
		{
			mod_quadric_o  tmp( new mod_quadric_t( *decimater_ ) );
			mod_quadric_ = tmp;
		}
		{
			mod_nf_o       tmp( new mod_nf_t     ( *decimater_ ) );
			mod_nf_      = tmp;
		}

		decimater_->initialize();
		if ( (rc=decimater_->decimate(pNumSteps)) )
		{
			decimater_->mesh().garbage_collection();
			std::cout << rc << " vertices removed!\n";
		}

	}
*/


  /*Control where informational, warning and error messages are sent.
  */
  // Set the output stream for information, can be set to NULL to disable messages.
  void BaseMesh::setInfoStream ( std::ostream *o )
  {
    info = o;
  }

  // Set the output stream for warnings, can be set to NULL to disable messages.
  void BaseMesh::setWarningStream ( std::ostream *o )
  {
    warn = o;
  }

  // Set the output stream for errors, Can be set to NULL to disable messages.
  void BaseMesh::setErrorStream ( std::ostream *o )
  {
    err = o;
  }

  // Set all streams at once.  Convienience function, can be set to NULL to disable messages.
  void BaseMesh::setStreams ( std::ostream *o )
  {
    info = o;
    warn = o;
    err = o;
  }

  // Calculate the byte distances between consecutive elements in memory.
  void BaseMesh::calcByteDistances ( )
  {
    // Check that the vertices are actually stored in an array-like manner.
    // We draw using OpenGL vertex arrays, so this must be true.

    vertexPointByteDist = 0;
    vertexColorByteDist = 0;
    vertexNormalByteDist = 0;
    vertexTexCoordByteDist = 0;

    const int N = Mesh->n_vertices ( );

    if ( N < 2 )
      return;

    //typedef const Vec3::value_type					PT; // e.g. float
    typedef const MeshDef::Point::value_type PT; // e.g. float
    typedef const MeshDef::Color::value_type CT; // e.g. unsigned char
    typedef const MeshDef::Normal::value_type NT; // e.g. float
    typedef const MeshDef::TexCoord2D::value_type TT; // e.g. float

    MeshDef::VertexHandle v1, v2;

    v1 = Mesh->vertex_handle ( 0 );
    v2 = Mesh->vertex_handle ( 1 );

    vertexPointByteDist = (( PT * ) &Mesh->point ( v2 ) - ( PT * ) &Mesh->point ( v1 ))*sizeof ( PT );

    if ( Mesh->has_vertex_colors ( ))
      vertexColorByteDist = (( CT * ) &Mesh->color ( v2 ) - ( CT * ) &Mesh->color ( v1 ))*sizeof ( CT );

    if ( Mesh->has_vertex_normals ( ))
      vertexNormalByteDist = (( NT * ) &Mesh->normal ( v2 ) - ( NT * ) &Mesh->normal ( v1 ))*sizeof ( NT );

    if ( Mesh->has_vertex_texcoords2D ( ))
      vertexTexCoordByteDist = (( TT * ) &Mesh->texcoord2D ( v2 ) - ( TT * ) &Mesh->texcoord2D ( v1 ))*sizeof ( TT );

    if ( info )
    {
      *info << "Vertex point byte distance: " << vertexPointByteDist << '\n';
      *info << "Vertex color byte distance: " << vertexColorByteDist << '\n';
      *info << "Vertex normal byte distance: " << vertexNormalByteDist << '\n';
      *info << "Vertex texture coordinate byte distance: " << vertexTexCoordByteDist << '\n';
    }
  }

  // Distance between sequential vertex points in memory
  int BaseMesh::getVertexPointDistance ( )
  {
    return vertexPointByteDist;
  }

  // Distance between sequential vertex colors in memory
  int BaseMesh::getVertexColorDistance ( )
  {
    return vertexColorByteDist;
  }

  // Distance between sequential vertex normals in memory
  int BaseMesh::getVertexNormalDistance ( )
  {
    return vertexNormalByteDist;
  }

  // Distance between sequential vertex texture coordinates in memory
  int BaseMesh::getVertexTexCoordDistance ( )
  {
    return vertexTexCoordByteDist;
  }

  unsigned int BaseMesh::getNumVertex ( )
  {
    return mNumVertex;
  }

  unsigned int BaseMesh::getNumFaces ( )
  {
    return mNumFaces;
  }

  void BaseMesh::calcFacesCentroid ( )
  {
    mFacesCentroid.clear ( );
    MeshDef::Point ltmpVertex ( 0, 0, 0 );

    for ( MeshDef::ConstFaceIter cfi = Mesh->faces_sbegin ( ); cfi != Mesh->faces_end ( ); ++cfi )
    {
      Mesh->calc_face_centroid ( *cfi, ltmpVertex );
      mFacesCentroid.push_back ( ltmpVertex );
    }
  }

  MeshDef::Point BaseMesh::getFaceCentroidById ( unsigned int pIndex )
  {
    return mFacesCentroid[pIndex];
  }

  unsigned int BaseMesh::calcNumVertex ( )
  {
    mNumVertex = 0;
    for ( MeshDef::ConstVertexIter v_it = Mesh->vertices_begin ( );
          v_it != Mesh->vertices_end ( );
          ++v_it )
    {
      ++mNumVertex;
    }
    return mNumVertex;
  }

  uint BaseMesh::calcNumFaces ( )
  {
    mNumFaces = ( n_tris + n_quads );
    return mNumFaces;
  }

  uint BaseMesh::calcNumTri ( )
  {
    int numTri = 0;
    for ( MeshDef::ConstFaceIter cfi = Mesh->faces_sbegin ( ); cfi != Mesh->faces_end ( ); ++cfi )
    {
      if ( Mesh->valence ( *cfi ) == 3 )
        ++numTri;
    }
    n_tris = numTri;
    return numTri;
  }

  uint BaseMesh::calcNumQuads ( )
  {
    int numQuads = 0;
    for ( MeshDef::ConstFaceIter cfi = Mesh->faces_sbegin ( );
          cfi != Mesh->faces_end ( ); ++cfi )
    {
      if ( Mesh->valence ( *cfi ) == 4 )
        ++numQuads;
    }
    n_quads = numQuads;
    return numQuads;
  }

  uint BaseMesh::getNumTri ( )
  {
    return n_tris;
  }

  uint BaseMesh::getNumQuads ( )
  {
    return n_quads;
  }

  void BaseMesh::calcTriIndices ( )
  {
    triIndices.clear ( );
    triIndices.reserve ( 3*getNumTri ( ));

    MeshDef::ConstFaceIter f, fEnd = Mesh->faces_end ( );
    MeshDef::ConstFaceVertexIter v;

    for ( f = Mesh->faces_begin ( ); f != fEnd; ++f )
    {
      if ( Mesh->valence ( *f ) != 3 ) // ignore non-triangles
        continue;
      for ( v = Mesh->cfv_iter ( *f ); v.is_valid ( ); ++v )
      {
        triIndices.push_back ( v->idx ( ));
      }
    }

    assert ( triIndices.size ( ) == 3*getNumTri ( ));
  }

  void BaseMesh::calcQuadIndices ( )
  {
    quadIndices.clear ( );
    quadIndices.reserve ( 4*getNumQuads ( ));

    MeshDef::ConstFaceIter f, fEnd = Mesh->faces_end ( );
    MeshDef::ConstFaceVertexIter v;

    for ( f = Mesh->faces_begin ( ); f != fEnd; ++f )
    {
      if ( Mesh->valence ( *f ) != 4 ) // ignore non-quads
        continue;
      for ( v = Mesh->cfv_iter ( *f ); v.is_valid ( ); ++v )
      {
        quadIndices.push_back ( v->idx ( ));
      }
    }

    assert ( quadIndices.size ( ) == 4*getNumQuads ( ));
  }

  void BaseMesh::calcVertexIterators ( )
  {
    mVertexIterators.clear ( );

    MeshDef::ConstVertexIter lCVIter = Mesh->vertices_begin ( );
    MeshDef::ConstVertexIter lCVIterFin = Mesh->vertices_end ( );;

    for ( ; lCVIter != lCVIterFin; ++lCVIter )
    {
      mVertexIterators.push_back ( lCVIter );
    }
  }

  std::vector < unsigned > BaseMesh::getTriIndices ( )
  {
    return triIndices;
  }

  std::vector < unsigned > BaseMesh::getQuadIndices ( )
  {
    return quadIndices;
  }

  void BaseMesh::prepareGeometry ( )
  {
    calcNumTri ( );
    calcNumQuads ( );

    calcTriIndices ( );
    calcQuadIndices ( );

    calcNumVertex ( );
    calcNumFaces ( );

    calcByteDistances ( );

    calcVertexIterators ( );

    if ( mBSphere != NULL )
      delete mBSphere;
    mBSphere = new BSphere ( this );

    if ( mAABB != NULL )
      delete mAABB;
    mAABB = new AABB ( this );

    readyMesh = true;
  }

  void BaseMesh::exportMesh ( string name )
  {
    std::string ext = name.substr( name.rfind( '.' )  + 1);
    if( boost::iequals( ext, "obj" ) )
    {
      Vec3 v, n;
      MeshDef::Color c;
      OpenMesh::VertexHandle vh;
      std::ofstream file( name, std::ios::out );
      if( !file.is_open( ) )
      {
        throw "Cant open file" + name + "for export Mesh";
      }

      //Header
      file << "# " << Mesh->n_vertices( ) << " vertices, ";
      file << Mesh->n_faces( ) << " faces" << '\n';

      //Vertex data (point, color, normals, textcoords)
      for( int i = 0, nV = Mesh->n_vertices( ); i < nV; ++i )
      {
        vh = Mesh->vertex_handle( i );
        v = Mesh->point( vh );
        n = Mesh->normal( vh );
        c = Mesh->color( vh );

        file << "v " << v[ 0 ] << " " << v[ 1 ] << " " << v[ 2 ] << " " <<
             c[ 0 ] << " " << c[ 1 ] << " " << c[ 2 ] << '\n';
      }


      std::vector<OpenMesh::VertexHandle> vhandles;
      for( int i = 0, nF = Mesh->n_faces( ); i < nF; ++i )
      {
        file << "f";
        auto f = Mesh->face_handle( i );
        for( auto v_it = Mesh->cfv_iter( f ); v_it.is_valid( ); ++v_it )
        {
          file << " " << ( *v_it ).idx( ) + 1;
        }
        file << "\n";
      }

      file.close();
    }
    else
    {
      if( !OpenMesh::IO::write_mesh( *Mesh, name ) )
      {
        std::cerr << "Error Writing File\n";
        throw "Error Writing " + name + " Mesh";
      }
    }
  }

  void BaseMesh::scaleBaseMesh ( float pScale )
  {
    //Mount transformation matrix
    namespace ublas      = boost::numeric::ublas;
    ublas::matrix < float > lScaleMatrix ( 4, 4 );
    generateSquareUniformScaleMatrix ( lScaleMatrix, 4, pScale );

    applyMatrixTransform ( lScaleMatrix, 4 );
  }

  void BaseMesh::buildTrisFromBuffers ( float *pVertexs, int pNumVertex, int *faces, int pNumFaces )
  {

    MeshDef::VertexHandle *vhandle = new MeshDef::VertexHandle[pNumVertex];

    for ( unsigned int i = 0; i < pNumVertex; ++i )
    {
      vhandle[i] = Mesh->add_vertex ( MeshDef::Point ( pVertexs[i*3], pVertexs[i*3 + 1], pVertexs[i*3 + 1]
                                      )
      );
    }

    //Index faces construct
    std::vector <MeshDef::VertexHandle> face_vhandles;

    for ( unsigned int i = 0; i < pNumFaces; ++i )
    {
      face_vhandles.clear ( );
      face_vhandles.push_back ( vhandle[faces[i*3]] );
      face_vhandles.push_back ( vhandle[faces[i*3 + 1]] );
      face_vhandles.push_back ( vhandle[faces[i*3 + 2]] );
      Mesh->add_face ( face_vhandles );
    }

    delete[] vhandle;
    updateBaseMesh ( );
  }

  void BaseMesh::buildQuadsFromBuffers ( float *pVertexs, int pNumVertex, int *faces, int pNumFaces )
  {
    MeshDef::VertexHandle *vhandle = new MeshDef::VertexHandle[pNumVertex];

    for ( unsigned int i = 0; i < pNumVertex; ++i )
    {
      vhandle[i] = Mesh->add_vertex ( MeshDef::Point ( pVertexs[i*3], pVertexs[i*3 + 1], pVertexs[i*3 + 1]
                                      )
      );
    }

    //Index faces construct
    std::vector <MeshDef::VertexHandle> face_vhandles;

    for ( unsigned int i = 0; i < pNumFaces; ++i )
    {
      face_vhandles.clear ( );
      face_vhandles.push_back ( vhandle[faces[i*4]] );
      face_vhandles.push_back ( vhandle[faces[i*4 + 1]] );
      face_vhandles.push_back ( vhandle[faces[i*4 + 2]] );
      face_vhandles.push_back ( vhandle[faces[i*4 + 3]] );
      Mesh->add_face ( face_vhandles );
    }

    delete[] vhandle;
    updateBaseMesh ( );
  }

}
