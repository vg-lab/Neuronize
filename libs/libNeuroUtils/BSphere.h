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

class BSphere
{
    OpenMesh::Vec3f mCenter;
    OpenMesh::Vec3f mRelativeCenter;

    float mRadius;
    const BaseMesh *mBaseMesh;

  public:
    BSphere ( void ): mCenter ( 0, 0, 0 ), mRelativeCenter ( 0, 0, 0 ), mRadius ( 0 ), mBaseMesh ( NULL ) {};

    BSphere ( const BaseMesh *pBaseMesh );

    ~BSphere ( );

    void setBaseMesh ( const BaseMesh *pBaseMesh );

    OpenMesh::Vec3f getCenter ( ) { return mCenter; };

    OpenMesh::Vec3f getRelativeCenter ( ) { return mRelativeCenter; };

    float getRadius ( ) { return mRadius; };
};
