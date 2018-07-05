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

#ifndef _VBOTRIMESH_H_
#define _VBOTRIMESH_H_

/**
 * @VBOTriMesh
 * @VBOTriMesh.h
 * @author Juan Pedro Brito M�ndez
 * @version 0.1
 * @revs: ...
 */
#include <string>
#include <GL/glew.h>

#include <BaseMesh.h>

#include <defs.h>

//#include "ShaderManager.h"
#include "Shader.h"

using namespace std;
using namespace NSBaseMesh;
/*
 * This class provide basic operatory for buffers object in GL
 * @author Juan Pedro Brito Mendez
 */

namespace NSVBOMeshRenderer
{
  class VBOMeshRenderer
  {

    protected:

      GLuint VertexArrayID;

      //GLuint varShPos;
      //GLuint varShNormal;
      //GLuint varShTex;


      int mFaceNumV;

      //Puntero al trimesh con el que trabajamos
      BaseMesh *LinkedMesh;

      //Number of faces and vertexs of the model in the CPU
      unsigned int NumFaces;    //Numero de facetas
      unsigned int NumVertexs;  //Numero de vertices

      GLuint mTextureId;

      //Auxiliars
      //GLuint *buffers;

      //Vertex buffer object
      GLuint vbo;

      //Index buffer object (for faces)
      GLuint ibo;

      //Normal buffer object for vertexs
      GLuint nboVertexs;

      //Color buffer object for vertexs
      GLuint cbo;

      //Texture buffer object for vertexs
      GLuint tbo;

      CShader *shader;

      GLuint bufferType;

      unsigned int TotalMemOnGPU;
      unsigned int DrawMode;

      float Color[3];

      /**
       * Aux function for create buffers.
       *  @param numBuffers		Number of buffers
       *  @param Buffer			Buffer id to create
       *  @param data				Data of the buffer
       *  @param size				Size of the buffer
       *  @param opt				Type of the buffer
       */
      void createBuffers ( unsigned int numBuffers, GLuint &Buffer, void *data, unsigned int size, int opt );

      /**
     * Aux function for delete buffers.
     *  @param numBuffers		Number of buffers
     *  @param Buffer			Buffer id to delete
     */
      void deleteBuffers ( unsigned int numBuffers, GLuint &Buffer );

      /**
       * Free all memory on the GPU
       */
      void DeAllocFromGPU ( );

      /**
     * Init all pointers of the class to NULL.
     */
      void initBasicAtributtes ( );

    public:

      /**
       *  Constructor
       *  @param linkedMesh	Trimesh to extract the info
       */
      VBOMeshRenderer ( BaseMesh *linkedMesh );

      /**
      * Copy Constructor
      */
      VBOMeshRenderer ( const VBOMeshRenderer &CuVBOTM );

      /**
       * Destructor.
       */
      ~VBOMeshRenderer ( );

      /**
       * Initialice the GPU model
       */
      void Init ( );

      /**
         * Draw the model with OpenGL
         */
      void Draw ( ) const;

      /**
         * Return how memory is allocate on the GPU
         */
      unsigned int getTotalMemOnGPU ( );

      /**
         *  Set the draw mode
       *  @param Mode	Select the draw moto to render the buffer
         */
      void setDrawMode ( unsigned int Mode );

      /**
         * Get the draw mode
         */
      unsigned int getDrawMode ( );

      /**
         * Set the color of the VBO
       *  @param R	Red componnent for the buffer color
       *  @param G	Green componnent for the buffer color
       *  @param B	BLue componnent for the buffer color
         */
      void setColor ( float R, float G, float B );

      /**
         *  Set a GLSL shader to render
       *  @param ShaderGLSL	Shader to use
         */
      void setShader ( CShader *ShaderGLSL );

      /**
       * Show the content of the VBO
       */
      string ToString ( );

      /**
       *  Write the content of the VBO to file
     *  @param file	File to write the data
       */
      string ToFile ( string file );

      BaseMesh *getLinkedMesh ( ) { return LinkedMesh; };

      GLuint getVBO ( ) { return vbo; };

      GLuint getNVBO ( ) { return nboVertexs; };

      GLuint getIBO ( ) { return ibo; };

      void setTextureId ( GLuint pTexId ) { mTextureId = pTexId; };
  };
}

#endif 
