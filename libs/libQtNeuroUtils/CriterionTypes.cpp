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

#include "CriterionTypes.h"

CriterionTypes::CriterionTypes ( void )
{
  mCriterionTypes.clear ( );

  Criterion_Node lAuxNode;

  /*
  lAuxNode.id				=	0;
  lAuxNode.CriterionName	=	"Bifurcations";
  mCriterionTypes.push_back(lAuxNode);

  lAuxNode.id				=	1;
  lAuxNode.CriterionName	=	"Dendritic_Lineal_Distance_To_Soma";
  mCriterionTypes.push_back(lAuxNode);

  lAuxNode.id				=	2;
  lAuxNode.CriterionName	=	"Dendritic_Real_Distance_To_Soma";
  mCriterionTypes.push_back(lAuxNode);
  */

  lAuxNode.id = 0;
  lAuxNode.CriterionName = "N";
  mCriterionTypes.push_back ( lAuxNode );

  lAuxNode.id = 1;
  lAuxNode.CriterionName = "S";
  mCriterionTypes.push_back ( lAuxNode );

  lAuxNode.id = 2;
  lAuxNode.CriterionName = "L";
  mCriterionTypes.push_back ( lAuxNode );

  lAuxNode.id = 3;
  lAuxNode.CriterionName = "L2";
  mCriterionTypes.push_back ( lAuxNode );

  lAuxNode.id = 4;
  lAuxNode.CriterionName = "V";
  mCriterionTypes.push_back ( lAuxNode );

  lAuxNode.id = 5;
  lAuxNode.CriterionName = "V2";
  mCriterionTypes.push_back ( lAuxNode );

  lAuxNode.id = 6;
  lAuxNode.CriterionName = "A";
  mCriterionTypes.push_back ( lAuxNode );

  lAuxNode.id = 7;
  lAuxNode.CriterionName = "A2";
  mCriterionTypes.push_back ( lAuxNode );

  lAuxNode.id = 8;
  lAuxNode.CriterionName = "D";
  mCriterionTypes.push_back ( lAuxNode );

  lAuxNode.id = 9;
  lAuxNode.CriterionName = "O";
  mCriterionTypes.push_back ( lAuxNode );

}

CriterionTypes::~CriterionTypes ( void )
{

}

const Criterion_Node CriterionTypes::getCriterionById ( unsigned int pIdx ) const
{
  return mCriterionTypes[pIdx];
}

const Criterion_Node CriterionTypes::getCriterionByName ( QString pCriterionName ) const
{
  Criterion_Node lAuxNode;
  lAuxNode.id = -1;
  lAuxNode.CriterionName = "No_Valid_Criterion";

  for ( int i = 0; i < mCriterionTypes.size ( ); ++i )
  {
    if ( pCriterionName == mCriterionTypes[i].CriterionName )
    {
      //lAuxNode = mCriterionTypes[i];
      //break;
      return mCriterionTypes[i];
    }
  }

  return lAuxNode;
}

unsigned int CriterionTypes::numCriterions ( ) const
{
  return mCriterionTypes.size ( );
}
