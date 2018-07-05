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

#include "XMLSomaDefDendriticModif.h"

XMLSomaDefDendriticModif::XMLSomaDefDendriticModif ( void )
{

  mParamValue1 = 0;
  mParamValue2 = 0;

  mId = 0;
  mDenType = "";
  mGeodeVertexDistanceFile = "";
}

XMLSomaDefDendriticModif::~XMLSomaDefDendriticModif ( void )
{
}

void XMLSomaDefDendriticModif::loadGeoDistanceFileContainerFromPath ( QString pPathFile )
{
  //Abrir el fichero
  std::ifstream inputFileTXT;
  QString fileTXT = pPathFile + "/" + mGeodeVertexDistanceFile;
  inputFileTXT.open ( fileTXT.toStdString ( ).c_str ( ), std::ios::in );

  //if (inputFileTXT.fail())
  if ( !inputFileTXT )
  {
    cerr << "An error occurred. Unable to read file: " << fileTXT.toStdString ( ).c_str ( ) << endl;
    exit ( 1 );
  }

  DenModifInfo lDenModifInfo;
  unsigned int i = 0;
  unsigned int lNumNodes = 0;

  //Datos
  mGeoDistanceContainer.clear ( );

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

  lDenModifInfo.id = i;
  lDenModifInfo.distance = atof ( str );

  mGeoDistanceContainer.append ( lDenModifInfo );

  ++i;

  int pos = 0;

  //while (!inputFileTXT.eof())
  while ( inputFileTXT >> lDenModifInfo.distance )
  {
    //inputFileTXT >> lDenModifInfo.distance;
    lDenModifInfo.id = i;

    mGeoDistanceContainer.append ( lDenModifInfo );

    ++i;
  }

  inputFileTXT.close ( );
}

void XMLSomaDefDendriticModif::loadGeoVertexIdsCandFileContainerFromPath ( QString pPathFile )
{
  //Abrir el fichero
  std::ifstream inputFileTXT;
  QString fileTXT = pPathFile + "/" + mGeodeVertexIdsCandByDisFile;
  inputFileTXT.open ( fileTXT.toStdString ( ).c_str ( ), std::ios::in );

  //if (inputFileTXT.fail())
  if ( !inputFileTXT )
  {
    cerr << "An error occurred. Unable to read file: " << fileTXT.toStdString ( ).c_str ( ) << endl;
    exit ( 1 );
  }

  char str[2000];
  unsigned int i = 0;

  mGeoVertexIdsCand.clear ( );

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

  i = atoi ( str );;
  mGeoVertexIdsCand.append ( i );

  while ( inputFileTXT >> i )
  {
    mGeoVertexIdsCand.append ( i );
  }

  inputFileTXT.close ( );
}
