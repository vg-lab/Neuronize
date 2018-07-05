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

#include <list>

using namespace std;

class NeuroLevelVizNode
{
  public:
    NeuroLevelVizNode ( void );
    ~NeuroLevelVizNode ( void );

    const int &getId ( ) const { return mId; }
    const list < int > &getChilds ( ) const { return mChilds; }
    const int &getLevel ( ) const { return mLevel; }
    const float &getParam ( ) const { return mParam; }

    void setId ( const int &pId ) { mId = pId; };
    void setChilds ( const list < int > &pChilds ) { mChilds = pChilds; };
    void addNewChild ( const int &pNewChild ) { mChilds.push_back ( pNewChild ); };
    void setLevel ( const int &pLevel ) { mLevel = pLevel; };
    void setParam ( const float &pParam ) { mParam = pParam; };

  private:
    int mId;
    list < int > mChilds;
    float mParam;
    int mLevel;
};
