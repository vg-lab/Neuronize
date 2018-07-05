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
 * @NeuroDistGenerator
 * @NeuroDistGenerator.cpp
 * @author Juan Pedro Brito M�ndez
 * @version 0.1
 */
#include "NeuroDistGenerator.h"

#include <randomUtils.h>

#include <iostream>
using namespace std;

/*
 * Description
 * @author Juan Pedro Brito Mendez
 */
namespace NSNeuroDistGenerator
{
  /**
   *
   */
  NeuroDistGenerator::NeuroDistGenerator ( )
  {
    fileVersion = 0.03;
  }

  NeuroDistGenerator::~NeuroDistGenerator ( )
  {

  }

  void NeuroDistGenerator::setAllParams ( int PTotalNeurons,
                                          int PDimX,
                                          int PDimY,
                                          int PDimZ,
                                          string PFileName,

                                          REAL PPorcentPiram,
                                          REAL PPorcentIntern,
                                          int PNumLevels,

                                          int PNumModelsPiram,
                                          int PNumModelsIntern,
                                          int pMinimalSeparation,
                                          float pRangoEscaladoMin,
                                          float pRangoEscaladoMax,

                                          std::vector < int > pPiramsPercent,
                                          std::vector < int > pIntersPercent
  )
  {

    totalNeurons = PTotalNeurons;
    dimX = PDimX;
    dimY = PDimY;
    dimZ = PDimZ;

    fileName = PFileName;

    porcentPiram = PPorcentPiram;
    porcentIntern = PPorcentIntern;
    numLevels = PNumLevels;

    numModelsPiram = PNumModelsPiram;
    numModelsIntern = PNumModelsIntern;

    mMinimalSeparation = pMinimalSeparation;

    mRangoEscaladoMin = pRangoEscaladoMin;
    mRangoEscaladoMax = pRangoEscaladoMax;

    mPiramsPercet = pPiramsPercent;
    mIntersPercet = pIntersPercent;
  }

  void NeuroDistGenerator::Prepare ( )
  {
    levelDistances = ( float ) dimY/( float ) numLevels;

    numPiramidals = ( totalNeurons*porcentPiram )/100;
    numInterneurons = ( totalNeurons*porcentIntern )/100;

    int lNumberOfNeuron = 0;
    int lTotalNumberOfNeuron = 0;

    for ( int i = 0; i < numLevels; ++i )
    {
      lNumberOfNeuron = ( numPiramidals*mPiramsPercet[i] )/100.0;
      mNumPiramsPerLayer.push_back ( lNumberOfNeuron );
      lTotalNumberOfNeuron += lNumberOfNeuron;
    }

    for ( int i = 0; i < numLevels; ++i )
    {
      lNumberOfNeuron = ( numInterneurons*mIntersPercet[i] )/100.0;
      mNumIntersPerLayer.push_back ( lNumberOfNeuron );
      lTotalNumberOfNeuron += lNumberOfNeuron;
    }

    //Testear si estan todas las neuronas en el total de los contenedores
    int lContainerSelection = 0;
    int lLayerPos = 0;

    if ( lTotalNumberOfNeuron < totalNeurons )
    {
      unsigned int lRestOfNeurons = abs ( lTotalNumberOfNeuron - totalNeurons );

      int i = 0;
      while ( i < lRestOfNeurons )
      {
        lContainerSelection = randint ( 1, 2 );
        lLayerPos = randint ( numLevels );
        if ( lContainerSelection == 1 )
        {
          if ( mNumIntersPerLayer[lLayerPos] > 0 )
          {
            mNumIntersPerLayer.at ( lLayerPos )++;
            ++i;
          }
        }
        else
        {
          if ( mNumPiramsPerLayer[lLayerPos] > 0 )
          {
            mNumPiramsPerLayer.at ( lLayerPos )++;
            ++i;
          }
        }
      }
    }

    mNodeContainer.clear ( );

    //Random must be initialized before
    //initrand();

  }

  ///**
  // * Generate the distribution with format: Position(x,y,z), rotation angle, neuron type.
  // */
  //void NeuroDistGenerator::generateDistribution()
  //{
  //	//Prepare the data
  //	Prepare();

  //	NodeNDist lNodeNDist;

  //	int id=0;	//Identificator of neuron model

  //	float lInitialLimit;
  //	float lFinalLimit;

  //	//Interneuros
  //	//!!! Old version (old the interneurons)
  //	//for (int i=0;i<numInterneurons;++i)

  //	for (int i=0;i<numLevels;++i)
  //	{
  //		for (int t=0;t<mNumIntersPerLayer[i];++t)
  //		{
  //			bool lCorrectPosition	=	false;
  //
  //			lInitialLimit	= i * levelDistances;
  //			lFinalLimit		= (i+1) * levelDistances;

  //			while (!lCorrectPosition)
  //			{
  //				//Position of the neurons
  //				lNodeNDist.mPos = QVector3D(randfloat(0.0,(float)dimX)
  //											,randfloat(lInitialLimit,(float)lFinalLimit)
  //											,randfloat(0.0,(float)dimZ));

  //				//for (int j=0;j<mNodeContainer.size();++j)
  //				if (mNodeContainer.size()==0) lCorrectPosition=true;

  //				int j=0;
  //				while (	(!lCorrectPosition)
  //						&& (j<mNodeContainer.size())
  //						)
  //				{
  //					if ( (mNodeContainer.at(j).mPos - lNodeNDist.mPos).length()>mMinimalSeparation )
  //					{
  //						lCorrectPosition	= true;
  //						//break;
  //					}
  //					++j;
  //				}
  //			}

  //			//Rotation angle of the neuron model
  //			lNodeNDist.mAngle = randint(360);

  //			//Scale of the neuron model
  //			lNodeNDist.mScale	= randfloat (mRangoEscaladoMin, mRangoEscaladoMax);

  //			//Name of the neuron
  //			int id = randint(numModelsIntern);
  //			string strNum;
  //
  //			if (id>=100)
  //			{
  //				strNum = to_string (id);
  //			}
  //			else
  //			{
  //				if (id>=10)
  //				{
  //					strNum="0";
  //					strNum+=to_string (id);
  //				}
  //				else
  //				{
  //					strNum="00";
  //					strNum+=to_string (id);
  //				}
  //			}

  //			lNodeNDist.mName = "Inter_";
  //			lNodeNDist.mName += strNum;

  //			mNodeContainer.push_back(lNodeNDist);
  //		}
  //	}

  //	//Pirams
  //	//outputFile <<"#Interneurons:"<<endl;
  //	//for (int i=0;i<numPiramidals;++i)
  //	//Interneuros
  //	//!!! Old version (old the interneurons)
  //	//for (int i=0;i<numInterneurons;++i)

  //	for (int i=0;i<numLevels;++i)
  //	{
  //		for (int t=0;t<mNumPiramsPerLayer[i];++t)
  //		{
  //			bool lCorrectPosition	=	false;
  //
  //			lInitialLimit	= i * levelDistances;
  //			lFinalLimit		= (i+1) * levelDistances;
  //			while (!lCorrectPosition)
  //			{
  //				//Position of the neurons
  //				lNodeNDist.mPos = QVector3D(randfloat(0.0,(float)dimX)
  //											,randfloat(lInitialLimit,(float)lFinalLimit)
  //											,randfloat(0.0,(float)dimZ));

  //				//for (int j=0;j<mNodeContainer.size();++j)
  //				int j=0;
  //				while (	(!lCorrectPosition)
  //						&& (j<mNodeContainer.size())
  //						)
  //				{
  //					if ( (mNodeContainer.at(j).mPos - lNodeNDist.mPos).length()>mMinimalSeparation )
  //					{
  //						lCorrectPosition	= true;
  //						//break;
  //					}
  //					++j;
  //				}
  //			}

  //			//Rotation angle of the neuron model
  //			lNodeNDist.mAngle = randint(360);

  //			//Scale of the neuron model
  //			lNodeNDist.mScale	= randfloat (mRangoEscaladoMin, mRangoEscaladoMax);

  //			//Name of the neuron
  //			int id = randint(numModelsIntern);
  //			string strNum;
  //
  //			if (id>=100)
  //			{
  //				strNum = to_string (id);
  //			}
  //			else
  //			{
  //				if (id>=10)
  //				{
  //					strNum="0";
  //					strNum+=to_string (id);
  //				}
  //				else
  //				{
  //					strNum="00";
  //					strNum+=to_string (id);
  //				}
  //			}

  //			lNodeNDist.mName = "Piram_";
  //			lNodeNDist.mName += strNum;

  //			mNodeContainer.push_back(lNodeNDist);
  //		}
  //	}

  //	//Reset the containers
  //	mPiramsPercet.clear();
  //	mIntersPercet.clear();

  //	mNumPiramsPerLayer.clear();
  //	mNumIntersPerLayer.clear();

  //}


  /**
   * Generate the distribution with format: Position(x,y,z), rotation angle, neuron type.
   */
  void NeuroDistGenerator::generateDistribution ( )
  {
    //Prepare the data
    Prepare ( );

    NodeNDist lNodeNDist;

    int id = 0;  //Identificator of neuron model

    float lInitialLimit;
    float lFinalLimit;

    unsigned int lInterCont = 0;
    unsigned int lPiramCont = 0;

    std::vector < unsigned int > lDistribLayer;

    //Interneuros
    //!!! Old version (old the interneurons)
    //for (int i=0;i<numInterneurons;++i)

    for ( int i = 0; i < numLevels; ++i )
    {

      lDistribLayer.clear ( );

      int lNP = 0;
      int lNI = 0;
      int lOpt = 0;
      int lAux = 0;
      int lNumNeurons = mNumIntersPerLayer[i] + mNumPiramsPerLayer[i];

      //Metemos todas las interneuronas
      for ( unsigned int j = 0; j < mNumIntersPerLayer[i]; ++j )
      {
        lDistribLayer.push_back ( 0 );
      }

      //Metemos todas las pramidales
      for ( unsigned int j = 0; j < mNumPiramsPerLayer[i]; ++j )
      {
        lDistribLayer.push_back ( 1 );
      }

      for ( unsigned int j = 0; j < lNumNeurons; ++j )
      {
        lNI = randint ( 0, lDistribLayer.size ( ) - 1 );
        lNP = randint ( 0, lDistribLayer.size ( ) - 1 );

        lAux = lDistribLayer[lNI];
        lDistribLayer[lNI] = lDistribLayer[lNP];
        lDistribLayer[lNP] = lAux;
      }


      ////make de distrib layer
      //for (unsigned int j=0;j<lNumNeurons;++j)
      //{
      //	int lIniLimit=0;
      //	int lFinLimit=0;

      //	//0 == Interneuronas
      //	//1 == Piramidales

      //	if (lNI<mNumIntersPerLayer[i])
      //	{
      //		lIniLimit=-1;
      //		++lNI;
      //	}
      //	else
      //	{
      //		lIniLimit=1;
      //	}

      //	if (lNP<mNumPiramsPerLayer[i])
      //	{
      //		lFinLimit=1;
      //		++lNP;
      //	}
      //	else
      //	{
      //		lFinLimit=-1;
      //	}

      //	float lVal = randfloat(lIniLimit,lFinLimit);
      //	if (lVal<0)	lDistribLayer.push_back(0);
      //	else		lDistribLayer.push_back(1);
      //}

      for ( unsigned int j = 0; j < lDistribLayer.size ( ); ++j )
      {

        //for (int lInterCont=0;lInterCont<mNumIntersPerLayer[i];++lInterCont)
        if ( lDistribLayer.at ( j ) == 0 )
        {
          bool lCorrectPosition = false;

          lInitialLimit = i*levelDistances;
          lFinalLimit = ( i + 1 )*levelDistances;

          while ( !lCorrectPosition )
          {
            //Position of the neurons
            lNodeNDist.mPos = QVector3D ( randfloat ( 0.0, ( float ) dimX ),
                                          randfloat ( lInitialLimit, ( float ) lFinalLimit ),
                                          randfloat ( 0.0, ( float ) dimZ ));

            //for (int j=0;j<mNodeContainer.size();++j)
            if ( mNodeContainer.size ( ) == 0 )
              lCorrectPosition = true;

            int j = 0;
            while (( !lCorrectPosition )
              && ( j < mNodeContainer.size ( ))
              )
            {
              if (( mNodeContainer.at ( j ).mPos - lNodeNDist.mPos ).length ( ) > mMinimalSeparation )
              {
                lCorrectPosition = true;
                //break;
              }
              ++j;
            }
          }

          //Rotation angle of the neuron model
          lNodeNDist.mAngle = randint ( 360 );

          //Scale of the neuron model
          lNodeNDist.mScale = randfloat ( mRangoEscaladoMin, mRangoEscaladoMax );

          //Name of the neuron
          int id = randint ( numModelsIntern );
          string strNum;

          if ( id >= 100 )
          {
            strNum = to_string ( id );
          }
          else
          {
            if ( id >= 10 )
            {
              strNum = "0";
              strNum += to_string ( id );
            }
            else
            {
              strNum = "00";
              strNum += to_string ( id );
            }
          }

          lNodeNDist.mName = "Inter_";
          lNodeNDist.mName += strNum;

          mNodeContainer.push_back ( lNodeNDist );
        }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

          //for (int lPiramCont=0;lPiramCont<mNumPiramsPerLayer[i];++lPiramCont)
        else
        {
          bool lCorrectPosition = false;

          lInitialLimit = i*levelDistances;
          lFinalLimit = ( i + 1 )*levelDistances;
          while ( !lCorrectPosition )
          {
            //Position of the neurons
            lNodeNDist.mPos = QVector3D ( randfloat ( 0.0, ( float ) dimX ),
                                          randfloat ( lInitialLimit, ( float ) lFinalLimit ),
                                          randfloat ( 0.0, ( float ) dimZ ));

            //for (int j=0;j<mNodeContainer.size();++j)
            if ( mNodeContainer.size ( ) == 0 )
              lCorrectPosition = true;

            int j = 0;
            while (( !lCorrectPosition )
              && ( j < mNodeContainer.size ( ))
              )
            {
              if (( mNodeContainer.at ( j ).mPos - lNodeNDist.mPos ).length ( ) > mMinimalSeparation )
              {
                lCorrectPosition = true;
                //break;
              }
              ++j;
            }
          }

          //Rotation angle of the neuron model
          lNodeNDist.mAngle = randint ( 360 );

          //Scale of the neuron model
          lNodeNDist.mScale = randfloat ( mRangoEscaladoMin, mRangoEscaladoMax );

          //Name of the neuron
          int id = randint ( numModelsPiram );
          string strNum;

          if ( id >= 100 )
          {
            strNum = to_string ( id );
          }
          else
          {
            if ( id >= 10 )
            {
              strNum = "0";
              strNum += to_string ( id );
            }
            else
            {
              strNum = "00";
              strNum += to_string ( id );
            }
          }

          lNodeNDist.mName = "Piram_";
          lNodeNDist.mName += strNum;

          mNodeContainer.push_back ( lNodeNDist );
        }
      }
    }

    //Pirams
    //outputFile <<"#Interneurons:"<<endl;
    //for (int i=0;i<numPiramidals;++i)
    //Interneuros
    //!!! Old version (old the interneurons)
    //for (int i=0;i<numInterneurons;++i)

    //for (int i=0;i<numLevels;++i)
    //{
    //}

    //Reset the containers
    mPiramsPercet.clear ( );
    mIntersPercet.clear ( );

    mNumPiramsPerLayer.clear ( );
    mNumIntersPerLayer.clear ( );

  }

  /*
   * Generate the files with the distribution with format: Position(x,y,z), rotation angle, neuron type.
   */
  void NeuroDistGenerator::generateFile ( )
  {
    string fileTXT = fileName + ".txt";
    string fileBIN = fileName + ".bin";


    //Clean files
    std::ofstream ( fileTXT.c_str ( ));
    std::ofstream ( fileBIN.c_str ( ));

    //Open files to add data
    outputFileTXT.open ( fileTXT.c_str ( ), std::ios::app );
    outputFileBIN.open ( fileBIN.c_str ( ), std::ofstream::binary );

    //Head of the file
    //File format version
    outputFileTXT << fileVersion << endl;
    outputFileBIN.write (( char * ) &fileVersion, sizeof ( float ));

    //Total of neurons
    outputFileTXT << mNodeContainer.size ( ) << endl;
    outputFileBIN.write (( char * ) &totalNeurons, sizeof ( int ));

    NodeNDist lNodeNDist;
    float lAuxPosX, lAuxPosY, lAuxPosZ;

    for ( int i = 0; i < mNodeContainer.size ( ); ++i )
    {
      lNodeNDist = mNodeContainer.at ( i );

      //Text exit
      outputFileTXT << lNodeNDist.mPos.x ( ) << " " << lNodeNDist.mPos.y ( ) << " " << lNodeNDist.mPos.z ( ) << " "
                    << lNodeNDist.mAngle << " "
                    << lNodeNDist.mScale << " "
                    << lNodeNDist.mName
                    << endl;

      //Bin file
      lAuxPosX = lNodeNDist.mPos.x ( );
      lAuxPosY = lNodeNDist.mPos.y ( );
      lAuxPosZ = lNodeNDist.mPos.z ( );

      outputFileBIN.write (( char * ) &lAuxPosX, sizeof ( float ));
      outputFileBIN.write (( char * ) &lAuxPosY, sizeof ( float ));
      outputFileBIN.write (( char * ) &lAuxPosZ, sizeof ( float ));
      outputFileBIN.write (( char * ) &lNodeNDist.mAngle, sizeof ( int ));
      outputFileBIN.write (( char * ) &lNodeNDist.mScale, sizeof ( float ));
      outputFileBIN.write ( lNodeNDist.mName.c_str ( ), 9*sizeof ( char ));
    }

    outputFileTXT.close ( );
    outputFileBIN.close ( );

  }

  void NeuroDistGenerator::generateFromTXT ( string pathFile, string pFileName )
  {
    unsigned int lIdNeuron = 150;
    //size_t lfoundSubStr;
    unsigned int lfoundSubStr;
    string lAux = " ";

    char *lNameBuff = new char[lIdNeuron];
    float lPosAux[3];

    NodeNDist lNodeNDist;
    mNodeContainer.clear ( );

    //Parser imput file
    std::ifstream inputFileTXT;
    inputFileTXT.open ( pathFile.c_str ( ), std::ios::in );

    //File version
    inputFileTXT >> fileVersion;

    //File version
    inputFileTXT >> totalNeurons;

    //File format
    //38.5376 90.2812 41.4978 298 1 Inter_000
    while ( !inputFileTXT.eof ( ))
    {
      inputFileTXT >> lPosAux[0];
      inputFileTXT >> lPosAux[1];
      inputFileTXT >> lPosAux[2];

      lNodeNDist.mPos = QVector3D ( lPosAux[0], lPosAux[1], lPosAux[2] );

      inputFileTXT >> lNodeNDist.mAngle;
      inputFileTXT >> lNodeNDist.mScale;

      //Read the string
      inputFileTXT.get ( lNameBuff, lIdNeuron*sizeof ( char ), '\n' );
      lNodeNDist.mName = lNameBuff;
      lAux = lNameBuff;
      lNodeNDist.mName = "";
      lNodeNDist.mName += Trim ( lAux.begin ( ), lAux.end ( ));
      mNodeContainer.push_back ( lNodeNDist );
    }

    mNodeContainer.pop_back ( );

    string lOldFileName = fileName;
    fileName = pFileName;
    fileName += "_ExportedFromTXT";

    generateFile ( );

    //restore default values
    fileName = lOldFileName;
    fileVersion = 0.3;

    delete lNameBuff;
    inputFileTXT.close ( );
  }

  void NeuroDistGenerator::centerInZ ( string pathFile, string pFileName )
  {
    unsigned int lIdNeuron = 150;
    //size_t lfoundSubStr;
    unsigned int lfoundSubStr;
    string lAux = " ";

    char *lNameBuff = new char[lIdNeuron];
    float lPosAux[3];

    NodeNDist lNodeNDist;
    mNodeContainer.clear ( );

    //Parser imput file
    std::ifstream inputFileTXT;
    inputFileTXT.open ( pathFile.c_str ( ), std::ios::in );

    //File version
    inputFileTXT >> fileVersion;

    //File version
    inputFileTXT >> totalNeurons;

    //File format
    //38.5376 90.2812 41.4978 298 1 Inter_000
    while ( !inputFileTXT.eof ( ))
    {
      inputFileTXT >> lPosAux[0];
      inputFileTXT >> lPosAux[1];
      inputFileTXT >> lPosAux[2];

      lNodeNDist.mPos = QVector3D ( lPosAux[0], lPosAux[1], lPosAux[2] );

      inputFileTXT >> lNodeNDist.mAngle;
      inputFileTXT >> lNodeNDist.mScale;

      //Read the string
      inputFileTXT.get ( lNameBuff, lIdNeuron*sizeof ( char ), '\n' );
      lNodeNDist.mName = lNameBuff;
      lAux = lNameBuff;
      lNodeNDist.mName = "";
      lNodeNDist.mName += Trim ( lAux.begin ( ), lAux.end ( ));
      mNodeContainer.push_back ( lNodeNDist );
    }

    mNodeContainer.pop_back ( );

    float lMinZPos = 100000;
    float lMaxZPos = -100000;

    for ( int i = 0; i < mNodeContainer.size ( ); ++i )
    {
      if ( mNodeContainer.at ( i ).mPos.z ( ) < lMinZPos )
        lMinZPos = mNodeContainer.at ( i ).mPos.z ( );
      if ( mNodeContainer.at ( i ).mPos.z ( ) > lMaxZPos )
        lMaxZPos = mNodeContainer.at ( i ).mPos.z ( );
    }

    //float lMediumPOint = (lMinZPos + lMaxZPos)/2.0;
    float lMediumPOint = ( lMaxZPos - lMinZPos )/2.0;

    for ( int i = 0; i < mNodeContainer.size ( ); ++i )
    {
      mNodeContainer.at ( i ).mPos.setZ ( mNodeContainer.at ( i ).mPos.z ( ) - lMediumPOint );
    }

    string lOldFileName = fileName;
    fileName = pFileName;
    fileName += "_CenteredInZ";

    generateFile ( );

    //restore default values
    fileName = lOldFileName;
    fileVersion = 0.3;

    delete lNameBuff;
    inputFileTXT.close ( );
  }

  void NeuroDistGenerator::loadFromFile ( string pathFile )
  {
    unsigned int lIdNeuron = 150;

    unsigned int lfoundSubStr;
    string lAux = " ";

    char *lNameBuff = new char[lIdNeuron];
    float lPosAux[3];

    NodeNDist lNodeNDist;
    mNodeContainer.clear ( );

    //Parser imput file
    std::ifstream inputFileTXT;
    inputFileTXT.open ( pathFile.c_str ( ), std::ios::in );

    //File version
    inputFileTXT >> fileVersion;

    //File version
    inputFileTXT >> totalNeurons;

    //File format
    //38.5376 90.2812 41.4978 298 1 Inter_000
    while ( !inputFileTXT.eof ( ))
    {
      inputFileTXT >> lPosAux[0];
      inputFileTXT >> lPosAux[1];
      inputFileTXT >> lPosAux[2];

      lNodeNDist.mPos = QVector3D ( lPosAux[0], lPosAux[1], lPosAux[2] );

      //####Esto es un martillazo temporal, q hay que quitar mas adelante
      //Es solo para los datos de Lidia
      //lNodeNDist.mPos = QVector3D(lPosAux[0]/1000,lPosAux[1]/1000,lPosAux[2]/1000);

      inputFileTXT >> lNodeNDist.mAngle;
      inputFileTXT >> lNodeNDist.mScale;

      //Read the string
      inputFileTXT.get ( lNameBuff, lIdNeuron*sizeof ( char ), '\n' );
      lNodeNDist.mName = lNameBuff;
      lAux = lNameBuff;
      lNodeNDist.mName = "";
      lNodeNDist.mName += Trim ( lAux.begin ( ), lAux.end ( ));
      mNodeContainer.push_back ( lNodeNDist );
    }

    mNodeContainer.pop_back ( );

    delete lNameBuff;
    inputFileTXT.close ( );
  }


  //std::string NeuroDistGenerator::Trim(std::string::const_iterator i, std::string::const_iterator f)
  //{
  //	std::string::const_iterator it;
  //	std::string buff;
  //	for (it = i; it != f; ++it)
  //	{
  //		if (*it != ' ') buff += *it;
  //	}
  //	return buff;
  //}


/*
	//Load the file
	void SWCImporter::loadFile(string fileName, bool pApplyStdDims)
	{	
		inputFileTXT.open(fileName.c_str(), std::ios::in);
		//if (inputFileTXT.fail())
		if (!inputFileTXT)
		{
			cerr << "An error occurred. Unable to read input file." << fileName << endl;
			exit(1);
		}

		char str[2000];
		char coment;
		//Read all the header
		bool continueReading=true;
		while (continueReading)
		{
			inputFileTXT.get(coment);
			if (coment!='#')	continueReading = false;
			else
			{
				inputFileTXT.getline(str,2000);
				description+=str;
				description+='\n';
			}
		}

		SWCNode nodeAux;

		//Node 0 dont exist
		nodeAux.id	= nodeAux.type	= nodeAux.position[0]				= nodeAux.position[1]
		= nodeAux.position[2]		= nodeAux.radius= nodeAux.parent	
		= nodeAux.mDendriticDistanceNorm = nodeAux.mDendriticDistanceReal = 0;

		preProcessNodeCollection.push_back(nodeAux);

		//Node readed
		int pos=0;
		while (!inputFileTXT.eof())
		{
			++pos;
			inputFileTXT >> nodeAux.id;
			inputFileTXT >> nodeAux.type;
			inputFileTXT >> nodeAux.position[0];
			inputFileTXT >> nodeAux.position[1];
			inputFileTXT >> nodeAux.position[2];
			inputFileTXT >> nodeAux.radius;
			inputFileTXT >> nodeAux.parent;

			preProcessNodeCollection.push_back(nodeAux);
		}

		//Remove the last element if it is replicated
		int tam = preProcessNodeCollection.size();
		nodeAux = preProcessNodeCollection[tam-1];
		if ( (tam-1)!=nodeAux.id )	preProcessNodeCollection.pop_back();

		//Close the file
		inputFileTXT.close();

		SomaBeltTest();

		centerSomaInOrigin();

		SomaDendriticTest();

		calcDendriticDistance();

		calcDendritics();

		//Finally preprocess the file
		nodeCollection = preProcessNodeCollection;

		//if (pApplyStdDims)	applyUniformModifiers(AXONRADIUSMODIF,DENDRITICRADIUSMODIF,APICALRADIUSMODIFSTD,BASERADIUSMODIF);

		preProcessNodeCollection.clear();
	}

*/

}







