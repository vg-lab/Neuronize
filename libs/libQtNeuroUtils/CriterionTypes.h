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

#include <QString>

#include <vector>

using namespace std;

/*
	Esto quedar�a bastante mejor si cargara los timpos desde fichero (xml)
	y asi lo orientamos todo a datos.
*/

struct Criterion_Node
{
  int id;
  QString CriterionName;
};

class CriterionTypes
{
    vector <Criterion_Node> mCriterionTypes;

  public:

    CriterionTypes ( void );

    ~CriterionTypes ( void );

    const Criterion_Node getCriterionById ( unsigned int pIdx ) const;

    const Criterion_Node getCriterionByName ( QString pCriterionName ) const;

    const vector <Criterion_Node> &getCriterionTypes ( ) { return mCriterionTypes; };

    unsigned int numCriterions ( ) const;
};
