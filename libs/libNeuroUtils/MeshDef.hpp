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

#ifndef _MESH_DEF_H_
#define _MESH_DEF_H_

//Must be included before mesh kernel
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/PolyConnectivity.hh>

#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>

//#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Utils/Property.hh>

struct BaseTraits: OpenMesh::DefaultTraits
{
  typedef OpenMesh::Vec3f Point;       // The position type
  typedef OpenMesh::Vec3f Normal;      // The vector type
  //typedef double           Scalar;    // The floating-point type

  //Not used at the moment
  typedef double TexCoord1D;  // The 1D texture coordinate type
  typedef OpenMesh::Vec2f TexCoord2D;  // The 2D texture coordinate type
  typedef OpenMesh::Vec3f TexCoord3D;  // The 3D texture coordinate type

  //Color Buffer
  typedef OpenMesh::Vec4f Color;      // The color type
};

typedef OpenMesh::TriMesh_ArrayKernelT <BaseTraits> MeshDef;
//typedef OpenMesh::PolyMesh_ArrayKernelT<BaseTraits>		MeshDef;


namespace NSBaseMesh
{
  // Useful 3D vector type using same scalar as the mesh.
  typedef OpenMesh::VectorT < MeshDef::Scalar, 3 > Vec3;

  // Useful 2D vector type using same scalar as the mesh.
  typedef OpenMesh::VectorT < MeshDef::Scalar, 2 > Vec2;

  // The same scalar as used in the mesh.
  typedef MeshDef::Scalar Scalar;

  // Type of custom source for vertex world positions
  typedef OpenMesh::VPropHandleT <MeshDef::Point> VertexPositionSource;

  // Type of custom source for vertex parametric positions
  typedef OpenMesh::VPropHandleT <MeshDef::TexCoord2D> VertexParamSource;

  // Type of custom source for vertex normals
  typedef OpenMesh::VPropHandleT <MeshDef::Normal> VertexNormalSource;

  // Type of custom source for vertex colors
  typedef OpenMesh::VPropHandleT <MeshDef::Color> VertexColorSource;

  template < typename Iter >
  void assignDefaultColor ( MeshDef *mesh, const Iter &begin, const Iter &end, MeshDef::Color pColor )
  {
    for ( Iter i = begin; i != end; ++i )
    {
      mesh->set_color ( *i, pColor );
    }
  }

  template < typename VertexHandle >
  void assignDefaultColor ( MeshDef *mesh, const VertexHandle &begin, const int end, MeshDef::Color pColor )
  {
    for ( int i = 0; i < end; ++i )
    {
      mesh->set_color ( mesh->vertex_handle ( begin.idx ( ) + i ), pColor );
    }
  }

  template < typename VertexHandle >
  void assignDefaultColor ( MeshDef *mesh, const VertexHandle &begin, MeshDef::Color pColor )
  {
    mesh->set_color ( mesh->vertex_handle ( begin.idx ( )), pColor );
  }
}

#endif //_MESH_DEF_H_
