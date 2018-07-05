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

#include "OldLODController.h"

OldLODController::OldLODController ( VBOMeshRenderer *pVBOMeshRenderer )
  : mVBOMeshRenderer ( pVBOMeshRenderer )
{
  mVertexLevels = NULL;
}

OldLODController::~OldLODController ( void )
{
  if ( mVertexLevels != NULL )
    delete mVertexLevels;

  //Active the buffer to reference them
  glBindBuffer ( 1, mBufferId );
  //Delete the buffer
  glDeleteBuffers ( 1, &mBufferId );
}

void OldLODController::AllocLevels ( int pLayer )
{
  if ( mVertexLevels != NULL )
    delete mVertexLevels;

  mVertexLevels = new float[mVBOMeshRenderer->getLinkedMesh ( )->getNumVertex ( )];
  for ( int i = 0; i < mVBOMeshRenderer->getLinkedMesh ( )->getNumVertex ( ); ++i )
    mVertexLevels[i] = 1;

  glGenBuffers ( 1, &mBufferId );
  glBindBuffer ( GL_ARRAY_BUFFER, mBufferId );
  glBufferData ( GL_ARRAY_BUFFER,
                 mVBOMeshRenderer->getLinkedMesh ( )->getNumVertex ( )*sizeof ( int ),
                 mVertexLevels,
                 GL_DYNAMIC_DRAW );
  //----glBindBuffer(GL_ARRAY_BUFFER, NULL);
  glBindBuffer ( GL_ARRAY_BUFFER, 0 );
  glEnableVertexAttribArray ( pLayer ); // Vertex levels
  glBindBuffer ( GL_ARRAY_BUFFER, mBufferId );
  glVertexAttribPointer ( pLayer, 1, GL_FLOAT, GL_FALSE, 0, ( GLubyte * ) NULL );
  glBindVertexArray ( 0 );
}

void OldLODController::setLevelsToVertexsInCPU ( int pVertexInit, int pVertexFin, int pLevel )
{
  for ( int i = pVertexInit; i < pVertexFin; ++i )
  {
    mVertexLevels[i] = pLevel;
  }

  glBindBuffer ( GL_ARRAY_BUFFER, mBufferId );
  glBufferData ( GL_ARRAY_BUFFER,
                 mVBOMeshRenderer->getLinkedMesh ( )->getNumVertex ( )*sizeof ( int ),
                 mVertexLevels,
                 GL_DYNAMIC_DRAW );

  glBindVertexArray ( 0 );
}

void OldLODController::setLevelToVertexInCPU ( int pVertex, int pLevel )
{
  if (( pVertex + 1 ) < mVBOMeshRenderer->getLinkedMesh ( )->getNumVertex ( ))
    setLevelsToVertexsInCPU ( pVertex, ( pVertex + 1 ), pLevel );
}

void OldLODController::setLevelsToIdNodesInCPU ( int pNodiIdIni,
                                                 int pNodiIdFin,
                                                 int pInitialDesp,
                                                 int pSegmentDesp,
                                                 int pLevel )
{
  for ( int i = pNodiIdIni; i < pNodiIdFin; ++i )
  {
    for ( int j = 0; j < pSegmentDesp; ++j )
    {
      mVertexLevels[pInitialDesp + ( i*pSegmentDesp ) + j] = pLevel;
    }
  }
}

void OldLODController::setLevelsTo ( int pGlobalLevel )
{
  int dim = mVBOMeshRenderer->getLinkedMesh ( )->getNumVertex ( );
  for ( int i = 0; i < dim; ++i )
    mVertexLevels[i] = pGlobalLevel;

  glBindBuffer ( GL_ARRAY_BUFFER, mBufferId );
  glBufferData ( GL_ARRAY_BUFFER,
                 mVBOMeshRenderer->getLinkedMesh ( )->getNumVertex ( )*sizeof ( int ),
                 mVertexLevels,
                 GL_DYNAMIC_DRAW );

  glBindVertexArray ( 0 );
}


