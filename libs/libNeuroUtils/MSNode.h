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

#include "BaseMesh.h"

using namespace NSBaseMesh;

class MSNode
{
  public:

    MSNode ( void );

    ~MSNode ( void );

    OpenMesh::Vec3f mPos;
    OpenMesh::Vec3f mPosPrev;
    OpenMesh::Vec3f mVel;
    OpenMesh::Vec3f mAccel;
    OpenMesh::Vec3f mForce;
    OpenMesh::Vec3f mWeight;
    OpenMesh::Vec3f mGravity;

    float mMass;
    float mInvMass;

    bool mFix;

    void initialPos ( OpenMesh::Vec3f p );

    void setSimulationParams ( float pMass, OpenMesh::Vec3f pGravity );

    void setPos ( OpenMesh::Vec3f p );

    OpenMesh::Vec3f getPos ( );

    void updatePos ( );

    void setFix ( bool f = true );

    void integration ( float dt = 0.005, int m = 0 );
};
