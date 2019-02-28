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

class AABB
{
    OpenMesh::Vec3f minPoint;
    OpenMesh::Vec3f maxPoint;

    const BaseMesh *mBaseMesh;

  public:
    AABB ( void ): minPoint ( 0, 0, 0 ), maxPoint ( 0, 0, 0 ) {};

    AABB ( const BaseMesh *pBaseMesh );

    ~AABB ( void );

    void setBaseMesh ( const BaseMesh *pBaseMesh );

    OpenMesh::Vec3f getMinPoint ( ) { return minPoint; };

    OpenMesh::Vec3f getMaxPoint ( ) { return maxPoint; };
};

