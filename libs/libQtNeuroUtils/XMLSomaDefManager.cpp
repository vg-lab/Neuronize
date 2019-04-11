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

#include "XMLSomaDefManager.h"

#include <QDebug>

XMLSomaDefManager::XMLSomaDefManager ( void )
{
  mXMLSomaDef = new XMLSomaDef ( );
  mPathToFiles = "";
}

XMLSomaDefManager::~XMLSomaDefManager ( void )
{

}

XMLSomaDef *XMLSomaDefManager::getXMLSomaDef ( )
{
  return mXMLSomaDef;
}

QDomDocument XMLSomaDefManager::getXMLDomDocument ( )
{
  return mDomDoc;
}

void XMLSomaDefManager::setPathToFiles ( QString pPathToFiles )
{
  mPathToFiles = pPathToFiles;
}

QString XMLSomaDefManager::getPathToFiles ( )
{
  return mPathToFiles;
}

void XMLSomaDefManager::loadGeodesicDistanceFilesContent ( )
{
  if ( mPathToFiles.length ( ) == 0 )
    throw "Path to files isnt seted";
  if ( mXMLSomaDef == NULL )
    throw "No content Loadaded in XMLSomaDef";

  //Iterador para recorrido d elas dendritas
  QList < XMLSomaDefDendriticModif >::iterator i;

  //Recorre el contenedor de objetos y los anexa de manera consecutiva en el XML
  for ( i = mXMLSomaDef->dendriticContainer.begin ( ); i != mXMLSomaDef->dendriticContainer.end ( ); ++i )
  {
    ( *i ).loadGeoDistanceFileContainerFromPath ( mPathToFiles );
  }
}

void XMLSomaDefManager::loadGeodesicVertexIdesCandFilesContent ( )
{
  if ( mPathToFiles.length ( ) == 0 )
    throw "Path to files isnt seted";
  if ( mXMLSomaDef == NULL )
    throw "No content Loadaded in XMLSomaDef";

  //Iterador para recorrido d elas dendritas
  QList < XMLSomaDefDendriticModif >::iterator i;

  //Recorre el contenedor de objetos y los anexa de manera consecutiva en el XML
  for ( i = mXMLSomaDef->dendriticContainer.begin ( ); i != mXMLSomaDef->dendriticContainer.end ( ); ++i )
  {
    ( *i ).loadGeoVertexIdsCandFileContainerFromPath ( mPathToFiles );
  }
}



//////////////////////////////////////////////////////////////////////////////////////////
//								Generacion												//
//////////////////////////////////////////////////////////////////////////////////////////

void XMLSomaDefManager::generateXMLContent ( QString pSWCFile,
                                             QString pModelFileName,
                                             QString pDefaultGeodesicFileName,
                                             QString pDefaultGeodesicVertsIdsFileName,
                                             unsigned int pSomaHorResol,
                                             unsigned int pSomaVerResol,
                                             unsigned int pNumDendritics,
                                             float pDefaultaDendriticeValue1,
                                             float pDefaultaDendriticeValue2
)
{
  if ( pSWCFile.length ( ) == 0 )
    throw "SWC file error in XML manager";
  if ( pDefaultGeodesicFileName.length ( ) == 0 )
    throw "Geodesic file name export error in XML manager";

  if ( mXMLSomaDef != NULL )
  {
    delete mXMLSomaDef;
  }

  //Generacion de los datos
  mXMLSomaDef = new XMLSomaDef;

  mXMLSomaDef->mSWCPathFile = pSWCFile;
  mXMLSomaDef->mModelPathFile = pModelFileName;
  mXMLSomaDef->mSomaHorResol = pSomaHorResol;
  mXMLSomaDef->mSomaVerResol = pSomaVerResol;

  //Elementos que se le van anyadiendo
  XMLSomaDefDendriticModif lXMLDendriticModif;

  for ( unsigned int i = 0; i < pNumDendritics; ++i )
  {

    lXMLDendriticModif.mDenType = "BAS";
    lXMLDendriticModif.mId = i;
    lXMLDendriticModif.mGeodeVertexDistanceFile = pDefaultGeodesicFileName + QString::number ( i ) + ".dat";
    lXMLDendriticModif.mGeodeVertexIdsCandByDisFile = pDefaultGeodesicVertsIdsFileName + QString::number ( i ) + ".vid";

    lXMLDendriticModif.mParamValue1 = pDefaultaDendriticeValue1;
    lXMLDendriticModif.mParamValue2 = pDefaultaDendriticeValue2;

    mXMLSomaDef->dendriticContainer.append ( lXMLDendriticModif );
  }

  //Aqui es donde se genera el objeto dom
  generateDomObject ( );
}

void XMLSomaDefManager::generateDomObject ( )
{
  mDomDoc.clear ( );

  //Primera linea del documento XML (clase de fichero XML)
  QDomProcessingInstruction instr = mDomDoc.createProcessingInstruction ( "xml", "version='1.0' encoding='UTF-8'" );

  //Anyade ese nodo al arbol DOM
  mDomDoc.appendChild ( instr );

  //Nodo m�s externo
  QDomElement XMLSomaDefSetElement = addElement ( mDomDoc, mDomDoc, "xmlsomadef", QString::null );

  //Contenido del nodo m�s externo
  if ( !mXMLSomaDef->mSWCPathFile.isEmpty ( ))
    addElement ( mDomDoc, XMLSomaDefSetElement, "mSWCPathFile", mXMLSomaDef->mSWCPathFile );

  if ( !mXMLSomaDef->mSWCPathFile.isEmpty ( ))
    addElement ( mDomDoc, XMLSomaDefSetElement, "mModelPathFile", mXMLSomaDef->mModelPathFile );

  if ( !mXMLSomaDef->mSomaHorResol != -1 )
    addElement ( mDomDoc, XMLSomaDefSetElement, "mSomaHorResol", QString::number ( mXMLSomaDef->mSomaHorResol ));

  if ( !mXMLSomaDef->mSomaVerResol != -1 )
    addElement ( mDomDoc, XMLSomaDefSetElement, "mSomaVerResol", QString::number ( mXMLSomaDef->mSomaVerResol ));


  //Iterador para recorrido d elas dendritas
  QList < XMLSomaDefDendriticModif >::const_iterator i;

  //Recorre el contenedor de objetos y los anexa de manera consecutiva en el XML
  for ( i = mXMLSomaDef->dendriticContainer.begin ( ); i != mXMLSomaDef->dendriticContainer.end ( ); ++i )
  {
    //Cuelga cada nuevo elemento del nodo raiz
    QDomElement h = addElement ( mDomDoc, XMLSomaDefSetElement, "DendriticModif", QString::null );

    //Con su correspondient informacion
    //Que cuelgan de cada nodo que se ha anyadido
    addElement ( mDomDoc, h, "mId", QString::number (( *i ).mId ));
    addElement ( mDomDoc, h, "mDenType", ( *i ).mDenType );
    addElement ( mDomDoc, h, "mGeodeVertexDistanceFile", ( *i ).mGeodeVertexDistanceFile );
    addElement ( mDomDoc, h, "mGeodeVertexIdsCandByDisFile", ( *i ).mGeodeVertexIdsCandByDisFile );

    addElement ( mDomDoc, h, "mParamValue1", QString::number (( *i ).mParamValue1 ));
    addElement ( mDomDoc, h, "mParamValue2", QString::number (( *i ).mParamValue2 ));
  }
}

//Anyade en el documento XML, un nuevo nodo con etiqueas tag y contenido content
QDomElement XMLSomaDefManager::addElement ( QDomDocument &doc,
                                            QDomNode &node,
                                            const QString &tag,
                                            const QString &value )
{
  QDomElement el = doc.createElement ( tag );

  node.appendChild ( el );

  if ( !value.isNull ( ))
  {
    QDomText txt = doc.createTextNode ( value );
    el.appendChild ( txt );
  }
  return el;
}

void XMLSomaDefManager::exportDomToFile ( std::string fileName )
{
  std::string fileContent = mDomDoc.toString ( ).toStdString ( );

  std::string fileTXT = fileName;
  fileTXT += ".xml";

  //Clean files
  std::ofstream ( fileTXT.c_str ( ));

  //Open files to add data
  std::ofstream outputFileTXT;
  outputFileTXT.open ( fileTXT.c_str ( ), std::ios::app );

  outputFileTXT << fileContent.c_str ( ) << endl;
  outputFileTXT.close ( );
}

//////////////////////////////////////////////////////////////////////////////////////////
//								Carga													//
//////////////////////////////////////////////////////////////////////////////////////////

void XMLSomaDefManager::loadXMLContent ( QString pFilePath )
{

  if ( mXMLSomaDef != NULL )
  {
    delete mXMLSomaDef;
  }
  mXMLSomaDef = new XMLSomaDef;

  mDomDoc.clear ( );

  if ( !pFilePath.isNull ( ))
  {
    //Cargamos el fichero
    QFile f ( pFilePath );

    //Control de errores
    QString errorMsg;
    int errorLine, errorColumn;

    //Cargamos todo el contenido del fichero en un objeto DOM para poder recorrerlo y procesarlo
    if ( mDomDoc.setContent ( &f, &errorMsg, &errorLine, &errorColumn ))
    {
      parseXMLwithDOM ( );
    }
  }
}

//Crearemos un objeto holydays desde un XML
void XMLSomaDefManager::parseXMLwithDOM ( )
{
  //Se inicaliza a nulo

  if ( mXMLSomaDef != NULL )
  {
    delete mXMLSomaDef;
  }
  mXMLSomaDef = new XMLSomaDef;

  //Nodo raiz
  QDomElement set = mDomDoc.namedItem ( "xmlsomadef" ).toElement ( );

  //Peta si no existe
  if ( set.isNull ( ))
  {
    qWarning ( ) << "No <holidayset> element found at the top-level "
                 << "of the XML file!";
  }

  // Buscamos una serie de elementos de manera secuencial
  QDomElement name;

  name = set.namedItem ( "mSWCPathFile" ).toElement ( );
  if ( !name.isNull ( ))
    mXMLSomaDef->mSWCPathFile = QString ( name.text ( ));

  name = set.namedItem ( "mModelPathFile" ).toElement ( );
  if ( !name.isNull ( ))
    mXMLSomaDef->mModelPathFile = QString ( name.text ( ));

  name = set.namedItem ( "mSomaHorResol" ).toElement ( );
  if ( !name.isNull ( ))
    mXMLSomaDef->mSomaHorResol = QString ( name.text ( )).toFloat ( );

  name = set.namedItem ( "mSomaVerResol" ).toElement ( );
  if ( !name.isNull ( ))
    mXMLSomaDef->mSomaVerResol = QString ( name.text ( )).toFloat ( );

  // Recorrer los nodos hijos ����cuyo nombre coincida!!!! con el tag suministrado
  // e ir extrayendo la informaci�n de cada uno
  QDomElement n = set.firstChildElement ( "DendriticModif" );

  //Recorrer todos esos nodos (colgados de set que es el padre a esta altura)
  for ( ; !n.isNull ( ); n = n.nextSiblingElement ( "DendriticModif" ))
  {
    //Generamos un objeto holiday que se anyadira a holidayset
    XMLSomaDefDendriticModif h;

    //Transformamos el objeto actual a elemtneo para procesarlo mas facilmente
    QDomElement e = n.toElement ( );

    //Extrameos el valor de name
    QDomElement v;

    v = e.namedItem ( "mId" ).toElement ( );
    if ( !v.isNull ( ))
      h.mId = QString ( v.text ( )).toFloat ( );

    v = e.namedItem ( "mDenType" ).toElement ( );
    if ( !v.isNull ( ))
      h.mDenType = v.text ( );

    v = e.namedItem ( "mGeodeVertexDistanceFile" ).toElement ( );
    if ( !v.isNull ( ))
      h.mGeodeVertexDistanceFile = v.text ( );

    v = e.namedItem ( "mGeodeVertexIdsCandByDisFile" ).toElement ( );
    if ( !v.isNull ( ))
      h.mGeodeVertexIdsCandByDisFile = v.text ( );

    v = e.namedItem ( "mParamValue1" ).toElement ( );
    if ( !v.isNull ( ))
      h.mParamValue1 = QString ( v.text ( )).toFloat ( );

    v = e.namedItem ( "mParamValue2" ).toElement ( );
    if ( !v.isNull ( ))
      h.mParamValue2 = QString ( v.text ( )).toFloat ( );

    //Anyadimos esas vacas al contenedor de vacas
    mXMLSomaDef->dendriticContainer.append ( h );
  }
}


