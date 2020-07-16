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

/**
 * @SWCImporter
 * @SWCImporter.cpp
 * @author Juan Pedro Brito M�ndez
 * @version 0.1
 */

#include "SWCImporter.h"


#include <iostream>
#include <unordered_map>
#include <stack>
#include <Eigen/Dense>

using namespace std;

#define SOMAID            1
#define MINSOMADESP          0.5

#define AXONRADIUSMODIF        1.0
#define DENDRITICRADIUSMODIF    3.0

#define SOMAAPICALDISTFACTOR    3.0

#define APICALRADIUSMODIFSUDDEN    10.0
#define APICALRADIUSMODIFINITIAL  8.0
#define APICALRADIUSMODIFINFINAL  6.0
#define APICALRADIUSMODIFSTD    4.0

//#define APICALRADIUSMODIFSUDDEN	40.0
//#define APICALRADIUSMODIFINITIAL	1.0
//#define APICALRADIUSMODIFINFINAL	40.0
//#define APICALRADIUSMODIFSTD		4.0

#define BASERADIUSMODIF        0.205

/*
 * Description
 * @author Juan Pedro Brito Mendez
 */
namespace NSSWCImporter
{

    /**
     *
     */
    SWCImporter::SWCImporter ( string fileName, bool pApplyStdDims, bool pApplyReplicantNodeTest )
    {
      mSomaNodesDiscarded = 0;
      loadFile ( fileName, pApplyStdDims, pApplyReplicantNodeTest );
      //loadFile(fileName, pApplyStdDims, true);
      generateConnectios ( );
      generateBifurcations ( );
      generateBranches ( );
      restructIdentifiers ( );
      //exportToFile("test.swc");

      //if (pApplyStdDims)	applyUniformModifiers(AXONRADIUSMODIF,DENDRITICRADIUSMODIF,APICALRADIUSMODIFSTD,BASERADIUSMODIF);
    }

    SWCImporter::~SWCImporter ( )
    {

    }

    //Load the file
    void SWCImporter::loadFile ( string fileName, bool pApplyStdDims, bool pApplyReplicantNodeTest )
    {
      inputFileTXT.open ( fileName.c_str ( ), std::ios::in );
      //if (inputFileTXT.fail())
      if ( !inputFileTXT )
      {
        cerr << "An error occurred. Unable to read input file." << fileName << endl;
        exit ( 1 );
      }

      SWCNode nodeAux;

      //Node 0 dont exist
      nodeAux.id = nodeAux.type = nodeAux.position[0] = nodeAux.position[1]
              = nodeAux.position[2] = nodeAux.radius = nodeAux.parent
              = nodeAux.mDendriticDistanceNorm = nodeAux.mDendriticDistanceReal = 0;

      preProcessNodeCollection.push_back ( nodeAux );

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
          description += str;
          description += '\n';
        }
      }

      //Node readed
      int pos = 0;

      if ( coment != ' ' )
      {
        ++pos;
        //nodeAux.id = (int)coment;
        nodeAux.id = 1;
        inputFileTXT >> nodeAux.type;
        inputFileTXT >> nodeAux.position[0];
        inputFileTXT >> nodeAux.position[1];
        inputFileTXT >> nodeAux.position[2];
        inputFileTXT >> nodeAux.radius;
        inputFileTXT >> nodeAux.parent;

        preProcessNodeCollection.push_back ( nodeAux );
      }

      while ( !inputFileTXT.eof ( ))
      {
        ++pos;
        inputFileTXT >> nodeAux.id;
        inputFileTXT >> nodeAux.type;
        inputFileTXT >> nodeAux.position[0];
        inputFileTXT >> nodeAux.position[1];
        inputFileTXT >> nodeAux.position[2];
        inputFileTXT >> nodeAux.radius;
        inputFileTXT >> nodeAux.parent;

        preProcessNodeCollection.push_back ( nodeAux );
      }

      //Remove the last element if it is replicated
      int tam = preProcessNodeCollection.size ( );
      nodeAux = preProcessNodeCollection[tam - 1];
      if (( tam - 1 ) != nodeAux.id )
        preProcessNodeCollection.pop_back ( );

      //Close the file
      inputFileTXT.close ( );

      SomaThreeCrazyNodesTest ( );

      SomaBeltTest ( );

      centerSomaInOrigin ( );

      SomaDendriticTest ( );

      if ( pApplyReplicantNodeTest )
        NodeRepetitionTest ( );

      calcDendriticDistance ( );

      calcDendritics ( );

      //Finally preprocess the file
      nodeCollection = preProcessNodeCollection;

      if ( pApplyStdDims )
        applyUniformModifiers ( AXONRADIUSMODIF, DENDRITICRADIUSMODIF, APICALRADIUSMODIFSTD, BASERADIUSMODIF );

      preProcessNodeCollection.clear ( );
    }

    void SWCImporter::centerSomaInOrigin ( )
    {
      SWCNode lNodeAux;
      unsigned int lNumNodes = preProcessNodeCollection.size ( );;

      lNodeAux = preProcessNodeCollection[1];

      displacement = preProcessNodeCollection[1].position;

      //Solo se centra si es necesario centrarlo, si no peta :(
      for ( unsigned int i = 1; i < lNumNodes; ++i )
      {
        preProcessNodeCollection[i].position[0] -= lNodeAux.position[0];
        preProcessNodeCollection[i].position[1] -= lNodeAux.position[1];
        preProcessNodeCollection[i].position[2] -= lNodeAux.position[2];
      }
    }

    void SWCImporter::SomaDendriticTest ( )
    {
      SWCNode lNodeAux;
      SWCNode lNodeSoma;
      OpenMesh::Vec3f lVecDir;
      float lDespDir = 0;

      unsigned int lNumNodes = preProcessNodeCollection.size ( );;

      lNodeSoma = preProcessNodeCollection[1];

      for ( unsigned int i = 1; i < lNumNodes; ++i )
      {
        if ( preProcessNodeCollection[i].parent == SOMAID )
        {
          if ( preProcessNodeCollection[i].position.length ( ) < lNodeSoma.radius )
          {
            lVecDir = preProcessNodeCollection[i].position - lNodeSoma.position;
            lVecDir.normalize ( );

            //Cantidad que se va a desplazar hacia afuera el nodo (la cantida que esta dentro)
            //lDespDir	= lNodeSoma.radius - preProcessNodeCollection[i].position.length();

            //Tb se puede utilizar un Epsilon al canto de la esfera
            lDespDir = MINSOMADESP;

            lVecDir *= ( lNodeSoma.radius + lDespDir );

            preProcessNodeCollection[i].position = lVecDir;
          }
        }
      }
    }

    void SWCImporter::SomaThreeCrazyNodesTest ( )
    {
      SWCNode lNodeAux;
      OpenMesh::Vec3f lBariCenter ( 0, 0, 0 );
      std::vector <SWCNode> lBeltSomaContainer;
      std::vector <SWCNode> lFinalContainer;

      float lRadUnifor = preProcessNodeCollection[1].radius;
      unsigned int lNumEles = 3;
      int lNumNodes = preProcessNodeCollection.size ( );;
      unsigned int i = 1;

      //Exportaci�n de LMeasure, hay que arreglarlo
      if (( preProcessNodeCollection[1].type == SOMAID )
          && ( preProcessNodeCollection[2].type == SOMAID )
          && ( preProcessNodeCollection[3].type == SOMAID )
          && ( preProcessNodeCollection[4].type != SOMAID )
          && ( lRadUnifor == preProcessNodeCollection[1].radius )
          && ( lRadUnifor == preProcessNodeCollection[2].radius )
          && ( lRadUnifor == preProcessNodeCollection[3].radius )
          && ( preProcessNodeCollection[1].position[0] == preProcessNodeCollection[2].position[0] )
          && ( preProcessNodeCollection[1].position[0] == preProcessNodeCollection[3].position[0] )
          && ( preProcessNodeCollection[1].position[2] == preProcessNodeCollection[2].position[2] )
          && ( preProcessNodeCollection[1].position[2] == preProcessNodeCollection[3].position[2] )
              )
      {

        //Calcular el baricentro del cinturon
        for ( i = 1; i <= 3; i++ )
        {
          //Calc the index of the vertex
          lBariCenter += preProcessNodeCollection[i].position;
        }
        //lBariCenter	/=	(3.0);
        lBariCenter = preProcessNodeCollection[1].position;

        ////////////////////////////////////////////////////////////////////////////////////////////
        //Copy soma new data to the final container
        lNodeAux = preProcessNodeCollection[1];
        lNodeAux.position = lBariCenter;
        lNodeAux.radius = lRadUnifor;
        lFinalContainer.push_back ( preProcessNodeCollection[0] );
        lFinalContainer.push_back ( lNodeAux );

        for ( unsigned int j = ( lNumEles + 1 ); j < ( unsigned int ) lNumNodes; j++ )
        {
          lNodeAux = preProcessNodeCollection.at ( j );

          //We mus decrement in 1 becouse we store 1 node for the Soma
          lNodeAux.id -= ( lNumEles - 1 );

          //Caso de nodos de soma en medio del fichero
          //Los transformamos a nodos de dendrita
          //El soma llegar� hasta este punto as� que es viable.
          //Copiamos el tipo y radio del siguiente nodo
          if ( lNodeAux.type == SOMAID )
          {
            lNodeAux.type = preProcessNodeCollection.at ( j + 1 ).type;
            lNodeAux.radius = preProcessNodeCollection.at ( j + 1 ).radius;
          }

          //Se pueden conectar a cualkiera de los nodos que conforma el cinturon inicial del soma
          if (( unsigned int ) lNodeAux.parent <= lNumEles )
          {
            lNodeAux.parent = SOMAID;
          }
          else
          {
            lNodeAux.parent -= ( lNumEles - 1 );
          }

          lFinalContainer.push_back ( lNodeAux );
        }

        preProcessNodeCollection.clear ( );
        preProcessNodeCollection = lFinalContainer;
        lFinalContainer.clear ( );
        ///////////////////////////////////////////////////////////////////////////////////////////////
      }
    }

    void SWCImporter::SomaBeltTest ( )
    {
      SWCNode lNodeAux;
      OpenMesh::Vec3f lBariCenter ( 0, 0, 0 );
      std::vector <SWCNode> lBeltSomaContainer;
      std::vector <SWCNode> lFinalContainer;

      float lDistNode = 10E10;
      unsigned int lNumEles = 0;

      unsigned int lNumNodes = preProcessNodeCollection.size ( );;

      unsigned int i = 1;

      //Solo seleccionamos el cintur�n de soma que se encuentre al principio del fichero
      //No tratamos nodos en medio del fichero
      while ( preProcessNodeCollection[i].type == SOMAID )
      {
        lBeltSomaContainer.push_back ( preProcessNodeCollection[i] );
        ++i;

      }



      //Belt soma case
      lNumEles = lBeltSomaContainer.size ( );

      //Descartamos todos los nodos del belt excepto 1, qu ees con el que definiremos el soma
      mSomaNodesDiscarded = lBeltSomaContainer.size ( ) - 1;

      if ( lNumEles > 1 )
      {

        lNodeAux.id = lNodeAux.type = lNodeAux.position[0] = lNodeAux.position[1]
                = lNodeAux.position[2] = lNodeAux.radius = lNodeAux.parent = 0;
        lFinalContainer.push_back ( lNodeAux );

        //Calcular el baricentro del cinturon
        for ( unsigned int j = 0; j < lNumEles; j++ )
        {
          //Calc the index of the vertex
          lBariCenter += lBeltSomaContainer.at ( j ).position;
        }
        lBariCenter /= ( float ) ( lNumEles );

        //Calcular el radio m�nimo desde el baricentro hasta cualkiera de los nodos del cinturon
        for ( unsigned int j = 0; j < lNumEles; j++ )
        {
          //Calc the index of the vertex
          if (( lBariCenter - lBeltSomaContainer.at ( j ).position ).length ( ) < lDistNode )
          {
            lDistNode = ( lBariCenter - lBeltSomaContainer.at ( j ).position ).length ( );
          }
        }


        //Copy soma new data to the final container
        lNodeAux = preProcessNodeCollection[1];
        lNodeAux.position = lBariCenter;
        lNodeAux.radius = lDistNode;
        lFinalContainer.push_back ( lNodeAux );

        for ( unsigned int j = ( lNumEles + 1 ); j < lNumNodes; j++ )
        {
          lNodeAux = preProcessNodeCollection.at ( j );

          //We mus decrement in 1 becouse we store 1 node for the Soma
          lNodeAux.id -= ( lNumEles - 1 );

          //Caso de nodos de soma en medio del fichero
          //Los transformamos a nodos de dendrita
          //El soma llegar� hasta este punto as� que es viable.
          //Copiamos el tipo y radio del siguiente nodo
          if ( lNodeAux.type == SOMAID )
          {
            lNodeAux.type = preProcessNodeCollection.at ( j + 1 ).type;
            lNodeAux.radius = preProcessNodeCollection.at ( j + 1 ).radius;
          }

          //Se pueden conectar a cualkiera de los nodos que conforma el cinturon inicial del soma
          if (( unsigned int ) lNodeAux.parent <= lNumEles )
          {
            lNodeAux.parent = SOMAID;
          }
          else
          {
            lNodeAux.parent -= ( lNumEles - 1 );
          }

          lFinalContainer.push_back ( lNodeAux );
        }

        preProcessNodeCollection.clear ( );
        preProcessNodeCollection = lFinalContainer;
        lFinalContainer.clear ( );
      }

      lBeltSomaContainer.clear ( );
    }

    void SWCImporter::NodeRepetitionTest ( )
    {
      SWCNode lNodeAux;
      SWCNode lNodeAuxParent;

      std::vector <SWCNode> lFinalContainer;
      std::vector < unsigned int > lRepesContainer;

      lFinalContainer.clear ( );
      lRepesContainer.clear ( );

      unsigned int lNodeToProces = preProcessNodeCollection.size ( );
      unsigned int lNodeRepes = 0;

      //return;

      //Obviamos el nodo sobrante inicial y el del soma (q pone -1)
      lRepesContainer.push_back ( lNodeRepes );
      lRepesContainer.push_back ( lNodeRepes );

      for ( unsigned int i = 2; i < lNodeToProces; ++i )
      {
        lNodeAux = preProcessNodeCollection.at ( i );
        lNodeAuxParent = preProcessNodeCollection.at ( lNodeAux.parent );

        if (
                SonIguales ( lNodeAux.position[0], lNodeAuxParent.position[0] )
                && SonIguales ( lNodeAux.position[1], lNodeAuxParent.position[1] )
                && SonIguales ( lNodeAux.position[2], lNodeAuxParent.position[2] )
                )
        {
          ++lNodeRepes;

          //!!!Ultra nyapa para cuadrar los indices, creo que no funciona con bifurcaciones multiples
          if ((( unsigned int ) lNodeAux.id + 1 ) <= lNodeToProces )
          {
            preProcessNodeCollection.at ( lNodeAux.id + 1 ).parent = lNodeAux.parent;
          }
        }

        lRepesContainer.push_back ( lNodeRepes );
      }

      lNodeRepes = 0;

      //Add the initial node and the soma node
      lFinalContainer.push_back ( preProcessNodeCollection.at ( 0 ));
      lFinalContainer.push_back ( preProcessNodeCollection.at ( 1 ));
      for ( unsigned int i = 2; i < lNodeToProces; ++i )
      {

        lNodeAux = preProcessNodeCollection.at ( i );
        lNodeAuxParent = preProcessNodeCollection.at ( lNodeAux.parent );

        if (
                SonIguales ( lNodeAux.position[0], lNodeAuxParent.position[0] )
                && SonIguales ( lNodeAux.position[1], lNodeAuxParent.position[1] )
                && SonIguales ( lNodeAux.position[2], lNodeAuxParent.position[2] )
                )
        {
          ++lNodeRepes;
        }
        else
        {
          lNodeAux.id -= lNodeRepes;
          lNodeAux.parent -= lRepesContainer.at ( lNodeAux.parent );
          lFinalContainer.push_back ( lNodeAux );
        }
      }

      preProcessNodeCollection.clear ( );
      preProcessNodeCollection = lFinalContainer;
      lFinalContainer.clear ( );
    }

    void SWCImporter::calcDendriticDistance ( )
    {
      //Hay que recorrer cada dendrita hasta detectar el inicio de la sigueinte o el fin del contenedor
      //Quedarnos con las distancia m�s grande, y con los inicios y fin de dendritas cada vez que se inicie y finalize una
      //La distancia es la distancia del nuevo nodo m�s el anterior -> se ira acumulando, si es el soma, sera el radio
      //Las distancias se calculan desde el centro de coordenadas
      //Al termino de cada dendrita, se ha de dividir esa cada distancia por la maxima calculada para que se quede normalizado

      SWCNode lNodeAux;

      float lLocalDistance = 0;
      float lMaxDendriticLong = 0;
      unsigned int lNumNodes = preProcessNodeCollection.size ( );

      unsigned int lInicialDendriticNode = 2;
      dendriticSomaConnection.push_back ( lInicialDendriticNode );

      for ( unsigned int i = 0; i < lNumNodes; ++i )
      {
        preProcessNodeCollection.at ( i ).mDendriticDistanceNorm
                = preProcessNodeCollection.at ( i ).mDendriticDistanceReal
                = preProcessNodeCollection.at ( i ).mDistanceToSoma
                = 0;
      }

      //No process the Soma
      for ( unsigned int i = lInicialDendriticNode; i < lNumNodes; ++i )
      {
        //Distance to soma
        preProcessNodeCollection.at ( i ).mDistanceToSoma = preProcessNodeCollection[i].position.length ( );

        //New dendritic
        if ( preProcessNodeCollection[i].parent == SOMAID )
        {
          //Si no es el nodo inicial -> Normalizamos las distancias entre los nodos, desde el inical al actual
          for ( unsigned int j = lInicialDendriticNode; j < i; ++j )
          {
            preProcessNodeCollection[j].mDendriticDistanceNorm /= lMaxDendriticLong;

            dendriticMaxDistance.push_back ( lMaxDendriticLong );
          }

          lMaxDendriticLong = 0;

          lInicialDendriticNode = i;
          dendriticSomaConnection.push_back ( lInicialDendriticNode );
        }

        lLocalDistance = ( preProcessNodeCollection[i].position
                           - preProcessNodeCollection[( preProcessNodeCollection[i].parent )].position ).length ( );

        preProcessNodeCollection[i].mDendriticDistanceNorm
                = preProcessNodeCollection[i].mDendriticDistanceReal
                = lLocalDistance + preProcessNodeCollection[( preProcessNodeCollection[i].parent )].mDendriticDistanceReal;

        if ( lMaxDendriticLong < preProcessNodeCollection[i].mDendriticDistanceReal )
        {
          lMaxDendriticLong = preProcessNodeCollection[i].mDendriticDistanceReal;
        }
      }

      //Normalize the las dendritic
      for ( unsigned int j = lInicialDendriticNode; j < lNumNodes; ++j )
      {
        preProcessNodeCollection[j].mDendriticDistanceNorm /= lMaxDendriticLong;

        dendriticMaxDistance.push_back ( lMaxDendriticLong );
      }
    }

    void SWCImporter::calcDendritics ( )
    {
      SWCDendriticInfo lSWCDendriticInfo;
      unsigned int lNumNodes = preProcessNodeCollection.size ( );

      unsigned int lInicialDendriticNodeNoSomaConnected = 3;

      //La primera dendrita siempre empieza en 2
      lSWCDendriticInfo.initialNode = 2;

      if ( preProcessNodeCollection[lSWCDendriticInfo.initialNode].type == 2 )
        lSWCDendriticInfo.type = DendriticType::AXON;
      if ( preProcessNodeCollection[lSWCDendriticInfo.initialNode].type == 3 )
        lSWCDendriticInfo.type = DendriticType::BASAL;
      if ( preProcessNodeCollection[lSWCDendriticInfo.initialNode].type == 4 )
        lSWCDendriticInfo.type = DendriticType::APICAL;


      //No process the Soma
      for ( unsigned int i = lInicialDendriticNodeNoSomaConnected; i < lNumNodes; ++i )
      {
        //First of new dendritic
        if ( preProcessNodeCollection[i].parent == SOMAID )
        {
          lSWCDendriticInfo.finalNode = i - 1;

          //Add the new dendritic
          dendritics.push_back ( lSWCDendriticInfo );

          //Prepare the new dendritic
          lSWCDendriticInfo.initialNode = i;

          if ( preProcessNodeCollection[lSWCDendriticInfo.initialNode].type == 2 )
            lSWCDendriticInfo.type = DendriticType::AXON;
          if ( preProcessNodeCollection[lSWCDendriticInfo.initialNode].type == 3 )
            lSWCDendriticInfo.type = DendriticType::BASAL;
          if ( preProcessNodeCollection[lSWCDendriticInfo.initialNode].type == 4 )
            lSWCDendriticInfo.type = DendriticType::APICAL;
        }
      }
      lSWCDendriticInfo.finalNode = lNumNodes - 1;

      //Add the last dendritic
      dendritics.push_back ( lSWCDendriticInfo );
    }

    //Get the description of the file loaded
    std::string SWCImporter::getDescription ( )
    {
      return description;
    }

    //Get the content of the nodes from ini to fin
    std::string SWCImporter::getContent ( int ini, int fin )
    {
      std::string aux = "---------------------------------\n";

      for ( int i = ini; i < fin; ++i )
      {
        aux += "Id:";
        aux += to_string ( nodeCollection[i].id );
        aux += '\n';
        aux += "Type:";
        aux += to_string ( nodeCollection[i].type );
        aux += '\n';
        aux += "Values:(";
        aux += to_string ( nodeCollection[i].position[0] );
        aux += " , ";
        aux += to_string ( nodeCollection[i].position[1] );
        aux += " , ";
        aux += to_string ( nodeCollection[i].position[2] );
        aux += ")";
        aux += '\n';
        aux += "Radius:";
        aux += to_string ( nodeCollection[i].radius );
        aux += '\n';
        aux += "Parent:";
        aux += to_string ( nodeCollection[i].parent );
        aux += '\n';
        aux += "---------------------------------\n";
      }

      return aux;
    }

    //Get number of nodes
    unsigned int SWCImporter::getNumNodes ( ) const
    {
      return nodeCollection.size ( );
    }

    //Get number of dendritic terminations
    unsigned int SWCImporter::getNumDendriticTermination ( ) const
    {
      return dendriticTermination.size ( );
    }

    //Get number of dendritic terminations
    unsigned int SWCImporter::getNumDendriticBifurcations ( ) const
    {
      return dendriticBifurcations.size ( );
    }

    std::vector < int > SWCImporter::getDendriticBifurcations ( ) const
    {
      return dendriticBifurcations;
    }

    SWCNode SWCImporter::getElementAt ( unsigned int i ) const
    {
      if ( i < nodeCollection.size ( ))
        return nodeCollection[i];
      else
        return nodeCollection[0];
    }

    void SWCImporter::modifElement ( unsigned int pId, SWCNode pNewValue )
    {
      nodeCollection[pId] = pNewValue;
    }

    //Generate the connectios between nodes
    void SWCImporter::generateConnectios ( )
    {
      SWCConnection nodeAux;
      for ( unsigned int i = 1; i < nodeCollection.size ( ); ++i )
      {
        nodeAux.ini = nodeCollection[i].parent;
        nodeAux.fin = nodeCollection[i].id;

        //Desechamos el soma
        if (( nodeAux.ini != -1 )
                )
        {
          if ( i + 1 < nodeCollection.size ( ))
          {
            //Si el sigiente no esta conectado asumimos con el actual lo desechamos
            if ( nodeAux.fin == nodeCollection[nodeAux.fin + 1].parent )
            {
              nodeAux.mTermination = false;
              nodeConnections.push_back ( nodeAux );

            }
            else
            {
              dendriticTermination.push_back ( i );

              nodeAux.mTermination = true;
              nodeConnections.push_back ( nodeAux );

            }
          }
            //Last node
          else
          {
            nodeAux.mTermination = false;
            nodeConnections.push_back ( nodeAux );

            dendriticTermination.push_back ( i );
          }
        }
      }
    }

    void SWCImporter::generateBifurcations ( )
    {
      int lConsecValues = 0;
      SWCConnection lNodeAux;
      for ( unsigned int i = 0; i < nodeConnections.size ( ); ++i )
      {
        lNodeAux = nodeConnections.at ( i );
        lConsecValues = lNodeAux.fin - lNodeAux.ini;
        if (( lConsecValues > 1 )
            && ( lNodeAux.ini != SOMAID )
                )
        {
          dendriticBifurcations.push_back ( lNodeAux.ini );
        }
      }
    }

    void SWCImporter::generateBranches ( )
    {
      SWCBranch lSWCBranch;
      unsigned int lFinalDendId;

      unsigned int lIdCounter;

      //Para cada dendrita
      for ( unsigned int i = 0; i < getNumDendritics ( ); ++i )
      {
        lIdCounter
                = lSWCBranch.initialNode
                = getDendritics ( )[i].initialNode;

        lFinalDendId = getDendritics ( )[i].finalNode;

        //Para cada rama
        while ( lIdCounter != lFinalDendId )
        {
          ++lIdCounter;

          if ( isDendriticTermination ( lIdCounter )
               || isDendriticBifurcation ( lIdCounter ))
          {
            lSWCBranch.finalNode = lIdCounter;

            if ((( lSWCBranch.initialNode - nodeCollection[lSWCBranch.initialNode].parent ) > 1 )
                || ( nodeCollection[lSWCBranch.initialNode].parent == 1 )
                    )
            {
              lSWCBranch.consecBranch = false;
            }
            else
              lSWCBranch.consecBranch = true;

            mBranches.push_back ( lSWCBranch );

            lSWCBranch.initialNode = lIdCounter + 1;
          }
        }
      }
    }

    SWCConnection SWCImporter::getConnectionAt ( unsigned int i )
    {
      if ( i < nodeConnections.size ( ))
        return nodeConnections[i];
      else
        return nodeConnections[0];
    }

    //Get number of connection
    unsigned int SWCImporter::getNumConnection ( )
    {
      return nodeConnections.size ( );
    }

    bool SWCImporter::isDendriticTermination ( int id ) const
    {
      int i = 0;
      int tam = getNumDendriticTermination ( );
      bool finded = false;

      while (( i < tam )
             && ( !finded )
              )
      {
        if ( dendriticTermination[i] == id )
          finded = true;
        ++i;
      }
      return finded;
    }

    bool SWCImporter::isDendriticBifurcation ( int id ) const
    {
      int i = 0;
      int tam = getNumDendriticBifurcations ( );
      bool finded = false;

      while (( i < tam )
             && ( !finded )
              )
      {
        if ( dendriticBifurcations[i] == id )
          finded = true;
        ++i;
      }
      return finded;
    }

    //Get number of dendritic terminations
    int SWCImporter::getDendriticTermination ( unsigned int index )
    {
      if ( index < dendriticTermination.size ( ))
        return dendriticTermination[index];
      else
        return -1;
    }

    //Dendritic soma conected
    std::vector < int > SWCImporter::getDendriticSomaConnection ( )
    {
      return dendriticSomaConnection;
    }

    //Dendritic max distance
    std::vector < int > SWCImporter::getDendriticMaxDistance ( )
    {
      return dendriticMaxDistance;
    }

    //Get dendritics
    std::vector <SWCDendriticInfo> SWCImporter::getDendritics ( ) const
    {
      return dendritics;
    }

    unsigned int SWCImporter::getNumDendritics ( )
    {
      return dendritics.size ( );
    }

    //std::string  SWCImporter::exportToFile(std::string pFile)
    void SWCImporter::exportToFile ( std::string pFile )
    {
      //comentarios
      string lFileTXT = pFile;

      std::ofstream outputFileTXT;

      //Clean files
      std::ofstream ( lFileTXT.c_str ( ));

      //Open files to add data
      outputFileTXT.open ( lFileTXT.c_str ( ), std::ios::app );

      //Head of the file
      outputFileTXT << "# " << "Generated with QtNeuroSynth" << endl;

      //Data
      SWCNode nodeAux;
      std::cout.precision ( 4 );

      //Soma, problem with the 0,0,0 position
      nodeAux = nodeCollection.at ( 1 );
      outputFileTXT << nodeAux.id << " "
                    << nodeAux.type << " "
                    << "0.0" << " "
                    << "0.0" << " "
                    << "0.0" << " "
                    << nodeAux.radius << " "
                    << nodeAux.parent << endl;

      for ( unsigned int i = 2; i < nodeCollection.size ( ); ++i )
      {
        nodeAux = nodeCollection.at ( i );

        //Text exit
        outputFileTXT << nodeAux.id << " "
                      << nodeAux.type << " "
                      << nodeAux.position[0] << " "
                      << nodeAux.position[1] << " "
                      << nodeAux.position[2] << " "
                      << nodeAux.radius << " "
                      << nodeAux.parent << endl;
      }

      outputFileTXT.close ( );
    }

    //void SWCImporter::applyUniformModifiers(float pAxonModifier, float pDendriticModifier, float pApicalModifier, float pValBase)
    //{
    //	SWCNode nodeAux;
    //	for (int i=1;i<preProcessNodeCollection.size();++i)
    //	{
    //		nodeAux = preProcessNodeCollection.at(i);
    //
    //		switch (nodeAux.type)
    //		{
    //			case 2:	//* 2 = axon
    //				//preProcessNodeCollection.at(i).radius*=pAxonModifier;
    //				preProcessNodeCollection.at(i).radius	=	pValBase*pAxonModifier;
    //				break;
    //
    //			case 3:	//* 3 = dendrite
    //				//preProcessNodeCollection.at(i).radius*=pDendriticModifier;
    //				//preProcessNodeCollection.at(i).radius	=	pValBase*pDendriticModifier;
    //				preProcessNodeCollection.at(i).radius	=	pValBase*pDendriticModifier - (pValBase*pDendriticModifier*preProcessNodeCollection.at(i).mDendriticDistanceNorm);
    //				break;

    //			case 4:	//* 4 = apical dendrite
    //				//preProcessNodeCollection.at(i).radius*=pApicalModifier;
    //				preProcessNodeCollection.at(i).radius	=	pValBase*pApicalModifier - (pValBase*pApicalModifier*preProcessNodeCollection.at(i).mDendriticDistanceNorm);
    //				break;
    //		}
    //	}
    //}


    void SWCImporter::applyUniformModifiers ( float pAxonModifier,
                                              float pDendriticModifier,
                                              float pApicalModifier,
                                              float pValBase )
    {
      SWCNode nodeAux;
      bool lParamsApicalCalculated = false;
      bool lAllNodesProcesed = false;
      unsigned int j = 0;

      float lApicalDecrement = 0;
      unsigned int lApicalNumNodes = 0;
      unsigned int lApicalLastNode = 0;

      unsigned int lSuddenDecrementLimit = 0;
      float lSuddenDecrement = 0;
      unsigned int lSuddenNumNodes = 0;

      float lSomaRadius = getElementAt ( SOMAID ).radius;

      //C�lculo del incremento para para adelgazar la dendrita apical
      while (
              ( !lParamsApicalCalculated )
              && ( !lAllNodesProcesed )
              )
      {
        nodeAux = nodeCollection.at ( j );

        //Comienzo de la dendrita apical
        if ( nodeAux.type == 4 )
        {
          bool lApicalNodesFinish = false;

          while (( nodeAux.type == 4 )
                 && ( !lApicalNodesFinish )
                 && ( !lAllNodesProcesed )
                  )
          {
            if ( !isDendriticBifurcation ( j ))
            {
              ++lApicalNumNodes;

              //Distacia del soma pero medida desde fuera del soma
              if ( nodeAux.mDendriticDistanceReal < ( SOMAAPICALDISTFACTOR*lSomaRadius ))
              {
                ++lSuddenNumNodes;
                lSuddenDecrementLimit = j;
              }
            }
            else
            {
              if ( lApicalNumNodes != 0 )
              {
                lApicalNodesFinish = true;
                lApicalLastNode = j;
              }
            }

            if ( j == nodeCollection.size ( ) - 1 )
            {
              lAllNodesProcesed = true;
            }
            else
            {
              ++j;
              nodeAux = nodeCollection.at ( j );
            }
          }

          //El m�s 1 es porque debemos tener en cuata el soma, para los decrementos)
          lSuddenDecrement =
                  (( pValBase*APICALRADIUSMODIFSUDDEN ) - ( pValBase*APICALRADIUSMODIFINITIAL ))/( lSuddenNumNodes - 1 );

          //lApicalDecrement = -(APICALRADIUSMODIFINITIAL-APICALRADIUSMODIFINFINAL)/lApicalFirstNumNodes;
          lApicalDecrement = (( pValBase*APICALRADIUSMODIFINITIAL ) - ( pValBase*APICALRADIUSMODIFINFINAL ))
                             /( lApicalNumNodes - ( lSuddenNumNodes + 1 ));

          lParamsApicalCalculated = true;
        }

        if ( j == nodeCollection.size ( ) - 1 )
        {
          lAllNodesProcesed = true;
        }

        ++j;
      }

      for ( unsigned int i = 1; i < nodeCollection.size ( ); ++i )
      {
        nodeAux = nodeCollection.at ( i );

        //C�lculo del radio a trav�s de las distancias normalizadas
        //switch (nodeAux.type)
        //{
        //	case 2:	//* 2 = axon
        //		//preProcessNodeCollection.at(i).radius*=pAxonModifier;
        //		nodeCollection.at(i).radius	=	pValBase*pAxonModifier;
        //		break;
        //
        //	case 3:	//* 3 = dendrite
        //		//preProcessNodeCollection.at(i).radius*=pDendriticModifier;
        //		//preProcessNodeCollection.at(i).radius	=	pValBase*pDendriticModifier;
        //		nodeCollection.at(i).radius	=	pValBase*pDendriticModifier - (pValBase*pDendriticModifier*nodeCollection.at(i).mDendriticDistanceNorm);
        //		break;

        //	case 4:	//* 4 = apical dendrite
        //		//preProcessNodeCollection.at(i).radius*=pApicalModifier;
        //		nodeCollection.at(i).radius	=	pValBase*pApicalModifier - (pValBase*pApicalModifier*nodeCollection.at(i).mDendriticDistanceNorm);
        //		break;
        //}


        switch ( nodeAux.type )
        {
          case 2:  //* 2 = axon
            nodeCollection.at ( i ).radius = pValBase*pAxonModifier;
            break;

          case 3:  //* 3 = dendrite
            nodeCollection.at ( i ).radius = pValBase*pDendriticModifier;
            break;

          case 4:  //* 4 = apical dendrite
            //preProcessNodeCollection.at(i).radius*=pApicalModifier;
            //nodeCollection.at(i).radius	=	pValBase*pApicalModifier - (pValBase*pApicalModifier*nodeCollection.at(i).mDendriticDistanceNorm);
            if ( i < lApicalLastNode )
            {
              //Si es la conexi�n con el Soma
              if ( nodeAux.parent == SOMAID )
              {
                nodeCollection.at ( i ).radius = pValBase*APICALRADIUSMODIFSUDDEN;
              }
                //Resto de nodos del arbol apical antes de la primera bifurcaci�n
              else
              {
                //Adelgazaamiento brusco del arbol apical cerca del soma -> de 10x ha 8x en una distancia equivalente al radio del soma
                if ( i <= lSuddenDecrementLimit )
                {
                  nodeCollection.at ( i ).radius = nodeCollection.at ( nodeAux.parent ).radius - lSuddenDecrement;
                }
                else
                {
                  nodeCollection.at ( i ).radius = nodeCollection.at ( nodeAux.parent ).radius - lApicalDecrement;
                }
              }
            }
            else
            {
              //Caso de anchura constante (dendritas del arbol apical)
              nodeCollection.at ( i ).radius = pValBase*pApicalModifier;
            }

            break;
        }

      }
    }

    void SWCImporter::appendSWCImporter ( const SWCImporter *pImporter )
    {
      unsigned int lActualNumEles = getNumNodes ( );
      unsigned int lNumElesToAppend = pImporter->getNumNodes ( );
      SWCNode lNodeAux;

      //Obviamos el soma
      for ( unsigned int i = 2; i < lNumElesToAppend; ++i )
      {
        lNodeAux = pImporter->getElementAt ( i );

        if ( lNodeAux.parent == SOMAID )
        {
          lNodeAux.id = lActualNumEles;
        }
        else
        {
          lNodeAux.id += lActualNumEles - 2;
          lNodeAux.parent += lActualNumEles - 2;
        }
        nodeCollection.push_back ( lNodeAux );
      }
    }

    void SWCImporter::applyTranformationMatrix ( boost::numeric::ublas::matrix < float > pMatrixTransform )
    {
      unsigned int lActualNumEles = getNumNodes ( );
      SWCNode lNodeAux;

      //Mount transformation matrix

      namespace ublas      = boost::numeric::ublas;
      OpenMesh::Vec3f lTmpPoint;
      ublas::vector < float > lTmpVertex ( 4 );

      for ( unsigned int i = 0; i < lActualNumEles; ++i )
      {
        lTmpPoint = nodeCollection[i].position;

        lTmpVertex[0] = lTmpPoint[0];
        lTmpVertex[1] = lTmpPoint[1];
        lTmpVertex[2] = lTmpPoint[2];
        lTmpVertex[3] = 1;

        lTmpVertex = prod ( pMatrixTransform, lTmpVertex );
        //lTmpVertex = prod (lTmpVertex, pMatrixTransform);

        lTmpPoint[0] = lTmpVertex[0];
        lTmpPoint[1] = lTmpVertex[1];
        lTmpPoint[2] = lTmpVertex[2];

        nodeCollection[i].position = lTmpPoint;
      }
    }

    bool SWCImporter::isIdInContainer ( SWCNode pId, const std::vector <SWCNode> &pVector )
    {

      for ( unsigned int i = 0; i < pVector.size ( ); ++i )
      {
        if (
                SonIguales ( pVector.at ( i ).position[0], pId.position[0] )
                && SonIguales ( pVector.at ( i ).position[1], pId.position[1] )
                && SonIguales ( pVector.at ( i ).position[2], pId.position[2] )
                )
        {
          return true;
        }
      }
      return false;
    }

    OpenMesh::Vec3f SWCImporter::getDisplacement() const {
      return displacement;
    }

    void SWCImporter::restructIdentifiers() {
      std::vector<SWCNode> nodeColletionAux;
      nodeColletionAux.reserve(this->nodeCollection.size());
      std::unordered_map<int,int> translateMap;
      translateMap.reserve(nodeCollection.size());
      nodeColletionAux.push_back(this->nodeCollection[0]);
      nodeColletionAux.push_back(this->nodeCollection[1]);
      translateMap.emplace(0,0);
      translateMap.emplace(1,1);

      std::stack<SWCNode> nodesToProcess;
      nodesToProcess.push(this->nodeCollection[2]);
      int counter = 2;

      while(!nodesToProcess.empty()) {
        SWCNode node = nodesToProcess.top();
        nodesToProcess.pop();
        translateMap[node.id] = counter;
        auto bifurcation = findBifurcations(node.id);
        if (bifurcation != nullptr) {
          OpenMesh::Vec3f segmentPreBif = node.position - nodeCollection[node.id -1].position;
          OpenMesh::Vec3f segmentPostBif1 = nodeCollection[node.id +1].position - node.position;
          OpenMesh::Vec3f segmentPostBif2 = nodeCollection[bifurcation->fin + 1].position - nodeCollection[bifurcation->fin].position;
          double angle1 = computeAngle(segmentPreBif,segmentPostBif1);
          double angle2 = computeAngle(segmentPreBif,segmentPostBif2);

          if ( angle1 < angle2 ) {
            nodesToProcess.push(this->nodeCollection[bifurcation->fin]);
            translateMap.emplace(bifurcation->fin,-1); // La marcamos como que se va a procesar para evitar repetir nodos
            if (translateMap.find(node.id + 1) == translateMap.end() && node.id + 1 < nodeCollection.size())  {
              nodesToProcess.push(nodeCollection[node.id + 1]);
            }
          } else {
            if (translateMap.find(node.id + 1) == translateMap.end() && node.id + 1 < nodeCollection.size()) {
              nodesToProcess.push(nodeCollection[node.id + 1]);
            }
            nodesToProcess.push(this->nodeCollection[bifurcation->fin]);
            translateMap.emplace(bifurcation->fin,-1);
          }

        } else {
          if (translateMap.find(node.id + 1) == translateMap.end() && node.id + 1 < nodeCollection.size()) {
            nodesToProcess.push(nodeCollection[node.id + 1]);
          }
        }

        node.id = counter;
        nodeColletionAux.push_back(node);
        counter++;

      }

      //Traducimos todos los identificadores anteriores a los nuevos identificadores.

      for (size_t i = 2; i < nodeColletionAux.size(); i++) {
        auto* node = &nodeColletionAux[i];
        node->parent = translateMap.at(node->parent);
      }

      for ( auto &node : nodeConnections) {
        node.ini = translateMap.at(node.ini);
        node.fin = translateMap.at(node.fin);
      }

      for ( auto &dentriticBif : dendriticBifurcations) {
        dentriticBif = translateMap.at(dentriticBif);
      }

      for (auto &dendriticTer : dendriticTermination) {
        dendriticTer = translateMap.at(dendriticTer);
      }

      for (auto &dendriticSomaCon : dendriticSomaConnection) {
        dendriticSomaCon = translateMap.at(dendriticSomaCon);
      }

      for (auto &dendritic: dendritics) {
        dendritic.initialNode = translateMap.at(dendritic.initialNode);
        dendritic.finalNode = translateMap.at(dendritic.finalNode);
      }

      for (auto &branch: mBranches) {
        branch.initialNode = translateMap.at(branch.initialNode);
        branch.finalNode = translateMap.at(branch.finalNode);
      }
      nodeCollection = nodeColletionAux;
    }

    const SWCConnection* SWCImporter::findBifurcations(int id) {
      auto it =  std::find (dendriticBifurcations.begin(), dendriticBifurcations.end(), id);
      if ( it != dendriticBifurcations.end()) {
        for (size_t i = 0; i < nodeConnections.size(); i++) {
          auto connection = nodeConnections[i];
          if (connection.ini == id ) {
            if (connection.fin - connection.ini >1) {
              return &nodeConnections[i];
            }
          }
        }
      }
      return nullptr;
    }

    double SWCImporter::computeAngle(OpenMesh::Vec3f v1,OpenMesh::Vec3f v2) {
      double v1_v2 = sqrt(OpenMesh::dot(v1, v1) * OpenMesh::dot(v2,v2) );
      return acos( (OpenMesh::dot(v1,v2)/v1_v2) );

    }


    void SWCImporter::toASCBranch(std::ofstream &file, std::string identation, SWCNode node) {
      identation += "\t";
      const SWCConnection *bifur;

      file << identation << "(\t" << node.position[0] << "\t" << node.position[1] << "\t" << node.position[2] << "\t" << node.radius
           << ")" << std::endl;
      node = nodeCollection[node.id + 1];
      while ((bifur = findBifurcations(node.id)) == nullptr) {
        file << identation << "(\t" << node.position[0] << "\t" << node.position[1] << "\t" << node.position[2] << "\t" << node.radius
             << ")" << std::endl;

        if (isDendriticTermination(node.id)|| isDendriticBifurcation(node.id)) {
          return;
        }

        node = nodeCollection[node.id + 1];
      }

      file << identation << "(" << std::endl;
      toASCBranch(file, identation, nodeCollection[node.id]);
      file << identation << "|" << std::endl;
      toASCBranch(file, identation, nodeCollection[bifur->fin]);
      file << identation << ")" << std::endl;
    }

    void SWCImporter::toASC(const std::vector<std::vector<Eigen::Vector3f>> &contours, const std::string &outFile) {
      //TODO No termina de funcionar bien la parte recursiva.
      std::ofstream file;
      file.open(outFile, std::ofstream::out);

      for (const auto &contour: contours) {
        file << "(\"Soma\" " << std::endl;
        file << "\t" << "(Closed)" << std::endl;
        file << "\t" << "(FillDensity 0)" << std::endl;
        file << "\t" << "(MBFObjectType 5)" << std::endl;
        for (const auto &point: contour) {
          file << "\t" << "(\t" << point[0] + displacement[0] << "\t" << point[1] + displacement[1] << "\t"
               << point[2] + displacement[2] << ")" << std::endl;
        }
        file << ")" << std::endl;
      }

      for (const auto &dend: this->dendritics) {
        std::string identation = "\t";
        if (dend.type == APICAL) {
          file <<"(\t(Color Blue)" << std::endl;
          file << "(Apical)" << std::endl;
        } else {
          file <<"(\t(Color Green)" << std::endl;
          file << "(Dendrite)" << std::endl;
        }

        toASCBranch(file, identation, nodeCollection[dend.initialNode]);
        file << ")" << std::endl;
      }
      file.close();
    }
}

/*
	//!!!Ultra nyapa para cuadrar los indices, creo que no funciona con bifurcaciones multiples
	if ( (lNodeAux.id+1) <= lNodeToProces )
	{
		preProcessNodeCollection.at(lNodeAux.id+1).parent=lNodeAux.parent;
	}

*/

