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

/**
 * @VBOTriMesh
 * @VBOTriMesh.cpp
 * @author Juan Pedro Brito M�ndez
 * @version 0.1
 * @revs: ...
 */

#include <iostream>
#include <string>
using namespace std;

#include "VBOMeshRenderer.h"

enum BufferType { VERTEXBUFFER, INDEXBUFFER, NORMALBUFFER, COLORBUFFER, TEXTUREBUFFER };

/*
 * This class provide basic operatory for buffers object in GL
 * @author Juan Pedro Brito Mendez
 */

namespace NSVBOMeshRenderer
{
  /**
 * Aux function for delete buffers.
 *  @param numBuffers		Number of buffers
 *  @param Buffer			Buffer id to delete
 */
  void VBOMeshRenderer::deleteBuffers ( unsigned int numBuffers, GLuint &Buffer )
  {
    //Active the buffer to reference them
    glBindBuffer ( numBuffers, Buffer );

    //Delete the buffer
    glDeleteBuffers ( numBuffers, &Buffer );

    //Set buffer pointer to NULL
    //---Buffer = NULL;
    Buffer = 0;
  }

  /**
   * Aux function for create buffers.
   *  @param numBuffers		Number of buffers
   *  @param Buffer			Buffer id to create
   *  @param data				Data of the buffer
   *  @param size				Size of the buffer
   *  @param opt				Type of the buffer
   */
  void VBOMeshRenderer::createBuffers ( unsigned int numBuffers,
                                        GLuint &Buffer,
                                        void *data,
                                        unsigned int size,
                                        int opt )
  {
    //Generate a buffer and reference them.
    glGenBuffers ( numBuffers, &Buffer );

    switch ( opt )
    {
      //case VERTEXBUFFER:
      case NORMALBUFFER:
        //Set the buffer active
        glBindBuffer ( GL_ARRAY_BUFFER, Buffer );

        //Configure the buffer for dinamic draw
        glBufferData ( GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW );

        //Dereference the buffer
        glBindBuffer ( GL_ARRAY_BUFFER, 0 );
        break;
      case VERTEXBUFFER:
        //Set the buffer active
        glBindBuffer ( GL_ARRAY_BUFFER, Buffer );

        //Configure the buffer for dinamic draw
        glBufferData ( GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW );

        //Dereference the buffer
        glBindBuffer ( GL_ARRAY_BUFFER, 0 );
        break;
      case TEXTUREBUFFER:
        //Set the buffer active
        glBindBuffer ( GL_ARRAY_BUFFER, Buffer );

        //Configure the buffer for dinamic draw
        glBufferData ( GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW );

        //Dereference the buffer
        glBindBuffer ( GL_ARRAY_BUFFER, 0 );
        break;
      case INDEXBUFFER:

        //Set the buffer active
        glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, Buffer );

        //### glEnableClientState(GL_ELEMENT_ARRAY_BUFFER);
        //Configure the buffer for static draw (dont change the values)
        glBufferData ( GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW );
        //### glDisableClientState(GL_ELEMENT_ARRAY_BUFFER);

        //Dereference the buffer
        glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, 0 );
        break;
      default: cout << "Error in params building Buffers Object, prepare to crash ..." << endl;
    };
  }

  /**
 * Init all pointers of the class to NULL.
 */
  void VBOMeshRenderer::initBasicAtributtes ( )
  {

    shader
      = NULL;

    TotalMemOnGPU = 0;

    //DrawMode=GL_POINT;
    //DrawMode=GL_LINE;
    DrawMode = GL_FILL;

    bufferType = GL_STATIC_DRAW;

    mTextureId = -1;

    mFaceNumV = 0;
  }

  /**
   *  Constructor
   *  @param linkedMesh	Trimesh to extract the info
   */
  VBOMeshRenderer::VBOMeshRenderer ( BaseMesh *linkedMesh )
  {
    LinkedMesh = linkedMesh;
    initBasicAtributtes ( );
  }

  /**
   * Copy Constructor
   */
  VBOMeshRenderer::VBOMeshRenderer ( const VBOMeshRenderer &CuVBOTM )
  {
    throw "Not implemented yet.";
  }

  /**
   * Destructor
   */
  VBOMeshRenderer::~VBOMeshRenderer ( )
  {
    //Free all resources on the GPU
    DeAllocFromGPU ( );
  }

  /**
   * Free all memory on the GPU
   */
  void VBOMeshRenderer::DeAllocFromGPU ( )
  {
    //Free all resources on the GPU
    deleteBuffers ( 1, vbo );
    deleteBuffers ( 1, ibo );
    //deleteBuffers(1, nboFaces);
    deleteBuffers ( 1, nboVertexs );
    if ( mTextureId != -1 )
      deleteBuffers ( 1, tbo );
  }

  /**
   * Initialice the GPU model
   */
  void VBOMeshRenderer::Init ( )
  {
    //Set the numbers of vertex on the GPU and reserve the memory for them
    NumVertexs = LinkedMesh->getNumVertex ( );

    TotalMemOnGPU = 0;
    TotalMemOnGPU += NumVertexs*sizeof ( float )*3,

      //Add vertex nomals
    TotalMemOnGPU += NumVertexs*sizeof ( float )*3,

    mFaceNumV = 3;
    if ( LinkedMesh->getNumQuads ( ) > 0 )
      mFaceNumV = 4;

    //Set the numbers of faces on the GPU and reserve the memory for them
    NumFaces = LinkedMesh->getNumFaces ( );
    TotalMemOnGPU += NumFaces*sizeof ( unsigned int )*mFaceNumV;

    cout << "GPU Mem:  " << TotalMemOnGPU << endl;

    ////Con esto no casca paro lo pinta mal
    glGenVertexArrays ( 1, &VertexArrayID );
    glBindVertexArray ( VertexArrayID );

    //Create the Buffers (vertexs, indexs y normals).
    createBuffers ( 1,
                    vbo,
                    ( void * ) &LinkedMesh->getMesh ( )->points ( )[0],
                    NumVertexs*sizeof ( float )*3,
                    VERTEXBUFFER );

    createBuffers ( 1,
                    nboVertexs,
                    ( void * ) &LinkedMesh->getMesh ( )->vertex_normals ( )[0],
                    NumVertexs*sizeof ( float )*3,
                    NORMALBUFFER );

    //createBuffers(1, ibo,(void *)&LinkedMesh->getTriIndices()[0], NumFaces*sizeof(unsigned int)*3,INDEXBUFFER);
    if ( mFaceNumV != 4 )
      createBuffers ( 1,
                      ibo,
                      ( void * ) &LinkedMesh->getTriIndices ( )[0],
                      NumFaces*sizeof ( unsigned int )*mFaceNumV,
                      INDEXBUFFER );
    else
      createBuffers ( 1,
                      ibo,
                      ( void * ) &LinkedMesh->getQuadIndices ( )[0],
                      NumFaces*sizeof ( unsigned int )*mFaceNumV,
                      INDEXBUFFER );

    if ( mTextureId != -1 )
    {
      float *mTexCoord = new float[NumVertexs*2];
      for ( int i = 0; i < NumVertexs*2; ++i )
        mTexCoord[i] = randfloat ( 0, 1 );
      createBuffers ( 1, tbo, ( void * ) mTexCoord, NumVertexs*sizeof ( float )*2, TEXTUREBUFFER );
    }

  }

  /**
     * Draw the model with OpenGL
     */
  void VBOMeshRenderer::Draw ( ) const
  {
    //GLuint varShPos = glGetAttribLocation(shader->GetProgram(),"iPosition");
    glBindBuffer ( GL_ARRAY_BUFFER, vbo );
    glEnableVertexAttribArray ( 0 );
    glVertexAttribPointer ( 0, 3, GL_FLOAT, GL_FALSE, 0, ( GLubyte * ) NULL );

    //GLuint varShNormal = glGetAttribLocation(shader->GetProgram(),"iNormal");
    glBindBuffer ( GL_ARRAY_BUFFER, nboVertexs );
    glEnableVertexAttribArray ( 1 );
    glVertexAttribPointer ( 1, 3, GL_FLOAT, GL_FALSE, 0, ( GLubyte * ) NULL );

    GLuint varShTex;


    // Map index 1 to the color buffer
    if ( mTextureId != -1 )
    {
      glBindTexture ( GL_TEXTURE_2D, mTextureId );
      glBindBuffer ( GL_ARRAY_BUFFER, tbo );
      //varShTex = glGetAttribLocation(shader->GetProgram(),"iTexCoord");
      glEnableVertexAttribArray ( 2 );
      glVertexAttribPointer ( 2, 2, GL_FLOAT, GL_FALSE, 0, ( GLubyte * ) NULL );
    }

    glEnableClientState ( GL_VERTEX_ARRAY );             // activate vertex coords array
    glVertexPointer ( 3, GL_FLOAT, 0, 0 );               // last param is offset, not ptr

    glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, ibo );

    if ( mFaceNumV == 3 )
      glDrawElements ( GL_PATCHES, 3*LinkedMesh->getNumTri ( ), GL_UNSIGNED_INT, NULL );
    else
      glDrawElements ( GL_PATCHES, 4*LinkedMesh->getNumQuads ( ), GL_UNSIGNED_INT, NULL );

    glDisableClientState ( GL_VERTEX_ARRAY );            // deactivate vertex array

    glDisableVertexAttribArray ( 0 );
    glDisableVertexAttribArray ( 1 );
    if ( mTextureId != -1 )
      glDisableVertexAttribArray ( 2 );

    glBindBuffer ( GL_ARRAY_BUFFER, 0 );
    glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, 0 );
  }

  /**
   * Return how memory is allocate on the GPU
   */
  unsigned int VBOMeshRenderer::getTotalMemOnGPU ( )
  {
    return TotalMemOnGPU;
  }

  /**
     * Set the draw mode
     */
  void VBOMeshRenderer::setDrawMode ( unsigned int Mode )
  {
    DrawMode = Mode;
  }

  /**
     * Get the draw mode
     */
  unsigned int VBOMeshRenderer::getDrawMode ( )
  {
    return DrawMode;
  }

  /**
     * Set the color of the VBO
     */
  void VBOMeshRenderer::setColor ( float R, float G, float B )
  {
    Color[0] = R;
    Color[1] = G;
    Color[2] = B;
  }

  /**
     *  Set a GLSL shader to render
   *  @param ShaderGLSL	Shader to use
     */
  void VBOMeshRenderer::setShader ( CShader *ShaderGLSL )
  {
    shader = ShaderGLSL;

    //GLuint varShPos = glGetAttribLocation(shader->GetProgram(),"iPosition");
    //glEnableVertexAttribArray(varShPos);
    //glVertexAttribPointer( varShPos, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL );

    //GLuint varShNormal = glGetAttribLocation(shader->GetProgram(),"iNormal");
    //glEnableVertexAttribArray(varShNormal);
    //glVertexAttribPointer( varShNormal, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL );

    //// Map index 1 to the color buffer
    //if (mTextureId!=-1)
    //{
    //	glBindTexture(GL_TEXTURE_2D, mTextureId);
    //	GLuint varShTex = glGetAttribLocation(shader->GetProgram(),"iTexCoord");
    //	glEnableVertexAttribArray(varShTex);
    //	glVertexAttribPointer( varShTex, 2, GL_FLOAT, GL_FALSE, 0,(GLubyte *)NULL );
    //}
  }

  /**
   * Show the content of the VBO
   */
  string VBOMeshRenderer::ToString ( )
  {
    return "file";
  }

  /**
   * Write the content of the VBO to file
   */
  string VBOMeshRenderer::ToFile ( string file )
  {
    return file;
  }
}
