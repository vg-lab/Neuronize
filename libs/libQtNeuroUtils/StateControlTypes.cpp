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

#include "StateControlTypes.h"

StateControlTypes::StateControlTypes ( void )
{
  mStateControls.clear ( );

  StateControl_Node lAuxNode;

  lAuxNode.id = StateControl::NEVER;
  lAuxNode.StateControlName = "NEVER";
  mStateControls.push_back ( lAuxNode );

  lAuxNode.id = StateControl::FULL;
  lAuxNode.StateControlName = "FULL";
  mStateControls.push_back ( lAuxNode );

  lAuxNode.id = StateControl::PROGRESSIVE;
  lAuxNode.StateControlName = "PROGRESSIVE";
  mStateControls.push_back ( lAuxNode );

}

StateControlTypes::~StateControlTypes ( void )
{

}

const StateControl_Node &StateControlTypes::getStateControlById ( unsigned int pIdx ) const
{
  return mStateControls[pIdx];
}

const StateControl_Node StateControlTypes::getStateControlByName ( QString pCriterionName ) const
{
  StateControl_Node lAuxNode;
  lAuxNode.id = -1;
  lAuxNode.StateControlName = "No_Valid_Criterion";

  for ( int i = 0; i < mStateControls.size ( ); ++i )
  {
    if ( pCriterionName == mStateControls[i].StateControlName )
    {
      //lAuxNode = mCriterionTypes[i];
      //break;
      return mStateControls[i];
    }
  }

  return lAuxNode;
}

unsigned int StateControlTypes::numStateControls ( ) const
{
  return mStateControls.size ( );
}
