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

enum StateControl { NEVER, FULL, PROGRESSIVE, STOPED };

struct StateControl_Node
{
  int id;
  QString StateControlName;
};

class StateControlTypes
{
    vector < StateControl_Node > mStateControls;

  public:

    StateControlTypes ( void );

    ~StateControlTypes ( void );

    const StateControl_Node &getStateControlById ( unsigned int pIdx ) const;

    const StateControl_Node getStateControlByName ( QString pStateControlName ) const;

    const vector < StateControl_Node > &getStateControlTypes ( ) { return mStateControls; };

    unsigned int numStateControls ( ) const;
};
