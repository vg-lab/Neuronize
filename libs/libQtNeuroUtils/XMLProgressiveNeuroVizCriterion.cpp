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

#include "XMLProgressiveNeuroVizCriterion.h"

XMLProgressiveNeuroVizCriterion::XMLProgressiveNeuroVizCriterion ( void )
{
}

XMLProgressiveNeuroVizCriterion::~XMLProgressiveNeuroVizCriterion ( void )
{
}

void XMLProgressiveNeuroVizCriterion::loadCriterionFileFromPath ( QString pPathFile )
{
  //Abrir el fichero
  std::ifstream inputFileTXT;
  QString fileTXT = pPathFile + "/" + mCriterionFile;
  inputFileTXT.open ( fileTXT.toStdString ( ).c_str ( ), std::ios::in );

  //if (inputFileTXT.fail())
  if ( !inputFileTXT )
  {
    cerr << "An error occurred. Unable to read file: " << fileTXT.toStdString ( ).c_str ( ) << endl;
    exit ( 1 );
  }

  //DenModifInfo	lDenModifInfo;
  unsigned int i = 0;
  unsigned int lNumNodes = 0;

  //Datos
  mCriterionNodeInfoContainer.clear ( );

  //Tratamiento de las l�neas de comentarios.
  char str[2000];
  char coment;

  //Read all the header
  bool continueReading = true;

  //inputFileTXT >> str;
  inputFileTXT.getline ( str, 2000 );

  if ( str[0] == '#' )
  {
    while ( continueReading )
    {
      if ( str[0] != '#' )
        continueReading = false;
      else
        inputFileTXT.getline ( str, 2000 );
    }
  }

  CriterionNodeInfo lCriterionNodeInfo;

  lCriterionNodeInfo.mNodeId
    //=	lCriterionNodeInfo.mLevel
    = lCriterionNodeInfo.mValue
    = 0;

  //Node readed
  while ( !inputFileTXT.eof ( ))
  {
    inputFileTXT >> lCriterionNodeInfo.mNodeId;
    inputFileTXT >> lCriterionNodeInfo.mValue;
    //inputFileTXT >> lCriterionNodeInfo.mLevel;

    mCriterionNodeInfoContainer.push_back ( lCriterionNodeInfo );
  }

  //Remove the last element if it is replicated
  int tam = mCriterionNodeInfoContainer.size ( );
  lCriterionNodeInfo = mCriterionNodeInfoContainer[tam - 1];
  if (( tam - 1 ) != lCriterionNodeInfo.mNodeId )
    mCriterionNodeInfoContainer.pop_back ( );

  //Close the file
  inputFileTXT.close ( );
}
