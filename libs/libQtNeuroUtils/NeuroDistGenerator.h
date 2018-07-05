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

#ifndef NEURODISTGENERATOR_H_
#define NEURODISTGENERATOR_H_

/**
 * @NeuroDistGenerator
 * @NeuroDistGenerator.h
 * @author Juan Pedro Brito M�ndez
 * @version 0.1
 */

#include <defs.h>
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <QVector3D>

using namespace std;

/**
 * Description
 * @author Juan Pedro Brito Mendez
 */

namespace NSNeuroDistGenerator
{
  struct NodeNDist
  {
    QVector3D mPos;
    int mAngle;
    float mScale;
    string mName;
  };

  class NeuroDistGenerator
  {
      int totalNeurons;
      int dimX, dimY, dimZ;
      string fileName;

      REAL porcentPiram;
      REAL porcentIntern;
      int numLevels;

      int numModelsPiram;
      int numModelsIntern;

      std::vector < int > mPiramsPercet;
      std::vector < int > mIntersPercet;

      std::vector < int > mNumPiramsPerLayer;
      std::vector < int > mNumIntersPerLayer;

      //Aux vars
      //PENDIENTE:De momento es constante la separacion entre niveles
      REAL levelDistances;
      int numPiramidals;
      int numInterneurons;

      int mMinimalSeparation;
      float mRangoEscaladoMin;
      float mRangoEscaladoMax;

      REAL fileVersion;

      //File exit
      std::ofstream outputFileTXT;
      std::ofstream outputFileBIN;

      std::vector < NodeNDist > mNodeContainer;

      //Imput File
      //std::ifstream inputFileTXT;

      void Prepare ( );

      //std::string Trim(std::string::const_iterator i, std::string::const_iterator f);

    public:

      /**
       * Default constructor
       */
      NeuroDistGenerator ( );

      /**
       * Destructor
       */
      ~NeuroDistGenerator ( );

      /**
       * Set the params for de distribution.
       */
      void setAllParams ( int PTotalNeurons,
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
      );

      /**
       * Generate the distribution with format: Position(x,y,z), rotation angle, neuron type.
       */
      void generateDistribution ( );

      /**
         * Generate the files with the distribution.
         */
      void generateFile ( );

      void generateFromTXT ( string pathFile, string pFileName );

      void centerInZ ( string pathFile, string pFileName );

      void loadFromFile ( string pathFile );

      const std::vector < NodeNDist > &getNodeContainer ( ) const { return mNodeContainer; }

      void addNodeToContainer ( NodeNDist pNodeNDist ) { mNodeContainer.push_back ( pNodeNDist ); };
  };
}

#endif