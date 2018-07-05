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

#include "MSNode.h"

MSNode::MSNode ( void )
{
  mMass = 0.01;
  mInvMass = 1.0f/mMass;

  //mGravity	= OpenMesh::Vec3f(0.0, -9.8, 0.0);
  mGravity = OpenMesh::Vec3f ( 0.0, 0.0, 0.0 );

  mWeight = mMass*mGravity;

  mFix = false;

  mVel = OpenMesh::Vec3f ( 0.0, 0, 0.0 );
  mAccel = OpenMesh::Vec3f ( 0.0, 0, 0.0 );
}

MSNode::~MSNode ( void )
{

}

void MSNode::initialPos ( OpenMesh::Vec3f p )
{
  //frame.setPosition(p);
  mPos = p;
  mPosPrev = mPos;
}

void MSNode::setPos ( OpenMesh::Vec3f p )
{
  //frame.setPosition(p);
  mPos = p;
}

OpenMesh::Vec3f MSNode::getPos ( )
{
  //return frame.position();
  return mPos;
}

void MSNode::updatePos ( )
{
  //x = frame.position();
}

void MSNode::setSimulationParams ( float pMass, OpenMesh::Vec3f pGravity )
{
  mMass = pMass;
  mInvMass = 1.0f/mMass;

  mGravity = pGravity;
  mWeight = mMass*mGravity;
}

void MSNode::setFix ( bool f )
{
  mFix = f;
}

void MSNode::integration ( float dt, int m )
{
  mAccel = ( mForce + mWeight )*mInvMass;

  switch ( m )
  {
    case 0: //Explicit Euler
      mPosPrev = mPos;
      mPos = mPos + ( mVel*dt );
      mVel = mVel + ( mAccel*dt );
      break;

    case 1: // RK2
      mPosPrev = mPos;
      mVel = mVel + ( mAccel*dt );
      mPos = mPos + ( mVel*dt ) + (( mAccel*dt*dt )*0.5 );
      break;

    case 2: //Verlet

      OpenMesh::Vec3f lPos_anterior;  // necesitamos la posicion anterior
      lPos_anterior = mPos;

      mPos = ( mPos*2.0 ) - mPosPrev + ( mAccel*dt*dt );
      mPosPrev = lPos_anterior;

      mVel = ( mPos - lPos_anterior )/( 2*dt );
      break;
  }

}
