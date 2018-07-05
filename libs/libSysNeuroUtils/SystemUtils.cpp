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

#include "SystemUtils.h"

//Nota:De momento solo funciona con longitud 1
string changeExps ( string cad, string expSearch, string expRepl )
{
  string aux;

  for ( int i = 0; i < ( int ) cad.length ( ); i++ )
  {
    if ( cad.substr ( i, 1 ) == expSearch )
      aux += expRepl;
    else
      aux += cad[i];
  }
  return aux;
}

string actTime ( )
{
  time_t tAct = time ( NULL );
  string timeStamp = ( string ) asctime ( localtime ( &tAct ));

  //Le quitamos el retorno de carro
  timeStamp = ( timeStamp.substr ( 0, timeStamp.length ( ) - 1 ));

  //Hemos de ustraer los dos puntos porque el SO no se los traga
  timeStamp = changeExps ( timeStamp, ":", "_" );

  return ( timeStamp );
}

void strToFile ( string pFileContent, string pFilePath )
{
  std::string fileContent = pFileContent;
  std::string fileTXT = pFilePath;

  //Clean files
  std::ofstream ( fileTXT.c_str ( ));

  //Open files to add data
  std::ofstream outputFileTXT;
  outputFileTXT.open ( fileTXT.c_str ( ), std::ios::app );

  outputFileTXT << fileContent.c_str ( ) << endl;
  outputFileTXT.close ( );
}



