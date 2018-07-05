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

#include "XMLProgressiveNeuroVizManager.h"

#include <QFileInfo>
#include <QDebug>

XMLProgressiveNeuroVizManager::XMLProgressiveNeuroVizManager ( void )
{
  mXMLProgressiveNeuroViz = new XMLProgressiveNeuroViz ( );
  mPathToFiles = "";
}

XMLProgressiveNeuroVizManager::~XMLProgressiveNeuroVizManager ( void )
{
  if ( mXMLProgressiveNeuroViz != NULL )
    delete mXMLProgressiveNeuroViz;
}

XMLProgressiveNeuroViz *XMLProgressiveNeuroVizManager::getXMLProgressiveNeuroViz ( ) const
{
  return mXMLProgressiveNeuroViz;
}

QDomDocument XMLProgressiveNeuroVizManager::getXMLDomDocument ( )
{
  return mDomDoc;
}

void XMLProgressiveNeuroVizManager::setPathToFiles ( QString pPathToFiles )
{
  mPathToFiles = pPathToFiles;
}

QString XMLProgressiveNeuroVizManager::getPathToFiles ( )
{
  return mPathToFiles;
}

QDomElement XMLProgressiveNeuroVizManager::addElement ( QDomDocument &doc,
                                                        QDomNode &node,
                                                        const QString &tag,
                                                        const QString &value )
{
  QDomElement lQDomElement = doc.createElement ( tag );

  node.appendChild ( lQDomElement );

  if ( !value.isNull ( ))
  {
    QDomText txt = doc.createTextNode ( value );
    lQDomElement.appendChild ( txt );
  }

  return lQDomElement;
}

void XMLProgressiveNeuroVizManager::loadCriterionsValues ( )
{
  if ( mPathToFiles.length ( ) == 0 )
    throw "Path to files is not seted";
  if ( mXMLProgressiveNeuroViz == NULL )
    throw "No content Loadaded in XMLProgressiveNeuroViz";

  //Iterador para recorrido de las dendritas
  QList < XMLProgressiveNeuroVizCriterion >::iterator i;

  //Recorre el contenedor de objetos y los anexa de manera consecutiva en el XML
  for ( i = mXMLProgressiveNeuroViz->mXMLProgressiveNeuroVizCriterionContainer.begin ( );
        i != mXMLProgressiveNeuroViz->mXMLProgressiveNeuroVizCriterionContainer.end ( ); ++i )
  {
    ( *i ).loadCriterionFileFromPath ( mPathToFiles );
  }
}

void XMLProgressiveNeuroVizManager::generateXMLContent ( QString pSWCFile,
                                                         QString pModelFileName,
                                                         NeuronType pNeuronType,
                                                         unsigned int pSomaResol,
                                                         unsigned int pModelHorResol,
                                                         unsigned int pModelVerResol
)
{
  if ( pSWCFile.length ( ) == 0 )
    throw "SWC file error in XML manager";
  //if (pDefaultGeodesicFileName.length()==0)	throw	"Geodesic file name export error in XML manager";

  if ( mXMLProgressiveNeuroViz != NULL )
  {
    delete mXMLProgressiveNeuroViz;
  }

  //Generacion de los datos
  mXMLProgressiveNeuroViz = new XMLProgressiveNeuroViz ( );

  QFileInfo file ( pSWCFile );
  mXMLProgressiveNeuroViz->mSWCFile = file.fileName ( );

  file = QFileInfo ( pModelFileName );
  mXMLProgressiveNeuroViz->mModelFile = file.fileName ( );

  mXMLProgressiveNeuroViz->mNeuronType = pNeuronType;

  mXMLProgressiveNeuroViz->mModelHorResol = pModelHorResol;
  mXMLProgressiveNeuroViz->mModelVerResol = pModelVerResol;
  mXMLProgressiveNeuroViz->mSomaResol = pSomaResol;

  //Elementos que se le van anyadiendo
  XMLProgressiveNeuroVizCriterion lXMLProgressiveNeuroVizCriterion;

  //for (unsigned int i=0;i<pNumDendritics;++i)
  for ( unsigned int i = 0; i < mXMLProgressiveNeuroViz->mCriterionTypes.numCriterions ( ); ++i )
  {
    //lXMLProgressiveNeuroVizCriterion.mCriterionType	= "CriterionType_" + mXMLProgressiveNeuroViz->mCriterionTypes.getCriterionById(i);
    lXMLProgressiveNeuroVizCriterion.mCriterionType = mXMLProgressiveNeuroViz->mCriterionTypes.getCriterionById ( i );
    lXMLProgressiveNeuroVizCriterion.mCriterionFile =
      "CriterionFile_" + mXMLProgressiveNeuroViz->mCriterionTypes.getCriterionById ( i ).CriterionName;

    mXMLProgressiveNeuroViz->mXMLProgressiveNeuroVizCriterionContainer.append ( lXMLProgressiveNeuroVizCriterion );
  }

  //Aqui es donde se genera el objeto dom
  generateDomObject ( );
}

void XMLProgressiveNeuroVizManager::generateDomObject ( )
{
  mDomDoc.clear ( );

  //Primera linea del documento XML (clase de fichero XML)
  QDomProcessingInstruction instr = mDomDoc.createProcessingInstruction ( "xml", "version='1.0' encoding='UTF-8'" );

  //Anyade ese nodo al arbol DOM
  mDomDoc.appendChild ( instr );

  //Nodo m�s externo
  QDomElement
    lXMLProgressiveNeuroVizSetElement = addElement ( mDomDoc, mDomDoc, "XMLProgressiveNeuroViz", QString::null );

  //Contenido del nodo m�s externo
  if ( !mXMLProgressiveNeuroViz->mSWCFile.isEmpty ( ))
    addElement ( mDomDoc, lXMLProgressiveNeuroVizSetElement, "mSWCFile", mXMLProgressiveNeuroViz->mSWCFile );

  if ( !mXMLProgressiveNeuroViz->mModelFile.isEmpty ( ))
    addElement ( mDomDoc, lXMLProgressiveNeuroVizSetElement, "mModelFile", mXMLProgressiveNeuroViz->mModelFile );

  if ( !mXMLProgressiveNeuroViz->mNeuronType != -1 )
    addElement ( mDomDoc,
                 lXMLProgressiveNeuroVizSetElement,
                 "mNeuronType",
                 QString::number ( mXMLProgressiveNeuroViz->mNeuronType ));

  if ( !mXMLProgressiveNeuroViz->mSomaResol != -1 )
    addElement ( mDomDoc,
                 lXMLProgressiveNeuroVizSetElement,
                 "mSomaResol",
                 QString::number ( mXMLProgressiveNeuroViz->mSomaResol ));

  if ( !mXMLProgressiveNeuroViz->mModelHorResol != -1 )
    addElement ( mDomDoc,
                 lXMLProgressiveNeuroVizSetElement,
                 "mModelHorResol",
                 QString::number ( mXMLProgressiveNeuroViz->mModelHorResol ));

  if ( !mXMLProgressiveNeuroViz->mModelVerResol != -1 )
    addElement ( mDomDoc,
                 lXMLProgressiveNeuroVizSetElement,
                 "mModelVerResol",
                 QString::number ( mXMLProgressiveNeuroViz->mModelVerResol ));


  //Iterador para recorrido de los criterios
  QList < XMLProgressiveNeuroVizCriterion >::const_iterator i;

  //Recorre el contenedor de objetos y los anexa de manera consecutiva en el XML
  for ( i = mXMLProgressiveNeuroViz->mXMLProgressiveNeuroVizCriterionContainer.begin ( );
        i != mXMLProgressiveNeuroViz->mXMLProgressiveNeuroVizCriterionContainer.end ( ); ++i )
  {
    //Cuelga cada nuevo elemento del nodo raiz
    QDomElement h = addElement ( mDomDoc, lXMLProgressiveNeuroVizSetElement, "Criterion", QString::null );

    //Con su correspondient informacion
    //Que cuelgan de cada nodo que se ha anyadido
    addElement ( mDomDoc, h, "mCriterionType", ( *i ).mCriterionType.CriterionName );
    addElement ( mDomDoc, h, "mCriterionFile", ( *i ).mCriterionFile );
  }

}

void XMLProgressiveNeuroVizManager::exportDomToFile ( std::string fileName )
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

void XMLProgressiveNeuroVizManager::loadXMLContent ( QString pFilePath )
{
  if ( mXMLProgressiveNeuroViz != NULL )
  {
    delete mXMLProgressiveNeuroViz;
  }
  mXMLProgressiveNeuroViz = new XMLProgressiveNeuroViz ( );

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

void XMLProgressiveNeuroVizManager::parseXMLwithDOM ( )
{
  //Se inicaliza a nulo

  if ( mXMLProgressiveNeuroViz != NULL )
  {
    delete mXMLProgressiveNeuroViz;
  }
  mXMLProgressiveNeuroViz = new XMLProgressiveNeuroViz ( );

  //Nodo raiz
  QDomElement set = mDomDoc.namedItem ( "XMLProgressiveNeuroViz" ).toElement ( );

  //Peta si no existe
  if ( set.isNull ( ))
  {
    qWarning ( ) << "Node problem at top level!";
  }

  // Buscamos una serie de elementos de manera secuencial
  QDomElement name;

  name = set.namedItem ( "mSWCFile" ).toElement ( );
  if ( !name.isNull ( ))
    mXMLProgressiveNeuroViz->mSWCFile = QString ( name.text ( ));

  name = set.namedItem ( "mModelFile" ).toElement ( );
  if ( !name.isNull ( ))
    mXMLProgressiveNeuroViz->mModelFile = QString ( name.text ( ));

  name = set.namedItem ( "mNeuronType" ).toElement ( );
  if ( !name.isNull ( ))
    mXMLProgressiveNeuroViz->mNeuronType = ( NeuronType ) ( QString ( name.text ( )).toInt ( ));

  name = set.namedItem ( "mSomaResol" ).toElement ( );
  if ( !name.isNull ( ))
    mXMLProgressiveNeuroViz->mSomaResol = QString ( name.text ( )).toFloat ( );

  name = set.namedItem ( "mModelHorResol" ).toElement ( );
  if ( !name.isNull ( ))
    mXMLProgressiveNeuroViz->mModelHorResol = QString ( name.text ( )).toFloat ( );

  name = set.namedItem ( "mModelVerResol" ).toElement ( );
  if ( !name.isNull ( ))
    mXMLProgressiveNeuroViz->mModelVerResol = QString ( name.text ( )).toFloat ( );

  // Recorrer los nodos hijos ����cuyo nombre coincida!!!! con el tag suministrado
  // e ir extrallendo la informaci�n de cada uno
  QDomElement n = set.firstChildElement ( "Criterion" );

  //Recorrer todos esos nodos (colgados de set que es el padre a esta altura)
  for ( ; !n.isNull ( ); n = n.nextSiblingElement ( "Criterion" ))
  {
    XMLProgressiveNeuroVizCriterion h;

    //Transformamos el objeto actual a elemtneo para procesarlo mas facilmente
    QDomElement e = n.toElement ( );

    //Extrameos el valor de name
    QDomElement v;

    v = e.namedItem ( "mCriterionType" ).toElement ( );
    if ( !v.isNull ( ))
    {
      h.mCriterionType.CriterionName = v.text ( );
      h.mCriterionType.id =
        mXMLProgressiveNeuroViz->mCriterionTypes.getCriterionByName ( h.mCriterionType.CriterionName ).id;
    }

    v = e.namedItem ( "mCriterionFile" ).toElement ( );
    if ( !v.isNull ( ))
      h.mCriterionFile = v.text ( );

    mXMLProgressiveNeuroViz->mXMLProgressiveNeuroVizCriterionContainer.append ( h );
  }
}
