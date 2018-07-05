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

#include "MSSpringConstraint.h"

MSSpringConstraint::MSSpringConstraint ( MSSpring *pSpring, constraint_type pConstType )
//MSSpringConstraint::MSSpringConstraint(MSSpring & pSpring, constraint_type pConstType)
{
  mSpringConnected = pSpring;
  //mSpringConnected	= & pSpring;
  mConstType = pConstType;

  autoCalcParamConstraint ( );
}

MSSpringConstraint::~MSSpringConstraint ( void )
{

}

MSSpringConstraint::constraint_type MSSpringConstraint::getConstraintType ( )
{
  return mConstType;
}

void MSSpringConstraint::applyConstraints ( )
{
  switch ( mConstType )
  {
    case MINLENGT: applyMinLenghtConstraint ( );
      break;
    case CTELENGHT: applyCteLenghtConstraint ( );
      break;
    default: break;
  }
}

void MSSpringConstraint::autoCalcParamConstraint ( )
{
  mMinLenghtConstraint = mCteLenghtConstraint = 0;

  switch ( mConstType )
  {
    case MINLENGT: mMinLenghtConstraint = mSpringConnected->L0;
      break;

    case CTELENGHT: mMinLenghtConstraint = mSpringConnected->L0;
      break;

    default: break;
  }
}

//http://www.gamasutra.com/resource_guide/20030121/jacobson_pfv.htm
void MSSpringConstraint::applyMinLenghtConstraint ( )
{
  if ( mSpringConnected->L < mMinLenghtConstraint )
  {
    MSNode *lNodeIni = mSpringConnected->mIniNode;
    MSNode *lNodeFin = mSpringConnected->mFinNode;
    float lTotalDesp = mMinLenghtConstraint - mSpringConnected->L;

    //Moveremos los nodos la inversa de su masa
    OpenMesh::Vec3f lDir = lNodeFin->getPos ( ) - lNodeIni->getPos ( );
    lDir.normalize ( );

    float lDiff = ( mMinLenghtConstraint - mSpringConnected->L )/
      ( mSpringConnected->L*( lNodeIni->mInvMass + lNodeFin->mInvMass ));

    if ( lNodeIni->mFix )
    {
      //Move fin
      if ( !lNodeFin->mFix )
      {
        lNodeFin->mPos = ( lNodeFin->getPos ( ) + ( lDir*( lNodeFin->mInvMass*lDiff )));
      }
    }
    else
    {
      //Move Ini
      if ( lNodeFin->mFix )
      {
        lNodeIni->mPos = ( lNodeIni->getPos ( ) + ( lDir*( lNodeIni->mInvMass*lDiff )));

      }
        //Move both
      else
      {
        lNodeIni->mPos = ( lNodeIni->getPos ( ) + ( lDir*( lNodeIni->mInvMass*lDiff )));
        lNodeFin->mPos = ( lNodeFin->getPos ( ) + ( lDir*( lNodeFin->mInvMass*lDiff )));
      }
    }
  }
}

void MSSpringConstraint::applyCteLenghtConstraint ( )
{

}
