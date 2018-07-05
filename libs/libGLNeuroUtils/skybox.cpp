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

#include "skybox.h"

skybox::skybox ( )
{
  const static GLfloat skyboxVertices[] = {
    // Positions
    -1.0f, 1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,

    -1.0f, -1.0f, 1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, -1.0f, 1.0f,

    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, -1.0f, 1.0f,
    -1.0f, -1.0f, 1.0f,

    -1.0f, 1.0f, -1.0f,
    1.0f, 1.0f, -1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, 1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, 1.0f,
    1.0f, -1.0f, 1.0f
  };

  // Setup skybox VAO
  glGenVertexArrays ( 1, &skyboxVAO );
  glGenBuffers ( 1, &skyboxVBO );
  glBindVertexArray ( skyboxVAO );
  glBindBuffer ( GL_ARRAY_BUFFER, skyboxVBO );
  glBufferData ( GL_ARRAY_BUFFER, sizeof ( skyboxVertices ), &skyboxVertices, GL_STATIC_DRAW );
  glEnableVertexAttribArray ( 0 );
  glVertexAttribPointer ( 0, 3, GL_FLOAT, GL_FALSE, 3*sizeof ( GLfloat ), ( GLvoid * ) 0 );
  glBindVertexArray ( 0 );

  //SetSkybox
  faces.push_back ( "./Content/Skybox/right.jpg" );
  faces.push_back ( "./Content/Skybox/left.jpg" );
  faces.push_back ( "./Content/Skybox/top.jpg" );
  faces.push_back ( "./Content/Skybox/bottom.jpg" );
  faces.push_back ( "./Content/Skybox/back.jpg" );
  faces.push_back ( "./Content/Skybox/front.jpg" );

  //cubeTexture		= loadTexture("/Content/Skybox/container.jpg");
  cubemapTexture = loadCubemap ( faces );
}

skybox::~skybox ( void )
{

}

GLuint skybox::loadCubemap ( vector < const GLchar * > faces )
{
  GLuint textureID;
  glGenTextures ( 1, &textureID );

  int width, height;
  unsigned char *image;

  glBindTexture ( GL_TEXTURE_CUBE_MAP, textureID );
  for ( GLuint i = 0; i < faces.size ( ); i++ )
  {
    image = SOIL_load_image ( faces[i], &width, &height, 0, SOIL_LOAD_RGB );
    glTexImage2D ( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image );
    SOIL_free_image_data ( image );
  }
  glTexParameteri ( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  glTexParameteri ( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  glTexParameteri ( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
  glTexParameteri ( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
  glTexParameteri ( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );
  glBindTexture ( GL_TEXTURE_CUBE_MAP, 0 );

  return textureID;
}

GLuint skybox::loadTexture ( GLchar *path )
{
  //Generate texture ID and load texture data
  GLuint textureID;
  glGenTextures ( 1, &textureID );
  int width, height;
  unsigned char *image = SOIL_load_image ( path, &width, &height, 0, SOIL_LOAD_RGB );
  // Assign texture to ID
  glBindTexture ( GL_TEXTURE_2D, textureID );
  glTexImage2D ( GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image );
  glGenerateMipmap ( GL_TEXTURE_2D );

  // Parameters
  glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
  glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
  glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
  glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  glBindTexture ( GL_TEXTURE_2D, 0 );
  SOIL_free_image_data ( image );
  return textureID;
}

void skybox::draw ( )
{
  // Draw skybox as last
  glDepthFunc ( GL_LEQUAL );  // Change depth function so depth test passes when values are equal to depth buffer's content
  //skyboxShader.Use();

  //view = glm::mat4(glm::mat3(camera.GetViewMatrix()));	// Remove any translation component of the view matrix
  //glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
  //glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

  // skybox cube
  glBindVertexArray ( skyboxVAO );
  glActiveTexture ( GL_TEXTURE0 );
  //glUniform1i(glGetUniformLocation(shader.Program, "skybox"), 0);
  glBindTexture ( GL_TEXTURE_CUBE_MAP, cubemapTexture );
  glDrawArrays ( GL_TRIANGLES, 0, 36 );
  glBindVertexArray ( 0 );
  glDepthFunc ( GL_LESS ); // Set depth function back to default
}
