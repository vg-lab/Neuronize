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

#ifndef SWCIMPORTER_H_
#define SWCIMPORTER_H_

/**
 * @SWCImporter
 * @SWCImporter.h
 * @author Juan Pedro Brito M�ndez
 * @version 0.1
 */

//#include <defs.h>
#include <string>
#include <map>
#include <vector>
#include <fstream>

#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>

#include <libs/libSysNeuroUtils/MathUtils.h>
//#include "defs.h"

using namespace std;

/**
 * Description
 * @author Juan Pedro Brito Mendez
 */

namespace NSSWCImporter
{
  struct SWCBranch
  {
    int initialNode;
    int finalNode;
    bool consecBranch;
  };

  struct SWCConnection
  {
    int ini;
    int fin;
    bool mTermination;
  };

  enum DendriticType
  {
    BASAL, APICAL, AXON
  };

  struct SWCDendriticInfo
  {
    int initialNode;
    int finalNode;
    DendriticType type;
  };

  struct SWCNode
  {
    int id;
    int type;
    //* 0 = undefined
    //* 1 = soma
    //* 2 = axon
    //* 3 = dendrite
    //* 4 = apical dendrite
    //* 5 = fork point
    //* 6 = end point
    //* 7 = custom

    OpenMesh::Vec3f position;

    float radius;
    int parent;

    float mDendriticDistanceNorm;
    float mDendriticDistanceReal;
    float mDistanceToSoma;

  };

  class SWCImporter
  {
      int mSomaNodesDiscarded;

      //File exit
      std::ifstream inputFileTXT;

      //Descripcion of the file
      std::string description;

      //Collection of the nodes in the original file
      std::vector <SWCNode> preProcessNodeCollection;

      //Collection of the nodes finally processed
      std::vector <SWCNode> nodeCollection;

      //Collection of Connections in the file
      std::vector <SWCConnection> nodeConnections;

      //Collection dendritic finisheds
      std::vector < int > dendriticTermination;

      //Collection dendritic bifurcations
      std::vector < int > dendriticBifurcations;

      //Dendritic soma conected
      std::vector < int > dendriticSomaConnection;

      //Dendritic max distance
      std::vector < int > dendriticMaxDistance;

      //Dendritics of the neuron
      std::vector <SWCDendriticInfo> dendritics;

      //branches of the neuron
      std::vector <SWCBranch> mBranches;

      OpenMesh::Vec3f displacement;

      //Load the file
      void loadFile ( string fileName, bool pApplyStdDims, bool pApplyReplicantNodeTest );

      //Generate the connectios between nodes
      void generateConnectios ( );

      void generateBifurcations ( );

      ///////////////
      void generateBranches ( );

      void centerSomaInOrigin ( );

      void SomaThreeCrazyNodesTest ( );

      void SomaDendriticTest ( );

      void SomaBeltTest ( );

      void NodeRepetitionTest ( );

      void calcDendriticDistance ( );

      void calcDendritics ( );

      bool isIdInContainer ( SWCNode pId, const std::vector <SWCNode> &pVector );

    public:

      /**
       * Default constructor
       */
      SWCImporter ( string fileName, bool pApplyStdDims = false, bool pApplyReplicantNodeTest = false );

      /**
       * Destructor
       */
      ~SWCImporter ( );

      //Get the description of the file loaded
      std::string getDescription ( );

      //Get the content of the nodes from ini to fin
      std::string getContent ( int ini, int fin );

      //Get number of nodes
      unsigned int getNumNodes ( ) const;

      OpenMesh::Vec3f getDisplacement() const;

      //Get number of connection
      unsigned int getNumConnection ( );

      //Get number of dendritic terminations
      unsigned int getNumDendriticTermination ( ) const;

      //Get number of dendritic bifurcations
      unsigned int getNumDendriticBifurcations ( ) const;

      std::vector < int > getDendriticBifurcations ( ) const;

      SWCNode getElementAt ( unsigned int i ) const;

      void modifElement ( unsigned int pId, SWCNode pNewValue );

      SWCConnection getConnectionAt ( unsigned int i );

      SWCBranch getBranchAt ( int i ) { return mBranches[i]; };

      bool isDendriticTermination ( int id ) const;

      bool isDendriticBifurcation ( int id ) const;

      //Get number of dendritic terminations
      int getDendriticTermination ( unsigned int index );
      //---int getDendriticTermination(unsigned int index);

      //Dendritic soma conected
      std::vector < int > getDendriticSomaConnection ( );

      //Dendritic max distance
      std::vector < int > getDendriticMaxDistance ( );

      //Get dendritics
      std::vector <SWCDendriticInfo> getDendritics ( ) const;

      //Get the number of dendritics
      unsigned int getNumDendritics ( );

      unsigned int getNumBranches ( ) { return mBranches.size ( ); }

      //std::string  exportToFile(std::string pFile);
      void exportToFile ( std::string pFile );

      void applyUniformModifiers ( float pAxonModifier,
                                   float pDendriticModifier,
                                   float pApicalModifier,
                                   float pValBase );

      void appendSWCImporter ( const SWCImporter *pImporter );

      void applyTranformationMatrix ( boost::numeric::ublas::matrix < float > pMatrixTransform );

      int getSomaNodesDiscarded ( ) { return mSomaNodesDiscarded; }
  };
}

#endif
