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

#include "MSSpring.h"

MSSpring::MSSpring ( MSNode *pIniNode, MSNode *pFinNode, float pStiffness, float pDumping )
{
  mIniNode = pIniNode;
  mFinNode = pFinNode;

  // longitud en reposo del muelle
  L0 = ( pFinNode->getPos ( ) - mIniNode->getPos ( )).length ( );
  L = 0;

  // rigidez
  mStiffness = pStiffness;

  // amortiguamiento
  mDumping = pDumping;
}

MSSpring::~MSSpring ( void )
{

}

void MSSpring::updateForce ( )
{
  // longitud actual del muelle
  L = ( mFinNode->getPos ( ) - mIniNode->getPos ( )).length ( );

  // direccion de la fuerza
  OpenMesh::Vec3f lFD = ( mIniNode->getPos ( ) - mFinNode->getPos ( ))/L;

  // Segunda ley de Newton
  mForce = -mStiffness*( L - L0 )*lFD;

  // Amortiguamiento en en muelle, velocidad relativa
  mForce = mForce - mDumping*( mIniNode->mVel - mFinNode->mVel );
}
