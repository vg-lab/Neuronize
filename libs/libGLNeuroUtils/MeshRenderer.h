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

#ifndef _MESH_RENDERER_H_
#define _MESH_RENDERER_H_

#include "GLHelper.hpp"

#include <libs/libNeuroUtils/BaseMesh.h>
#include <libs/libNeuroUtils/ProceduralMesh.h>

using namespace NSBaseMesh;
using namespace NSProceduralMesh;
using namespace NSGLHelper;

namespace NSMeshRenderer
{
  class MeshRenderer
  {

    public:
      // Shading mode
      enum ShadingMode
      {
        SHADE_FLAT      // Flat shading
        , SHADE_SMOOTH      // Smooth shading
        , SHADE_NUM_MODES    // Total numbero of shading modes
      };

      // Debug mode
      enum DebugMode
      {
        DEBUG_SEGMENTS, DEN_DISTANCE_NORM, DEN_DISTANCE_REAL, DEN_DISTANCE_TO_SOMA, DEBUG_VERTEXS, DEBUG_FACES
      };

      bool mUseGLPatches;

    private:
      //Mesh where store the data
      BaseMesh *RenderMesh;

      //bool lightingMode;					// Lighting mode

      int renderModeSelected;          // Rendering mode bitmask
      int renderOptionSelected;        // Rendering mode bitmask

      ShadingMode shadingMode;        // Shading mode

      int pixelScale;

      Scalar NormalScale;

      bool twoSidedMode;            //Draw Normal two sides

    public:

      //Render mode: Inmediatly or buffers objects
      enum RenderMode
      {
        RENDER_INMEDIATLY = 0, RENDER_BUFFERS_OBJECTS = 1
      };

      //Display mode is a bitwise combination of these options
      enum RenderOptions
      {
        RENDER_NONE = 0    // Do not render anything
        , RENDER_VERTICES = 1    // Render vertices
        , RENDER_EDGES = 2    // Render edges
        , RENDER_SURFACE = 4    // Render surface
        , RENDER_NORMALS = 8    // Render normals
        //,RENDER_BOUNDARIES       = 16	// Render boundaries
        //,RENDER_PARAMETERIZATION = 32	// Render the parameterization
        //,RENDER_ANNOTATIONS      = 64	// Render annotations
        //,RENDER_AXES             = 128	// Render the axes
        , RENDER_MAX = 255  // Maximum possible rendering mode
      };

      //Hint options that allow for rendering decisions
      enum HintOptions
      {
        HINT_NONE = 0,   // No rendering hints
        HINT_CLOSED_SURFACE = 1,   // Surface is closed (can't see inside)
        HINT_MAX = 255  // Maximum possible hint mode
      };

      MeshRenderer ( );

      ~MeshRenderer ( );

      void setMeshToRender ( BaseMesh *mesh );

      void setMeshToRender ( ProceduralMesh *mesh );

      // Apply needed render state to the current OpenGL state.
      void setRenderState ( ) const;

      // Set the mesh properties to render.
      // The argument is a bitwise-OR'd list of RenderOptions options.
      void setRenderOptions ( const int m );

      // Get the current rendering mode.
      int getRenderOptions ( ) const;

      void setRenderMode ( RenderMode pRenderMode );

      // Apply the shading settings to the current OpenGL state.
      void setShading ( ) const;

      // How to shade the surface?
      void setShadingMode ( const ShadingMode m );

      void setPixelScale ( Scalar val );

      void setNormalScale ( Scalar val );

      // Get the current shading mode.
      ShadingMode getShadingMode ( ) const;

      void Render ( ) const;

      /* Set the OpenGL Buffer Objects for:
        * vertex positions
        * vertex normals
        * vertex colors
        *
      */
      void setVertexPositionArray ( ) const;

      void setVertexNormalArray ( ) const;

      void setVertexColorArray ( ) const;

      // Render the mesh as a set of vertices.
      void renderVertices ( ) const;

      // Render the mesh as a set of edges.
      void renderEdges ( ) const;

      // Render the mesh as a surface.
      void renderSurface ( ) const;

      // Render the surface normals.
      void renderNormals ( ) const;

      //get the Mesh to render
      BaseMesh *getBaseMesh ( ) const;

      OpenMesh::Vec3f mPosition;

      OpenMesh::Vec3f mRotation;

      OpenMesh::Vec3f mScale;

      OpenMesh::Vec3f getPositionToRender ( ) { return mPosition; }
      OpenMesh::Vec3f getRotationToRender ( ) { return mRotation; }
      OpenMesh::Vec3f getScaleToRender ( ) { return mScale; }

      void setPositionToRender ( OpenMesh::Vec3f pPosition ) { mPosition = pPosition; }
      void setRotationToRender ( OpenMesh::Vec3f pRotation ) { mRotation = pRotation; }
      void setScaleToRender ( OpenMesh::Vec3f pScale ) { mScale = pScale; }
  };
}

#endif
