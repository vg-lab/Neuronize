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

#include "ProceduralMesh.h"

namespace NSProceduralMesh
{

  ProceduralMesh::ProceduralMesh ( void ): BaseMesh ( )
  {
    //glb_meshVHandle	=	NULL;;

    //meshVHandleTotal
    //=
    mHResol
      = mVResol
      = 0;
  }

  ProceduralMesh::~ProceduralMesh ( void )
  {
  }

  void ProceduralMesh::generateCloth ( unsigned int pHResol, unsigned int pVResol, float pNodeSeparation )
  {

  }

  //void NeuroSWC::generateSphere(OpenMesh::Vec3f center, float radio, int width, int height)

  void ProceduralMesh::generateSphere ( OpenMesh::Vec3f center,
                                        float radio,
                                        unsigned int pHResol,
                                        unsigned int pVResol )
  {
    mHResol = pHResol;
    mVResol = pVResol;

    float theta, phi;
    int i, j, t;

    unsigned int height = pVResol;
    unsigned int width = pHResol;

    const int nvec = ( height - 2 )*width + 2;

    MeshDef::VertexHandle *vhandle = new MeshDef::VertexHandle[nvec];

    for ( t = 0, j = 1; j < height - 1; j++ )
    {
      for ( i = 0; i < width; i++ )
      {
        theta = float ( j )/( height - 1 )*PI;
        phi = float ( i )/( width )*PI*2;
        vhandle[t] = Mesh->add_vertex ( MeshDef::Point ( center[0] + radio*(( sinf ( theta )*cosf ( phi ))),
                                                         center[1] + radio*(( cosf ( theta ))),
                                                         center[2] + radio*(( -sinf ( theta )*sinf ( phi )))
                                        )
        );

        //glb_meshVHandle[meshVHandleTotal] = vhandle[t];
        //meshVHandleTotal++;

        t++;
      }
    }

    //Extreme points
    vhandle[t] = Mesh->add_vertex ( MeshDef::Point ( center[0], center[1] + radio, center[2]
                                    )
    );

    //glb_meshVHandle[meshVHandleTotal] = vhandle[t];
    //meshVHandleTotal++;

    t++;

    vhandle[t] = Mesh->add_vertex ( MeshDef::Point ( center[0], center[1] - radio, center[2]
                                    )
    );

    //glb_meshVHandle[meshVHandleTotal] = vhandle[t];
    //meshVHandleTotal++;

    t++;

    //Index faces construct
    std::vector <MeshDef::VertexHandle> face_vhandles;

    for ( t = 0, j = 0; j < height - 3; j++ )
    {
      for ( i = 0; i < width - 1; i++ )
      {
        //Uper face
        face_vhandles.clear ( );
        face_vhandles.push_back ( vhandle[(( j )*width + i )] );
        face_vhandles.push_back ( vhandle[(( j + 1 )*width + i + 1 )] );
        face_vhandles.push_back ( vhandle[(( j )*width + i + 1 )] );
        Mesh->add_face ( face_vhandles );

        ////Down face
        face_vhandles.clear ( );
        face_vhandles.push_back ( vhandle[(( j )*width + i )] );
        face_vhandles.push_back ( vhandle[(( j + 1 )*width + i )] );
        face_vhandles.push_back ( vhandle[(( j + 1 )*width + i + 1 )] );
        Mesh->add_face ( face_vhandles );
      }

      //Last Uper face
      face_vhandles.clear ( );
      face_vhandles.push_back ( vhandle[(( j )*width + i )] );
      face_vhandles.push_back ( vhandle[(( j + 1 )*width )] );
      face_vhandles.push_back ( vhandle[(( j )*width )] );
      Mesh->add_face ( face_vhandles );

      ////Last Down face
      face_vhandles.clear ( );
      face_vhandles.push_back ( vhandle[(( j )*width + i )] );
      face_vhandles.push_back ( vhandle[(( j + 1 )*width + i )] );
      face_vhandles.push_back ( vhandle[(( j + 1 )*width )] );
      Mesh->add_face ( face_vhandles );
    }

    for ( i = 0; i < width - 1; i++ )
    {
      //Superior circle
      face_vhandles.clear ( );
      face_vhandles.push_back ( vhandle[(( height - 2 )*width )] );
      face_vhandles.push_back ( vhandle[( i )] );
      face_vhandles.push_back ( vhandle[( i + 1 )] );
      Mesh->add_face ( face_vhandles );

      //Inferior circle
      face_vhandles.clear ( );
      face_vhandles.push_back ( vhandle[(( height - 2 )*width + 1 )] );
      face_vhandles.push_back ( vhandle[(( height - 3 )*width + i + 1 )] );
      face_vhandles.push_back ( vhandle[(( height - 3 )*width + i )] );
      Mesh->add_face ( face_vhandles );
    }

    //Las triangle Superior circle
    face_vhandles.clear ( );
    face_vhandles.push_back ( vhandle[(( height - 2 )*width )] );
    face_vhandles.push_back ( vhandle[( width - 1 )] );
    face_vhandles.push_back ( vhandle[( 0 )] );
    Mesh->add_face ( face_vhandles );

    //Last triangle Inferior circle
    face_vhandles.clear ( );
    face_vhandles.push_back ( vhandle[(( height - 2 )*width + 1 )] );
    face_vhandles.push_back ( vhandle[(( height - 3 )*width )] );
    face_vhandles.push_back ( vhandle[(( height - 3 )*width + ( width - 1 ))] );
    Mesh->add_face ( face_vhandles );

    delete[] vhandle;

    updateBaseMesh ( );
  }

  void ProceduralMesh::generateTetra ( )
  {

  }

  void ProceduralMesh::generateGeomFromSWC ( )
  {

  }

}