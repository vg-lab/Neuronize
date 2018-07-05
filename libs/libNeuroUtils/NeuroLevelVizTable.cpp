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

#include "NeuroLevelVizTable.h"

NeuroLevelVizTable::NeuroLevelVizTable ( void )
{
  mContainer.clear ( );
  mMaxLevel = 0;
  mEvaluatorLevelParam = 1;
}

NeuroLevelVizTable::NeuroLevelVizTable ( SWCImporter &pSWCImporter )
{
  loadFromSWC ( pSWCImporter );
  mMaxLevel = 0;
  mEvaluatorLevelParam = 1;
}

NeuroLevelVizTable::~NeuroLevelVizTable ( void )
{

}

void NeuroLevelVizTable::loadFromSWC ( SWCImporter &pSWCImporter )
{
  mSWCImporter = &pSWCImporter;

  mContainer.clear ( );
  NeuroLevelVizNode lNLVNode;
  const int lNumNodes = pSWCImporter.getNumNodes ( );
  SWCNode lAuxSWCNode;

  mContainer.resize ( lNumNodes );

  //EL primer nodo se desecha ya que est�
  //solo para que cuadren los �ndices del vector con el identificador de nodo
  for ( int i = 0; i <= lNumNodes; ++i )
  {
    lAuxSWCNode = pSWCImporter.getElementAt ( i );
    mContainer.at ( lAuxSWCNode.id ).setId ( lAuxSWCNode.id );

    if ( lAuxSWCNode.parent != -1 )
    {
      mContainer.at ( lAuxSWCNode.parent ).addNewChild ( lAuxSWCNode.id );
    }
    else
    {
      mContainer.at ( 0 ).addNewChild ( -1 );
    }
  }
}

void NeuroLevelVizTable::loadValuesFromFile ( string pFile )
{
  inputFileTXT.clear ( );
  inputFileTXT.open ( pFile.c_str ( ), std::ios::in );

  if ( !inputFileTXT )
  {
    cerr << "An error occurred. Unable to read input file." << pFile << endl;
    exit ( 1 );
  }

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
      //description+=str;
      //description+='\n';
    }
  }

  int lNode;
  float lValue;

  //Leemos le primer identificador del nodo (id del soma)
  //y lo enchufamos
  inputFileTXT >> lNode;
  inputFileTXT >> lValue;

  //Este es par ael nodo 0, que solo esta por motivos de indexaci�n
  mContainer.at ( lNode ).setParam ( lValue );
  //Este es el que correponde al soma
  mContainer.at ( 1 ).setParam ( lValue );

  //Descarte de nodos descartados por el testBelt
  int lSomaNodesDiscarted = mSWCImporter->getSomaNodesDiscarded ( );

  //Desechamos los valores siguientes que tengan que ver con el SOma
  for ( int i = 0; i < lSomaNodesDiscarted; ++i )
  {
    inputFileTXT >> lNode;
    inputFileTXT >> lValue;
  }

  //Anyadimos la info del resto de nodos
  while ( !inputFileTXT.eof ( ))
  {
    inputFileTXT >> lNode;
    inputFileTXT >> lValue;

    //Hemos de sumar 1 para contar adem�s del identificador del Soma, la posici�n 0 para indexar correctamente
    mContainer.at (( lNode - ( lSomaNodesDiscarted ))).setParam ( lValue );
  }

  //Close the file
  inputFileTXT.close ( );
}

void NeuroLevelVizTable::calculateLevelsEvaluatorLevelParam ( )
{
  const int lNumNodes = mContainer.size ( );
  mMaxLevel = 0;

  for ( int i = 1; i < lNumNodes; ++i )
  {
    mContainer.at ( i ).setLevel ( mContainer.at ( i ).getParam ( )/mEvaluatorLevelParam );

    if ( mContainer.at ( i ).getLevel ( ) > mMaxLevel )
    {
      mMaxLevel = mContainer.at ( i ).getLevel ( );
    }
  }
}

void NeuroLevelVizTable::addNode ( const NeuroLevelVizNode &pNode )
{
  mContainer[pNode.getId ( )] = pNode;
}

string NeuroLevelVizTable::toString ( )
{
  string lAuxString = "#Neuro Level Visualization Container State\n";

  //Num entries in the table
  const int lNumEles = mContainer.size ( );

  //Iterators for child nodes
  list < int >::const_iterator lCIt;

  for ( int i = 0; i < lNumEles; ++i )
  {
    lAuxString += "\n ";
    lAuxString += "Node Id=";
    lAuxString += to_string ( mContainer.at ( i ).getId ( ));
    lAuxString += " ";
    lAuxString += "Level=";
    lAuxString += to_string ( mContainer.at ( i ).getLevel ( ));
    lAuxString += " ";

    lCIt = mContainer.at ( i ).getChilds ( ).begin ( );

    lAuxString += "Childs= (";
    for ( ; lCIt != mContainer.at ( i ).getChilds ( ).end ( ); ++lCIt )
    {
      lAuxString += to_string ( *lCIt );
      lAuxString += ", ";
    }

    lAuxString += ")";
  }

  /*
  //Prueba de exportacion temporal para nodes
  //Iterators for child nodes
  list<int>::const_iterator lCIt;

  for (int i=0;i<lNumEles;++i)
  {
    lAuxString+=to_string(mContainer.at(i).getId() ); lAuxString+=" ";
    lAuxString+=to_string(mContainer.at(i).getParam() ); lAuxString+=" ";
    lAuxString+="\n";
  }
*/

  return lAuxString;
}

void NeuroLevelVizTable::autoLoadDistanceLevels ( const SWCImporter &pSWCImporter, float pDistDivisor )
{

  for ( int i = 0; i < mContainer.size ( ); ++i )
  {
    mContainer.at ( i ).setParam ( pSWCImporter.getElementAt ( i ).mDendriticDistanceReal );
  }
}



