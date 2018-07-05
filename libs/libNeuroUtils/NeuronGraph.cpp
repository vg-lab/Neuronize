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

#include "NeuronGraph.h"

NeuronGraph::NeuronGraph ( void )
{
  mSWCImporter = NULL;
  mVertexDesc = NULL;
  mEdgeDesc = NULL;
}

NeuronGraph::~NeuronGraph ( void )
{
  if ( mSWCImporter != NULL )
    delete mSWCImporter;
  if ( mVertexDesc != NULL )
    delete mVertexDesc;
  if ( mEdgeDesc != NULL )
    delete mEdgeDesc;

  NGraph.clear ( );
}

void NeuronGraph::generateGraphFromSWCImporter ( const std::string pSWCFile )
{
  if ( mSWCImporter != NULL )
    delete mSWCImporter;
  if ( mVertexDesc != NULL )
    delete mVertexDesc;
  if ( mEdgeDesc != NULL )
    delete mEdgeDesc;

  NGraph.clear ( );

  mSWCImporter = new NSSWCImporter::SWCImporter ( pSWCFile );

  //Fill the graph
  mVertexDesc = new Vertex[mSWCImporter->getNumNodes ( )];
  GraphSWCNode lGraphSWCNode;
  for ( unsigned int i = 0; i < mSWCImporter->getNumNodes ( ); ++i )
  {
    lGraphSWCNode.OriginalNode = mSWCImporter->getElementAt ( i );
    lGraphSWCNode.NewId = lGraphSWCNode.OriginalNode.id;
    lGraphSWCNode.NewParent = lGraphSWCNode.OriginalNode.parent;
    lGraphSWCNode.isBifurcation = mSWCImporter->isDendriticBifurcation ( i );
    lGraphSWCNode.isTermination = mSWCImporter->isDendriticTermination ( i );

    mVertexDesc[i] = boost::add_vertex ( lGraphSWCNode, NGraph );
  }

  //mVertexSomaDesc = mVertexDesc[1];

  //Connect the Graph
  Edge lEdge;
  mEdgeDesc = new Edge[mSWCImporter->getNumNodes ( )];
  for ( unsigned int i = 2; i < mSWCImporter->getNumNodes ( ); ++i )
  {
    mEdgeDesc[i] =
      ( boost::add_edge ( mVertexDesc[i], mVertexDesc[mSWCImporter->getElementAt ( i ).parent], NGraph )).first;;
  }

  node_id = get ( boost::vertex_name, NGraph );
  node_id ( mVertexDesc[0] ).OriginalNode = node_id ( mVertexDesc[1] ).OriginalNode;
}

const GraphSWCNode &NeuronGraph::getNodeInfo ( unsigned int pNodeId )
{
  boost::tie ( vi, vi_end ) = vertices ( NGraph );
  vi_tmp = vi;//+pNodeId;
  for ( unsigned int i = 0; i < pNodeId; ++i )
    ++vi_tmp;
  return node_id[*vi_tmp];

  /*
  //Para el grafo anexo
  boost::tie(vi_simp, vi_end_simp)=boost::vertices(mTotalSimplifiedGraph);
  vi_tmp_simp		=	vi_simp;
  for (int i=0;i<pNodeId;++i) ++vi_tmp_simp;
  return node_Simp_id[*vi_tmp_simp];
  */

  /*
  //unsigned int lAux=(*vi)+pNodeId;
  GraphSWCNode tmp = node_id[(*vi)];
  return tmp;
  */
}

GraphSWCNode &NeuronGraph::getUnprotectedNodeInfo ( unsigned int pNodeId )
{
  boost::tie ( vi, vi_end ) = vertices ( NGraph );
  vi_tmp = vi;//+pNodeId;
  for ( unsigned int i = 0; i < pNodeId; ++i )
    ++vi_tmp;
  return node_id[*vi_tmp];
}

void NeuronGraph::exportToSWC ( std::string pFileName )
{
  boost::tie ( vi, vi_end ) = vertices ( NGraph );

  std::string fileTXT = pFileName;
  std::ofstream outputFileTXT;

  //Clean files
  std::ofstream ( fileTXT.c_str ( ));

  //Open files to add data
  outputFileTXT.open ( fileTXT.c_str ( ), std::ios::app );

  //Head of the file
  //File format version
  outputFileTXT << "# File exported from QtSkeletonOptimizer" << endl;

  GraphSWCNode lNode;

  lNode = getNodeInfo ( 1 );

  //Text exit
  outputFileTXT << " "
                << 1 << " "
                << lNode.OriginalNode.type << " "
                << "0.0" << " "
                << "0.0" << " "
                << "0.0" << " "
                << lNode.OriginalNode.radius << " "
                << -1 << " "
                << endl;

  //	node_id[*vi_tmp].id=1;

  for ( unsigned int i = 2; i < getNumNodes ( ); ++i )
    //for (int i=0;i<getNumNodes();++i)
  {
    lNode = getNodeInfo ( i );

    //Text exit
    outputFileTXT << " "
                  << lNode.NewId << " "
                  << lNode.OriginalNode.type << " "
                  << lNode.OriginalNode.position[0] << " "
                  << lNode.OriginalNode.position[1] << " "
                  << lNode.OriginalNode.position[2] << " "
                  << lNode.OriginalNode.radius << " "
                  << lNode.NewParent << " "
                  << endl;
  }

  outputFileTXT.close ( );
}

void NeuronGraph::attachSWCImporter ( NSSWCImporter::SWCImporter &pSWCImporter )
{
  if ( mSWCImporter != NULL )
    delete mSWCImporter;

  //Vertex lVertex = mVertexDesc[0];

  //if (mVertexDesc	!=	NULL)		delete mVertexDesc;
  Vertex *lVertexDesc;

  if ( mEdgeDesc != NULL )
    delete mEdgeDesc;

  //NGraph.clear();

  mSWCImporter = &pSWCImporter;
  lVertexDesc = new Vertex[mSWCImporter->getNumNodes ( )];

  //Fill the graph
  unsigned int lInitialSize = num_vertices ( NGraph );

  GraphSWCNode lGraphSWCNode;

  unsigned int lSomaNodes = 0;
  int lDesp = 0;

  //Soma iter
  boost::tie ( vi, vi_end ) = vertices ( NGraph );
  vi_tmp = vi;//+pNodeId;
  ++vi_tmp;


  //leave the Zero node soma
  for ( unsigned int i = 1; i < mSWCImporter->getNumNodes ( ); ++i )
  {

    if ( mSWCImporter->getElementAt ( i ).type == 1 )
      ++lSomaNodes;
    else
    {
      lDesp = -lSomaNodes + lInitialSize - 1;
      lGraphSWCNode.OriginalNode = mSWCImporter->getElementAt ( i );
      lGraphSWCNode.NewId = lGraphSWCNode.OriginalNode.id + lDesp;

      if ( lGraphSWCNode.OriginalNode.parent == 1 )
        lGraphSWCNode.NewParent = lGraphSWCNode.OriginalNode.parent;
      else
        lGraphSWCNode.NewParent = lGraphSWCNode.OriginalNode.parent + lDesp;

      lGraphSWCNode.isBifurcation = mSWCImporter->isDendriticBifurcation ( i );
      lGraphSWCNode.isTermination = mSWCImporter->isDendriticTermination ( i );

      lVertexDesc[i] = boost::add_vertex ( lGraphSWCNode, NGraph );
    }
  }

  //Connect the Graph
  Edge lEdge;
  mEdgeDesc = new Edge[mSWCImporter->getNumNodes ( )];

  lVertexDesc[0] = *vi_tmp;

  for ( unsigned int i = 2; i < mSWCImporter->getNumNodes ( ); ++i )
  {
    if ( mSWCImporter->getElementAt ( i ).parent == 1 )
      mEdgeDesc[i] = ( boost::add_edge ( lVertexDesc[0], lVertexDesc[i], NGraph )).first;
    else
      mEdgeDesc[i] =
        ( boost::add_edge ( lVertexDesc[i], lVertexDesc[mSWCImporter->getElementAt ( i ).parent], NGraph )).first;
  }

  //????
  //node_id = get(boost::vertex_name, NGraph);
  //node_id(lVertexDesc[0]).OriginalNode = node_id(lVertexDesc[1]).OriginalNode;
}

void NeuronGraph::ExtractNeurite ( int lNodeOfNeurite, const std::string pFileName )
{
  std::string fileTXT = pFileName;
  std::ofstream outputFileTXT;

  //Clean files
  std::ofstream ( fileTXT.c_str ( ));

  //Open files to add data
  outputFileTXT.open ( fileTXT.c_str ( ), std::ios::app );

  //Head of the file
  //File format version
  outputFileTXT << "# File exported from QtSkeletonOptimizer" << endl;

  GraphSWCNode lNode;

  lNode = getNodeInfo ( 1 );

  //Text exit
  outputFileTXT << " "
                << 1 << " "
                << lNode.OriginalNode.type << " "
                << "0.0" << " "
                << "0.0" << " "
                << "0.0" << " "
                << lNode.OriginalNode.radius << " "
                << -1 << " "
                << endl;

  //	node_id[*vi_tmp].id=1;
  unsigned int lNeuriteOri = lNodeOfNeurite;
  --lNeuriteOri;

  while ( getNodeInfo ( lNeuriteOri ).NewParent != 1 )
    --lNeuriteOri;

  unsigned int lInitialDesp = lNeuriteOri;
  lNode = getNodeInfo ( lNeuriteOri );

  //Text exit
  outputFileTXT << " "
                << lNode.NewId - lInitialDesp << " "
                << lNode.OriginalNode.type << " "
                << lNode.OriginalNode.position[0] << " "
                << lNode.OriginalNode.position[1] << " "
                << lNode.OriginalNode.position[2] << " "
                << lNode.OriginalNode.radius << " "
                << lNode.NewParent - lInitialDesp << " "
                << endl;

  while (( getNodeInfo ( lNeuriteOri++ ).NewParent != 1 ) && ( lNeuriteOri < getNumNodes ( )))
  {
    lNode = getNodeInfo ( lNeuriteOri );

    //Text exit
    outputFileTXT << " "
                  << lNode.NewId << " "
                  << lNode.OriginalNode.type << " "
                  << lNode.OriginalNode.position[0] << " "
                  << lNode.OriginalNode.position[1] << " "
                  << lNode.OriginalNode.position[2] << " "
                  << lNode.OriginalNode.radius << " "
                  << lNode.NewParent << " "
                  << endl;
  }

  outputFileTXT.close ( );
}

void NeuronGraph::setRadiusToSelectedNode ( unsigned int id, float lRadius )
{
  getUnprotectedNodeInfo ( id ).OriginalNode.radius = lRadius;
}

void NeuronGraph::brokeLink ( unsigned int pId )
{
  for ( boost::tie ( this->ei, this->ei_end ) = boost::edges ( this->NGraph ); this->ei != this->ei_end; ++this->ei )
  {
    if ( this->node_id[boost::source ( *this->ei, this->NGraph )].OriginalNode.id == ( int ) pId )
    {
      boost::remove_edge ( *this->ei, this->NGraph );
      break;
    }
  }

  for ( boost::tie ( this->ei, this->ei_end ) = boost::edges ( this->NGraph ); this->ei != this->ei_end; ++this->ei )
  {
    if ( this->node_id[boost::target ( *this->ei, this->NGraph )].OriginalNode.id == ( int ) pId )
    {
      boost::remove_edge ( *this->ei, this->NGraph );
      break;
    }
  }
}

void NeuronGraph::setLink ( unsigned int pIdIni, unsigned int pIdFin )
{
  if ( mSWCImporter->getElementAt ( pIdIni ).id < mSWCImporter->getElementAt ( pIdFin ).id )
    boost::add_edge ( mVertexDesc[pIdIni], mVertexDesc[pIdFin], NGraph ).first;
  else
    boost::add_edge ( mVertexDesc[pIdFin], mVertexDesc[pIdIni], NGraph ).first;
}

