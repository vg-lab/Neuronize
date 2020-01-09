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

#include "SpinesSWC.h"
#include <math.h>

#define SOMA_MIN_DISTACE  20
#define BASAL_INTERVALS    10
#define APICAL_INTERVALS  22
#define DESP_PERCENT    20.0
#define DESP_SPINES_IN_TERM  3;
#define SPINE_EXTRA_SCALE  1.5;
#define SPINE_INCRE_DESP  5;

#define SPINE_SEGMENT_PROG  20.0
#define SPINE_SEGMENT_DESP  1.0

namespace NSSpinesSWC
{

    const std::vector<Qt::GlobalColor > SpinesSWC::spineColors={
            Qt::cyan, Qt::darkCyan, Qt::red,Qt::darkRed,Qt::magenta, Qt::darkMagenta,
            Qt::green,Qt::darkGreen,Qt::yellow,Qt::darkYellow,Qt::blue,Qt::darkBlue
    };
    QRandomGenerator SpinesSWC::random;

    SpinesSWC::SpinesSWC ( ): BaseMesh ( )
  //SpinesSWC::SpinesSWC()
  {
    HorResol = VerResol = totalNumSpines = 0;
    mMinLongSpine = mMaxLongSpine = mMinRadio = mMaxRadio = 0;
    spinesVHandleTotal = spineGlobalDesp = 0;

    NeuronMesh = NULL;

    mMultipleSpinesGropus = true;
    mDistributorModificator = 1.0;

    mSpinesSynthContainer = new BaseMeshContainer ( );
    glb_GlobalSpinesVHandle = nullptr;
    //glb_GlobalSpinesVHandle = new MeshDef::VertexHandle[100];
  }

  SpinesSWC::SpinesSWC ( NeuroSWC *neuron,
                         unsigned int numSpines,
                         unsigned int HorizontalResol,
                         unsigned int VerticalResol,
                         float minLongitudSpine,
                         float maxLongitudSpine,
                         float minRadio,
                         float maxRadio )
  {
    HorResol = HorizontalResol;
    VerResol = VerticalResol;
    totalNumSpines = numSpines;
    mMinLongSpine = minLongitudSpine;
    mMaxLongSpine = maxLongitudSpine;
    mMinRadio = minRadio;
    mMaxRadio = maxRadio;
    spinesVHandleTotal = 0;
    spineGlobalDesp = ( HorResol*VerResol ) + 1;
    NeuronMesh = neuron;
    mMultipleSpinesGropus = true;
    mDistributorModificator = 1.0;

    mSpinesSynthContainer = new BaseMeshContainer ( );

    //Reseve memory
    glb_GlobalSpinesVHandle = new MeshDef::VertexHandle[( spineGlobalDesp )*totalNumSpines];

    //Generate spine model
    //generateLocalSpine(OpenMesh::Vec3f(0,randfloat(mMinLongSpine, mMaxLongSpine),0) , randfloat(mMinRadio, mMaxRadio));

    //Generate real distribution
    generateSpinesDistribution ( );

    //Mark the vertex
    //calcVertexCandDistances();

    //distributeSpinesProcedural();
  }

  SpinesSWC::SpinesSWC ( NeuroSWC *neuron,
                         unsigned int numSpines,
                         unsigned int HorizontalResol,
                         unsigned int VerticalResol,
                         float minLongitudSpine,
                         float maxLongitudSpine,
                         float minRadio,
                         float maxRadio,
                         std::vector <VertexDistances> pVertexDistancesContainer,
                         std::vector <FacesDistances> pFacesDistancesContainer )
  {
    HorResol = HorizontalResol;
    VerResol = VerticalResol;
    totalNumSpines = numSpines;
    mMinLongSpine = minLongitudSpine;
    mMaxLongSpine = maxLongitudSpine;
    mMinRadio = minRadio;
    mMaxRadio = maxRadio;
    spinesVHandleTotal = 0;
    spineGlobalDesp = ( HorResol*VerResol ) + 1;
    NeuronMesh = neuron;
    mMultipleSpinesGropus = true;
    mDistributorModificator = 1.0;

    mSpinesSynthContainer = new BaseMeshContainer ( );

    mVertexDistancesContainer = pVertexDistancesContainer;
    mFacesDistancesContainer = pFacesDistancesContainer;

    //Reseve memory
    glb_GlobalSpinesVHandle = new MeshDef::VertexHandle[( spineGlobalDesp )*totalNumSpines];

    //Generate real distribution
    generateSpinesDistribution ( );

    //distributeSpinesProcedural();
  }

  void SpinesSWC::setAllParams ( unsigned int numSpines,
                                 unsigned int HorizontalResol,
                                 unsigned int VerticalResol,
                                 float minLongitudSpine,
                                 float maxLongitudSpine,
                                 float minRadio,
                                 float maxRadio )
  {
    HorResol = HorizontalResol;
    VerResol = VerticalResol;
    totalNumSpines = numSpines;
    mMinLongSpine = minLongitudSpine;
    mMaxLongSpine = maxLongitudSpine;
    mMinRadio = minRadio;
    mMaxRadio = maxRadio;
    spinesVHandleTotal = 0;
    spineGlobalDesp = ( HorResol*VerResol ) + 1;
  }

  SpinesSWC::~SpinesSWC ( )
  {
    //delete [] glb_LocalSpineVHandle;
    delete[] glb_GlobalSpinesVHandle;

    delete mSpinesSynthContainer;
  }

  void SpinesSWC::generateLocalSpine ( OpenMesh::Vec3f YDirVec, float radio )
  {
    namespace ublas = boost::numeric::ublas;

    int i, j, t;
    float lRadioBase = 0.01;

    float lValAux = (( VerResol - 2.0 )/2.0 );
    unsigned int lLimitIncX = ( int ) lValAux;

    if (( lValAux - lLimitIncX ) >= 0.49 )
      ++lLimitIncX;

    //The base of the spine
    lLimitIncX += 2;

    //Long of the Spine
    float modulo = YDirVec.length ( );

    //Inc in X edge (consecutive slices are more opened)
    float lIncX = ( radio - lRadioBase )/( VerResol );
    float lFinalIncX = 0;

    //Inc in Y edge (distance between slices)
    float lIncY = modulo/VerResol;

    //Denerate spine inside the dendrite
    //float lInsertion=-lIncY;

    //Generate the spine in the surface of the dendrite
    float lInsertion = -( lIncY*10.0/100.0 );

    ublas::vector < float > glb_AuxVec;
    glb_AuxVec.resize ( 3 );

    //Point anexed to the neuron
    //LocalSpineVContainer.push_back(MeshDef::Point(0,0,0));

    LocalSpineVContainer.clear ( );

    //Each slice
    for ( j = 0; j < VerResol; j++ )
    {
      //Incrementos parciales para las abombadas
      if ( j == 1 )
      {
        lInsertion = 0;
      }
      if (( j > 1 ) && ( j < lLimitIncX ))
        lFinalIncX += lIncX;
      else if ( j >= lLimitIncX )
        lFinalIncX -= lIncX;

      //Each vertex
      for ( i = 0; i < HorResol; i++ )
      {
        float angle = ( PI*2 )*(( float ) i/( float ) HorResol );

        //Conos
        //glb_AuxVec[0]=(radio)*(cos( angle ));
        //glb_AuxVec[1]=(modulo);
        //glb_AuxVec[2]=(radio)*(sin( angle ));

        //Crecientes
        //glb_AuxVec[0]=(lRadioBase+j*lIncX)*(cos( angle ));
        //glb_AuxVec[1]=(j*lIncY);
        ////glb_AuxVec[1]=( (j+1) * lIncY);
        //glb_AuxVec[2]=(lRadioBase+j*lIncX)*(sin( angle ));


        //Abombadas
        glb_AuxVec[0] = ( lRadioBase + lFinalIncX )*( cos ( angle ));
        glb_AuxVec[1] = ( j*lIncY ) + lInsertion;
        glb_AuxVec[2] = ( lRadioBase + lFinalIncX )*( sin ( angle ));

        LocalSpineVContainer.push_back ( MeshDef::Point ( glb_AuxVec[0], glb_AuxVec[1], glb_AuxVec[2]
                                         )
        );
      }
    }

    //Point that cover the sipine
    //LocalSpineVContainer.push_back(MeshDef::Point(0,(modulo),0));
    LocalSpineVContainer.push_back ( MeshDef::Point ( 0, ( modulo - ( modulo/( VerResol + 1.0 ))), 0 ));
  }

  void SpinesSWC::distributeSpinesModeled ( )
  {
    BaseMesh *tmpMesh;
    unsigned int lSpineModelSelected;

    unsigned int idVCandidate = 0;
    unsigned int idFCandidate = 0;

    std::vector < VertexDistances >::iterator lItV;
    std::vector < FacesDistances >::iterator lItF;

    unsigned int lNumVertAtached = 0;
    unsigned int lNumFacesAtached = 0;

    //Selection for vertex
    unsigned int numVertices = NeuronMesh->getNumVertex ( );
    unsigned int somaNumVertices = NeuronMesh->getSomaNumVertex ( );

    //Selection for faces
    unsigned int numFaces = NeuronMesh->calcNumFaces ( );
    unsigned int somaNumFaces = NeuronMesh->getSomaNumFaces ( );

    //Incremet for dispose ramdomly the spines
    float lTmpCalc = ( float ) ( numVertices - somaNumVertices )/( float ) totalNumSpines;
    unsigned int lIncRand = ( int ) lTmpCalc;
    if (( lTmpCalc - lIncRand ) > 0.49 )
      ++lIncRand;
    unsigned int lInfLimit = somaNumVertices;

    initrand ( );

    unsigned int index = 0;

    unsigned int i, j, k;

    OpenMesh::Vec3f auxPoint;
    OpenMesh::Vec3f auxNormal;

    //MeshDef::VertexHandle vhandle;

    namespace ublas = boost::numeric::ublas;

    ublas::vector < float > glb_AuxVec;
    glb_AuxVec.resize ( 4 );

    ublas::vector < float > XVector;
    XVector.resize ( 3 );
    XVector[0] = 1;
    XVector[1] = 0;
    XVector[2] = 0;

    ublas::vector < float > YVector;
    YVector.resize ( 3 );
    YVector[0] = 0;
    YVector[1] = 1;
    YVector[2] = 0;

    ublas::vector < float > glb_ZeroVec;
    glb_ZeroVec.resize ( 3 );
    glb_ZeroVec[0] = glb_ZeroVec[1] = glb_ZeroVec[2] = 0;

    boost::numeric::ublas::matrix < float > glb_mat;
    glb_mat.resize ( 4, 4 );

    boost::numeric::ublas::vector < float > glb_forwardVec;
    glb_forwardVec.resize ( 3 );

    boost::numeric::ublas::vector < float > glb_DespVec;
    glb_DespVec.resize ( 3 );

    //Auxiliar forwardvector
    boost::numeric::ublas::vector < float > glb_TransVectorAux;
    glb_TransVectorAux.resize ( 3 );

    //Right vector
    boost::numeric::ublas::vector < float > glb_RightVector;
    glb_RightVector.resize ( 3 );

    //Up vector
    boost::numeric::ublas::vector < float > glb_UpVector;
    glb_UpVector.resize ( 3 );

    NeuronMesh->freeFaceFusionCandidates ( );

    initrand ( );

    //Auxiliar vectors
    for ( i = 0; i < totalNumSpines; ++i )
    {

      //Select the spine modelled
      lSpineModelSelected = ( int ) randfloat ( 0, mSpinesModeledContainer->getContainer ( ).size ( ));

      ////Old version
      //tmpMesh				=	mSpinesModeledContainer->getElementAt(lSpineModelSelected);

      //New version
      //////!!!!!!
      tmpMesh = new BaseMesh ( );
      tmpMesh->JoinBaseMesh ( mSpinesModeledContainer->getElementAt ( lSpineModelSelected ));
      //tmpMesh->scaleBaseMesh(lSpineScale);

      MeshDef::ConstVertexFaceIter CVFIter;
      MeshDef::FaceHandle lAuxFaceHandle;
      std::vector <MeshDef::FaceHandle> vecFaceCands;
      int lFaceCand;
      float lLenght;
      OpenMesh::Vec3f lVecAB;
      float lSegPercetn;
      unsigned int lNumVertexToChoose = 0;

      //Colocaci�n de las espinas en los centros de las facetas
      //From vertex candidate, we choose his incident face
      MeshDef::VertexHandle vhandle;
      MeshDef::HalfedgeHandle hehandle;
      MeshDef::FaceHandle fhandle;

      //Selection by vertices

      MeshDef::Point lTmpVertex ( 0, 0, 0 );

      unsigned int lIniOpt;
      unsigned int lFinOpt;

      //if (mVertexDistancesContainer.size()>0)
      //{
      //	lIniOpt=2;
      //	if (mFacesDistancesContainer.size()>0)
      //	{
      //		lFinOpt=3;
      //	}
      //	else
      //	{
      //		lFinOpt=2;
      //	}
      //}
      //else
      //{
      //	if (mFacesDistancesContainer.size()>0)
      //	{
      //		lIniOpt=lFinOpt=3;
      //	}
      //	else
      //	{
      //		//Abortamos la ejecuci�n
      //		updateBaseMesh();
      //		return;
      //	}
      //}
      //unsigned int lSelection=randint(lIniOpt,lFinOpt);


      if ( mVertexDistancesContainer.size ( ) > 0 )
      {
        lIniOpt = 2;
      }
      else
        return;
      unsigned int lSelection = 2;

      switch ( lSelection )
      {
        case 0:
          //idVCandidate    = randint(somaNumVertices,numVertices);	->Problems with randint -> Use rand float and cast
          idVCandidate = ( int ) randfloat ( somaNumVertices, numVertices );

          //!!!
          //Problems with randInt
          while ( idVCandidate > numVertices )
          {
            idVCandidate = randint ( somaNumVertices, numVertices );
            //idVCandidate    = (int)randfloat(lInfLimit,(lInfLimit+lIncRand));
          }

          //Consecutive Vertex selection
          //idVCandidate    = (int)randfloat(lInfLimit,(lInfLimit+lIncRand));
          //lInfLimit+=lIncRand;

          //Selecci�n del vertice candidato
          auxPoint = NeuronMesh->getMesh ( )->points ( )[idVCandidate];
          auxNormal = NeuronMesh->getMesh ( )->vertex_normals ( )[idVCandidate];

          //From vertex to face
          //vhandle		= NeuronMesh->getMesh()->vertex_handle(idVCandidate);
          //hehandle		= NeuronMesh->getMesh()->halfedge_handle(vhandle);
          //fhandle		= NeuronMesh->getMesh()->face_handle(hehandle);*/
          break;

        case 1:
          //Selection by faces
          ////////////////////
          idFCandidate = ( int ) randfloat ( somaNumFaces, numFaces );

          //!!!
          //Problems with randInt
          while ( idFCandidate > numFaces )
          {
            idFCandidate = randint ( somaNumFaces, numFaces );
            //idFCandidate    = (int)randfloat(lInfLimit,(lInfLimit+lIncRand));
          }

          //Selecci�n de la faceta candidata
          fhandle = NeuronMesh->getMesh ( )->face_handle ( idFCandidate );

          NeuronMesh->getMesh ( )->calc_face_centroid ( fhandle, lTmpVertex );

          auxPoint = OpenMesh::Vec3f ( lTmpVertex[0], lTmpVertex[1], lTmpVertex[2] );
          auxNormal = NeuronMesh->getMesh ( )->calc_face_normal ( fhandle );

          NeuronMesh->addFaceFusionCandidate ( fhandle );
          break;

        case 2:

          //Previus
          ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


          //lNumVertAtached = mVertexDistancesContainer.size()-1;

          //idVCandidate    = (int)randfloat(0, lNumVertAtached);

          // lItV = mVertexDistancesContainer.begin() + idVCandidate;

          ////Selecci�n del vertice candidato
          //idVCandidate	= lItV->lId;
          //auxPoint		= NeuronMesh->getMesh()->points()[idVCandidate];
          //auxNormal		= NeuronMesh->getMesh()->vertex_normals()[idVCandidate];

          //mVertexDistancesContainer.erase(lItV);

          //Actual
          ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
          //idVCandidate    = randint(somaNumVertices,numVertices);	->Problems with randint -> Use rand float and cast

          lNumVertexToChoose = ( unsigned int ) randfloat ( 0, mVertexDistancesContainer.size ( ) - 1 );
          idVCandidate = ( int ) randfloat ( 0, lNumVertexToChoose );
          //idVCandidate		= mVertexDistancesContainer.at(lNumVertexToChoose);

          //Eliminamos el marcador para no volver a seleccionarlo
          lItV = mVertexDistancesContainer.begin ( ) + idVCandidate;

          ////!!!!!!
          tmpMesh->scaleBaseMesh ( mVertexDistancesContainer.at ( idVCandidate ).lDendriticRadiusFactor );

          idVCandidate = lItV->lId;

          //mVertexDistancesContainer.at(lItV).lUsed	=	true;

          //Consecutive Vertex selection
          //idVCandidate    = (int)randfloat(lInfLimit,(lInfLimit+lIncRand));
          //lInfLimit+=lIncRand;

          ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
          //Aki va la disposici�n aleatoria sobre las facetas
          //Store all the incidente vertex handles

          vhandle = NeuronMesh->getMesh ( )->vertex_handle ( idVCandidate );

          for ( CVFIter = NeuronMesh->getMesh ( )->cvf_iter ( vhandle ); CVFIter.is_valid ( ); ++CVFIter )
          {
            lAuxFaceHandle = NeuronMesh->getMesh ( )->face_handle ( CVFIter->idx ( ));
            vecFaceCands.push_back ( lAuxFaceHandle );
          }

          lFaceCand = ( int ) randfloat ( 0, vecFaceCands.size ( ) - 1 );
          fhandle = vecFaceCands.at ( lFaceCand );

          //Selecci�n de la normal de la faceta
          auxNormal = NeuronMesh->getMesh ( )->calc_face_normal ( fhandle );

          //Calculo del punto sobre el que poner la espina

          NeuronMesh->getMesh ( )->calc_face_centroid ( fhandle, lTmpVertex );
          auxPoint = NeuronMesh->getMesh ( )->points ( )[idVCandidate];

          lVecAB = OpenMesh::Vec3f (
            lTmpVertex
              - auxPoint
          );
          //Longitud desde el vertice hasta el centro de la faceta
          lLenght = lVecAB.length ( );
          lVecAB.normalize ( );

          ////Colcaci�n de la espina en la mitad del segmento
          //auxPoint		= auxPoint + lVecAB*(lLenght/2.0);

          //COlcaci�n de la espina en la mitad del segmento

          lSegPercetn = ( lLenght*DESP_PERCENT )/100.0;
          lLenght -= lSegPercetn;
          lLenght = randfloat ( lSegPercetn, lLenght );

          auxPoint = auxPoint + lVecAB*( lLenght );

          mVertexDistancesContainer.erase ( lItV );

          break;

        case 3:

          //Selection by faces
          ////////////////////

          lNumFacesAtached = mFacesDistancesContainer.size ( ) - 1;

          idFCandidate = ( int ) randfloat ( 0, lNumFacesAtached );

          //!!!
          //Problems with randInt
          while ( idFCandidate > lNumFacesAtached )
          {
            idFCandidate = ( int ) randfloat ( 0, lNumFacesAtached );
            //idFCandidate    = (int)randfloat(lInfLimit,(lInfLimit+lIncRand));
          }

          //Selecci�n de la faceta candidata
          lItF = mFacesDistancesContainer.begin ( ) + idFCandidate;

          //Selecci�n del vertice candidato
          idFCandidate = lItF->lId;
          fhandle = NeuronMesh->getMesh ( )->face_handle ( idFCandidate );

          NeuronMesh->getMesh ( )->calc_face_centroid ( fhandle, lTmpVertex );

          auxPoint = OpenMesh::Vec3f ( lTmpVertex[0], lTmpVertex[1], lTmpVertex[2] );
          auxNormal = NeuronMesh->getMesh ( )->calc_face_normal ( fhandle );

          //NeuronMesh->addFaceFusionCandidate(fhandle);

          mFacesDistancesContainer.erase ( lItF );

          break;

      }
      /////////////////////////////////////////////////////////////////////////////////////////////////////////

      //Codigo comun para posicionamiento por vertice y por faceta
      /////////////////////////////////////////////////////////////////////////////////////////////////////////
      glb_direction = ( auxPoint + auxNormal*mMaxLongSpine ) - auxPoint;

      //Orientaci�n seg�n la normal
      glb_forwardVec[0] = glb_direction[0];
      glb_forwardVec[1] = glb_direction[1];
      glb_forwardVec[2] = glb_direction[2];

      //Giro de la dendrita para no alinearla exatamente con la normal
      //glb_forwardVec[0] = glb_direction[0]+randfloat(-mMaxLongSpine, mMaxLongSpine);
      //glb_forwardVec[1] = glb_direction[1];
      //glb_forwardVec[2] = glb_direction[2]+randfloat(-mMaxLongSpine, mMaxLongSpine);

      //Montaje de la matriz
      generateTransformationMatrix ( glb_mat,
                                     glb_DespVec,
                                     glb_forwardVec,
                                     glb_TransVectorAux,
                                     glb_RightVector,
                                     glb_UpVector,
                                     XVector,
                                     YVector
      );

      //Unimos la malla y calculamos los vertices antes y despues
      unsigned int lIniLimit = this->getNumVertex ( );
      JoinBaseMesh ( tmpMesh );
      unsigned int lFinLimit = this->getNumVertex ( );

      //All the vertices are ready now
      for ( j = ( lIniLimit ); j < ( lFinLimit ); ++j )
      {

        //Hay q coger el precomputado
        glb_AuxVec[0] = Mesh->points ( )[j][0];
        glb_AuxVec[1] = Mesh->points ( )[j][1];
        glb_AuxVec[2] = Mesh->points ( )[j][2];
        glb_AuxVec[3] = 1.0;

        //Matrix operations
        glb_AuxVec = prod ( glb_mat, glb_AuxVec );

        //Explicit translate
        //Muchmore faster than apply the translate matrix
        glb_AuxVec[0] += auxPoint[0];
        glb_AuxVec[1] += auxPoint[1];
        glb_AuxVec[2] += auxPoint[2];

        //Mesh->points()[j] = (MeshDef::Point(glb_AuxVec[0]
        //									,glb_AuxVec[1]
        //									,glb_AuxVec[2]
        //								  )
        //					);

        MeshDef::VertexHandle vhandle = Mesh->vertex_handle ( j );
        Mesh->set_point ( vhandle, ( MeshDef::Point ( glb_AuxVec[0], glb_AuxVec[1], glb_AuxVec[2]
                          )
                          )
        );
      }

      //NeuronMesh->addFaceFusionCandidate(fhandle);
      ////!!!!!!
      delete tmpMesh;

    }
    updateBaseMesh ( );
  }

  void SpinesSWC::distributeSpinesProcedural ( )
  {
    BaseMesh *tmpMesh;

    unsigned int idVCandidate = 0;
    unsigned int idFCandidate = 0;

    std::vector < VertexDistances >::iterator lItV;
    std::vector < FacesDistances >::iterator lItF;

    unsigned int lNumVertAtached = 0;
    unsigned int lNumFacesAtached = 0;

    //Selection for vertex
    unsigned int numVertices = NeuronMesh->getNumVertex ( );
    unsigned int somaNumVertices = NeuronMesh->getSomaNumVertex ( );

    //Selection for faces
    unsigned int numFaces = NeuronMesh->calcNumFaces ( );
    unsigned int somaNumFaces = NeuronMesh->getSomaNumFaces ( );

    //Incremet for dispose ramdomly the spines
    float lTmpCalc = ( float ) ( numVertices - somaNumVertices )/( float ) totalNumSpines;
    unsigned int lIncRand = ( int ) lTmpCalc;
    if (( lTmpCalc - lIncRand ) > 0.49 )
      ++lIncRand;
    unsigned int lInfLimit = somaNumVertices;

    //initrand();

    unsigned int index = 0;

    unsigned int i, j, k;

    OpenMesh::Vec3f auxPoint;
    OpenMesh::Vec3f auxNormal;

    //MeshDef::VertexHandle vhandle;

    namespace ublas = boost::numeric::ublas;

    ublas::vector < float > glb_AuxVec;
    glb_AuxVec.resize ( 4 );

    ublas::vector < float > XVector;
    XVector.resize ( 3 );
    XVector[0] = 1;
    XVector[1] = 0;
    XVector[2] = 0;

    ublas::vector < float > YVector;
    YVector.resize ( 3 );
    YVector[0] = 0;
    YVector[1] = 1;
    YVector[2] = 0;

    ublas::vector < float > glb_ZeroVec;
    glb_ZeroVec.resize ( 3 );
    glb_ZeroVec[0] = glb_ZeroVec[1] = glb_ZeroVec[2] = 0;

    boost::numeric::ublas::matrix < float > glb_mat;
    glb_mat.resize ( 4, 4 );

    boost::numeric::ublas::vector < float > glb_forwardVec;
    glb_forwardVec.resize ( 3 );

    boost::numeric::ublas::vector < float > glb_DespVec;
    glb_DespVec.resize ( 3 );

    //Auxiliar forwardvector
    boost::numeric::ublas::vector < float > glb_TransVectorAux;
    glb_TransVectorAux.resize ( 3 );

    //Right vector
    boost::numeric::ublas::vector < float > glb_RightVector;
    glb_RightVector.resize ( 3 );

    //Up vector
    boost::numeric::ublas::vector < float > glb_UpVector;
    glb_UpVector.resize ( 3 );

    NeuronMesh->freeFaceFusionCandidates ( );

    //Auxiliar vectors
    for ( i = 0; i < totalNumSpines; ++i )
    {
      //Generate diferent Spines
      generateLocalSpine ( OpenMesh::Vec3f ( 0, randfloat ( mMinLongSpine, mMaxLongSpine ), 0 ),
                           randfloat ( mMinRadio, mMaxRadio ));
      //generateLocalSpine(OpenMesh::Vec3f(randfloat(-mMinLongSpine, mMinLongSpine),randfloat(mMinLongSpine, mMaxLongSpine),randfloat(-mMinLongSpine, mMinLongSpine) )
      //									, randfloat(mMinRadio, mMaxRadio));


      //Colocaci�n de las espinas en los centros de las facetas
      //From vertex candidate, we choose his incident face
      MeshDef::VertexHandle vhandle;
      MeshDef::HalfedgeHandle hehandle;
      MeshDef::FaceHandle fhandle;

      //Selection by vertices

      MeshDef::Point lTmpVertex ( 0, 0, 0 );

      unsigned int lIniOpt;
      unsigned int lFinOpt;

      //if (mVertexDistancesContainer.size()>0)
      //{
      //	lIniOpt=2;
      //	if (mFacesDistancesContainer.size()>0)
      //	{
      //		lFinOpt=3;
      //	}
      //	else
      //	{
      //		lFinOpt=2;
      //	}
      //}
      //else
      //{
      //	if (mFacesDistancesContainer.size()>0)
      //	{
      //		lIniOpt=lFinOpt=3;
      //	}
      //	else
      //	{
      //		//Abortamos la ejecuci�n
      //		updateBaseMesh();
      //		return;
      //	}
      //}
      //unsigned int lSelection=randint(lIniOpt,lFinOpt);

      unsigned int lSelection;
      if ( mVertexDistancesContainer.size ( ) > 0 )
      {
        lSelection = 2;
      }
      else
        return;

      switch ( lSelection )
      {
        case 0:
          //idVCandidate    = randint(somaNumVertices,numVertices);	->Problems with randint -> Use rand float and cast
          idVCandidate = ( int ) randfloat ( somaNumVertices, numVertices );

          //!!!
          //Problems with randInt
          while ( idVCandidate > numVertices )
          {
            idVCandidate = randint ( somaNumVertices, numVertices );
            //idVCandidate    = (int)randfloat(lInfLimit,(lInfLimit+lIncRand));
          }

          //Consecutive Vertex selection
          //idVCandidate    = (int)randfloat(lInfLimit,(lInfLimit+lIncRand));
          //lInfLimit+=lIncRand;

          //Selecci�n del vertice candidato
          auxPoint = NeuronMesh->getMesh ( )->points ( )[idVCandidate];
          auxNormal = NeuronMesh->getMesh ( )->vertex_normals ( )[idVCandidate];

          //From vertex to face
          //vhandle		= NeuronMesh->getMesh()->vertex_handle(idVCandidate);
          //hehandle		= NeuronMesh->getMesh()->halfedge_handle(vhandle);
          //fhandle		= NeuronMesh->getMesh()->face_handle(hehandle);*/
          break;

        case 1:
          //Selection by faces
          ////////////////////
          idFCandidate = ( int ) randfloat ( somaNumFaces, numFaces );

          //!!!
          //Problems with randInt
          while ( idFCandidate > numFaces )
          {
            idFCandidate = randint ( somaNumFaces, numFaces );
            //idFCandidate    = (int)randfloat(lInfLimit,(lInfLimit+lIncRand));
          }

          //Selecci�n de la faceta candidata
          fhandle = NeuronMesh->getMesh ( )->face_handle ( idFCandidate );

          NeuronMesh->getMesh ( )->calc_face_centroid ( fhandle, lTmpVertex );

          auxPoint = OpenMesh::Vec3f ( lTmpVertex[0], lTmpVertex[1], lTmpVertex[2] );
          auxNormal = NeuronMesh->getMesh ( )->calc_face_normal ( fhandle );

          NeuronMesh->addFaceFusionCandidate ( fhandle );
          break;

        case 2:

          lNumVertAtached = mVertexDistancesContainer.size ( ) - 1;

          idVCandidate = ( int ) randfloat ( 0, lNumVertAtached );

          lItV = mVertexDistancesContainer.begin ( ) + idVCandidate;

          //Selecci�n del vertice candidato
          idVCandidate = lItV->lId;
          auxPoint = NeuronMesh->getMesh ( )->points ( )[idVCandidate];
          auxNormal = NeuronMesh->getMesh ( )->vertex_normals ( )[idVCandidate];

          mVertexDistancesContainer.erase ( lItV );

          //From vertex to face
          //vhandle		= NeuronMesh->getMesh()->vertex_handle(idVCandidate);
          //hehandle		= NeuronMesh->getMesh()->halfedge_handle(vhandle);
          //fhandle		= NeuronMesh->getMesh()->face_handle(hehandle);*/
          break;

        case 3:

          //Selection by faces
          ////////////////////

          lNumFacesAtached = mFacesDistancesContainer.size ( ) - 1;

          idFCandidate = ( int ) randfloat ( 0, lNumFacesAtached );

          //!!!
          //Problems with randInt
          while ( idFCandidate > lNumFacesAtached )
          {
            idFCandidate = ( int ) randfloat ( 0, lNumFacesAtached );
            //idFCandidate    = (int)randfloat(lInfLimit,(lInfLimit+lIncRand));
          }

          //Selecci�n de la faceta candidata
          lItF = mFacesDistancesContainer.begin ( ) + idFCandidate;

          //Selecci�n del vertice candidato
          idFCandidate = lItF->lId;
          fhandle = NeuronMesh->getMesh ( )->face_handle ( idFCandidate );

          NeuronMesh->getMesh ( )->calc_face_centroid ( fhandle, lTmpVertex );

          auxPoint = OpenMesh::Vec3f ( lTmpVertex[0], lTmpVertex[1], lTmpVertex[2] );
          auxNormal = NeuronMesh->getMesh ( )->calc_face_normal ( fhandle );

          //NeuronMesh->addFaceFusionCandidate(fhandle);

          mFacesDistancesContainer.erase ( lItF );

          break;

      }
      /////////////////////////////////////////////////////////////////////////////////////////////////////////

      //Codigo comun para posicionamiento por vertice y por faceta
      /////////////////////////////////////////////////////////////////////////////////////////////////////////
      glb_direction = ( auxPoint + auxNormal*mMaxLongSpine ) - auxPoint;

      //Orientaci�n seg�n la normal
      glb_forwardVec[0] = glb_direction[0];
      glb_forwardVec[1] = glb_direction[1];
      glb_forwardVec[2] = glb_direction[2];

      //Giro de la dendrita para no alinearla exatamente con la normal
      //glb_forwardVec[0] = glb_direction[0]+randfloat(-mMaxLongSpine, mMaxLongSpine);
      //glb_forwardVec[1] = glb_direction[1];
      //glb_forwardVec[2] = glb_direction[2]+randfloat(-mMaxLongSpine, mMaxLongSpine);

      //Montaje de la matriz
      generateTransformationMatrix ( glb_mat,
                                     glb_DespVec,
                                     glb_forwardVec,
                                     glb_TransVectorAux,
                                     glb_RightVector,
                                     glb_UpVector,
                                     XVector,
                                     YVector
      );


      //Procedural Spines

      //All the vertices are ready now
      for ( j = 0; j < ( spineGlobalDesp ); ++j )
      {

        //Hay q coger el precomputado
        glb_AuxVec[0] = LocalSpineVContainer[j][0];
        glb_AuxVec[1] = LocalSpineVContainer[j][1];
        glb_AuxVec[2] = LocalSpineVContainer[j][2];
        glb_AuxVec[3] = 1.0;

        //Matrix operations
        glb_AuxVec = prod ( glb_mat, glb_AuxVec );

        //Explicit translate
        //Muchmore faster than apply the translate matrix
        glb_AuxVec[0] += auxPoint[0];
        glb_AuxVec[1] += auxPoint[1];
        glb_AuxVec[2] += auxPoint[2];

        glb_GlobalSpinesVHandle[spinesVHandleTotal] =
          Mesh->add_vertex ( MeshDef::Point ( glb_AuxVec[0], glb_AuxVec[1], glb_AuxVec[2]
                             )
          );
        spinesVHandleTotal++;
      }

      //Costura del cuerpo de la espina
      int t = 0;
      for ( j = 0; j < VerResol - 1; j++ )
      {
        for ( t = 0; t < HorResol - 1; t++ )
        {
          glb_spineFace_VHandles.clear ( );

          index = (( j )*HorResol + t );
          index += i*spineGlobalDesp;
          glb_spineFace_VHandles.push_back ( glb_GlobalSpinesVHandle[index] );

          index = (( j + 1 )*HorResol + t + 1 );
          index += i*spineGlobalDesp;
          glb_spineFace_VHandles.push_back ( glb_GlobalSpinesVHandle[index] );

          index = (( j )*HorResol + t + 1 );
          index += i*spineGlobalDesp;
          glb_spineFace_VHandles.push_back ( glb_GlobalSpinesVHandle[index] );

          Mesh->add_face ( glb_spineFace_VHandles );

          glb_spineFace_VHandles.clear ( );

          index = (( j )*HorResol + t );
          index += i*spineGlobalDesp;
          glb_spineFace_VHandles.push_back ( glb_GlobalSpinesVHandle[index] );

          index = (( j + 1 )*HorResol + t );
          index += i*spineGlobalDesp;
          glb_spineFace_VHandles.push_back ( glb_GlobalSpinesVHandle[index] );

          index = (( j + 1 )*HorResol + t + 1 );
          index += i*spineGlobalDesp;
          glb_spineFace_VHandles.push_back ( glb_GlobalSpinesVHandle[index] );
          Mesh->add_face ( glb_spineFace_VHandles );
        }

        ////Last two faces
        glb_spineFace_VHandles.clear ( );

        index = ( j*HorResol + t );
        index += i*spineGlobalDesp;
        glb_spineFace_VHandles.push_back ( glb_GlobalSpinesVHandle[index] );

        index = ( j*HorResol + t + 1 );
        index += i*spineGlobalDesp;
        glb_spineFace_VHandles.push_back ( glb_GlobalSpinesVHandle[index] );

        index = ( j*HorResol );
        index += i*spineGlobalDesp;
        glb_spineFace_VHandles.push_back ( glb_GlobalSpinesVHandle[index] );
        Mesh->add_face ( glb_spineFace_VHandles );

        glb_spineFace_VHandles.clear ( );

        index = ( j*HorResol + t );
        index += i*spineGlobalDesp;
        glb_spineFace_VHandles.push_back ( glb_GlobalSpinesVHandle[index] );

        index = (( j + 1 )*HorResol + t );
        index += i*spineGlobalDesp;
        glb_spineFace_VHandles.push_back ( glb_GlobalSpinesVHandle[index] );

        index = ( j*HorResol + t + 1 );
        index += i*spineGlobalDesp;
        glb_spineFace_VHandles.push_back ( glb_GlobalSpinesVHandle[index] );
        Mesh->add_face ( glb_spineFace_VHandles );

      }

      //Tapa de la espina
      for ( j = 0; j < ( HorResol - 1 ); ++j )
      {
        //Superior circle
        glb_spineFace_VHandles.clear ( );

        index = (( i*spineGlobalDesp ) + ( spineGlobalDesp - HorResol - 2 ) + j + 2 );
        glb_spineFace_VHandles.push_back ( glb_GlobalSpinesVHandle[index] );

        index = (( i*spineGlobalDesp ) + ( spineGlobalDesp - HorResol - 2 ) + j + 1 );
        glb_spineFace_VHandles.push_back ( glb_GlobalSpinesVHandle[index] );

        index = ( i*spineGlobalDesp );
        index += spineGlobalDesp - 1;
        glb_spineFace_VHandles.push_back ( glb_GlobalSpinesVHandle[index] );

        Mesh->add_face ( glb_spineFace_VHandles );
      }

      glb_spineFace_VHandles.clear ( );

      index = (( i*spineGlobalDesp ) + ( spineGlobalDesp - HorResol - 2 ) + 1 );
      glb_spineFace_VHandles.push_back ( glb_GlobalSpinesVHandle[index] );

      index = (( i*spineGlobalDesp ) + ( spineGlobalDesp - HorResol - 2 ) + j + 1 );
      glb_spineFace_VHandles.push_back ( glb_GlobalSpinesVHandle[index] );

      index = ( i*spineGlobalDesp );
      index += spineGlobalDesp - 1;
      glb_spineFace_VHandles.push_back ( glb_GlobalSpinesVHandle[index] );

      Mesh->add_face ( glb_spineFace_VHandles );
    }
    updateBaseMesh ( );
  }

  void SpinesSWC::distributeSpineAlongVertexs ( unsigned int pNumSpines,
                                                std::vector < unsigned int > pVertexSegmentsCandidate,
                                                float pSpinesPerVertex )
  {

    BaseMesh *tmpMesh;

    std::vector < int > lSpinesIds;
    float lSpineTypeProb;
    unsigned int lNumVertices = 0;

    unsigned int lNumSpinesModels = mSpinesModeledContainer->getContainer ( ).size ( ) - 1;
    unsigned int lModelSpine = 0;
    unsigned int lNumModeledSpinesAdded = 0;
    unsigned int lNumProceduralSpines = 0;

    if ( glb_GlobalSpinesVHandle != NULL )
      delete glb_GlobalSpinesVHandle;

    initrand ( );

    //Calc total number of vertex
    lNumProceduralSpines = 0;//(unsigned int) randfloat(0, pNumSpines);

    //for (int i=0;i<lNumProceduralSpines;++i)
    for ( int i = 0; i < pNumSpines; ++i )
    {
      lModelSpine = ( unsigned int ) randfloat ( 0, lNumSpinesModels );
      lSpinesIds.push_back ( lModelSpine );
      //lNumVertices	+=	mSpinesModeledContainer->getElementAt(lModelSpine)->getNumVertex();

    }

    ////for (int i=lNumProceduralSpines;i<pNumSpines;++i)
    //for (int i=0;i<pNumSpines;++i)
    //{

    //	lNumVertices	+=	spineGlobalDesp;
    //	lSpinesIds.push_back(-1);

    //}

    glb_GlobalSpinesVHandle = new MeshDef::VertexHandle[lNumVertices];
    spinesVHandleTotal = 0;

    //!!!
    //->Mezclar espinas procedurales y modeladas
    //El problema radica ak�, al hacerse espinas procedurales y modeladas, lo indices no concidir�n ...
    //Lo mejor es hacer un vector de ramdoms donde se elja si es procedural (-1) o modelada, y si es modelada seleccionar el modelo
    //calcular sus vertices e ir sumando para hacer la reserva d ememoria total desde le principio
    //Luego en la saintesism en lugar de hacer los ramdoms recorrer ese vector

    //->Cambiar forma de distribuir las espinas
    //Recorrer las face3tas adyacentes y calcular una distancia de desplazamiento desde el vertice hasta el centro de la faceta (elijiendo una faceta aleatoria)
    //Y posicionar esa espina en a una distancia aleatoria de entre el vertice y el centro de la faceta, utilizar proporci�n de espinas por faceta

    //->No coincidencia
    //cada vez q se elija un vertice y se procese mediante lo anterir, marcarlo en el vector de vertices como usado y no volver a cogerlo en la elecci�n de vertices candidatos

    //Generaci�n de varios grupos de espinas
    //Recorrer varias veces la generaci�n de espinas por dendritas ... tantas como grupos hayamos decidid y reducir la proporci�n de espinas por al n�mero de grupos

    //Costura d elas dendritas
    //Utilizar una costura como la del soma, atacar un vertice, almacaner los esteriores, eliminar la faceta y coser
    //Metodo adicional para acercarlos (es necesario almacenar los vertices para calcular varicentro y acercarlos)

    unsigned int idVCandidate = 0;
    unsigned int idFCandidate = 0;

    std::vector < VertexDistances >::iterator lItV;
    std::vector < FacesDistances >::iterator lItF;

    unsigned int lNumVertAtached = 0;
    unsigned int lNumFacesAtached = 0;

    //Selection for vertex
    unsigned int numVertices = NeuronMesh->getNumVertex ( );
    unsigned int somaNumVertices = NeuronMesh->getSomaNumVertex ( );

    //Selection for faces
    unsigned int numFaces = NeuronMesh->calcNumFaces ( );
    unsigned int somaNumFaces = NeuronMesh->getSomaNumFaces ( );

    unsigned int index = 0;

    unsigned int i, j, k;

    OpenMesh::Vec3f auxPoint;
    OpenMesh::Vec3f auxNormal;

    //MeshDef::VertexHandle vhandle;

    //Begin: Prepare Matrix paramas generation
    //////////////////////////////////////////
    namespace ublas = boost::numeric::ublas;

    ublas::vector < float > glb_AuxVec;
    glb_AuxVec.resize ( 4 );

    ublas::vector < float > XVector;
    XVector.resize ( 3 );
    XVector[0] = 1;
    XVector[1] = 0;
    XVector[2] = 0;

    ublas::vector < float > YVector;
    YVector.resize ( 3 );
    YVector[0] = 0;
    YVector[1] = 1;
    YVector[2] = 0;

    ublas::vector < float > glb_ZeroVec;
    glb_ZeroVec.resize ( 3 );
    glb_ZeroVec[0] = glb_ZeroVec[1] = glb_ZeroVec[2] = 0;

    boost::numeric::ublas::matrix < float > glb_mat;
    glb_mat.resize ( 4, 4 );

    boost::numeric::ublas::vector < float > glb_forwardVec;
    glb_forwardVec.resize ( 3 );

    boost::numeric::ublas::vector < float > glb_DespVec;
    glb_DespVec.resize ( 3 );

    //Auxiliar forwardvector
    boost::numeric::ublas::vector < float > glb_TransVectorAux;
    glb_TransVectorAux.resize ( 3 );

    //Right vector
    boost::numeric::ublas::vector < float > glb_RightVector;
    glb_RightVector.resize ( 3 );

    //Up vector
    boost::numeric::ublas::vector < float > glb_UpVector;
    glb_UpVector.resize ( 3 );

    //End: Prepare Matrix paramas generation
    ////////////////////////////////////////

    //initrand();
    unsigned int lLimit = pNumSpines;

    if ( lLimit > pVertexSegmentsCandidate.size ( ))
    {
      lLimit = pVertexSegmentsCandidate.size ( );
    }

    std::vector < unsigned int >::iterator lIt;

    for ( i = 0; i < lLimit; ++i )
      //for (i=0;i<10;++i)
    {

      //Colocaci�n de las espinas en los centros de las facetas
      //From vertex candidate, we choose his incident face
      MeshDef::VertexHandle vhandle;
      MeshDef::HalfedgeHandle hehandle;
      MeshDef::FaceHandle fhandle;

      MeshDef::ConstVertexFaceIter CVFIter;
      MeshDef::FaceHandle lAuxFaceHandle;
      std::vector <MeshDef::FaceHandle> vecFaceCands;
      int lFaceCand;
      float lLenght;
      OpenMesh::Vec3f lVecAB;
      float lSegPercetn;

      //Selection by vertices

      MeshDef::Point lTmpVertex ( 0, 0, 0 );

      unsigned int lIniOpt;
      unsigned int lFinOpt;

      //Si tenemos m�s espinas que v�rtices no hay problema, cada espina a un vertice
      //si no d emomento pasamos

      /*
				0	-> Only selection by vertexs (no erase)
				1	-> Circulate arrond vertexs		(no erase)
				2	-> Only by vertexs (erase from container)
				3)	-> Circulate arround vertexs (mark the ponits choosed)

			*/
      unsigned int lSelection = 0;

      //if (pNumSpines<pVertexSegmentsCandidate.size())
      //{
      //	lSelection = 0;
      //}
      //else
      //{
      //	//return;
      //}

      lIniOpt = 0;
      lFinOpt = pVertexSegmentsCandidate.size ( ) - 1;

      //unsigned int lSelection = 3;

      unsigned int lNumVertexToChoose;
      switch ( lSelection )
      {
        case 0:
          //idVCandidate    = randint(somaNumVertices,numVertices);	->Problems with randint -> Use rand float and cast

          lNumVertexToChoose = ( unsigned int ) randfloat ( lIniOpt, lFinOpt );
          //idVCandidate		= (int)randfloat(0,lNumVertexToChoose);
          idVCandidate = pVertexSegmentsCandidate.at ( lNumVertexToChoose );

          //Eliminamos el marcador para no volver a seleccionarlo
          lIt = pVertexSegmentsCandidate.begin ( ) + lNumVertexToChoose;
          pVertexSegmentsCandidate.erase ( lIt );

          mVertexDistancesContainer.at ( idVCandidate ).lUsed = true;

          //Consecutive Vertex selection
          //idVCandidate    = (int)randfloat(lInfLimit,(lInfLimit+lIncRand));
          //lInfLimit+=lIncRand;

          ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
          //Aki va la disposici�n aleatoria sobre las facetas
          //Store all the incidente vertex handles

          vhandle = NeuronMesh->getMesh ( )->vertex_handle ( idVCandidate );

          for ( CVFIter = NeuronMesh->getMesh ( )->cvf_iter ( vhandle ); CVFIter.is_valid ( ); ++CVFIter )
          {
            lAuxFaceHandle = NeuronMesh->getMesh ( )->face_handle ( CVFIter->idx ( ));
            vecFaceCands.push_back ( lAuxFaceHandle );
          }

          lFaceCand = ( int ) randfloat ( 0, vecFaceCands.size ( ) - 1 );
          fhandle = vecFaceCands.at ( lFaceCand );

          //Selecci�n de la normal de la faceta
          auxNormal = NeuronMesh->getMesh ( )->calc_face_normal ( fhandle );

          //Calculo del punto sobre el que poner la espina

          NeuronMesh->getMesh ( )->calc_face_centroid ( fhandle, lTmpVertex );
          auxPoint = NeuronMesh->getMesh ( )->points ( )[idVCandidate];

          lVecAB = OpenMesh::Vec3f (
            lTmpVertex
              - auxPoint
          );
          //Longitud desde el vertice hasta el centro de la faceta
          lLenght = lVecAB.length ( );
          lVecAB.normalize ( );

          ////Colcaci�n de la espina en la mitad del segmento
          //auxPoint		= auxPoint + lVecAB*(lLenght/2.0);

          //COlcaci�n de la espina en la mitad del segmento

          lSegPercetn = ( lLenght*DESP_PERCENT )/100.0;
          lLenght -= lSegPercetn;
          lLenght = randfloat ( lSegPercetn, lLenght );

          auxPoint = auxPoint + lVecAB*( lLenght );




          ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

          ////Selecci�n del vertice candidato
          //auxPoint		= NeuronMesh->getMesh()->points()[idVCandidate];
          //auxNormal		= NeuronMesh->getMesh()->vertex_normals()[idVCandidate];

          //From vertex to face
          //vhandle		= NeuronMesh->getMesh()->vertex_handle(idVCandidate);
          //hehandle		= NeuronMesh->getMesh()->halfedge_handle(vhandle);
          //fhandle		= NeuronMesh->getMesh()->face_handle(hehandle);*/
          break;

        case 1:
          //Selection by faces
          ////////////////////
          idFCandidate = ( int ) randfloat ( somaNumFaces, numFaces );

          //!!!
          //Problems with randInt
          while ( idFCandidate > numFaces )
          {
            idFCandidate = randint ( somaNumFaces, numFaces );
            //idFCandidate    = (int)randfloat(lInfLimit,(lInfLimit+lIncRand));
          }

          //Selecci�n de la faceta candidata
          fhandle = NeuronMesh->getMesh ( )->face_handle ( idFCandidate );

          NeuronMesh->getMesh ( )->calc_face_centroid ( fhandle, lTmpVertex );

          auxPoint = OpenMesh::Vec3f ( lTmpVertex[0], lTmpVertex[1], lTmpVertex[2] );
          auxNormal = NeuronMesh->getMesh ( )->calc_face_normal ( fhandle );

          NeuronMesh->addFaceFusionCandidate ( fhandle );
          break;

        case 2:

          lNumVertAtached = mVertexDistancesContainer.size ( ) - 1;

          idVCandidate = ( int ) randfloat ( 0, lNumVertAtached );

          lItV = mVertexDistancesContainer.begin ( ) + idVCandidate;

          //Selecci�n del vertice candidato
          idVCandidate = lItV->lId;
          auxPoint = NeuronMesh->getMesh ( )->points ( )[idVCandidate];
          auxNormal = NeuronMesh->getMesh ( )->vertex_normals ( )[idVCandidate];

          mVertexDistancesContainer.erase ( lItV );

          //From vertex to face
          //vhandle		= NeuronMesh->getMesh()->vertex_handle(idVCandidate);
          //hehandle		= NeuronMesh->getMesh()->halfedge_handle(vhandle);
          //fhandle		= NeuronMesh->getMesh()->face_handle(hehandle);*/
          break;

        case 3:

          //Selection by faces
          ////////////////////

          lNumFacesAtached = mFacesDistancesContainer.size ( ) - 1;

          idFCandidate = ( int ) randfloat ( 0, lNumFacesAtached );

          //!!!
          //Problems with randInt
          while ( idFCandidate > lNumFacesAtached )
          {
            idFCandidate = ( int ) randfloat ( 0, lNumFacesAtached );
            //idFCandidate    = (int)randfloat(lInfLimit,(lInfLimit+lIncRand));
          }

          //Selecci�n de la faceta candidata
          lItF = mFacesDistancesContainer.begin ( ) + idFCandidate;

          //Selecci�n del vertice candidato
          idFCandidate = lItF->lId;
          fhandle = NeuronMesh->getMesh ( )->face_handle ( idFCandidate );

          NeuronMesh->getMesh ( )->calc_face_centroid ( fhandle, lTmpVertex );

          auxPoint = OpenMesh::Vec3f ( lTmpVertex[0], lTmpVertex[1], lTmpVertex[2] );
          auxNormal = NeuronMesh->getMesh ( )->calc_face_normal ( fhandle );

          //NeuronMesh->addFaceFusionCandidate(fhandle);

          mFacesDistancesContainer.erase ( lItF );

          break;

      }
      /////////////////////////////////////////////////////////////////////////////////////////////////////////

      //Codigo comun para posicionamiento por vertice y por faceta
      /////////////////////////////////////////////////////////////////////////////////////////////////////////
      glb_direction = ( auxPoint + auxNormal*mMaxLongSpine ) - auxPoint;

      //Orientaci�n seg�n la normal
      glb_forwardVec[0] = glb_direction[0];
      glb_forwardVec[1] = glb_direction[1];
      glb_forwardVec[2] = glb_direction[2];

      //Giro de la dendrita para no alinearla exatamente con la normal
      //glb_forwardVec[0] = glb_direction[0]+randfloat(-mMaxLongSpine, mMaxLongSpine);
      //glb_forwardVec[1] = glb_direction[1];
      //glb_forwardVec[2] = glb_direction[2]+randfloat(-mMaxLongSpine, mMaxLongSpine);

      //Montaje de la matriz
      generateTransformationMatrix ( glb_mat,
                                     glb_DespVec,
                                     glb_forwardVec,
                                     glb_TransVectorAux,
                                     glb_RightVector,
                                     glb_UpVector,
                                     XVector,
                                     YVector
      );

      float lSpineScale = randfloat ( 0.25, 1 );
      unsigned int lModelSpine = lSpinesIds[i];

      //if (lSpineType>=0.5)
      if ( lModelSpine == -1 )
        //if (false)
        //if (true)
      {

        //Generate diferent Spines
        generateLocalSpine ( OpenMesh::Vec3f ( 0, randfloat ( mMinLongSpine, mMaxLongSpine ), 0 ),
                             randfloat ( mMinRadio, mMaxRadio ));

        //All the vertices are ready now
        for ( j = 0; j < ( spineGlobalDesp ); ++j )
        {

          //Hay q coger el precomputado
          glb_AuxVec[0] = LocalSpineVContainer[j][0];
          glb_AuxVec[1] = LocalSpineVContainer[j][1];
          glb_AuxVec[2] = LocalSpineVContainer[j][2];
          glb_AuxVec[3] = 1.0;

          //Matrix operations
          glb_AuxVec = prod ( glb_mat, glb_AuxVec );

          //Explicit translate
          //Muchmore faster than apply the translate matrix
          glb_AuxVec[0] += auxPoint[0];
          glb_AuxVec[1] += auxPoint[1];
          glb_AuxVec[2] += auxPoint[2];

          glb_GlobalSpinesVHandle[spinesVHandleTotal] =
            Mesh->add_vertex ( MeshDef::Point ( glb_AuxVec[0], glb_AuxVec[1], glb_AuxVec[2]
                               )
            );
          spinesVHandleTotal++;
        }

        //Costura del cuerpo de la espina
        int t = 0;
        for ( j = 0; j < VerResol - 1; j++ )
        {
          for ( t = 0; t < HorResol - 1; t++ )
          {
            glb_spineFace_VHandles.clear ( );

            index = (( j )*HorResol + t );
            index += ( i - lNumModeledSpinesAdded )*spineGlobalDesp;
            glb_spineFace_VHandles.push_back ( glb_GlobalSpinesVHandle[index] );

            index = (( j + 1 )*HorResol + t + 1 );
            index += ( i - lNumModeledSpinesAdded )*spineGlobalDesp;
            glb_spineFace_VHandles.push_back ( glb_GlobalSpinesVHandle[index] );

            index = (( j )*HorResol + t + 1 );
            index += ( i - lNumModeledSpinesAdded )*spineGlobalDesp;
            glb_spineFace_VHandles.push_back ( glb_GlobalSpinesVHandle[index] );

            Mesh->add_face ( glb_spineFace_VHandles );

            glb_spineFace_VHandles.clear ( );

            index = (( j )*HorResol + t );
            index += ( i - lNumModeledSpinesAdded )*spineGlobalDesp;
            glb_spineFace_VHandles.push_back ( glb_GlobalSpinesVHandle[index] );

            index = (( j + 1 )*HorResol + t );
            index += ( i - lNumModeledSpinesAdded )*spineGlobalDesp;
            glb_spineFace_VHandles.push_back ( glb_GlobalSpinesVHandle[index] );

            index = (( j + 1 )*HorResol + t + 1 );
            index += ( i - lNumModeledSpinesAdded )*spineGlobalDesp;
            glb_spineFace_VHandles.push_back ( glb_GlobalSpinesVHandle[index] );
            Mesh->add_face ( glb_spineFace_VHandles );
          }

          ////Last two faces
          glb_spineFace_VHandles.clear ( );

          index = ( j*HorResol + t );
          index += ( i - lNumModeledSpinesAdded )*spineGlobalDesp;
          glb_spineFace_VHandles.push_back ( glb_GlobalSpinesVHandle[index] );

          index = ( j*HorResol + t + 1 );
          index += ( i - lNumModeledSpinesAdded )*spineGlobalDesp;
          glb_spineFace_VHandles.push_back ( glb_GlobalSpinesVHandle[index] );

          index = ( j*HorResol );
          index += ( i - lNumModeledSpinesAdded )*spineGlobalDesp;
          glb_spineFace_VHandles.push_back ( glb_GlobalSpinesVHandle[index] );
          Mesh->add_face ( glb_spineFace_VHandles );

          glb_spineFace_VHandles.clear ( );

          index = ( j*HorResol + t );
          index += ( i - lNumModeledSpinesAdded )*spineGlobalDesp;
          glb_spineFace_VHandles.push_back ( glb_GlobalSpinesVHandle[index] );

          index = (( j + 1 )*HorResol + t );
          index += ( i - lNumModeledSpinesAdded )*spineGlobalDesp;
          glb_spineFace_VHandles.push_back ( glb_GlobalSpinesVHandle[index] );

          index = ( j*HorResol + t + 1 );
          index += ( i - lNumModeledSpinesAdded )*spineGlobalDesp;
          glb_spineFace_VHandles.push_back ( glb_GlobalSpinesVHandle[index] );
          Mesh->add_face ( glb_spineFace_VHandles );

        }

        //Tapa de la espina
        for ( j = 0; j < ( HorResol - 1 ); ++j )
        {
          //Superior circle
          glb_spineFace_VHandles.clear ( );

          index = ((( i - lNumModeledSpinesAdded )*spineGlobalDesp ) + ( spineGlobalDesp - HorResol - 2 ) + j + 2 );
          glb_spineFace_VHandles.push_back ( glb_GlobalSpinesVHandle[index] );

          index = ((( i - lNumModeledSpinesAdded )*spineGlobalDesp ) + ( spineGlobalDesp - HorResol - 2 ) + j + 1 );
          glb_spineFace_VHandles.push_back ( glb_GlobalSpinesVHandle[index] );

          index = (( i - lNumModeledSpinesAdded )*spineGlobalDesp );
          index += spineGlobalDesp - 1;
          glb_spineFace_VHandles.push_back ( glb_GlobalSpinesVHandle[index] );

          Mesh->add_face ( glb_spineFace_VHandles );
        }

        glb_spineFace_VHandles.clear ( );

        index = ((( i - lNumModeledSpinesAdded )*spineGlobalDesp ) + ( spineGlobalDesp - HorResol - 2 ) + 1 );
        glb_spineFace_VHandles.push_back ( glb_GlobalSpinesVHandle[index] );

        index = ((( i - lNumModeledSpinesAdded )*spineGlobalDesp ) + ( spineGlobalDesp - HorResol - 2 ) + j + 1 );
        glb_spineFace_VHandles.push_back ( glb_GlobalSpinesVHandle[index] );

        index = (( i - lNumModeledSpinesAdded )*spineGlobalDesp );
        index += spineGlobalDesp - 1;
        glb_spineFace_VHandles.push_back ( glb_GlobalSpinesVHandle[index] );

        Mesh->add_face ( glb_spineFace_VHandles );
      }
      else
      {
        tmpMesh = new BaseMesh ( );

        lModelSpine = lSpinesIds[i];

        tmpMesh->JoinBaseMesh ( mSpinesModeledContainer->getElementAt ( lModelSpine ));
        tmpMesh->scaleBaseMesh ( lSpineScale );

        //Unimos la malla y calculamos los vertices antes y despues
        unsigned int lIniLimit = this->getNumVertex ( );
        JoinBaseMesh ( tmpMesh );
        unsigned int lFinLimit = this->getNumVertex ( );

        //All the vertices are ready now
        for ( j = ( lIniLimit ); j < ( lFinLimit ); ++j )
        {

          //Hay q coger el precomputado
          glb_AuxVec[0] = Mesh->points ( )[j][0];
          glb_AuxVec[1] = Mesh->points ( )[j][1];
          glb_AuxVec[2] = Mesh->points ( )[j][2];
          glb_AuxVec[3] = 1.0;

          //Matrix operations
          glb_AuxVec = prod ( glb_mat, glb_AuxVec );

          //Explicit translate
          //Muchmore faster than apply the translate matrix
          glb_AuxVec[0] += auxPoint[0];
          glb_AuxVec[1] += auxPoint[1];
          glb_AuxVec[2] += auxPoint[2];

          MeshDef::VertexHandle vhandle = Mesh->vertex_handle ( j );
          Mesh->set_point ( vhandle, ( MeshDef::Point ( glb_AuxVec[0], glb_AuxVec[1], glb_AuxVec[2]
                            )
                            )
          );
        }

        ++lNumModeledSpinesAdded;

        delete tmpMesh;
      }
    }
    updateBaseMesh ( );
  }

  void SpinesSWC::distributeSpineAlongDendritic ( unsigned int pIdDendritic )
  {
    unsigned int lDendriticSegmentIni = NeuronMesh->getImporter ( )->getDendritics ( )[pIdDendritic].initialNode;
    unsigned int lDendriticSegmentFin = NeuronMesh->getImporter ( )->getDendritics ( )[pIdDendritic].finalNode;

    unsigned int lTubeResol = NeuronMesh->getTubeNumVertex ( );

    //Puee que haga falta mover los indices -> no cuadran bien
    unsigned int lDendriticVertexIni =
      ( lDendriticSegmentIni - 2 )*NeuronMesh->getTubeNumVertex ( ) + NeuronMesh->getSomaNumVertex ( );
    unsigned int lDendriticVertexFin =
      ( lDendriticSegmentFin - 1 )*NeuronMesh->getTubeNumVertex ( ) + NeuronMesh->getSomaNumVertex ( ) - 1;

    std::vector <SpineDenNode> *lContainer = NULL;
    unsigned int lNumIntervals = 0;
    unsigned int indexCand = 0;

    SpineDenNode lIniDensityNode;
    SpineDenNode lFinDensityNode;
    float lMedSpinesDensity = 0;

    float lRealDistance = 0;
    float lNormalizedDistance = 0;

    unsigned int lParentId = 0;
    unsigned int lNumSpinesSegment = 0;
    unsigned int lTotalNumSpines = 0;
    float lSpinesPerVertex = 0;

    //Procesamos cada intervalo de la dendrita para saber cuantas espinas ponemos
    //Select the container with the distribution
    if ( NeuronMesh->getImporter ( )->getDendritics ( )[pIdDendritic].type != DendriticType::AXON )
    {
      //lContainer = &BasalSpineDensityContainer;

      if ( NeuronMesh->getImporter ( )->getDendritics ( )[pIdDendritic].type == DendriticType::BASAL )
        lContainer = &BasalSpineDensityContainer;

      if ( NeuronMesh->getImporter ( )->getDendritics ( )[pIdDendritic].type == DendriticType::APICAL )
        lContainer = &ApicalSpineDensityContainer;

      //Saber cuantas espinas hemos de poner para la neurona actual en cada intervalo
      lNumIntervals = lContainer->size ( );

      for ( int i = 0; i < ( lNumIntervals - 1 ); ++i )
      {
        lIniDensityNode = lContainer->at ( i );
        lFinDensityNode = lContainer->at ( i + 1 );

        lMedSpinesDensity = ( lIniDensityNode.SpinesPer_um + lFinDensityNode.SpinesPer_um )/2.0;

        //!!!Modificator of the distributor
        lMedSpinesDensity *= mDistributorModificator;

        lRealDistance = 0;

        //Limpiamos el contenedor de vertices candidatos
        mDendriticVertexContainer.clear ( );

        //Recorremos la dendrita para saber que segmentos-vertices se encuentra en el intervalo actual
        for ( unsigned int j = lDendriticSegmentIni; j < lDendriticSegmentFin; ++j )
        {
          //Comparamos el parent con la distancia normalizada, para saber si esta en el intervalo, si el paren est� pillamos ese intervalo
          lParentId = NeuronMesh->getImporter ( )->getElementAt ( j ).parent;
          lNormalizedDistance = NeuronMesh->getImporter ( )->getElementAt ( lParentId ).mDendriticDistanceNorm;

          //Si el parent se encuentra en el intervalo, sumamos esa distancia
          //Y marcamos esos v�rtices
          if (( lNormalizedDistance > lIniDensityNode.DistToSomaNormalized )
            && ( lNormalizedDistance < lFinDensityNode.DistToSomaNormalized ))
          {
            lRealDistance += NeuronMesh->getImporter ( )->getElementAt ( j ).mDendriticDistanceReal
              - NeuronMesh->getImporter ( )->getElementAt ( lParentId ).mDendriticDistanceReal;

            //->Almacenar vertices de ese segmento como candidatos a poner spinas
            //-> Se puede afinar mas y solo poner los que cumplan la restriccion de distancia exacta
            for ( unsigned int k = 1; k <= lTubeResol; ++k )
            {
              //unsigned int	lDendriticVertexIni		=	(lDendriticSegmentIni-2) * NeuronMesh->getTubeNumVertex() + NeuronMesh->getSomaNumVertex();
              indexCand = (( j )*lTubeResol ) + NeuronMesh->getSomaNumVertex ( ) - k;
              if ( !mVertexDistancesContainer.at ( indexCand ).lUsed )
              {
                mDendriticVertexContainer.push_back ( indexCand );
              }
            }
          }
        }

        //En este punto ya tenemos la distancia total y los v�rtices donde posicionar las espinas

        //!!!->Ojo, repasar a ver si es correcto!!!
        lNumSpinesSegment = lRealDistance/lMedSpinesDensity;
        //lNumSpinesSegment	= lRealDistance * lMedSpinesDensity;
        lTotalNumSpines += lNumSpinesSegment;

        if ( mDendriticVertexContainer.size ( ) > 0 )
        {
          //lSpinesPerVertex	= lTotalNumSpines / mDendriticVertexContainer.size();
          lNumSpinesSegment = lTotalNumSpines/mDendriticVertexContainer.size ( );

          if (( lSpinesPerVertex < 1 )
            && ( lSpinesPerVertex > ( -0.1 ))
            )
          {
            lSpinesPerVertex = 1;
          }

          distributeSpineAlongVertexs ( lNumSpinesSegment, mDendriticVertexContainer, lSpinesPerVertex );
        }
      }
    }
  }

  void SpinesSWC::distributeSpineAlongAllDendritics ( )
  {
    unsigned int lNumDendritics = NeuronMesh->getImporter ( )->getNumDendritics ( );

    //Restore the dendrititc modificator
    //mDistributorModificator = 1.0;

    for ( int i = 0; i < lNumDendritics; ++i )
    {
      distributeSpineAlongDendritic ( i );
    }

//		updateBaseMesh();
//		distributeSpineAlongDendritic(0);
  }

  void SpinesSWC::distributeSpinesInSegments ( float pSpinesDesp,
                                               float pBProgressionFactor,
                                               float pAProgressionFactor,
                                               float pBasalMinDistance,
                                               float pBasalCteDistance,
                                               float pApicalMinDistance,
                                               float pApicalCteDistance,
                                               BBDD::BBDD bbdd,
                                               const std::string& neuronName,
                                               const std::string& tmpPath
  )
  {
    SWCSpinesDistributor *lpSWCSpinesDistributor = new SWCSpinesDistributor ( );

    lpSWCSpinesDistributor->setSpinesDistributionParamas ( NeuronMesh->getImporter ( ),
                                                           pSpinesDesp,
                                                           pBProgressionFactor,
                                                           pAProgressionFactor,
                                                           pBasalMinDistance,
                                                           pBasalCteDistance,
                                                           pApicalMinDistance,
                                                           pApicalCteDistance
    );
    lpSWCSpinesDistributor->generateDistribution ( );

    std::vector <NodeSpinesDistrib> lAuxSpinesDistrib = lpSWCSpinesDistributor->getSpinesDistributionContainer ( );

    mNumVerticesEnSpina = mSpinesModeledContainer->getContainer ( ).at ( 0 )->getNumVertex ( );

    SpineInfo lSpineInfo;
    mSpinesInfo.clear ( );

    //delete lpSWCSpinesDistributor;

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///######

    unsigned int lNumSegmentsWithSpines = 0;
    unsigned int lNumSpinesInSegment = 0;

    BaseMesh *tmpMesh;
    unsigned int lSpineModelSelected;

    unsigned int idVCandidate = 0;
    unsigned int idFCandidate = 0;

    std::vector < VertexDistances >::iterator lItV;
    std::vector < FacesDistances >::iterator lItF;

    unsigned int lNumVertAtached = 0;
    unsigned int lNumFacesAtached = 0;

    //Selection for vertex
    unsigned int numVertices = NeuronMesh->getNumVertex ( );
    unsigned int somaNumVertices = NeuronMesh->getSomaNumVertex ( );

    //Selection for faces
    unsigned int numFaces = NeuronMesh->calcNumFaces ( );
    unsigned int somaNumFaces = NeuronMesh->getSomaNumFaces ( );

    //Incremet for dispose ramdomly the spines
    float lTmpCalc = ( float ) ( numVertices - somaNumVertices )/( float ) totalNumSpines;
    unsigned int lIncRand = ( int ) lTmpCalc;
    if (( lTmpCalc - lIncRand ) > 0.49 )
      ++lIncRand;
    unsigned int lInfLimit = somaNumVertices;

    initrand ( );

    unsigned int index = 0;

    unsigned int i, j, k;

    OpenMesh::Vec3f auxPoint;
    OpenMesh::Vec3f auxNormal;

    //MeshDef::VertexHandle vhandle;

    namespace ublas = boost::numeric::ublas;

    ublas::vector < float > glb_AuxVec;
    glb_AuxVec.resize ( 4 );

    ublas::vector < float > XVector;
    XVector.resize ( 3 );
    XVector[0] = 1;
    XVector[1] = 0;
    XVector[2] = 0;

    ublas::vector < float > YVector;
    YVector.resize ( 3 );
    YVector[0] = 0;
    YVector[1] = 1;
    YVector[2] = 0;

    ublas::vector < float > glb_ZeroVec;
    glb_ZeroVec.resize ( 3 );
    glb_ZeroVec[0] = glb_ZeroVec[1] = glb_ZeroVec[2] = 0;

    boost::numeric::ublas::matrix < float > glb_mat;
    glb_mat.resize ( 4, 4 );

    boost::numeric::ublas::vector < float > glb_forwardVec;
    glb_forwardVec.resize ( 3 );

    boost::numeric::ublas::vector < float > glb_DespVec;
    glb_DespVec.resize ( 3 );

    //Auxiliar forwardvector
    boost::numeric::ublas::vector < float > glb_TransVectorAux;
    glb_TransVectorAux.resize ( 3 );

    //Right vector
    boost::numeric::ublas::vector < float > glb_RightVector;
    glb_RightVector.resize ( 3 );

    //Up vector
    boost::numeric::ublas::vector < float > glb_UpVector;
    glb_UpVector.resize ( 3 );

    //NeuronMesh->freeFaceFusionCandidates();

    initrand ( );

    lNumSegmentsWithSpines = lAuxSpinesDistrib.size ( );
    int total_spines = 0;
    for (int k = 0; k < lNumSegmentsWithSpines; k++) {
        total_spines += lAuxSpinesDistrib.at ( k ).mSpinesSegmentContainer.size ( );
    }

    auto spines = bbdd.getRandomSpines(total_spines,tmpPath);
    int numSpines = spines.size();
    int counter = 0;
    bool haveSpinesNeuron = bbdd.haveSpinesNeuron(neuronName);
    bbdd.openTransaction();
    //Auxiliar vectors
    for ( int k = 0; k < lNumSegmentsWithSpines; ++k )
    {
      lNumSpinesInSegment = lAuxSpinesDistrib.at ( k ).mSpinesSegmentContainer.size ( );

      for ( i = 0; i < lNumSpinesInSegment; ++i )
      {
          auto spine = spines[counter % numSpines];
        counter ++;
        //Select the spine modelled
        lSpineModelSelected = std::get<0>(spine);


        //New version
        //////!!!!!!
        tmpMesh = new BaseMesh ( );
        tmpMesh->loadModel(std::get<1>(spine));
        MeshDef::Color color = getRandomColor();
        tmpMesh->setVertexColor(tmpMesh->getMesh()->vertices_begin(),tmpMesh->getMesh()->vertices_end(),color);
        //tmpMesh->JoinBaseMesh ( mSpinesModeledContainer->getElementAt ( lSpineModelSelected ));
        //tmpMesh->scaleBaseMesh(lSpineScale);

        MeshDef::ConstVertexFaceIter CVFIter;
        MeshDef::FaceHandle lAuxFaceHandle;
        std::vector <MeshDef::FaceHandle> vecFaceCands;
        int lFaceCand;
        float lLenght;
        OpenMesh::Vec3f lVecAB;
        float lSegPercetn;
        unsigned int lNumVertexToChoose = 0;


        //Colocaci�n de las espinas en los centros de las facetas
        //From vertex candidate, we choose his incident face
        MeshDef::VertexHandle vhandle;
        MeshDef::HalfedgeHandle hehandle;
        MeshDef::FaceHandle fhandle;

        //Selection by vertices

        MeshDef::Point lTmpVertex ( 0, 0, 0 );

        unsigned int lIniOpt;
        unsigned int lFinOpt;

        auxPoint = lAuxSpinesDistrib.at ( k ).mSpinesSegmentContainer.at ( i ).mPosition;
        //auxNormal = NeuronMesh->getMesh()->calc_face_normal(fhandle);
        auxNormal = lAuxSpinesDistrib.at ( k ).mSpinesSegmentContainer.at ( i ).mOrientation;

        lSpineInfo.mSpineId = lSpineModelSelected;
        lSpineInfo.mSpinePosition = auxPoint;
        lSpineInfo.mSpineDirection = auxNormal;
        lSpineInfo.mIndexationNode = lAuxSpinesDistrib.at ( k ).mNodeIni;
        mSpinesInfo.push_back ( lSpineInfo );

        //lNumVertexToChoose	=	(unsigned int)randfloat(0,mVertexDistancesContainer.size()-1);
        //idVCandidate		= (int)randfloat(0,lNumVertexToChoose);
        ////idVCandidate		= mVertexDistancesContainer.at(lNumVertexToChoose);

        ////Eliminamos el marcador para no volver a seleccionarlo
        //lItV = mVertexDistancesContainer.begin() + idVCandidate;

        //////!!!!!!
        //tmpMesh->scaleBaseMesh(mVertexDistancesContainer.at(idVCandidate).lDendriticRadiusFactor);


        //idVCandidate = lItV->lId;

        ////mVertexDistancesContainer.at(lItV).lUsed	=	true;

        ////Consecutive Vertex selection
        ////idVCandidate    = (int)randfloat(lInfLimit,(lInfLimit+lIncRand));
        ////lInfLimit+=lIncRand;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////Aki va la disposici�n aleatoria sobre las facetas
        ////Store all the incidente vertex handles

        //vhandle		=	NeuronMesh->getMesh()->vertex_handle(idVCandidate);

        //for (CVFIter=NeuronMesh->getMesh()->cvf_iter(vhandle);CVFIter.is_valid();++CVFIter)
        //{
        //	lAuxFaceHandle = NeuronMesh->getMesh()->face_handle(CVFIter->idx());
        //	vecFaceCands.push_back(lAuxFaceHandle);
        //}

        //lFaceCand = (int)randfloat(0,vecFaceCands.size()-1);
        //fhandle	= vecFaceCands.at(lFaceCand);

        ////Selecci�n de la normal de la faceta
        //auxNormal = NeuronMesh->getMesh()->calc_face_normal(fhandle);

        ////Calculo del punto sobre el que poner la espina

        //NeuronMesh->getMesh()->calc_face_centroid(fhandle,lTmpVertex);
        //auxPoint		= NeuronMesh->getMesh()->points()[idVCandidate];

        //lVecAB	=	OpenMesh::Vec3f(
        //							lTmpVertex
        //							- auxPoint
        //							);
        ////Longitud desde el vertice hasta el centro de la faceta
        //lLenght=lVecAB.length();
        //lVecAB.normalize();

        //////Colcaci�n de la espina en la mitad del segmento
        ////auxPoint		= auxPoint + lVecAB*(lLenght/2.0);

        ////COlcaci�n de la espina en la mitad del segmento

        //lSegPercetn	=	(lLenght*DESP_PERCENT)/100.0;
        //lLenght		-=	lSegPercetn;
        //lLenght		= randfloat(lSegPercetn,lLenght);

        //auxPoint		= auxPoint + lVecAB*(lLenght);

        //mVertexDistancesContainer.erase(lItV);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////

        //Codigo comun para posicionamiento por vertice y por faceta
        /////////////////////////////////////////////////////////////////////////////////////////////////////////
        glb_direction = ( auxPoint + auxNormal*mMaxLongSpine ) - auxPoint;







        //Orientaci�n seg�n la normal
        glb_forwardVec[0] = glb_direction[0];
        glb_forwardVec[1] = glb_direction[1];
        glb_forwardVec[2] = glb_direction[2];

        //Giro de la dendrita para no alinearla exatamente con la normal
        //glb_forwardVec[0] = glb_direction[0]+randfloat(-mMaxLongSpine, mMaxLongSpine);
        //glb_forwardVec[1] = glb_direction[1];
        //glb_forwardVec[2] = glb_direction[2]+randfloat(-mMaxLongSpine, mMaxLongSpine);

        //Montaje de la matriz
        generateTransformationMatrix ( glb_mat,
                                       glb_DespVec,
                                       glb_forwardVec,
                                       glb_TransVectorAux,
                                       glb_RightVector,
                                       glb_UpVector,
                                       XVector,
                                       YVector
        );
        if (!haveSpinesNeuron) {
            bbdd.addSpine(neuronName, lSpineModelSelected, auxPoint, glb_mat);
        }

        //Unimos la malla y calculamos los vertices antes y despues
        unsigned int lIniLimit = this->getNumVertex ( );
        JoinBaseMesh ( tmpMesh );
        unsigned int lFinLimit = this->getNumVertex ( );

        //All the vertices are ready now
        for ( j = ( lIniLimit ); j < ( lFinLimit ); ++j )
        {

          //Hay q coger el precomputado
          glb_AuxVec[0] = Mesh->points ( )[j][0];
          glb_AuxVec[1] = Mesh->points ( )[j][1];
          glb_AuxVec[2] = Mesh->points ( )[j][2];
          glb_AuxVec[3] = 1.0;

          //Matrix operations
          glb_AuxVec = prod ( glb_mat, glb_AuxVec );

          //Explicit translate
          //Muchmore faster than apply the translate matrix
          glb_AuxVec[0] += auxPoint[0];
          glb_AuxVec[1] += auxPoint[1];
          glb_AuxVec[2] += auxPoint[2];

          //Mesh->points()[j] = (MeshDef::Point(glb_AuxVec[0]
          //									,glb_AuxVec[1]
          //									,glb_AuxVec[2]
          //								  )
          //					);

          MeshDef::VertexHandle vhandle = Mesh->vertex_handle ( j );
          Mesh->set_point ( vhandle, ( MeshDef::Point ( glb_AuxVec[0], glb_AuxVec[1], glb_AuxVec[2]
                            )
                            )
          );
        }

        //NeuronMesh->addFaceFusionCandidate(fhandle);
        ////!!!!!!
        delete tmpMesh;

      }
    }
    bbdd.closeTransaction();
    updateBaseMesh ( );

    ///####
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    delete lpSWCSpinesDistributor;
  }




  //void SpinesSWC::distributeSpineAlongAllDendriticsInGroups(unsigned int pNumberOfGropus)
  //{
  //	unsigned int lNumDendritics = NeuronMesh->getImporter()->getNumDendritics();

  //	mDistributorModificator = 1.0/pNumberOfGropus;

  //	for (int i=0;i<lNumDendritics;++i)
  //	{
  //		distributeSpineAlongDendritic(i);
  //	}
  //}

  void SpinesSWC::setDistributorModificator ( float pDistModificator )
  {
    mDistributorModificator = pDistModificator;
  }

  void SpinesSWC::setMultipleGroupsOfSpines ( bool pMultiGroup )
  {
    mMultipleSpinesGropus = pMultiGroup;
  }

  unsigned int SpinesSWC::preCalcNumSpines ( )
  {
    unsigned int lTubeResol = NeuronMesh->getTubeNumVertex ( );

    std::vector <SpineDenNode> *lContainer = NULL;
    unsigned int lNumIntervals = 0;
    unsigned int indexCand = 0;

    SpineDenNode lIniDensityNode;
    SpineDenNode lFinDensityNode;
    float lMedSpinesDensity = 0;

    float lRealDistance = 0;
    float lNormalizedDistance = 0;

    unsigned int lParentId = 0;
    unsigned int lNumSpinesSegment = 0;
    unsigned int lTotalNumSpines = 0;
    float lSpinesPerVertex = 0;

    unsigned int lTotalSpines = 0;

    //Procesamos cada intervalo de la dendrita para saber cuantas espinas ponemos
    //Select the container with the distribution
    unsigned int lNumDendritics = NeuronMesh->getImporter ( )->getNumDendritics ( );

    for ( int k = 0; k < lNumDendritics; ++k )
    {
      unsigned int lDendriticSegmentIni = NeuronMesh->getImporter ( )->getDendritics ( )[k].initialNode;
      unsigned int lDendriticSegmentFin = NeuronMesh->getImporter ( )->getDendritics ( )[k].finalNode;

      unsigned int lDendriticVertexIni =
        ( lDendriticSegmentIni - 2 )*NeuronMesh->getTubeNumVertex ( ) + NeuronMesh->getSomaNumVertex ( );
      unsigned int lDendriticVertexFin =
        ( lDendriticSegmentFin - 1 )*NeuronMesh->getTubeNumVertex ( ) + NeuronMesh->getSomaNumVertex ( ) - 1;


      //Si nos es un axon
      if ( NeuronMesh->getImporter ( )->getDendritics ( )[k].type != DendriticType::AXON )
      {

        if ( NeuronMesh->getImporter ( )->getDendritics ( )[k].type == DendriticType::BASAL )
          lContainer = &BasalSpineDensityContainer;

        if ( NeuronMesh->getImporter ( )->getDendritics ( )[k].type == DendriticType::APICAL )
          lContainer = &ApicalSpineDensityContainer;

        //Saber cuantas espinas hemos de poner para la neurona actual en cada intervalo
        lNumIntervals = lContainer->size ( );

        for ( int i = 0; i < ( lNumIntervals - 1 ); ++i )
        {
          lIniDensityNode = lContainer->at ( i );
          lFinDensityNode = lContainer->at ( i + 1 );

          lMedSpinesDensity = ( lIniDensityNode.SpinesPer_um + lFinDensityNode.SpinesPer_um )/2.0;

          lRealDistance = 0;

          //Limpiamos el contenedor de vertices candidatos
          mDendriticVertexContainer.clear ( );

          //Recorremos la dendrita para saber que segmentos-vertices se encuentra en el intervalo actual
          for ( unsigned int j = lDendriticSegmentIni; j < lDendriticSegmentFin; ++j )
          {
            //Comparamos el parent con la distancia normalizada, para saber si esta en el intervalo, si el paren est� pillamos ese intervalo
            lParentId = NeuronMesh->getImporter ( )->getElementAt ( j ).parent;
            lNormalizedDistance = NeuronMesh->getImporter ( )->getElementAt ( lParentId ).mDendriticDistanceNorm;

            //Si el parent se encuentra en el intervalo, sumamos esa distancia
            //Y marcamos esos v�rtices
            if (( lNormalizedDistance > lIniDensityNode.DistToSomaNormalized )
              && ( lNormalizedDistance < lFinDensityNode.DistToSomaNormalized ))
            {
              lRealDistance += NeuronMesh->getImporter ( )->getElementAt ( j ).mDendriticDistanceReal
                - NeuronMesh->getImporter ( )->getElementAt ( lParentId ).mDendriticDistanceReal;

            }
          }

          //En este punto ya tenemos la distancia total y los v�rtices donde posicionar las espinas
          //lNumSpinesSegment	= lRealDistance / lMedSpinesDensity;
          lNumSpinesSegment = lRealDistance*lMedSpinesDensity;
          lTotalNumSpines += lNumSpinesSegment;
        }
      }
    }

    return lTotalNumSpines;
  }

  void SpinesSWC::generateSpinesDistribution ( )
  {
    SpineDenNode lAuxNode;

    //Generate basal distribution
    lAuxNode.DistToSoma = lAuxNode.SpinesPer_um = 0;
    for ( int i = 0; i <= BASAL_INTERVALS; ++i )
    {
      lAuxNode.DistToSoma += 20;
      BasalSpineDensityContainer.push_back ( lAuxNode );
    }

    //Normalize distance
    for ( int i = 0; i <= BASAL_INTERVALS; ++i )
    {
      BasalSpineDensityContainer[i].DistToSomaNormalized = ( float ) BasalSpineDensityContainer[i].DistToSoma
        /( float ) BasalSpineDensityContainer[BASAL_INTERVALS].DistToSoma;
    }

    BasalSpineDensityContainer.at ( 0 ).SpinesPer_um = 0;
    BasalSpineDensityContainer.at ( 1 ).SpinesPer_um = 0.45;
    BasalSpineDensityContainer.at ( 2 ).SpinesPer_um = 1.1;
    BasalSpineDensityContainer.at ( 3 ).SpinesPer_um = 1.7;
    BasalSpineDensityContainer.at ( 4 ).SpinesPer_um = 2;
    BasalSpineDensityContainer.at ( 5 ).SpinesPer_um = 1.9;
    BasalSpineDensityContainer.at ( 6 ).SpinesPer_um = 1.8;
    BasalSpineDensityContainer.at ( 7 ).SpinesPer_um = 1.85;
    BasalSpineDensityContainer.at ( 8 ).SpinesPer_um = 1.6;
    BasalSpineDensityContainer.at ( 9 ).SpinesPer_um = 1.5;
    BasalSpineDensityContainer.at ( 10 ).SpinesPer_um = 1.55;

    //Generate apical distribution
    lAuxNode.DistToSoma = lAuxNode.SpinesPer_um = 0;
    for ( int i = 0; i <= APICAL_INTERVALS; ++i )
    {
      lAuxNode.DistToSoma += 20;
      ApicalSpineDensityContainer.push_back ( lAuxNode );
    }

    //Normalize distance
    for ( int i = 0; i <= APICAL_INTERVALS; ++i )
    {
      ApicalSpineDensityContainer[i].DistToSomaNormalized = ( float ) ApicalSpineDensityContainer[i].DistToSoma
        /( float ) ApicalSpineDensityContainer[APICAL_INTERVALS].DistToSoma;
    }

    ApicalSpineDensityContainer.at ( 0 ).SpinesPer_um = 0;
    ApicalSpineDensityContainer.at ( 1 ).SpinesPer_um = 0.1;
    ApicalSpineDensityContainer.at ( 2 ).SpinesPer_um = 0.2;
    ApicalSpineDensityContainer.at ( 3 ).SpinesPer_um = 0.5;
    ApicalSpineDensityContainer.at ( 4 ).SpinesPer_um = 1.1;
    ApicalSpineDensityContainer.at ( 5 ).SpinesPer_um = 1.5;
    ApicalSpineDensityContainer.at ( 6 ).SpinesPer_um = 1.8;
    ApicalSpineDensityContainer.at ( 7 ).SpinesPer_um = 2.2;
    ApicalSpineDensityContainer.at ( 8 ).SpinesPer_um = 2.3;
    ApicalSpineDensityContainer.at ( 9 ).SpinesPer_um = 2.1;
    ApicalSpineDensityContainer.at ( 10 ).SpinesPer_um = 1.9;
    ApicalSpineDensityContainer.at ( 11 ).SpinesPer_um = 1.8;
    ApicalSpineDensityContainer.at ( 12 ).SpinesPer_um = 1.5;
    ApicalSpineDensityContainer.at ( 13 ).SpinesPer_um = 1.3;
    ApicalSpineDensityContainer.at ( 14 ).SpinesPer_um = 1.35;
    ApicalSpineDensityContainer.at ( 15 ).SpinesPer_um = 1.25;
    ApicalSpineDensityContainer.at ( 16 ).SpinesPer_um = 1.20;
    ApicalSpineDensityContainer.at ( 17 ).SpinesPer_um = 1.22;
    ApicalSpineDensityContainer.at ( 18 ).SpinesPer_um = 0.9;
    ApicalSpineDensityContainer.at ( 19 ).SpinesPer_um = 0.8;
    ApicalSpineDensityContainer.at ( 20 ).SpinesPer_um = 1;
    ApicalSpineDensityContainer.at ( 21 ).SpinesPer_um = 0.8;
    ApicalSpineDensityContainer.at ( 22 ).SpinesPer_um = 0.8;
  }

  //void SpinesSWC::calcVertexCandDistances(bool pCompleteMesh)
  //{
  //	VertexDistances		lVertexDistanceNode;
  //	FacesDistances		lFaceDistanceNode;

  //	unsigned int		lTubeResolution		=0;
  //	unsigned int		lSomaResolution		=0;
  //	unsigned int		lVertexToProcess	=0;
  //	unsigned int		lFacesToProcess		=0;

  //	OpenMesh::Vec3f		lAuxVector;
  //	MeshDef::Point		lAuxPoint;

  //	lVertexDistanceNode.lDendriticDistanceNorm	=	0;
  //	lVertexDistanceNode.lDendriticDistanceReal	=	0;
  //	lVertexDistanceNode.lDistanceToSoma			=	0;
  //	lVertexDistanceNode.lUsed					=	false;

  //	lSomaResolution		= NeuronMesh->getSomaNumVertex();
  //	lVertexToProcess	= NeuronMesh->getNumVertex();

  //	MeshDef::FaceHandle		 fhandle;

  //	unsigned int			lTubeResol				=	NeuronMesh->getTubeNumVertex();

  //	//Hay que ver que sean diferentes del axon los v�rtices sobre lso que situar las espinas
  //	//Hay que recorrer la neurona por dendrtias


  //	//Restore the dendrititc modificator
  //	//mDistributorModificator = 1.0;

  //	//we need all vertices to index in real distribution
  //	if (pCompleteMesh)
  //	{
  //		for (int i=0;i<lVertexToProcess;++i)
  //		{
  //			lVertexDistanceNode.lId = i;
  //			mVertexDistancesContainer.push_back(lVertexDistanceNode);
  //		}
  //	}
  //	else
  //	{
  //		unsigned int lNumDendritics			=	NeuronMesh->getImporter()->getNumDendritics();

  //		for (int i=0;i<lNumDendritics;++i)
  //		{
  //			//Si la dendrita aprocesar no es una axon, tomamos su v�rtices si:
  //			if (NeuronMesh->getImporter()->getDendritics()[i].type != DendriticType::AXON)
  //			{
  //				unsigned int lDendriticSegmentIni	=	NeuronMesh->getImporter()->getDendritics()[i].initialNode;
  //				unsigned int lDendriticSegmentFin	=	NeuronMesh->getImporter()->getDendritics()[i].finalNode;

  //				//Process all the segments in the actual dendritic
  //				for (unsigned int j = lDendriticSegmentIni; j<lDendriticSegmentFin; ++j)
  //				{
  //					unsigned int lInc=1;
  //					unsigned int lParent =  NeuronMesh->getImporter()->getElementAt(j).parent;

  //					//float lRadiusFactor = ( ( NeuronMesh->getImporter()->getElementAt(j).radius
  //					//					    + NeuronMesh->getImporter()->getElementAt(lParent).radius) / 2.0
  //					//					   ) * 2.0 * SPINE_EXTRA_SCALE;

  //					float lRadiusFactor = 1.0;

  //					//�apa!!!! -> Los indices de las dendritic terminations pueden que esten mal
  //					if (NeuronMesh->getImporter()->isDendriticTermination(j+1))
  //					{
  //						//++lInc;
  //						lInc=DESP_SPINES_IN_TERM;
  //					}

  //					//Process all the vertices in the actual segment of the dendritic
  //					for (unsigned int k = ((j-1) * lTubeResol) + NeuronMesh->getSomaNumVertex()
  //									;k < ( (j) * lTubeResol) + NeuronMesh->getSomaNumVertex()
  //									;k+=lInc)
  //					{
  //						lVertexDistanceNode.lId = k;

  //						lAuxVector	= OpenMesh::Vec3f(NeuronMesh->getMesh()->points()[k][0]
  //													,NeuronMesh->getMesh()->points()[k][1]
  //													,NeuronMesh->getMesh()->points()[k][2]);

  //						lVertexDistanceNode.lDistanceToSoma	= lAuxVector.length();

  //						lVertexDistanceNode.lDendriticRadiusFactor = lRadiusFactor;

  //						if (lVertexDistanceNode.lDistanceToSoma>SOMA_MIN_DISTACE)
  //						{
  //							mVertexDistancesContainer.push_back(lVertexDistanceNode);
  //						}
  //					}
  //				}
  //			}
  //		}
  //	}

  //	//Faces with min distance
  //	//for (; fIt!=fEnd; ++fIt)
  //	lFacesToProcess = NeuronMesh->getNumFaces();

  //	for (int i=0;i<lFacesToProcess;++i)
  //	{
  //		lFaceDistanceNode.lId = i;

  //		fhandle		= NeuronMesh->getMesh()->face_handle(i);

  //		NeuronMesh->getMesh()->calc_face_centroid(fhandle,lAuxVector);

  //		lFaceDistanceNode.lDistanceToSoma	= lAuxVector.length();

  //		if (lFaceDistanceNode.lDistanceToSoma>SOMA_MIN_DISTACE)
  //		{
  //			mFacesDistancesContainer.push_back(lFaceDistanceNode);
  //		}
  //	}
  //}

  void SpinesSWC::calcVertexCandDistances ( bool pCompleteMesh )
  {
    VertexDistances lVertexDistanceNode;
    FacesDistances lFaceDistanceNode;

    unsigned int lTubeResolution = 0;
    unsigned int lSomaResolution = 0;
    unsigned int lVertexToProcess = 0;
    unsigned int lFacesToProcess = 0;

    OpenMesh::Vec3f lAuxVector;
    MeshDef::Point lAuxPoint;

    lVertexDistanceNode.lDendriticDistanceNorm = 0;
    lVertexDistanceNode.lDendriticDistanceReal = 0;
    lVertexDistanceNode.lDistanceToSoma = 0;
    lVertexDistanceNode.lUsed = false;

    lSomaResolution = NeuronMesh->getSomaNumVertex ( );
    lVertexToProcess = NeuronMesh->getNumVertex ( );

    MeshDef::FaceHandle fhandle;

    unsigned int lTubeResol = NeuronMesh->getTubeNumVertex ( );

    //Hay que ver que sean diferentes del axon los v�rtices sobre lso que situar las espinas
    //Hay que recorrer la neurona por dendrtias


    //Restore the dendrititc modificator
    //mDistributorModificator = 1.0;

    //we need all vertices to index in real distribution
    if ( pCompleteMesh )
    {
      for ( int i = 0; i < lVertexToProcess; ++i )
      {
        lVertexDistanceNode.lId = i;
        mVertexDistancesContainer.push_back ( lVertexDistanceNode );
      }
    }
    else
    {
      unsigned int lNumDendritics = NeuronMesh->getImporter ( )->getNumDendritics ( );
      //unsigned int lNumDendritics				=	1;

      for ( int i = 0; i < lNumDendritics; ++i )
      {
        //unsigned int	lVertexIncreasingDist	=	SPINE_INCRE_DESP;
        unsigned int lVertexIncreasingDist = NeuronMesh->getTubeNumVertex ( );
        bool lStartDist = false;

        //Si la dendrita aprocesar no es una axon, tomamos su v�rtices si:
        if (( NeuronMesh->getImporter ( )->getDendritics ( )[i].type != DendriticType::AXON )

          //###For fast draw on develop
          //&& (NeuronMesh->getImporter()->getDendritics()[i].type != DendriticType::APICAL)
          )
        {
          //lOnly1Dendritic = false;

          unsigned int lDendriticSegmentIni = NeuronMesh->getImporter ( )->getDendritics ( )[i].initialNode;
          unsigned int lDendriticSegmentFin = NeuronMesh->getImporter ( )->getDendritics ( )[i].finalNode;

          //Process all the segments in the actual dendritic
          for ( unsigned int j = lDendriticSegmentIni; j < lDendriticSegmentFin; ++j )
          {
            unsigned int lInc = 1;
            unsigned int lParent = NeuronMesh->getImporter ( )->getElementAt ( j ).parent;

            //float lRadiusFactor = ( ( NeuronMesh->getImporter()->getElementAt(j).radius
            //					    + NeuronMesh->getImporter()->getElementAt(lParent).radius) / 2.0
            //					   ) * 2.0 * SPINE_EXTRA_SCALE;

            float lRadiusFactor = 1.0;

            //�apa!!!! -> Los indices de las dendritic terminations pueden que esten mal
            if ( NeuronMesh->getImporter ( )->isDendriticTermination ( j + 1 ))
            {
              //++lInc;

              //Limitar el n�mero de espinas en las puntas de las dendritas
              //por defecto tres veces menos
              lInc = DESP_SPINES_IN_TERM;
            }
            else
            {
              if (( lVertexIncreasingDist > 1 )
                && ( lStartDist )
                )
              {
                --lVertexIncreasingDist;
                lInc = lVertexIncreasingDist;
              }
            }

            //Process all the vertices in the actual segment of the dendritic
            for ( unsigned int k = (( j - 1 )*lTubeResol ) + NeuronMesh->getSomaNumVertex ( );
                  k < (( j )*lTubeResol ) + NeuronMesh->getSomaNumVertex ( ); k += lInc )
            {
              lVertexDistanceNode.lId = k;

              lAuxVector = OpenMesh::Vec3f ( NeuronMesh->getMesh ( )->points ( )[k][0],
                                             NeuronMesh->getMesh ( )->points ( )[k][1],
                                             NeuronMesh->getMesh ( )->points ( )[k][2] );

              lVertexDistanceNode.lDistanceToSoma = lAuxVector.length ( );

              lVertexDistanceNode.lDendriticRadiusFactor = lRadiusFactor;

              if ( lVertexDistanceNode.lDistanceToSoma > SOMA_MIN_DISTACE )
              {
                lStartDist = true;
                mVertexDistancesContainer.push_back ( lVertexDistanceNode );
              }
            }

          }
        }
      }
    }

    //Faces with min distance
    //for (; fIt!=fEnd; ++fIt)
    lFacesToProcess = NeuronMesh->getNumFaces ( );

    for ( int i = 0; i < lFacesToProcess; ++i )
    {
      lFaceDistanceNode.lId = i;

      fhandle = NeuronMesh->getMesh ( )->face_handle ( i );

      NeuronMesh->getMesh ( )->calc_face_centroid ( fhandle, lAuxVector );

      lFaceDistanceNode.lDistanceToSoma = lAuxVector.length ( );

      if ( lFaceDistanceNode.lDistanceToSoma > SOMA_MIN_DISTACE )
      {
        mFacesDistancesContainer.push_back ( lFaceDistanceNode );
      }
    }
  }

  std::vector <VertexDistances> SpinesSWC::getVertexDistancesContainer ( )
  {
    return mVertexDistancesContainer;
  }

  std::vector <FacesDistances> SpinesSWC::getFacesDistancesContainer ( )
  {
    return mFacesDistancesContainer;
  }

  void SpinesSWC::setVertexDistancesContainer ( std::vector <VertexDistances> pContainer )
  {
    mVertexDistancesContainer = pContainer;
  }

  void SpinesSWC::setFacesDistancesContainer ( std::vector <FacesDistances> pContainer )
  {
    mFacesDistancesContainer = pContainer;
  }

  //BaseMeshContainer * SpinesSWC::getSpinesContainer()
  //{
  //	return mSpinesContainer;
  //}

  //void SpinesSWC::setSpinesContainer(BaseMeshContainer * pContainer)
  //{
  //	mSpinesContainer = pContainer;
  //}

  //BaseMeshContainer SpinesSWC::getSpinesContainer()
  //{
  //	return mSpinesContainer;
  //}

  void SpinesSWC::setSpinesContainer ( BaseMeshContainer *pContainer )
  {
    mSpinesModeledContainer = pContainer;
  }

  void SpinesSWC::exportSpinesInfo ( string fileName )
  {
    std::ofstream outputFileTXT;
    string fileTXT = fileName + ".SpinesInfo";

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
    outputFileTXT << mNumVerticesEnSpina << endl;

    //Total of neurons
    outputFileTXT << mSpinesInfo.size ( ) << endl;

    SpineInfo lSpineInfo;
    float lAuxPosX, lAuxPosY, lAuxPosZ;

    for ( int i = 0; i < mSpinesInfo.size ( ); ++i )
    {
      lSpineInfo = mSpinesInfo.at ( i );

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

  void SpinesSWC::importSpinesInfo ( string fileName )
  {
    mSpinesInfo.clear ( );

    std::ifstream inputFileTXT;
    inputFileTXT.open ( fileName.c_str ( ), std::ios::in );
    //if (inputFileTXT.fail())
    if ( !inputFileTXT )
    {
      cerr << "An error occurred. Unable to read input file." << fileName << endl;
      exit ( 1 );
    }

    SpineInfo lSpineInfo;

    char str[2000];
    char coment;
    //Read all the header
    bool continueReading = true;
    while ( continueReading )
    {
      inputFileTXT.get ( coment );
      if ( coment != '#' )
        continueReading = false;
      else
      {
        inputFileTXT.getline ( str, 2000 );
      }
    }

    //Node readed
    int pos = 0;
    int lNumSpines;

    inputFileTXT >> mNumVerticesEnSpina;

    inputFileTXT >> lNumSpines;

    if ( coment != ' ' )
    {
      ++pos;

      inputFileTXT >> lSpineInfo.mSpinePosition[0];
      inputFileTXT >> lSpineInfo.mSpinePosition[1];
      inputFileTXT >> lSpineInfo.mSpinePosition[2];
      inputFileTXT >> lSpineInfo.mSpineDirection[0];
      inputFileTXT >> lSpineInfo.mSpineDirection[1];
      inputFileTXT >> lSpineInfo.mSpineDirection[2];
      inputFileTXT >> lSpineInfo.mSpineId;
      inputFileTXT >> lSpineInfo.mIndexationNode;

      mSpinesInfo.push_back ( lSpineInfo );
    }

    while ( !inputFileTXT.eof ( ))
    {
      ++pos;

      inputFileTXT >> lSpineInfo.mSpinePosition[0];
      inputFileTXT >> lSpineInfo.mSpinePosition[1];
      inputFileTXT >> lSpineInfo.mSpinePosition[2];
      inputFileTXT >> lSpineInfo.mSpineDirection[0];
      inputFileTXT >> lSpineInfo.mSpineDirection[1];
      inputFileTXT >> lSpineInfo.mSpineDirection[2];
      inputFileTXT >> lSpineInfo.mSpineId;
      inputFileTXT >> lSpineInfo.mIndexationNode;

      mSpinesInfo.push_back ( lSpineInfo );
    }

    mSpinesInfo.pop_back ( );

    //Remove the last element if it is replicated
    int tam = mSpinesInfo.size ( );
    lSpineInfo = mSpinesInfo[tam - 1];
    //if ( (tam-1)!=lSpineInfo.id )	lSpineInfo.pop_back();

    //Close the file
    inputFileTXT.close ( );
  }

  void SpinesSWC::generateSpinesFromInfo ( )
  {
    if ( mSpinesInfo.size ( ) > 0 )
    {
      //delete Mesh;
      //Mesh = NULL;
      Mesh->clear ( );
      updateBaseMesh ( );

      BaseMesh *tmpMesh;
      unsigned int lSpineModelSelected;

      /*
			unsigned int idVCandidate=0;
			unsigned int idFCandidate=0;

			std::vector<VertexDistances>::iterator		lItV;
			std::vector<FacesDistances>::iterator		lItF;

			unsigned int lNumVertAtached	=0;
			unsigned int lNumFacesAtached	=0;

			//Selection for vertex
			unsigned int numVertices		= NeuronMesh->getNumVertex();
			unsigned int somaNumVertices	= NeuronMesh->getSomaNumVertex();

			//Selection for faces
			unsigned int numFaces		= NeuronMesh->calcNumFaces();
			unsigned int somaNumFaces	= NeuronMesh->getSomaNumFaces();

			//Incremet for dispose ramdomly the spines
			float lTmpCalc = (float)(numVertices-somaNumVertices)/(float)totalNumSpines;
			unsigned int lIncRand	= (int)lTmpCalc;
			if ( (lTmpCalc-lIncRand)>0.49 ) ++lIncRand;
			unsigned int lInfLimit	= somaNumVertices;

			initrand();

			unsigned int index=0;

			unsigned int i,k;
			*/

      unsigned int j;

      OpenMesh::Vec3f auxPoint;
      OpenMesh::Vec3f auxNormal;
      //int j=0;

      //MeshDef::VertexHandle vhandle;

      namespace ublas = boost::numeric::ublas;

      ublas::vector < float > glb_AuxVec;
      glb_AuxVec.resize ( 4 );

      ublas::vector < float > XVector;
      XVector.resize ( 3 );
      XVector[0] = 1;
      XVector[1] = 0;
      XVector[2] = 0;

      ublas::vector < float > YVector;
      YVector.resize ( 3 );
      YVector[0] = 0;
      YVector[1] = 1;
      YVector[2] = 0;

      ublas::vector < float > glb_ZeroVec;
      glb_ZeroVec.resize ( 3 );
      glb_ZeroVec[0] = glb_ZeroVec[1] = glb_ZeroVec[2] = 0;

      boost::numeric::ublas::matrix < float > glb_mat;
      glb_mat.resize ( 4, 4 );

      boost::numeric::ublas::vector < float > glb_forwardVec;
      glb_forwardVec.resize ( 3 );

      boost::numeric::ublas::vector < float > glb_DespVec;
      glb_DespVec.resize ( 3 );

      //Auxiliar forwardvector
      boost::numeric::ublas::vector < float > glb_TransVectorAux;
      glb_TransVectorAux.resize ( 3 );

      //Right vector
      boost::numeric::ublas::vector < float > glb_RightVector;
      glb_RightVector.resize ( 3 );

      //Up vector
      boost::numeric::ublas::vector < float > glb_UpVector;
      glb_UpVector.resize ( 3 );


      //Auxiliar vectors
      for ( int t = 0; t < mSpinesInfo.size ( ); ++t )
        //for (int t=0;t<100;++t)
      {

        //Select the spine modelled
        lSpineModelSelected = mSpinesInfo.at ( t ).mSpineId;

        //New version
        //////!!!!!!
        tmpMesh = new BaseMesh ( );
        tmpMesh->JoinBaseMesh ( mSpinesModeledContainer->getElementAt ( lSpineModelSelected ));

        MeshDef::ConstVertexFaceIter CVFIter;
        MeshDef::FaceHandle lAuxFaceHandle;
        std::vector <MeshDef::FaceHandle> vecFaceCands;
        int lFaceCand;
        float lLenght;
        OpenMesh::Vec3f lVecAB;
        float lSegPercetn;
        unsigned int lNumVertexToChoose = 0;

        //Colocaci�n de las espinas en los centros de las facetas
        //From vertex candidate, we choose his incident face
        MeshDef::VertexHandle vhandle;
        MeshDef::HalfedgeHandle hehandle;
        MeshDef::FaceHandle fhandle;

        MeshDef::Point lTmpVertex ( 0, 0, 0 );

        unsigned int lIniOpt;
        unsigned int lFinOpt;

        auxPoint = mSpinesInfo.at ( t ).mSpinePosition;
        auxNormal = mSpinesInfo.at ( t ).mSpineDirection;

        //Codigo comun para posicionamiento por vertice y por faceta
        /////////////////////////////////////////////////////////////////////////////////////////////////////////
        glb_direction = ( auxPoint + auxNormal*mMaxLongSpine ) - auxPoint;

        //Orientaci�n seg�n la normal
        glb_forwardVec[0] = glb_direction[0];
        glb_forwardVec[1] = glb_direction[1];
        glb_forwardVec[2] = glb_direction[2];

        //Montaje de la matriz
        generateTransformationMatrix ( glb_mat,
                                       glb_DespVec,
                                       glb_forwardVec,
                                       glb_TransVectorAux,
                                       glb_RightVector,
                                       glb_UpVector,
                                       XVector,
                                       YVector
        );

        //Unimos la malla y calculamos los vertices antes y despues
        unsigned int lIniLimit = this->getNumVertex ( );
        JoinBaseMesh ( tmpMesh );
        unsigned int lFinLimit = this->getNumVertex ( );

        //All the vertices are ready now
        for ( j = ( lIniLimit ); j < ( lFinLimit ); ++j )
        {

          //Hay q coger el precomputado
          glb_AuxVec[0] = Mesh->points ( )[j][0];
          glb_AuxVec[1] = Mesh->points ( )[j][1];
          glb_AuxVec[2] = Mesh->points ( )[j][2];
          glb_AuxVec[3] = 1.0;

          //Matrix operations
          glb_AuxVec = prod ( glb_mat, glb_AuxVec );

          //Explicit translate
          //Muchmore faster than apply the translate matrix
          //Muchmore faster than appaly the translate matrix
          glb_AuxVec[0] += auxPoint[0];
          glb_AuxVec[1] += auxPoint[1];
          glb_AuxVec[2] += auxPoint[2];

          MeshDef::VertexHandle vhandle = Mesh->vertex_handle ( j );
          Mesh->set_point ( vhandle, ( MeshDef::Point ( glb_AuxVec[0], glb_AuxVec[1], glb_AuxVec[2]
                            )
                            )
          );
        }
        delete tmpMesh;
      }
      updateBaseMesh ( );
    }
  }

  void SpinesSWC::vectorizeSpineDesp ( )
  {
    mSpineDesp.clear ( );

    //Nodo extra de indexaci�n y el soma
    //mSpineDesp.push_back(0);
    //mSpineDesp.push_back(0);

    unsigned int lPrevNodeId = 0;
    unsigned int lPrevNumSpines = 0;
    unsigned int lActNumSpines = 0;

    for ( int i = 0; i < mSpinesInfo.size ( ); ++i )
    {
      if ( mSpinesInfo.at ( i ).mIndexationNode == lPrevNodeId )
      {
        ++lActNumSpines;
      }
      else
      {

        while ( lPrevNodeId < mSpinesInfo.at ( i ).mIndexationNode )
        {
          ++lPrevNodeId;
          mSpineDesp.push_back ( lPrevNumSpines + lActNumSpines );
        }
        lPrevNumSpines = lPrevNumSpines + lActNumSpines;
        lActNumSpines = 1; //La iteraci�n ha parado cuando la ha encontrado
      }
    }
  }

    void SpinesSWC::distributeSpines(const vector<Spine> &spines,const std::string& neuronName,
            const OpenMesh::Vec3f& diplacement, BBDD::BBDD bbdd, const std::string& tmpPath) {
      mNumVerticesEnSpina = mSpinesModeledContainer->getContainer ( ).at ( 0 )->getNumVertex ( );

      SpineInfo lSpineInfo;
      mSpinesInfo.clear ( );

      //delete lpSWCSpinesDistributor;

      ///////////////////////////////////////////////////////////////////////////////////////////////////////////
      ///######

      unsigned int lNumSegmentsWithSpines = 0;
      unsigned int lNumSpinesInSegment = 0;

      BaseMesh *tmpMesh;
      unsigned int lSpineModelSelected;

      unsigned int idVCandidate = 0;
      unsigned int idFCandidate = 0;

      std::vector < VertexDistances >::iterator lItV;
      std::vector < FacesDistances >::iterator lItF;

      unsigned int lNumVertAtached = 0;
      unsigned int lNumFacesAtached = 0;

      //Selection for vertex
      unsigned int numVertices = NeuronMesh->getNumVertex ( );
      unsigned int somaNumVertices = NeuronMesh->getSomaNumVertex ( );

      //Selection for faces
      unsigned int numFaces = NeuronMesh->calcNumFaces ( );
      unsigned int somaNumFaces = NeuronMesh->getSomaNumFaces ( );

      //Incremet for dispose ramdomly the spines
      float lTmpCalc = ( float ) ( numVertices - somaNumVertices )/( float ) totalNumSpines;
      unsigned int lIncRand = ( int ) lTmpCalc;
      if (( lTmpCalc - lIncRand ) > 0.49 )
        ++lIncRand;
      unsigned int lInfLimit = somaNumVertices;

      initrand ( );

      unsigned int index = 0;

      unsigned int i, j, k;

      OpenMesh::Vec3f auxPoint;
      OpenMesh::Vec3f auxNormal;

      //MeshDef::VertexHandle vhandle;

      namespace ublas = boost::numeric::ublas;

      ublas::vector < float > glb_AuxVec;
      glb_AuxVec.resize ( 4 );

      ublas::vector < float > XVector;
      XVector.resize ( 3 );
      XVector[0] = 1;
      XVector[1] = 0;
      XVector[2] = 0;

      ublas::vector < float > YVector;
      YVector.resize ( 3 );
      YVector[0] = 0;
      YVector[1] = 1;
      YVector[2] = 0;

      ublas::vector < float > glb_ZeroVec;
      glb_ZeroVec.resize ( 3 );
      glb_ZeroVec[0] = glb_ZeroVec[1] = glb_ZeroVec[2] = 0;

      boost::numeric::ublas::matrix < float > glb_mat;
      glb_mat.resize ( 4, 4 );

      boost::numeric::ublas::vector < float > glb_forwardVec;
      glb_forwardVec.resize ( 3 );

      boost::numeric::ublas::vector < float > glb_DespVec;
      glb_DespVec.resize ( 3 );

      //Auxiliar forwardvector
      boost::numeric::ublas::vector < float > glb_TransVectorAux;
      glb_TransVectorAux.resize ( 3 );

      //Right vector
      boost::numeric::ublas::vector < float > glb_RightVector;
      glb_RightVector.resize ( 3 );

      //Up vector
      boost::numeric::ublas::vector < float > glb_UpVector;
      glb_UpVector.resize ( 3 );

      //NeuronMesh->freeFaceFusionCandidates();

      initrand ( );

      bool haveSpinesNeuron = bbdd.haveSpinesNeuron(neuronName);
      auto spinesModels = bbdd.getRandomSpines(spines.size(),tmpPath);
      int numModels = spinesModels.size();
      //Auxiliar vectors
        for ( i = 0; i < spines.size(); ++i ) {

            auto spine = spinesModels[i % numModels];

          //Select the spine modelled
          lSpineModelSelected = std::get<0>(spine);

          //New version
          //////!!!!!!
          tmpMesh = new BaseMesh();
          tmpMesh->loadModel(std::get<1>(spine));
          MeshDef::Color color = getRandomColor();
          tmpMesh->setVertexColor(tmpMesh->getMesh()->vertices_begin(),tmpMesh->getMesh()->vertices_end(),color);
          //tmpMesh->scaleBaseMesh(lSpineScale);

          MeshDef::ConstVertexFaceIter CVFIter;
          MeshDef::FaceHandle lAuxFaceHandle;
          std::vector<MeshDef::FaceHandle> vecFaceCands;
          int lFaceCand;
          float lLenght;
          OpenMesh::Vec3f lVecAB;
          float lSegPercetn;
          unsigned int lNumVertexToChoose = 0;

          //Colocaci�n de las espinas en los centros de las facetas
          //From vertex candidate, we choose his incident face
          MeshDef::VertexHandle vhandle;
          MeshDef::HalfedgeHandle hehandle;
          MeshDef::FaceHandle fhandle;

          //Selection by vertices

          MeshDef::Point lTmpVertex(0, 0, 0);

          unsigned int lIniOpt;
          unsigned int lFinOpt;

          auxPoint = spines[i].point;
          //auxNormal = NeuronMesh->getMesh()->calc_face_normal(fhandle);
          auxNormal = spines[i].finalPoint.point - spines[i].point;

          lSpineInfo.mSpineId = lSpineModelSelected;
          lSpineInfo.mSpinePosition = auxPoint;
          lSpineInfo.mSpineDirection = auxNormal;
          mSpinesInfo.push_back(lSpineInfo);

          //lNumVertexToChoose	=	(unsigned int)randfloat(0,mVertexDistancesContainer.size()-1);
          //idVCandidate		= (int)randfloat(0,lNumVertexToChoose);
          ////idVCandidate		= mVertexDistancesContainer.at(lNumVertexToChoose);

          ////Eliminamos el marcador para no volver a seleccionarlo
          //lItV = mVertexDistancesContainer.begin() + idVCandidate;

          //////!!!!!!
          //tmpMesh->scaleBaseMesh(mVertexDistancesContainer.at(idVCandidate).lDendriticRadiusFactor);


          //idVCandidate = lItV->lId;

          ////mVertexDistancesContainer.at(lItV).lUsed	=	true;

          ////Consecutive Vertex selection
          ////idVCandidate    = (int)randfloat(lInfLimit,(lInfLimit+lIncRand));
          ////lInfLimit+=lIncRand;

          /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
          ////Aki va la disposici�n aleatoria sobre las facetas
          ////Store all the incidente vertex handles

          //vhandle		=	NeuronMesh->getMesh()->vertex_handle(idVCandidate);

          //for (CVFIter=NeuronMesh->getMesh()->cvf_iter(vhandle);CVFIter.is_valid();++CVFIter)
          //{
          //	lAuxFaceHandle = NeuronMesh->getMesh()->face_handle(CVFIter->idx());
          //	vecFaceCands.push_back(lAuxFaceHandle);
          //}

          //lFaceCand = (int)randfloat(0,vecFaceCands.size()-1);
          //fhandle	= vecFaceCands.at(lFaceCand);

          ////Selecci�n de la normal de la faceta
          //auxNormal = NeuronMesh->getMesh()->calc_face_normal(fhandle);

          ////Calculo del punto sobre el que poner la espina

          //NeuronMesh->getMesh()->calc_face_centroid(fhandle,lTmpVertex);
          //auxPoint		= NeuronMesh->getMesh()->points()[idVCandidate];

          //lVecAB	=	OpenMesh::Vec3f(
          //							lTmpVertex
          //							- auxPoint
          //							);
          ////Longitud desde el vertice hasta el centro de la faceta
          //lLenght=lVecAB.length();
          //lVecAB.normalize();

          //////Colcaci�n de la espina en la mitad del segmento
          ////auxPoint		= auxPoint + lVecAB*(lLenght/2.0);

          ////COlcaci�n de la espina en la mitad del segmento

          //lSegPercetn	=	(lLenght*DESP_PERCENT)/100.0;
          //lLenght		-=	lSegPercetn;
          //lLenght		= randfloat(lSegPercetn,lLenght);

          //auxPoint		= auxPoint + lVecAB*(lLenght);

          //mVertexDistancesContainer.erase(lItV);


          /////////////////////////////////////////////////////////////////////////////////////////////////////////

          //Codigo comun para posicionamiento por vertice y por faceta
          /////////////////////////////////////////////////////////////////////////////////////////////////////////
          glb_direction = (auxPoint + auxNormal * mMaxLongSpine) - auxPoint;







          //Orientaci�n seg�n la normal
          glb_forwardVec[0] = glb_direction[0];
          glb_forwardVec[1] = glb_direction[1];
          glb_forwardVec[2] = glb_direction[2];

          //Giro de la dendrita para no alinearla exatamente con la normal
          //glb_forwardVec[0] = glb_direction[0]+randfloat(-mMaxLongSpine, mMaxLongSpine);
          //glb_forwardVec[1] = glb_direction[1];
          //glb_forwardVec[2] = glb_direction[2]+randfloat(-mMaxLongSpine, mMaxLongSpine);

          //Montaje de la matriz
          generateTransformationMatrix(glb_mat,
                                       glb_DespVec,
                                       glb_forwardVec,
                                       glb_TransVectorAux,
                                       glb_RightVector,
                                       glb_UpVector,
                                       XVector,
                                       YVector
          );
          if (!haveSpinesNeuron) {
              bbdd.addSpine(neuronName, lSpineModelSelected, diplacement, glb_mat);
          }

          //Unimos la malla y calculamos los vertices antes y despues
          unsigned int lIniLimit = this->getNumVertex();
          JoinBaseMesh(tmpMesh);
          unsigned int lFinLimit = this->getNumVertex();

          //All the vertices are ready now
          for (j = (lIniLimit); j < (lFinLimit); ++j) {

            //Hay q coger el precomputado
            glb_AuxVec[0] = Mesh->points()[j][0];
            glb_AuxVec[1] = Mesh->points()[j][1];
            glb_AuxVec[2] = Mesh->points()[j][2];
            glb_AuxVec[3] = 1.0;

            //Matrix operations
            glb_AuxVec = prod(glb_mat, glb_AuxVec);

            //Explicit translate
            //Muchmore faster than apply the translate matrix
            glb_AuxVec[0] += auxPoint[0];
            glb_AuxVec[1] += auxPoint[1];
            glb_AuxVec[2] += auxPoint[2];

            //Mesh->points()[j] = (MeshDef::Point(glb_AuxVec[0]
            //									,glb_AuxVec[1]
            //									,glb_AuxVec[2]
            //								  )
            //					);

            MeshDef::VertexHandle vhandle = Mesh->vertex_handle(j);
            Mesh->set_point(vhandle, (MeshDef::Point(glb_AuxVec[0], glb_AuxVec[1], glb_AuxVec[2]
                            )
                            )
            );
          }

          //NeuronMesh->addFaceFusionCandidate(fhandle);
          ////!!!!!!
          delete tmpMesh;

        }
      updateBaseMesh ( );

      ///####
      ///////////////////////////////////////////////////////////////////////////////////////////////////////////


    }

    MeshDef::Color SpinesSWC::getRandomColor() {
        auto qtColor = QColor(SpinesSWC::spineColors[SpinesSWC::random.bounded(0,SpinesSWC::spineColors.size())]);
        return {qtColor.redF(),qtColor.greenF(),qtColor.blueF(),1.0f};
    }
}



