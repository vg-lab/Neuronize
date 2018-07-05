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

#include <GL/glew.h>
#include <GL/glut.h>

#include "MeshRenderer.h"

namespace NSMeshRenderer
{
  MeshRenderer::MeshRenderer ( )
  {
    renderModeSelected = RENDER_BUFFERS_OBJECTS;

    renderOptionSelected = RENDER_SURFACE;

    shadingMode = SHADE_SMOOTH;

    pixelScale = 0.5;
    NormalScale = 0.5;
    twoSidedMode = false;

    mPosition = OpenMesh::Vec3f ( 0, 0, 0 );
    mRotation = OpenMesh::Vec3f ( 0, 0, 0 );
    mScale = OpenMesh::Vec3f ( 0, 0, 0 );

    mUseGLPatches = false;
  }

  MeshRenderer::~MeshRenderer ( )
  {
  }

  void MeshRenderer::setRenderMode ( RenderMode pRenderMode )
  {
    renderModeSelected = pRenderMode;
  }

  void MeshRenderer::setMeshToRender ( BaseMesh *mesh )
  {
    RenderMesh = mesh;
  }

  void MeshRenderer::setMeshToRender ( ProceduralMesh *mesh )
  {
    RenderMesh = dynamic_cast<ProceduralMesh *>(mesh);
  }

  void MeshRenderer::setRenderOptions ( const int m )
  {
    assert ( m >= 0 && m < RENDER_MAX );
    renderOptionSelected = m;

    if (( renderModeSelected & RENDER_NORMALS )
      && !( RenderMesh->getMesh ( )->has_vertex_normals ( )
        || RenderMesh->getMesh ( )->has_face_normals ( )))
    {
      //*warn << "Mesh has neither vertex nor face normals, disabling rendering of normals." << std::endl;
      renderModeSelected &= ~RENDER_NORMALS;
    }
  }

  // Get the current rendering mode.
  int MeshRenderer::getRenderOptions ( ) const
  {
    return renderOptionSelected;
  }

  void MeshRenderer::Render ( ) const
  {
    if ( RenderMesh )
    {

      if ( renderModeSelected == RENDER_INMEDIATLY )
      {
        if ( renderOptionSelected & RENDER_SURFACE )
        {
          MeshDef::ConstFaceIter fIt ( RenderMesh->getMesh ( )->faces_begin ( )),
            fEnd ( RenderMesh->getMesh ( )->faces_end ( ));

          MeshDef::ConstFaceVertexIter fvIt;

          glBegin ( GL_TRIANGLES );
          for ( ; fIt != fEnd; ++fIt )
          {
            //---glNormal3fv ( &RenderMesh->getMesh()->normal(fIt)[0] );
            glNormal3fv ( &RenderMesh->getMesh ( )->normal ( *fIt )[0] );

            fvIt = RenderMesh->getMesh ( )->cfv_iter ( *fIt );
            glVertex3fv ( &RenderMesh->getMesh ( )->point ( *fvIt )[0] );

            ++fvIt;
            glVertex3fv ( &RenderMesh->getMesh ( )->point ( *fvIt )[0] );

            ++fvIt;
            glVertex3fv ( &RenderMesh->getMesh ( )->point ( *fvIt )[0] );
          }
          glEnd ( );
        }
      }
      else //Render with Buffers
      {
        if ( renderOptionSelected & RENDER_SURFACE )
        {
          // Superponer los vertices y el alemabre sobre el modelo
          if (
            renderOptionSelected & RENDER_EDGES
              || renderOptionSelected & RENDER_VERTICES
            )
          {
            glPolygonOffset ( 1.0, 1.0 );
            glEnable ( GL_POLYGON_OFFSET_FILL );

            renderSurface ( );

            glDisable ( GL_TEXTURE_2D );
            glDisable ( GL_POLYGON_OFFSET_FILL );

          }
          else
          {
            renderSurface ( );
          }

          ///////////////////////////////////////////////////////////////////////
          ////Alternative drawer (direct) -> VBOs

          //glEnableClientState(GL_VERTEX_ARRAY);
          //glVertexPointer(3, GL_DOUBLE, 0, &RenderMesh->getMesh()->points()[0]);

          //glEnableClientState(GL_NORMAL_ARRAY);
          //glNormalPointer(GL_DOUBLE, 0, &RenderMesh->getMesh()->vertex_normals()[0]);

          //if (RenderMesh->getNumTri()>0)	glDrawElements(GL_TRIANGLES
          //												,3 * RenderMesh->getNumTri()
          //												,GL_UNSIGNED_INT
          //												,&RenderMesh->getTriIndices()[0]);

          //else							glDrawElements(GL_QUADS
          //												,4 * RenderMesh->getNumQuads()
          //												,GL_UNSIGNED_INT
          //												,&RenderMesh->getQuadIndices()[0]);

          //glDisableClientState(GL_VERTEX_ARRAY);
          //glDisableClientState(GL_NORMAL_ARRAY);
          ///////////////////////////////////////////////////////////////////////

        }
        if ( renderOptionSelected & RENDER_VERTICES )
        {
          glDisable ( GL_LIGHTING );
          glLineWidth ( 1.0*pixelScale );

          glColor3f ( 0.75, 0.75, 0 );
          renderVertices ( );

          glEnable ( GL_LIGHTING );
        }
        if ( renderOptionSelected & RENDER_EDGES )
        {
          glColor3f ( 0, 0, 0 );
          glDisable ( GL_LIGHTING );

          renderEdges ( );

          glEnable ( GL_LIGHTING );
        }

        if ( renderOptionSelected & RENDER_NORMALS )
        {
          glColor3f ( 0.75, 0, 0 );
          glDisable ( GL_LIGHTING );
          renderNormals ( );
          glEnable ( GL_LIGHTING );
        }
      }
    }
  }

  void MeshRenderer::setVertexPositionArray ( ) const
  {
    const MeshDef::ConstVertexIter v = RenderMesh->getMesh ( )->vertices_begin ( );

    glVertexPointer ( 3,
                      getTypeEnum < MeshDef::Point::value_type > ( ),
                      RenderMesh->getVertexPointDistance ( ),
                      &RenderMesh->getMesh ( )->points ( )[0] );
  }

  void MeshRenderer::setVertexNormalArray ( ) const
  {
    const MeshDef::ConstVertexIter v = RenderMesh->getMesh ( )->vertices_begin ( );

    glNormalPointer ( getTypeEnum < MeshDef::Normal::value_type > ( ),
                      RenderMesh->getVertexNormalDistance ( ),
                      &RenderMesh->getMesh ( )->vertex_normals ( )[0] );
  }

  void MeshRenderer::setVertexColorArray ( ) const
  {
    const MeshDef::ConstVertexIter v = RenderMesh->getMesh ( )->vertices_begin ( );

    glColorPointer ( 4,
                     getTypeEnum < MeshDef::Color::value_type > ( ),
                     RenderMesh->getVertexColorDistance ( ),
                     &RenderMesh->getMesh ( )->vertex_colors ( )[0] );
  }

  void MeshRenderer::renderVertices ( ) const
  {
    glEnableClientState ( GL_VERTEX_ARRAY );
    glEnableClientState ( GL_COLOR_ARRAY );

    setVertexPositionArray ( );

    if ( RenderMesh->getMesh ( )->has_vertex_colors ( ))
      setVertexColorArray ( );

    glPointSize ( 1.0*pixelScale );

    glDrawArrays ( GL_POINTS, 0, RenderMesh->getMesh ( )->n_vertices ( ));

    glDisableClientState ( GL_VERTEX_ARRAY );
    glDisableClientState ( GL_COLOR_ARRAY );
  }

  void MeshRenderer::renderEdges ( ) const
  {
    // TODO: dpi line width
    glEnableClientState ( GL_VERTEX_ARRAY );

    setVertexPositionArray ( );

    glPolygonMode ( GL_FRONT_AND_BACK, GL_LINE );

    if ( mUseGLPatches )
    {
      glPatchParameteri ( GL_PATCH_VERTICES, 3 );
      glDrawElements ( GL_PATCHES, 3*RenderMesh->getNumTri ( ), GL_UNSIGNED_INT, &RenderMesh->getTriIndices ( )[0] );
    }
    else
    {

      if ( RenderMesh->getNumTri ( ) > 0 )
        glDrawElements ( GL_TRIANGLES,
                         3*RenderMesh->getNumTri ( ),
                         GL_UNSIGNED_INT,
                         &RenderMesh->getTriIndices ( )[0] );
      else
        glDrawElements ( GL_QUADS, 4*RenderMesh->getNumQuads ( ), GL_UNSIGNED_INT, &RenderMesh->getQuadIndices ( )[0] );
    }

    glPolygonMode ( GL_FRONT_AND_BACK, GL_FILL );

    glDisableClientState ( GL_VERTEX_ARRAY );
  }

  void MeshRenderer::renderSurface ( ) const
  {
    glEnableClientState ( GL_VERTEX_ARRAY );
    setVertexPositionArray ( );

    setVertexNormalArray ( );
    glEnableClientState ( GL_NORMAL_ARRAY );

    const MeshDef::ConstVertexIter vb = RenderMesh->getMesh ( )->vertices_begin ( );

    if ( shadingMode == SHADE_SMOOTH && RenderMesh->getMesh ( )->has_vertex_colors ( ))
    {
      setVertexColorArray ( );
      glEnableClientState ( GL_COLOR_ARRAY );
    }
    else
    {
      glColor3ub ( 156, 186, 214 );
    }

    if ( shadingMode == SHADE_FLAT && RenderMesh->getMesh ( )->has_face_colors ( ))
    {
      glDisableClientState ( GL_COLOR_ARRAY );

      MeshDef::ConstFaceIter f, fEnd = RenderMesh->getMesh ( )->faces_end ( );

      for ( f = RenderMesh->getMesh ( )->faces_begin ( ); f != fEnd; ++f )
      {
        color3 ( &RenderMesh->getMesh ( )->color ( *f )[0] );

        glBegin ( GL_POLYGON );
        for ( MeshDef::ConstFaceVertexIter v = RenderMesh->getMesh ( )->cfv_iter ( *f ); v->is_valid ( ); ++v )
          glArrayElement ( v->idx ( ));
        glEnd ( );
      }

    }
    else
    {
      assert ( RenderMesh->getTriIndices ( ).size ( ) == 3*RenderMesh->getNumTri ( ));
      assert ( RenderMesh->getQuadIndices ( ).size ( ) == 4*RenderMesh->getNumQuads ( ));

      if ( mUseGLPatches )
      {
        if ( RenderMesh->getNumTri ( ) > 0 )
        {
          glPatchParameteri ( GL_PATCH_VERTICES, 3 );

          glDrawElements ( GL_PATCHES,
                           3*RenderMesh->getNumTri ( ),
                           GL_UNSIGNED_INT,
                           &RenderMesh->getTriIndices ( )[0] );
        }
      }
      else
      {
        if ( RenderMesh->getNumTri ( ) > 0 )
          glDrawElements ( GL_TRIANGLES,
                           3*RenderMesh->getNumTri ( ),
                           GL_UNSIGNED_INT,
                           &RenderMesh->getTriIndices ( )[0] );

        else
          glDrawElements ( GL_QUADS,
                           4*RenderMesh->getNumQuads ( ),
                           GL_UNSIGNED_INT,
                           &RenderMesh->getQuadIndices ( )[0] );
      }
    }

    glDisableClientState ( GL_VERTEX_ARRAY );
    glDisableClientState ( GL_COLOR_ARRAY );
    glDisableClientState ( GL_NORMAL_ARRAY );
    glDisableClientState ( GL_TEXTURE_COORD_ARRAY );
  }

  void MeshRenderer::renderNormals ( ) const
  {

    std::vector <MeshDef::Point> points;
    points.reserve ( 2*RenderMesh->getMesh ( )->n_vertices ( ));

    const Scalar boundingRadius = 0.20;
    const Scalar side1Scale = NormalScale*boundingRadius;
    const Scalar side2Scale = ( twoSidedMode ? -NormalScale : 0.0 )*boundingRadius;

    MeshDef::ConstVertexIter v, vEnd = RenderMesh->getMesh ( )->vertices_end ( );

    for ( v = RenderMesh->getMesh ( )->vertices_begin ( ); v != vEnd; ++v )
    {
      const MeshDef::Point &p = RenderMesh->getMesh ( )->point ( *v );
      const MeshDef::Normal &n = RenderMesh->getMesh ( )->normal ( *v );

      points.push_back ( p + side1Scale*n );
      points.push_back ( p + side2Scale*n );
    }

    assert ( points.size ( ) == 2*RenderMesh->getMesh ( )->n_vertices ( ));
    const int dist = (( char * ) &points[1] ) - (( char * ) &points[0] );
    glVertexPointer ( 3, getTypeEnum < MeshDef::Point::value_type > ( ), dist, &points[0] );

    glEnableClientState ( GL_VERTEX_ARRAY );
    glDrawArrays ( GL_LINES, 0, points.size ( ));
    glDisableClientState ( GL_VERTEX_ARRAY );
  }

  void MeshRenderer::setPixelScale ( Scalar val )
  {
    pixelScale = val;
  }

  void MeshRenderer::setNormalScale ( Scalar val )
  {
    NormalScale = val;
  }

  BaseMesh *MeshRenderer::getBaseMesh ( ) const
  {
    return RenderMesh;
  }
}
