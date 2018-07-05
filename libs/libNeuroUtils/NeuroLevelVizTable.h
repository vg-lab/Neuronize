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

#pragma once

#include "NeuroLevelVizNode.h"

#include <SWCImporter.h>
using namespace NSSWCImporter;

#include <string>
#include <vector>
using namespace std;

//Read carefully
//http://www.zator.com/Cpp/E3_2_1c.htm
//http://trucosinformaticos.wordpress.com/2010/05/02/como-usar-const-en-c/


class NeuroLevelVizTable
{
  public:
    NeuroLevelVizTable ( void );

    NeuroLevelVizTable ( SWCImporter &pSWCImporter );

    ~NeuroLevelVizTable ( void );

    /*
      El primero significa que el valor devuelto por la funci�n no podr� ser utilizado
      para modificar el objeto original. El segundo indica que el argumento recibido
      por la funci�n no podr� ser modificado en el cuerpo de esta. El tercero se�ala
      que el m�todo C::foo no podr� ser utilizado para modificar ning�n miembro de la
      estructura C a la que pertenece.

      const int& C::foo(const int& i) const {...};
    */
    const vector <NeuroLevelVizNode> &getContainer ( ) const { return this->mContainer; };

    void addNode ( const NeuroLevelVizNode &pNode );

    void loadFromSWC ( SWCImporter &pSWCImporter );

    void loadValuesFromFile ( string pFile );

    string toString ( );

    //### Auxiliar funtion to load levels
    void autoLoadDistanceLevels ( const SWCImporter &pSWCImporter, float pDistDivisor );

    void setEvaluatorLevelParam ( float mLevelParam ) { mEvaluatorLevelParam = mLevelParam; };

    float getEvaluatorLevelParam ( ) { return mEvaluatorLevelParam; };

    int getMaxLevel ( ) { return mMaxLevel; };

    void calculateLevelsEvaluatorLevelParam ( );

  private:

    vector <NeuroLevelVizNode> mContainer;

    float mEvaluatorLevelParam;

    int mMaxLevel;

    //File exit
    std::ifstream inputFileTXT;

    SWCImporter *mSWCImporter;
};
