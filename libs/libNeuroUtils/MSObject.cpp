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

#include "MSObject.h"

namespace NSMSObject
{
  MSObject::MSObject ( void )
  {
    //Inicializtions
    //mIntegrationMethod	=	0;
    mIntegrationMethod = 2; //Verlet


    mDt = 0.005;
    mUseConstraits = true;

    mStiffness = 20.0;
    mDumping = 0.2f;

    mSystemMass = 0.64f;
    mNodeMass = 0;
    mGravity = OpenMesh::Vec3f ( 0, -9.8, 0 );

    //mUnCollapseSpringsPercent	= 0;
    mUnCollapseSpringsPercent = 7;
  }

  MSObject::~MSObject ( void )
  {

  }

  void MSObject::generateMSBaseStructure ( )
  {
    mNodeContainer.clear ( );
    mSpringContainer.clear ( );
    mConstraints.clear ( );
    mSelectionContainer.clear ( );
    //mExternalForces.clear();

    MeshDef::Point lAuxPos;

    //recorrer todos los vertices, generar cada nodo y almacenarlo
    unsigned int i = 0;
    for ( MeshDef::ConstVertexIter v_it = mPMesh->getMesh ( )->vertices_begin ( );
          v_it != mPMesh->getMesh ( )->vertices_end ( );
          ++v_it )
    {
      lAuxPos = mPMesh->getMesh ( )->points ( )[i];
      ++i;

      MSNode lNode;
      lNode.initialPos ( OpenMesh::Vec3f ( lAuxPos[0], lAuxPos[1], lAuxPos[2] ));
      mNodeContainer.push_back ( lNode );
    }

    //Recorrer las aristas y almacenarlas como muelles
    i = 0;

    MSNode *lNodeIni;
    unsigned int lIdNodeIni = 0;

    MSNode *lNodeFin;
    unsigned int lIdNodeFin = 0;

    MeshDef::HalfedgeHandle h0;
    MeshDef::HalfedgeHandle h1;

    for ( MeshDef::ConstEdgeIter e_it = mPMesh->getMesh ( )->edges_begin ( );
          e_it != mPMesh->getMesh ( )->edges_end ( );
          ++e_it )
    {

      h0 = mPMesh->getMesh ( )->halfedge_handle ( *e_it, 0 );
      h1 = mPMesh->getMesh ( )->halfedge_handle ( *e_it, 1 );

      lIdNodeIni = mPMesh->getMesh ( )->from_vertex_handle ( h0 ).idx ( );
      lIdNodeFin = mPMesh->getMesh ( )->from_vertex_handle ( h1 ).idx ( );

      lNodeIni = &mNodeContainer[lIdNodeIni];
      lNodeFin = &mNodeContainer[lIdNodeFin];

      MSSpring lMSSpring ( lNodeIni, lNodeFin, mStiffness, mDumping );
      mSpringContainer.push_back ( lMSSpring );
    }

    //unsigned int lNumSpringsIni = mSpringContainer.size();
    //unsigned int lNumSpringsFin = 0;


    //Nodos y muelles extra
    //Anticolapso usando restricciones
    //Nodes data
    MSNode lNode;
    lNode.initialPos ( OpenMesh::Vec3f ( 0, 0, 0 ));
    mNodeContainer.push_back ( lNode );

    lIdNodeIni = mNodeContainer.size ( ) - 1;
    lNodeIni = &mNodeContainer[lIdNodeIni];
    lNodeIni->mFix = true;

    MSSpring *lSpring;

    unsigned int lNumSpringsIni = mSpringContainer.size ( ) - 1;
    unsigned int lNumSpringsFin = 0;



    //////////////////////////////////////////////////////////////////////////////////////
    //Recorrer los nodos y a�adir muelles excepto el �ltimo
    //Que es el de contenci�n central
    if ( mUnCollapseSpringsPercent > 0 )
    {

      //Calcular el n�mero de muelles
      int numUnCollapseSptrings = (( mNodeContainer.size ( )*mUnCollapseSpringsPercent )/100.0 );

      int lSpringStep = mNodeContainer.size ( )/numUnCollapseSptrings;


      //for (i=0;i<(mNodeContainer.size()-1);++i)
      for ( i = 0; i < ( mNodeContainer.size ( ) - 1 ); i += lSpringStep )
      {
        lIdNodeFin = i;
        lNodeFin = &mNodeContainer[lIdNodeFin];

        MSSpring lMSSpring ( lNodeIni, lNodeFin, mStiffness, mDumping );
        mSpringContainer.push_back ( lMSSpring );

        ++lNumSpringsFin;
      }
    }
    //////////////////////////////////////////////////////////////////////////////////////


    //Las constraints se gener� siempre, luego se decide si usarla o no.
    for ( i = 0; i < lNumSpringsFin; ++i )
    {
      MSSpringConstraint lMSSpringConstraint
        ( &( mSpringContainer[lNumSpringsIni + i] ), MSSpringConstraint::constraint_type::MINLENGT );
      mConstraints.push_back ( lMSSpringConstraint );
    }

  }

  void MSObject::destroyMSBaseStructure ( )
  {

    mNodeContainer.clear ( );
    mSpringContainer.clear ( );
    mConstraints.clear ( );

    //mExternalForces.clear();

    mSelectionContainer.clear ( );
  }

  void MSObject::setMSIntegrator ( int pMethod )
  {
    mIntegrationMethod = pMethod;
  }

  void MSObject::setMSDt ( float pDt )
  {
    mDt = pDt;
  }

  void MSObject::setMSConstraints ( bool pUseConstraits )
  {
    mUseConstraits = pUseConstraits;
  }

  void MSObject::setMSSpringParams ( float pStiffness, float pDumping )
  {
    //Update the stiffnes
    if (( mStiffness != pStiffness ) || ( mDumping != pDumping ))
    {
      mStiffness = pStiffness;
      mDumping = pDumping;
      unsigned int lNumSprings = mSpringContainer.size ( );
      for ( unsigned int i = 0; i < lNumSprings; ++i )
      {
        mSpringContainer.at ( i ).mStiffness = pStiffness;
        mSpringContainer.at ( i ).mDumping = pDumping;
      }
    }
  }

  void MSObject::setUnCollapseSprings ( int pUnCollapseSpringsPercent )
  {
    mUnCollapseSpringsPercent = pUnCollapseSpringsPercent;
  }

  void MSObject::setMSNodeParams ( float pMass, OpenMesh::Vec3f pGravity )
  {
    //Update the stiffnes
    if (
      ( mSystemMass != pMass )
        || ( mGravity != pGravity )
      )
    {
      mSystemMass = pMass;
      mGravity = pGravity;

      unsigned int lNumNodes = mNodeContainer.size ( );

      mNodeMass = mSystemMass/lNumNodes;

      for ( unsigned int i = 0; i < lNumNodes; ++i )
      {
        mNodeContainer.at ( i ).setSimulationParams ( mNodeMass, mGravity );
      }
    }
  }

  vector <MSNode> &MSObject::getNodeContainer ( )
  {
    return mNodeContainer;
  }

  vector <MSSpring> &MSObject::getSpringContainer ( )
  {
    return mSpringContainer;
  }

  std::list < int > &MSObject::getSelectionContainer ( )
  {
    return mSelectionContainer;
  }

  //Actualizamos los nodos
  void MSObject::update ( )
  {
    //Reseteamos las fuerzas de los nodos
    for ( unsigned int i = 0; i < mNodeContainer.size ( ); ++i )
    {
      mNodeContainer.at ( i ).mForce = OpenMesh::Vec3f ( 0, 0, 0 );
    }

    //Actualizamos las fuerzas de los muelles
    for ( unsigned int i = 0; i < mSpringContainer.size ( ); ++i )
    {
      mSpringContainer.at ( i ).updateForce ( );
      mSpringContainer.at ( i ).mIniNode->mForce += mSpringContainer.at ( i ).mForce;
      mSpringContainer.at ( i ).mFinNode->mForce -= mSpringContainer.at ( i ).mForce;

      //// add all external forces to each node
      //for (j = 0; j < externalForces_.size(); j++)
      //{
      //	springs_.at(i)->na->F += externalForces_.at(j);
      //	springs_.at(i)->nb->F += externalForces_.at(j);
      //}

    }

    //Actualizamos los nodos
    for ( unsigned int i = 0; i < mNodeContainer.size ( ); ++i )
    {
      //mNodeContainer.at(i).initialPos( mNodeContainer.at(i).getPos()+OpenMesh::Vec3f(0,0.1,0) );
      if ( !mNodeContainer.at ( i ).mFix )
      {
        mNodeContainer.at ( i ).integration ( mDt, mIntegrationMethod );
      }
    }


    //Apply the constraints
    //Actualizamos los nodos
    //if (mUseConstraits) //Da problemas en algunos nodos locos.
    if ( false )
    {
      for ( unsigned int i = 0; i < mConstraints.size ( ); ++i )
      {
        mConstraints[i].applyConstraints ( );
      }
    }

    //Actualizamos las posiciones finales
    updateMeshPosition ( );
  }

  //Actualizamos la malla en funcion de los nodos
  void MSObject::updateMeshPosition ( )
  {
    //aqu� actualizar la malla
    OpenMesh::Vec3f lAuxVec;
    MeshDef::Point lAuxPoint;

    unsigned int i = 0;
    for ( MeshDef::ConstVertexIter v_it = mPMesh->getMesh ( )->vertices_begin ( );
          v_it != mPMesh->getMesh ( )->vertices_end ( );
          ++v_it )
    {
      lAuxVec = mNodeContainer.at ( i ).getPos ( );
      lAuxPoint = MeshDef::Point ( lAuxVec[0], lAuxVec[1], lAuxVec[2] );

      //mPMesh->getMesh()->points()[i] = lAuxPoint;
      mPMesh->getUnprotectedMesh ( )->set_point ( *v_it, lAuxPoint );

      ++i;
    }
  }

  int MSObject::getParamIntegrationMethod ( )
  {
    return mIntegrationMethod;
  }

  float MSObject::getParamDt ( )
  {
    return mDt;
  }

  bool MSObject::getParamUseConstraits ( )
  {
    return mUseConstraits;
  }

  float MSObject::getParamStiffness ( )
  {
    return mStiffness;
  }

  float MSObject::getParamDumping ( )
  {
    return mDumping;
  }

  int MSObject::getParamUnCollapseSpringsPercent ( )
  {
    return mUnCollapseSpringsPercent;
  }

  float MSObject::getParamNodeMass ( )
  {
    return mNodeMass;
  }

  float MSObject::getParamSystemMass ( )
  {
    return mSystemMass;
  }

  OpenMesh::Vec3f MSObject::getParamGravity ( )
  {
    return mGravity;
  }

}



//void Viewer::renderSilhouettes() const
//{
//  Vec L = light->position();
//  glBegin(GL_LINES);
//  for (Mesh::ConstEdgeIter e=mesh.edges_begin();e!=mesh.edges_end();++e)
//  {
//    Mesh::HalfedgeHandle h0 = mesh.halfedge_handle(e,0);
//    Mesh::HalfedgeHandle h1 = mesh.halfedge_handle(e,1);
//    Vec F(mesh.point(mesh.from_vertex_handle(h0)));
//    Vec T(mesh.point(mesh.from_vertex_handle(h1)));
//    Vec n0(mesh.normal(mesh.face_handle(h0)));
//    Vec n1(mesh.normal(mesh.face_handle(h1)));
//    float ps0 = ((F-L)*n0);
//    float ps1 = ((F-L)*n1);
//    if (ps0*ps1 < 0)
//    {
//      glVertex3fv(F);
//      glVertex3fv(T);
//    }
//  }  
//  glEnd();
//}
