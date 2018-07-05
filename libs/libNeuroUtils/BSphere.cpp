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

#include "BSphere.h"

BSphere::BSphere ( const BaseMesh *pBaseMesh )
{
  setBaseMesh ( pBaseMesh );
}

BSphere::~BSphere ( void )
{

}

void BSphere::setBaseMesh ( const BaseMesh *pBaseMesh )
{
  mBaseMesh = pBaseMesh;

  //Calculate the boundign box
  MeshDef::ConstVertexIter vIt ( mBaseMesh->getMesh ( )->vertices_begin ( ));
  MeshDef::ConstVertexIter vEnd ( mBaseMesh->getMesh ( )->vertices_end ( ));

  MeshDef::Point Point;
  using OpenMesh::Vec3f;

  Vec3f bbMin, bbMax;

  bbMin = bbMax = OpenMesh::vector_cast < Vec3f > ( mBaseMesh->getMesh ( )->point ( *vIt )/*[0]*/);

  for ( size_t count = 0; vIt != vEnd; ++vIt, ++count )
  {
    bbMin.minimize ( OpenMesh::vector_cast < Vec3f > ( mBaseMesh->getMesh ( )->point ( *vIt )));
    bbMax.maximize ( OpenMesh::vector_cast < Vec3f > ( mBaseMesh->getMesh ( )->point ( *vIt )));
  }

  // set center and radius -> Fow bounding sppheres

  mCenter = ( bbMin + bbMax )*0.5;
  mRadius = ( bbMin - bbMax ).norm ( )*0.5;
  mRelativeCenter = mCenter;
}



