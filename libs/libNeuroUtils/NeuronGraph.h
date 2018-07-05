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

#pragma once

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/adjacency_iterator.hpp>

//#include <boost/graph/dijkstra_shortest_paths.hpp>

#include <SWCImporter.h>
#include <glmSpline.hpp>

//#include <QVector>

class NeuronGraphRenderer;

struct GraphSWCNode
{
  NSSWCImporter::SWCNode OriginalNode;
  unsigned int NewId;
  int NewParent;
  bool isBifurcation;
  bool isTermination;
};

struct GraphEdgeInfo
{
  int Parent;
  int Id;
};

class NeuronGraph
{
  protected:
    //typedef boost::property<boost::edge_weight_t, float>					EdgeWeightProperty;
    typedef boost::property <boost::edge_name_t, GraphEdgeInfo> EdgeInfoProperty;

    typedef boost::property <boost::vertex_name_t, GraphSWCNode> VertexInfoProperty;

    typedef boost::adjacency_list <boost::listS
    //boost::setS
    // boost::vecS
    //, boost::vecS
    , boost::listS, boost::undirectedS
    //, boost::bidirectionalS
    //, boost::directedS
    , VertexInfoProperty
    //, EdgeWeightProperty
    , EdgeInfoProperty, boost::listS
    > Graph;

    NSSWCImporter::SWCImporter *mSWCImporter;

    Graph NGraph;

    boost::property_map < Graph, boost::vertex_name_t >::type node_id;
    boost::property_map < Graph, boost::edge_name_t >::type edge_id;

    /*
    // Property accessors
    boost::property_map<Graph, boost::vertex_name_t>::type
              city_name = boost::get(vertex_name, NGraph);
    */

    // Create the vertices
    typedef boost::graph_traits < Graph >::vertex_descriptor Vertex;
    Vertex *mVertexDesc;

    // Create the edges
    typedef boost::graph_traits < Graph >::edge_descriptor Edge;
    Edge *mEdgeDesc;

    // Iterate through the vertices
    typedef boost::graph_traits < Graph >::vertex_iterator vertex_iter;
    std::pair <vertex_iter, vertex_iter> vp, vp_simp;
    /*OpenMesh::Vec3f pos, pos2;*/

    /*
    typedef boost::property_map<Graph, boost::vertex_index_t>::type vIndex;
    vIndex mVIndex;
    */

    // Iterate through the edges
    typedef boost::graph_traits < Graph >::edge_iterator edge_iter;
    //typedef boost::graph_traits<Graph>::out_edge_iterator edge_iter;
    std::pair <edge_iter, edge_iter> ep;
    edge_iter ei, ei_end, ei_tmp, ei_next;

    typedef boost::graph_traits < Graph >::out_edge_iterator out_edge_iter;
    std::pair <edge_iter, edge_iter> oep;
    out_edge_iter oei, oei_end, oei_tmp, oei_next;
    typedef boost::graph_traits < Graph >::edge_descriptor ed;

    boost::graph_traits < Graph >::vertex_iterator vi, vi_end, vi_tmp, vi_tmp_reverse, next;

    //Graph::adjacency_iterator ai, ai_end;
    typedef boost::graph_traits < Graph >::adjacency_iterator adjacency_iterator;
    std::pair <adjacency_iterator, adjacency_iterator> aip_it, aip_it2;
    adjacency_iterator aip, aip_end;

    /*
    typedef boost::property_map<Graph, boost::vertex_index_t>::type vertex_id_pmap;
    vertex_id_pmap vertex_id;
    */

    //QVector  <OpenMesh::Vec3f> mSimplifiedCoords;
    //std::vector<OpenMesh::Vec3f> mSimplifiedCoords;

    /*
    //Esto no deber�a pertenecer a esta clase, ya que es para la optimizaci�n
    std::vector<float> mSimplifiedCoords;
    std::vector<float> mTotalSimplifiedCoords;

    Graph mTotalSimplifiedGraph;
    boost::property_map<Graph, boost::vertex_name_t>::type node_Simp_id;

    boost::graph_traits<Graph>::vertex_iterator vi_simp, vi_end_simp, vi_tmp_simp, vi_tmp_simp_2;
    edge_iter ei_simp, ei_end_simp, ei_tmp_simp, ei_next_simp;
    */


  public:

    NeuronGraph ( void );

    ~NeuronGraph ( void );

    void generateGraphFromSWCImporter ( const std::string pSWCFile );

    const GraphSWCNode &getNodeInfo ( unsigned int pNodeId );

    GraphSWCNode &getUnprotectedNodeInfo ( unsigned int pNodeId );

    unsigned int getNumNodes ( ) { return boost::num_vertices ( NGraph ); }

    void exportToSWC ( std::string pFileName );

    friend class NeuronGraphRenderer;

    friend class NeuronGraphOptimizer;

    void attachSWCImporter ( NSSWCImporter::SWCImporter &pSWCImporter );

    void ExtractNeurite ( int lNodeOfNeurite, const std::string pFileName );

    void setRadiusToSelectedNode ( unsigned int id, float lRadius );

    NSSWCImporter::SWCImporter *getSWCImporter ( ) { return mSWCImporter; }

    void brokeLink ( unsigned int pId );

    void setLink ( unsigned int pIdIni, unsigned int pIdFin );

};

