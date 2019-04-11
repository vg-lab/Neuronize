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

#include "NeuronGraphOptimizer.h"

#include <libs/libSysNeuroUtils/randomUtils.h>

//NeuronGraphOptimizer::NeuronGraphOptimizer(const NeuronGraph & pOriginalGraph, NeuronGraph & pOptimizedGraph)
//	:mOriginalGraph(pOriginalGraph)

NeuronGraphOptimizer::NeuronGraphOptimizer ( NeuronGraph &pOriginalGraph, NeuronGraph &pOptimizedGraph )
  : mOriginalGraph ( pOriginalGraph ), mOptimizedGraph ( pOptimizedGraph )
{
  mIniValue = 0;
  mFinalValue = 0;

  mOptimizationMethod = 0;

  mNPoints = 1;
  mRadialDistance = 0.1;
  mPerpendDistance = 0.1;
  mRWDistance = 0.1;
  mOpheinMinThreshold = 0.1;
  mOpheinMaxThreshold = 0.1;
  mLangThreshold = 0.1;
  mLangSize = 0.1;
  mDPThreshold = 0.1;
  mDPModifNumPoints = 1;
}

NeuronGraphOptimizer::~NeuronGraphOptimizer ( void )
{
}

void NeuronGraphOptimizer::setSimplificationInterval ( unsigned int pIniVertex, unsigned int pFinalVertex )
{
  mIniValue = pIniVertex;
  mFinalValue = pFinalVertex;
}

void NeuronGraphOptimizer::setOptimizeParams ( int pNPoints,
                                               float pRadialDistance,
                                               float pPerpendDistance,
                                               float pRWDistance,
                                               float pOpheinMinThreshold,
                                               float pOpheinMaxThreshold,
                                               float pLangThreshold,
                                               float pLangSize,
                                               float pDPThreshold,
                                               int pDPModifNumPoints )
{
  mNPoints = pNPoints;
  mRadialDistance = pRadialDistance;
  mPerpendDistance = pPerpendDistance;
  mRWDistance = pRWDistance;
  mOpheinMinThreshold = pOpheinMinThreshold;
  mOpheinMaxThreshold = pOpheinMaxThreshold;
  mLangThreshold = pLangThreshold;
  mLangSize = pLangSize;
  mDPThreshold = pDPThreshold;
  mDPModifNumPoints = pDPModifNumPoints;
}

void NeuronGraphOptimizer::optimize ( )
{
  GraphSWCNode lGraphSWCNode;
  OpenMesh::Vec3f lLocalPos, lLocalPos2;
  mOptimizedGraph.NGraph.clear ( );

  boost::tie ( mOriginalGraph.vi, mOriginalGraph.vi_end ) = vertices ( mOriginalGraph.NGraph );
  boost::tie ( mOptimizedGraph.vi, mOptimizedGraph.vi_end ) = vertices ( mOptimizedGraph.NGraph );
  mOptimizedGraph.node_id = get ( boost::vertex_name, mOptimizedGraph.NGraph );

  lGraphSWCNode.NewId
    = lGraphSWCNode.NewParent
    = lGraphSWCNode.OriginalNode.id
    = lGraphSWCNode.OriginalNode.mDendriticDistanceNorm
    = lGraphSWCNode.OriginalNode.mDendriticDistanceReal
    = lGraphSWCNode.OriginalNode.mDistanceToSoma
    = lGraphSWCNode.OriginalNode.parent
    = lGraphSWCNode.OriginalNode.radius
    = lGraphSWCNode.OriginalNode.type = 0;

  std::vector <NeuronGraph::Vertex> mVertex;
  unsigned int lNumPrevAct;
  unsigned int lNewParentId = 0;

  mVertex.clear ( );

  //Anyadimos el nodo 0 para la indexaci�n
  mVertex.push_back ( boost::add_vertex ( lGraphSWCNode, mOptimizedGraph.NGraph ));

  //Anyadimos el soma
  lGraphSWCNode.OriginalNode = mOriginalGraph.mSWCImporter->getElementAt ( 1 );
  lGraphSWCNode.NewId = 1;
  lGraphSWCNode.NewParent = -1;

  lGraphSWCNode.isBifurcation = true;
  lGraphSWCNode.isTermination = false;

  mVertex.push_back ( boost::add_vertex ( lGraphSWCNode, mOptimizedGraph.NGraph ));

  lGraphSWCNode.NewId = 1;

  int lDim = mFinalValue - mIniValue;

  //Vector de importancias de test
  std::vector < float > lImportancias;
  std::vector < float > lErrors;

  /*
  if (lDim>0)
  {
    lImportancias.resize(lDim);
    for (int i=0;i<lDim;++i)
      lImportancias[i] = randfloat(0, 5);
  }
  */

  if ( lDim < 0 )
    return;
  else
  {
    //Esto es para si queremos simplificar un trozito de la neurona nada m�s, es decir desde donde hasta donde
    if ( lDim > 0 )
    {
      ++lDim;

      mSimplifiedCoords.clear ( );

      boost::tie ( mOriginalGraph.vi, mOriginalGraph.vi_end ) = vertices ( mOriginalGraph.NGraph );
      mOriginalGraph.vi_tmp = mOriginalGraph.vi;
      for ( unsigned int k = 0; k < mIniValue; ++k )
        ++mOriginalGraph.vi_tmp;

      float *generatedPoints = new float[lDim*3];

      for ( int c = 0; c < lDim; c++ )
      {
        generatedPoints[c*3] = mOriginalGraph.node_id[*mOriginalGraph.vi_tmp].OriginalNode.position[0];
        generatedPoints[c*3 + 1] = mOriginalGraph.node_id[*mOriginalGraph.vi_tmp].OriginalNode.position[1];
        generatedPoints[c*3 + 2] = mOriginalGraph.node_id[*mOriginalGraph.vi_tmp].OriginalNode.position[2];

        ++mOriginalGraph.vi_tmp;
      }

      float *begin = generatedPoints;
      float *end = generatedPoints + ( lDim*3 );

      ////////////////////////////////////////////////////////////////////////////////
      switch ( mOptimizationMethod )
      {
        case 1:
          imppsimpl::simplify_nth_point < 3 > ( begin, end, mNPoints,
            //std::back_inserter (mSimplifiedCoords), lImportancias, lErrors);
                                                std::back_inserter ( mSimplifiedCoords ));

          break;

        case 2:
          imppsimpl::simplify_radial_distance < 3 > ( begin, end, mRadialDistance,
                                                      std::back_inserter ( mSimplifiedCoords ));
          break;

        case 3:
          imppsimpl::simplify_perpendicular_distance < 3 > ( begin, end, mPerpendDistance,
                                                             std::back_inserter ( mSimplifiedCoords ));
          break;

        case 4:
          imppsimpl::simplify_reumann_witkam < 3 > ( begin, end, mRWDistance,
                                                     std::back_inserter ( mSimplifiedCoords ));
          break;

        case 5:
          imppsimpl::simplify_opheim < 3 > ( begin, end, mOpheinMinThreshold, mOpheinMaxThreshold,
                                             std::back_inserter ( mSimplifiedCoords ));
          break;

        case 6:
          imppsimpl::simplify_lang < 3 > ( begin, end, mLangThreshold, mLangSize,
                                           std::back_inserter ( mSimplifiedCoords ));
          break;

        case 7:
          imppsimpl::simplify_douglas_peucker < 3 > ( begin, end, mDPThreshold,
                                                      std::back_inserter ( mSimplifiedCoords ));
          break;

        case 8:
          imppsimpl::simplify_douglas_peucker_n < 3 > ( begin, end, mDPModifNumPoints,
                                                        std::back_inserter ( mSimplifiedCoords ));
          break;
      }
      ////////////////////////////////////////////////////////////////////////////////

      delete[] generatedPoints;

      for ( unsigned int i = 0; i < mSimplifiedCoords.size ( ); i += 3 )
      {
        lGraphSWCNode.OriginalNode.position[0] = mSimplifiedCoords.at ( i );
        lGraphSWCNode.OriginalNode.position[1] = mSimplifiedCoords.at ( i + 1 );
        lGraphSWCNode.OriginalNode.position[2] = mSimplifiedCoords.at ( i + 2 );

        boost::add_vertex ( lGraphSWCNode, mOptimizedGraph.NGraph );
      }

    }
      //Este else es para simplificarla totalmente, es decir, simplificar todas y cada una de sus ramas en funci�n
      //del criterio seleccionado.
    else
    {
      for ( unsigned int i = 0; i < mOriginalGraph.mSWCImporter->getNumBranches ( ); ++i )
      {
        //Calculamos los nodos iniciales y finales de la rama actual
        mIniValue = mOriginalGraph.mSWCImporter->getBranchAt ( i ).initialNode;
        mFinalValue = mOriginalGraph.mSWCImporter->getBranchAt ( i ).finalNode;

        lDim = mFinalValue - mIniValue;
        ++lDim;

        int lExtraNode;
        mSimplifiedCoords.clear ( );

        float *generatedPoints =
          NULL; //Vector temporal que almacenara las corrdenadas a simplificar y las simplificadas tras la llamada a los metodos

        //Reseteamos los punteros del grafo
        boost::tie ( mOriginalGraph.vi, mOriginalGraph.vi_end ) = vertices ( mOriginalGraph.NGraph );
        mOriginalGraph.vi_tmp = mOriginalGraph.vi;

        //Si el padre es el soma??? Los nodos adyacentes al soma siempre se dejan, aqu� los tenemos en cuenta y los dejamos
        if ( mOriginalGraph.mSWCImporter->getElementAt ( mIniValue ).parent == 1 )
        {
          lExtraNode = 0;
          generatedPoints = new float[( lDim + lExtraNode )*3];

          //Hemos de partir desde el padre, luego hemos de retorceder uno
          for ( unsigned int k = 0; k < ( mIniValue ); ++k )
            ++mOriginalGraph.vi_tmp;
        }
        else
        {
          lExtraNode = 1;
          generatedPoints = new float[( lDim + lExtraNode )*3];

          //Caso de rama consecutiva en bifurcaci�n
          if ( mOriginalGraph.mSWCImporter->getBranchAt ( i ).consecBranch )
          {
            //Hemos de partir desde el padre, luego hemos de retorceder uno
            for ( unsigned int k = 0; k < ( mIniValue - lExtraNode ); ++k )
              ++mOriginalGraph.vi_tmp;

            //Copiamos el padre
            generatedPoints[0] = mOriginalGraph.node_id[*mOriginalGraph.vi_tmp].OriginalNode.position[0];
            generatedPoints[1] = mOriginalGraph.node_id[*mOriginalGraph.vi_tmp].OriginalNode.position[1];
            generatedPoints[2] = mOriginalGraph.node_id[*mOriginalGraph.vi_tmp].OriginalNode.position[2];

            lNewParentId = mOriginalGraph.node_id[*mOriginalGraph.vi_tmp].OriginalNode.id;

            ++mOriginalGraph.vi_tmp;
          }
            //Rama no consecutiva, con lo cual el padre que es la bifurcaci�n no se puede eliminar, ya lo ha a�adido
            //la rama que era consecutiva y adem�s hay que tener en cuenta que el nodo siguiente al bifurcaci�n no consecutivo
            //no se a�ade como inamovible (se puede eliminar)
          else
          {
            for ( int k = 0; k < mOriginalGraph.mSWCImporter->getElementAt ( mIniValue ).parent; ++k )
              ++mOriginalGraph.vi_tmp;

            //Copiamos el padre
            generatedPoints[0] = mOriginalGraph.node_id[*mOriginalGraph.vi_tmp].OriginalNode.position[0];
            generatedPoints[1] = mOriginalGraph.node_id[*mOriginalGraph.vi_tmp].OriginalNode.position[1];
            generatedPoints[2] = mOriginalGraph.node_id[*mOriginalGraph.vi_tmp].OriginalNode.position[2];

            lNewParentId = mOriginalGraph.node_id[*mOriginalGraph.vi_tmp].OriginalNode.id;

            for ( unsigned int k = ( unsigned int ) mOriginalGraph.mSWCImporter->getElementAt ( mIniValue ).parent;
                  k < mIniValue; ++k )
              ++mOriginalGraph.vi_tmp;;
          }
        }

        //Copiamos el resto de nodos de esa rama
        for ( int c = 0 + lExtraNode; c < lDim + lExtraNode; c++ )
        {
          generatedPoints[c*3] = mOriginalGraph.node_id[*mOriginalGraph.vi_tmp].OriginalNode.position[0];
          generatedPoints[c*3 + 1] = mOriginalGraph.node_id[*mOriginalGraph.vi_tmp].OriginalNode.position[1];
          generatedPoints[c*3 + 2] = mOriginalGraph.node_id[*mOriginalGraph.vi_tmp].OriginalNode.position[2];

          ++mOriginalGraph.vi_tmp;
        }

        float *begin = generatedPoints;
        float *end = generatedPoints + (( lDim + lExtraNode )*3 );

        //Prueba temporal

        //Ojo hay que tener cuidado cuando se enlaza con nodos m�s anteriores ala hora de pillar la importancia
        if ( lDim > 0 )
        {
          lImportancias.resize ( lDim + lExtraNode );
          lErrors.resize ( lDim + lExtraNode );

          for ( int j = 0; j < ( lDim + lExtraNode ); ++j )
            lImportancias[j] = randfloat ( 0, 10 );
        }

        //Se hace la simplificaci�n
        ////////////////////////////////////////////////////////////////////////////////
        switch ( mOptimizationMethod )
        {
          case 1:
            imppsimpl::simplify_nth_point < 3 > ( begin, end, mNPoints,
              //std::back_inserter (mSimplifiedCoords), lImportancias, lErrors);
                                                  std::back_inserter ( mSimplifiedCoords ));
            break;

          case 2:
            imppsimpl::simplify_radial_distance < 3 > ( begin, end, mRadialDistance,
                                                        std::back_inserter ( mSimplifiedCoords ));
            break;

          case 3:
            imppsimpl::simplify_perpendicular_distance < 3 > ( begin, end, mPerpendDistance,
                                                               std::back_inserter ( mSimplifiedCoords ));
            break;

          case 4:
            imppsimpl::simplify_reumann_witkam < 3 > ( begin, end, mRWDistance,
                                                       std::back_inserter ( mSimplifiedCoords ));
            break;

          case 5:
            imppsimpl::simplify_opheim < 3 > ( begin, end, mOpheinMinThreshold, mOpheinMaxThreshold,
                                               std::back_inserter ( mSimplifiedCoords ));
            break;

          case 6:
            imppsimpl::simplify_lang < 3 > ( begin, end, mLangThreshold, mLangSize,
                                             std::back_inserter ( mSimplifiedCoords ));
            break;

          case 7:
            imppsimpl::simplify_douglas_peucker < 3 > ( begin, end, mDPThreshold,
                                                        std::back_inserter ( mSimplifiedCoords ));
            break;

          case 8:
            imppsimpl::simplify_douglas_peucker_n < 3 > ( begin, end, mDPModifNumPoints,
                                                          std::back_inserter ( mSimplifiedCoords ));
            break;
        }
        ////////////////////////////////////////////////////////////////////////////////


        delete[] generatedPoints;

        //For edges
        lNumPrevAct = boost::num_vertices ( mOptimizedGraph.NGraph );

        //Add Vertexs
        //Anyadimos los nuevos v�rtices del grafo (nodos realmente)
        //Utilizamos la posicion para encontrar el nodo en el grafo, copiamos toda su info desde le grafo original
        //Y lo a�adimos la grafo simplificado.
        for ( unsigned int j = ( lExtraNode*3 ); j < mSimplifiedCoords.size ( ); j += 3 )
        {
          lGraphSWCNode.NewId++;

          //Avanzamos de 3 en 3
          lLocalPos[0] = lGraphSWCNode.OriginalNode.position[0] = mSimplifiedCoords.at ( j );
          lLocalPos[1] = lGraphSWCNode.OriginalNode.position[1] = mSimplifiedCoords.at ( j + 1 );
          lLocalPos[2] = lGraphSWCNode.OriginalNode.position[2] = mSimplifiedCoords.at ( j + 2 );

          //int lDim = mFinalValue-mIniValue;
          int n = mFinalValue;
          bool lFound = false;

          //Recorremos hacia atr�s porque el ultimo nodo de cada rama siempre va a estar
          //Esto es muy poco eficiente)
          while ( !lFound )
          {
            //Para localizar el nodo comparamos por su posici�n (No poseemos m�s informaci�n)
            if ( lLocalPos == mOriginalGraph.mSWCImporter->getElementAt ( n ).position )
            {
              lFound = true;

              lGraphSWCNode.isBifurcation = mOriginalGraph.mSWCImporter->isDendriticBifurcation ( n );
              lGraphSWCNode.isTermination = mOriginalGraph.mSWCImporter->isDendriticTermination ( n );

              lGraphSWCNode.OriginalNode.radius = mOriginalGraph.mSWCImporter->getElementAt ( n ).radius;
              lGraphSWCNode.OriginalNode.id = mOriginalGraph.mSWCImporter->getElementAt ( n ).id;
              lGraphSWCNode.OriginalNode.type = mOriginalGraph.mSWCImporter->getElementAt ( n ).type;
              lGraphSWCNode.OriginalNode.parent = lNewParentId;

            }
            --n;
          }

          //Se a�ade el nodo
          mVertex.push_back ( boost::add_vertex ( lGraphSWCNode, mOptimizedGraph.NGraph ));
        }

        //Add edges
        //Caso de que le padre sea el soma
        if ( mOriginalGraph.mSWCImporter->getElementAt ( mIniValue ).parent == 1 )
        {
          boost::add_edge ( mVertex[lNumPrevAct], mVertex[1], mOptimizedGraph.NGraph );
        }
          //Resto de casos
        else
        {
          //Si es una rama consecutiva se enlaza con el anterior
          if ( mOriginalGraph.mSWCImporter->getBranchAt ( i ).consecBranch )
          {
            boost::add_edge ( mVertex[lNumPrevAct], mVertex[lNumPrevAct - 1], mOptimizedGraph.NGraph );
          }
            //Caso de no serlo, se ha de encontrar al padre
          else
          {
            int n = mVertex.size ( ) - 1;
            bool lFound = false;
            while ( !lFound )
            {
              if ( mOptimizedGraph.node_id[mVertex[lNumPrevAct]].OriginalNode.parent
                == mOptimizedGraph.node_id[mVertex[n]].OriginalNode.id )
              {
                lFound = true;
              }
              else
              {
                --n;
              }
            }
            boost::add_edge ( mVertex[lNumPrevAct], mVertex[n], mOptimizedGraph.NGraph );
          }
        }

        for ( unsigned int j = lNumPrevAct; j < boost::num_vertices ( mOptimizedGraph.NGraph ) - 1; ++j )
        {
          boost::add_edge ( mVertex[j], mVertex[j + 1], mOptimizedGraph.NGraph );
        }

      }
      mFinalValue = mIniValue = 0;
    }
  }

  //Update parents ids
  NeuronGraph::Vertex lVertexDescIni;
  NeuronGraph::Vertex lVertexDescFin;

  for ( boost::tie ( mOptimizedGraph.ei, mOptimizedGraph.ei_end ) = boost::edges ( mOptimizedGraph.NGraph );
        mOptimizedGraph.ei != mOptimizedGraph.ei_end; ++mOptimizedGraph.ei )
  {
    lVertexDescIni = boost::source ( *mOptimizedGraph.ei, mOptimizedGraph.NGraph );
    lVertexDescFin = boost::target ( *mOptimizedGraph.ei, mOptimizedGraph.NGraph );

    //Detectamos el orden de la conectividad
    if ( mOptimizedGraph.node_id[lVertexDescIni].OriginalNode.id
      > mOptimizedGraph.node_id[lVertexDescFin].OriginalNode.id )
    {
      mOptimizedGraph.node_id[lVertexDescIni].NewParent = mOptimizedGraph.node_id[lVertexDescFin].NewId;
    }
    else
    {
      mOptimizedGraph.node_id[lVertexDescFin].NewParent = mOptimizedGraph.node_id[lVertexDescIni].NewId;
    }
  }

}

void NeuronGraphOptimizer::directDeleteVertexs ( unsigned int pStep )
{
  //Es que no se ha simplificado, hemos de copiarlo para aplicar esta optimizaci�n
  if ( mOptimizedGraph.getNumNodes ( ) == 0 )
  {
    mOptimizedGraph.NGraph.clear ( );
    mOptimizedGraph.NGraph = mOriginalGraph.NGraph;

    //boost::tie(mOriginalGraph.vi, mOriginalGraph.vi_end)	= vertices(mOriginalGraph.NGraph);
    boost::tie ( mOptimizedGraph.vi, mOptimizedGraph.vi_end ) = vertices ( mOptimizedGraph.NGraph );
    mOptimizedGraph.node_id = get ( boost::vertex_name, mOptimizedGraph.NGraph );
  }

  int i = 0;
  int j = 0;
  int lInters = 0;

  if ( mFinalValue < mIniValue )
    return;

  if ( mFinalValue != 0 )
    lInters = ( mFinalValue - mIniValue ) + 1;
  else
    lInters = ( mOptimizedGraph.getNumNodes ( ) - mIniValue ) + 1;

  mOptimizedGraph.vi_tmp = mOptimizedGraph.vi;
  for ( unsigned int k = 0; k < mIniValue; ++k )
    ++mOptimizedGraph.vi_tmp;

  for ( mOptimizedGraph.next = mOptimizedGraph.vi_tmp; ( mOptimizedGraph.vi_tmp != mOptimizedGraph.vi_end )
    && ( j < lInters ); mOptimizedGraph.vi_tmp = mOptimizedGraph.next )
  {
    //Se borra si no es bifurcacion, terminaci�n o soma
    ++mOptimizedGraph.next;
    ++j;
    if (( mOptimizedGraph.node_id[*mOptimizedGraph.vi_tmp].OriginalNode.parent != 1 )
      && ( mOptimizedGraph.node_id[*mOptimizedGraph.vi_tmp].OriginalNode.id != 1 )
      && ( !mOptimizedGraph.node_id[*mOptimizedGraph.vi_tmp].isBifurcation )
      && ( !mOptimizedGraph.node_id[*mOptimizedGraph.vi_tmp].isTermination )
      )
    {
      if (( i%pStep ) == 0 )
      {
        mOptimizedGraph.aip_it2 = mOptimizedGraph.aip_it =
          boost::adjacent_vertices ( *mOptimizedGraph.vi_tmp, mOptimizedGraph.NGraph );
        ++mOptimizedGraph.aip_it2.first;

        boost::add_edge ( *mOptimizedGraph.aip_it.first, *mOptimizedGraph.aip_it2.first, mOptimizedGraph.NGraph );

        clear_vertex ( *mOptimizedGraph.vi_tmp, mOptimizedGraph.NGraph );
        boost::remove_vertex ( *mOptimizedGraph.vi_tmp, mOptimizedGraph.NGraph );

        i = 0;
      }
      ++i;
    }
  }

  recalculateConectivity ( );
}

void NeuronGraphOptimizer::recalculateConectivity ( )
{
  //Update the conectivity (ids and parent!!! )
  boost::tie ( mOptimizedGraph.vi, mOptimizedGraph.vi_end ) = vertices ( mOptimizedGraph.NGraph );

  //Desechamos el Soma
  mOptimizedGraph.vi_tmp = mOptimizedGraph.vi;

  mOptimizedGraph.node_id[*mOptimizedGraph.vi].NewParent = -1;
  mOptimizedGraph.node_id[*mOptimizedGraph.vi].OriginalNode.parent = -1;

  int i = 0;
  //Actualizamos todos los ids de los nodos
  for ( mOptimizedGraph.next = mOptimizedGraph.vi; mOptimizedGraph.vi_tmp != ( mOptimizedGraph.vi_end );
        mOptimizedGraph.vi_tmp = mOptimizedGraph.next )
  {
    ++mOptimizedGraph.next;
    //clear_vertex(*vi_tmp, NGraph);
    mOptimizedGraph.node_id[*mOptimizedGraph.vi_tmp].NewId = i;
    ++i;
  }
  boost::tie ( mOptimizedGraph.vi, mOptimizedGraph.vi_end ) = vertices ( mOptimizedGraph.NGraph );

  NeuronGraph::Vertex lVertexDescIni;
  NeuronGraph::Vertex lVertexDescFin;

  for ( boost::tie ( mOptimizedGraph.ei, mOptimizedGraph.ei_end ) = boost::edges ( mOptimizedGraph.NGraph );
        mOptimizedGraph.ei != mOptimizedGraph.ei_end; ++mOptimizedGraph.ei )
  {
    lVertexDescIni = boost::source ( *mOptimizedGraph.ei, mOptimizedGraph.NGraph );
    lVertexDescFin = boost::target ( *mOptimizedGraph.ei, mOptimizedGraph.NGraph );

    //Detectamos el orden de la conectividad
    if ( mOptimizedGraph.node_id[lVertexDescIni].OriginalNode.id
      > mOptimizedGraph.node_id[lVertexDescFin].OriginalNode.id )
    {
      mOptimizedGraph.node_id[lVertexDescIni].NewParent = mOptimizedGraph.node_id[lVertexDescFin].NewId;
    }
    else
    {
      mOptimizedGraph.node_id[lVertexDescFin].NewParent = mOptimizedGraph.node_id[lVertexDescIni].NewId;
    }
  }

  boost::tie ( mOptimizedGraph.vi, mOptimizedGraph.vi_end ) = vertices ( mOptimizedGraph.NGraph );
  boost::tie ( mOptimizedGraph.ei, mOptimizedGraph.ei_end ) = boost::edges ( mOptimizedGraph.NGraph );

}

void NeuronGraphOptimizer::copyGraph ( )
{
  unsigned int lOldOptimizationMethod = mOptimizationMethod;
  unsigned int lOldNPoints = mNPoints;

  mOptimizationMethod = 1;
  mNPoints = 0;

  optimize ( );

  mOptimizationMethod = lOldOptimizationMethod;
  mNPoints = lOldNPoints;
}

void NeuronGraphOptimizer::applyNodeSplineDistribution ( unsigned int pNumNodesPerSegment )
{
  GraphSWCNode lGraphSWCNode;
  OpenMesh::Vec3f lLocalPos, lLocalPos2;
  mOptimizedGraph.NGraph.clear ( );

  boost::tie ( mOriginalGraph.vi, mOriginalGraph.vi_end ) = vertices ( mOriginalGraph.NGraph );
  boost::tie ( mOptimizedGraph.vi, mOptimizedGraph.vi_end ) = vertices ( mOptimizedGraph.NGraph );
  mOptimizedGraph.node_id = get ( boost::vertex_name, mOptimizedGraph.NGraph );

  lGraphSWCNode.NewId
    = lGraphSWCNode.NewParent
    = lGraphSWCNode.OriginalNode.id
    = lGraphSWCNode.OriginalNode.mDendriticDistanceNorm
    = lGraphSWCNode.OriginalNode.mDendriticDistanceReal
    = lGraphSWCNode.OriginalNode.mDistanceToSoma
    = lGraphSWCNode.OriginalNode.parent
    = lGraphSWCNode.OriginalNode.radius
    = lGraphSWCNode.OriginalNode.type = 0;

  std::vector <NeuronGraph::Vertex> mVertex;

  mVertex.clear ( );

  //Anyadimos el nodo 0 para la indexaci�n
  mVertex.push_back ( boost::add_vertex ( lGraphSWCNode, mOptimizedGraph.NGraph ));

  //Anyadimos el soma
  lGraphSWCNode.OriginalNode = mOriginalGraph.mSWCImporter->getElementAt ( 1 );
  lGraphSWCNode.NewId = 1;
  lGraphSWCNode.NewParent = -1;
  lGraphSWCNode.isBifurcation = true;
  lGraphSWCNode.isTermination = false;
  mVertex.push_back ( boost::add_vertex ( lGraphSWCNode, mOptimizedGraph.NGraph ));

  int lDim = mFinalValue - mIniValue;

  //Vector de importancias de test
  std::vector < float > lImportancias;
  std::vector < float > lErrors;

  //unsigned int lNewParentId=0;

  if ( lDim < 0 )
    return;
  else
  {
    //Esto es para si queremos simplificar un trozito de la neurona nada m�s, es decir desde donde hasta donde
    if ( lDim > 0 )
    {
      ++lDim;

      mSimplifiedCoords.clear ( );

      boost::tie ( mOriginalGraph.vi, mOriginalGraph.vi_end ) = vertices ( mOriginalGraph.NGraph );
      mOriginalGraph.vi_tmp = mOriginalGraph.vi;
      for ( unsigned int k = 0; k < mIniValue; ++k )
        ++mOriginalGraph.vi_tmp;

      vector <glm::vec3> lOriginalPoints;
      vector < float > lRadius;

      for ( int c = 0; c < lDim; c++ )
      {
        glm::vec3 lvec3 ( mOriginalGraph.node_id[*mOriginalGraph.vi_tmp].OriginalNode.position[0],
                          mOriginalGraph.node_id[*mOriginalGraph.vi_tmp].OriginalNode.position[1],
                          mOriginalGraph.node_id[*mOriginalGraph.vi_tmp].OriginalNode.position[2] );

        lOriginalPoints.push_back ( lvec3 );
        lRadius.push_back ( mOriginalGraph.node_id[*mOriginalGraph.vi_tmp].OriginalNode.radius );
        ++mOriginalGraph.vi_tmp;
      }

      const int lNumOriNodes = lOriginalPoints.size ( );
      vector < float > lTimes;

      float lRef = 1.0f/( lNumOriNodes - 1 );

      lTimes.push_back ( 0 );
      for ( int i = 1; i < ( lNumOriNodes - 1 ); ++i )
        lTimes.push_back ( i*lRef );
      lTimes.push_back ( 1 );

      NSSpline::Spline < float, glm::vec3 > sp ( lTimes, lOriginalPoints );

      unsigned int lMult = pNumNodesPerSegment;
      const float lNumNewEles = lMult*( lNumOriNodes - 1 );
      lRef = 1.0/( lNumNewEles );

      for ( int i = 0; i < lNumNewEles; ++i )
      {
        const float lAux = lRef*i;
        glm::vec3 value ( sp[lAux] );

        lGraphSWCNode.OriginalNode.position[0] = value.x;
        lGraphSWCNode.OriginalNode.position[1] = value.y;
        lGraphSWCNode.OriginalNode.position[2] = value.z;

        unsigned int lTramo = i/lMult;
        unsigned int lNodoSegmento = i%lMult;
        lGraphSWCNode.OriginalNode.radius =
          lRadius[lTramo] + ( lRadius[lTramo] - lRadius[lTramo + 1] )*( lNodoSegmento/lMult );

        boost::add_vertex ( lGraphSWCNode, mOptimizedGraph.NGraph );
      }

      //El ultimo nodo hay que anyadirlo a mano
      mOriginalGraph.vi_tmp--;
      lGraphSWCNode.OriginalNode.position = mOriginalGraph.node_id[*mOriginalGraph.vi_tmp].OriginalNode.position;
      lGraphSWCNode.OriginalNode.radius = mOriginalGraph.node_id[*mOriginalGraph.vi_tmp].OriginalNode.radius;
      mOriginalGraph.vi_tmp++;

      boost::add_vertex ( lGraphSWCNode, mOptimizedGraph.NGraph );
    }

      //Este else es para simplificarla totalmente, es decir, simplificar todas y cada una de sus ramas en funci�n
      //del criterio seleccionado.
    else
    {
      //Recorremos todas las ramas del trazado
      for ( unsigned int i = 0; i < mOriginalGraph.mSWCImporter->getNumBranches ( ); ++i )
      {
        //Calculamos los nodos iniciales y finales de la rama actual
        mIniValue = mOriginalGraph.mSWCImporter->getBranchAt ( i ).initialNode;
        mFinalValue = mOriginalGraph.mSWCImporter->getBranchAt ( i ).finalNode;

        lDim = mFinalValue - mIniValue;
        ++lDim;

        int lExtraNode;
        mSimplifiedCoords.clear ( );

        vector <glm::vec3> lOriginalPoints;
        vector < float > lRadius;

        NSSWCImporter::SWCNode lInitialNode;

        //Vector temporal que almacenara las corrdenadas a simplificar y las simplificadas tras la llamada a los metodos

        //Reseteamos los punteros del grafo
        boost::tie ( mOriginalGraph.vi, mOriginalGraph.vi_end ) = vertices ( mOriginalGraph.NGraph );
        mOriginalGraph.vi_tmp = mOriginalGraph.vi;

        //------->>>>En este trozo lo primero es determinar correctametne el nodo inicial que se pilla

        //unsigned int lNewParentId=0;
        bool lNoConsecBrach = false;

        //Si el padre es el soma??? Los nodos adyacentes al soma siempre se
        //dejan, aqu� los tenemos en cuenta y los dejamos
        if ( mOriginalGraph.mSWCImporter->getElementAt ( mIniValue ).parent == 1 )
        {
          lExtraNode = 0;
          //generatedPoints = new float[(lDim+lExtraNode)*3];

          //Hemos de partir desde el padre, luego hemos de retorceder uno
          for ( unsigned int k = 0; k < ( mIniValue ); ++k )
            ++mOriginalGraph.vi_tmp;

          lInitialNode = mOriginalGraph.node_id[*mOriginalGraph.vi_tmp].OriginalNode;
        }
        else
        {
          lExtraNode = 1;

          //Caso de rama consecutiva en bifurcaci�n
          if ( mOriginalGraph.mSWCImporter->getBranchAt ( i ).consecBranch )
          {
            //Hemos de partir desde el padre, luego hemos de retorceder uno
            for ( unsigned int k = 0; k < ( mIniValue - lExtraNode ); ++k )
              ++mOriginalGraph.vi_tmp;

            glm::vec3 lvec3 ( mOriginalGraph.node_id[*mOriginalGraph.vi_tmp].OriginalNode.position[0],
                              mOriginalGraph.node_id[*mOriginalGraph.vi_tmp].OriginalNode.position[1],
                              mOriginalGraph.node_id[*mOriginalGraph.vi_tmp].OriginalNode.position[2] );

            lOriginalPoints.push_back ( lvec3 );
            lRadius.push_back ( mOriginalGraph.node_id[*mOriginalGraph.vi_tmp].OriginalNode.radius );

            lInitialNode = mOriginalGraph.node_id[*mOriginalGraph.vi_tmp].OriginalNode;

            //lNewParentId = mOriginalGraph.node_id[*mOriginalGraph.vi_tmp].OriginalNode.id;

            ++mOriginalGraph.vi_tmp;
          }
            //Rama no consecutiva, con lo cual el padre que es la bifurcaci�n no se puede eliminar, ya lo ha a�adido
            //la rama que era consecutiva y adem�s hay que tener en cuenta que el nodo siguiente al bifurcaci�n no consecutivo
            //no se a�ade como inamovible (se puede eliminar)
          else
          {
            lNoConsecBrach = true;

            for ( int k = 0; k < mOriginalGraph.mSWCImporter->getElementAt ( mIniValue ).parent; ++k )
              ++mOriginalGraph.vi_tmp;

            //Nodo inicial de la rama (el de la bifurcacion)
            glm::vec3 lvec3 ( mOriginalGraph.node_id[*mOriginalGraph.vi_tmp].OriginalNode.position[0],
                              mOriginalGraph.node_id[*mOriginalGraph.vi_tmp].OriginalNode.position[1],
                              mOriginalGraph.node_id[*mOriginalGraph.vi_tmp].OriginalNode.position[2] );

            lOriginalPoints.push_back ( lvec3 );
            lRadius.push_back ( mOriginalGraph.node_id[*mOriginalGraph.vi_tmp].OriginalNode.radius );

            lInitialNode = mOriginalGraph.node_id[*mOriginalGraph.vi_tmp].OriginalNode;

            //lNewParentId = mOriginalGraph.node_id[*mOriginalGraph.vi_tmp].OriginalNode.id;

            //Nodo del hijo no consecutivo
            for ( unsigned int k = ( unsigned int ) mOriginalGraph.mSWCImporter->getElementAt ( mIniValue ).parent;
                  k < mIniValue; ++k )
              ++mOriginalGraph.vi_tmp;;
          }
        }

        //Copiamos el resto de nodos de esa rama (desde el .vi_tmp hasta que termina)
        for ( int c = lExtraNode; c < lDim + lExtraNode; c++ )
        {
          glm::vec3 lvec3 ( mOriginalGraph.node_id[*mOriginalGraph.vi_tmp].OriginalNode.position[0],
                            mOriginalGraph.node_id[*mOriginalGraph.vi_tmp].OriginalNode.position[1],
                            mOriginalGraph.node_id[*mOriginalGraph.vi_tmp].OriginalNode.position[2] );

          lOriginalPoints.push_back ( lvec3 );
          lRadius.push_back ( mOriginalGraph.node_id[*mOriginalGraph.vi_tmp].OriginalNode.radius );

          ++mOriginalGraph.vi_tmp;
        }

        //Ojo hay que tener cuidado cuando se enlaza con nodos m�s anteriores a la hora de pillar la importancia
        if ( lDim > 0 )
        {
          lImportancias.resize ( lDim + lExtraNode );
          lErrors.resize ( lDim + lExtraNode );

          for ( unsigned int j = 0; j < ( unsigned int ) ( lDim + lExtraNode ); ++j )
            lImportancias[j] = randfloat ( 0, 10 );
        }

        int lNumOriNodes = lOriginalPoints.size ( );

        //El spline necesita la menos tres puntos
        if ( lNumOriNodes < 3 )
        {
          glm::vec3 lvec3Med (( lOriginalPoints.at ( 0 ).x + lOriginalPoints.at ( 1 ).x )/2.0,
                              ( lOriginalPoints.at ( 0 ).y + lOriginalPoints.at ( 1 ).y )/2.0,
                              ( lOriginalPoints.at ( 0 ).z + lOriginalPoints.at ( 1 ).z )/2.0 );

          //lOriginalPoints.push_back(lOriginalPoints.at(lOriginalPoints.size()-1));
          lOriginalPoints.push_back ( lOriginalPoints.at ( 1 ));
          lOriginalPoints[1] = lvec3Med;

          lRadius.push_back ( lRadius.at ( 0 ));

          ++lNumOriNodes;
        }

        vector < float > lTimes;

        float lRef = 1.0f/( lNumOriNodes - 1 );

        lTimes.push_back ( 0.0f );
        for ( unsigned int j = 1; j < ( unsigned int ) ( lNumOriNodes - 1 ); ++j )
          lTimes.push_back ( j*lRef );
        lTimes.push_back ( 1.0f );

        NSSpline::Spline < float, glm::vec3 > sp ( lTimes, lOriginalPoints );

        unsigned int lMult = pNumNodesPerSegment;
        float lNumNewEles = lMult*( lNumOriNodes - 1 );
        lRef = 1.0f/lNumNewEles;

        //if (lNoConsecBrach) lInitialDespNoConsecNode=0.1;
        float lAux;
        for ( unsigned int j = 0; j < lNumNewEles; ++j )
        {
          //Si es la bifurcaci�n no consecutiva no puede mepezar en 0, se debe desplazar algo
          if (( j == 0 ) && ( lNoConsecBrach ))
            lAux = lRef*( j + 0.1 );
          else
            lAux = lRef*( j );

          glm::vec3 value ( sp[lAux] ); //ak� es donde se a�ade el nuevo valor

          lGraphSWCNode.OriginalNode.position[0] = value.x;
          lGraphSWCNode.OriginalNode.position[1] = value.y;
          lGraphSWCNode.OriginalNode.position[2] = value.z;

          unsigned int lTramo = j/lMult;
          unsigned int lNodoSegmento = j%lMult;

          //Para poner el radio interpolado ###################################### -> Recalcular el radio bien
          //lGraphSWCNode.OriginalNode.radius=lRadius[lTramo] + (lRadius[lTramo]-lRadius[lTramo+1])*(lNodoSegmento/lMult);
          lGraphSWCNode.OriginalNode.radius =
            lRadius[lTramo] + ((( lRadius[lTramo + 1] - lRadius[lTramo] )/lMult )*( lNodoSegmento ));

          //Hay q calcular esta informaci�n

          lGraphSWCNode.NewId = boost::num_vertices ( mOptimizedGraph.NGraph );
          lGraphSWCNode.OriginalNode.type = lInitialNode.type;

          if ( j != 0 )
          {
            //lGraphSWCNode.isBifurcation			= mOriginalGraph.mSWCImporter->isDendriticBifurcation(n);
            //lGraphSWCNode.isTermination			= mOriginalGraph.mSWCImporter->isDendriticTermination(n);

            lGraphSWCNode.NewParent = lGraphSWCNode.NewId - 1;
          }
          else
          {
            //--->>>>Arreglar
            lGraphSWCNode.NewParent = -424242;

          }

          mVertex.push_back ( boost::add_vertex ( lGraphSWCNode, mOptimizedGraph.NGraph ));

          //Nodos consecutivos, se anyade el enlace a medida que avanza
          if ( lGraphSWCNode.NewParent != -424242 )
          {
            boost::add_edge ( mVertex[mVertex.size ( ) - 2], mVertex[mVertex.size ( ) - 1], mOptimizedGraph.NGraph );
          }
            //Arreglar bifurcaciones
          else
          {
            if ( lGraphSWCNode.NewId > 1 )
              //if (false)
            {
              //Recorro hacia atr�s buscando el m�s cercano en la rama
              boost::tie ( mOptimizedGraph.vi, mOptimizedGraph.vi_end ) = vertices ( mOptimizedGraph.NGraph );
              mOptimizedGraph.vi_tmp = mOptimizedGraph.vi_end;

              //Recorremos la rama hasta la raiz (evitando el nodo actual, para que no enlace consigo mismo)
              unsigned int lVertexIndex = mVertex.size ( ) - 2;
              unsigned int lFinalVertexIndex = lVertexIndex;

              float lDistance = 100000;
              float lNewDistance;
              --mOptimizedGraph.vi_tmp;
              //--mOptimizedGraph.vi_tmp;

              if ( lInitialNode.parent == 1 )
              {
                lFinalVertexIndex = 1;
                lGraphSWCNode.NewParent = lFinalVertexIndex;

                mOptimizedGraph.node_id[*mOptimizedGraph.vi_tmp].NewParent = lFinalVertexIndex;
                //mOptimizedGraph.node_id[*mOptimizedGraph.vi_tmp].OriginalNode.parent = lFinalVertexIndex;
                boost::add_edge ( mVertex[mVertex.size ( ) - 1], mVertex[lFinalVertexIndex], mOptimizedGraph.NGraph );
              }
              else
              {
                --mOptimizedGraph.vi_tmp;

                //Esto deber�a hacerse buscando el id en el original del padre
                //Haci�ndolopor distancia
                while (( mOptimizedGraph.node_id[*mOptimizedGraph.vi_tmp].OriginalNode.parent != 1 )
                  && ( mOptimizedGraph.vi_tmp != mOptimizedGraph.vi ))
                {
                  lNewDistance = ( lGraphSWCNode.OriginalNode.position
                    - mOptimizedGraph.node_id[*mOptimizedGraph.vi_tmp].OriginalNode.position ).length ( );

                  if ( lNewDistance < lDistance )
                  {
                    lFinalVertexIndex = lVertexIndex;
                    lDistance = lNewDistance;
                  }

                  --mOptimizedGraph.vi_tmp;
                  --lVertexIndex;
                }

                lGraphSWCNode.NewParent = lFinalVertexIndex;
                boost::add_edge ( mVertex[lFinalVertexIndex], mVertex[mVertex.size ( ) - 1], mOptimizedGraph.NGraph );
              }

            }
          }

        }












        //Se fuerza pillar el ultimo nodo de la dendrita
        if (( mOriginalGraph.mSWCImporter->isDendriticTermination ( mFinalValue )))
        {
          //El ultimo nodo hay que anyadirlo a mano
          mOriginalGraph.vi_tmp--;
          lGraphSWCNode.OriginalNode.position = mOriginalGraph.node_id[*mOriginalGraph.vi_tmp].OriginalNode.position;
          lGraphSWCNode.OriginalNode.radius = mOriginalGraph.node_id[*mOriginalGraph.vi_tmp].OriginalNode.radius;
          lGraphSWCNode.NewId++;
          mOriginalGraph.vi_tmp++;

          mVertex.push_back ( boost::add_vertex ( lGraphSWCNode, mOptimizedGraph.NGraph ));
          boost::add_edge ( mVertex[mVertex.size ( ) - 2], mVertex[mVertex.size ( ) - 1], mOptimizedGraph.NGraph );
        }


        //###############################

        //For edges
        //lNumPrevAct=boost::num_vertices(mOptimizedGraph.NGraph)-1;

        ////Add Vertexs
        ////Anyadimos los nuevos v�rtices del grafo (nodos realmente)
        ////Utilizamos la posicion para encontrar el nodo en el grafo, copiamos toda su info desde le grafo original
        ////Y lo a�adimos la grafo simplificado.
        //for (int j=(lExtraNode*3);j<mSimplifiedCoords.size();j+=3)
        //{
        //	lGraphSWCNode.NewId++;

        //	//Avanzamos de 3 en 3
        //	lLocalPos[0] = lGraphSWCNode.OriginalNode.position[0]=mSimplifiedCoords.at(j);
        //	lLocalPos[1] = lGraphSWCNode.OriginalNode.position[1]=mSimplifiedCoords.at(j+1);
        //	lLocalPos[2] = lGraphSWCNode.OriginalNode.position[2]=mSimplifiedCoords.at(j+2);

        //	//int lDim = mFinalValue-mIniValue;
        //	int	 n		=	mFinalValue;
        //	bool lFound	=	false;
        //
        //	//Recorremos hacia atr�s porque el ultimo nodo de cada rama siempre va a estar
        //	//Esto es muy poco eficiente)
        //	while (!lFound)
        //	{
        //		//Para localizar el nodo comparamos por su posici�n (No poseemos m�s informaci�n)
        //		if (lLocalPos == mOriginalGraph.mSWCImporter->getElementAt(n).position)
        //		{
        //			lFound								=	true;

        //			lGraphSWCNode.isBifurcation			= mOriginalGraph.mSWCImporter->isDendriticBifurcation(n);
        //			lGraphSWCNode.isTermination			= mOriginalGraph.mSWCImporter->isDendriticTermination(n);

        //			lGraphSWCNode.OriginalNode.radius	=	mOriginalGraph.mSWCImporter->getElementAt(n).radius;
        //			lGraphSWCNode.OriginalNode.id		=	mOriginalGraph.mSWCImporter->getElementAt(n).id;
        //			lGraphSWCNode.OriginalNode.type		=	mOriginalGraph.mSWCImporter->getElementAt(n).type;
        //			lGraphSWCNode.OriginalNode.parent	=	lNewParentId;

        //		}
        //		--n;
        //	}

        //	//Se a�ade el nodo
        //	mVertex.push_back(boost::add_vertex(lGraphSWCNode, mOptimizedGraph.NGraph));
        //}

        ////Add edges
        ////Caso de que le padre sea el soma

        /*
        //lNumPrevAct--;
        if (mOriginalGraph.mSWCImporter->getElementAt(mIniValue).parent==1)
        {
          boost::add_edge(mVertex[lNumPrevAct], mVertex[1], mOptimizedGraph.NGraph);
        }
        //Resto de casos
        else
        {
          //Si es una rama consecutiva se enlaza con el anterior
          if (mOriginalGraph.mSWCImporter->getBranchAt(i).consecBranch)
          {
            boost::add_edge(mVertex[lNumPrevAct], mVertex[lNumPrevAct-1], mOptimizedGraph.NGraph);
          }
          //Caso de no serlo, se ha de encontrar al padre
          else
          {
            NeuronGraph::Vertex lVertexParent;
            int n		=	mVertex.size()-1;
            bool lFound	=	false;
            //while (!lFound)
            //{
            //	if ( mOptimizedGraph.node_id[mVertex[lNumPrevAct]].OriginalNode.parent
            //			==	mOptimizedGraph.node_id[mVertex[n]].OriginalNode.id)
            //	{
            //		lFound	=	true;
            //	}
            //	else
            //	{
            //		--n;
            //	}
            //}
            //boost::add_edge(mVertex[lNumPrevAct], mVertex[n], mOptimizedGraph.NGraph);
          }
        }

        //Enlazado de todos los nuevos nodos (todo lo anterior es para elprincipio)
        for (int i=lNumPrevAct;i<boost::num_vertices(mOptimizedGraph.NGraph)-1;++i)
        {
          boost::add_edge(mVertex[i], mVertex[i+1], mOptimizedGraph.NGraph);
        }
        */
      }
      mFinalValue = mIniValue = 0;
    }
  }


  //Update parents ids
  NeuronGraph::Vertex lVertexDescIni;
  NeuronGraph::Vertex lVertexDescFin;

  for ( boost::tie ( mOptimizedGraph.ei, mOptimizedGraph.ei_end ) = boost::edges ( mOptimizedGraph.NGraph );
        mOptimizedGraph.ei != mOptimizedGraph.ei_end; ++mOptimizedGraph.ei )
  {
    lVertexDescIni = boost::source ( *mOptimizedGraph.ei, mOptimizedGraph.NGraph );
    lVertexDescFin = boost::target ( *mOptimizedGraph.ei, mOptimizedGraph.NGraph );

    //Detectamos el orden de la conectividad
    if ( mOptimizedGraph.node_id[lVertexDescIni].OriginalNode.id
      > mOptimizedGraph.node_id[lVertexDescFin].OriginalNode.id )
    {
      mOptimizedGraph.node_id[lVertexDescIni].NewParent = mOptimizedGraph.node_id[lVertexDescFin].NewId;
    }
    else
    {
      mOptimizedGraph.node_id[lVertexDescFin].NewParent = mOptimizedGraph.node_id[lVertexDescIni].NewId;
    }
  }

}

//void NeuronGraphOptimizer::applyNodeLinearDistribution(unsigned int pNumNodesPerSegment)
void NeuronGraphOptimizer::applyNodeLinearDistribution ( )
{

}
