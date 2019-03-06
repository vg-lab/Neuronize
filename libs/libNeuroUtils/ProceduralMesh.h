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

#include <neuroutils/api.h>

#include "BaseMesh.h"

using namespace NSBaseMesh;

namespace NSProceduralMesh
{
  class NEUROUTILS_API ProceduralMesh: public BaseMesh
  {
    protected:

      ////Global Mesh Vertex Handle
      //MeshDef::VertexHandle				*	glb_meshVHandle;
      //unsigned int						meshVHandleTotal;

      unsigned int mHResol;
      unsigned int mVResol;

    public:
      ProceduralMesh ( void );

      ~ProceduralMesh ( void );

      void generateCloth ( unsigned int pHResol, unsigned int pVResol, float pNodeSeparation );

      void generateSphere ( OpenMesh::Vec3f center, float radio, unsigned int pHResol, unsigned int pVResol );

      void generateTetra ( );

      void generateGeomFromSWC ( );
  };
};
