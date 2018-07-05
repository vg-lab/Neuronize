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

#include "VBOMeshRenderer.h"

using namespace NSVBOMeshRenderer;

class OldLODController
{
    //const VBOMeshRenderer * mVBOMeshRenderer;
    VBOMeshRenderer *mVBOMeshRenderer;

    //int * mVertexLevels;
    float *mVertexLevels;

    GLuint mBufferId;

  public:
    //LODController(const VBOMeshRenderer * pVBOMeshRenderer);
    OldLODController ( VBOMeshRenderer *pVBOMeshRenderer );

    ~OldLODController ( void );

    void AllocLevels ( int pLayer );

    void setLevelsToVertexsInCPU ( int pVertexInit, int pVertexFin, int pLevel );

    void setLevelToVertexInCPU ( int pVertex, int pLevel );

    void setLevelsToIdNodesInCPU ( int pNodiIdIni, int pNodiIdFin, int pInitialDesp, int pSegmentDesp, int pLevel );

    void setLevelsTo ( int pGlobalLevel );

    float *getVertexLevels ( ) { return mVertexLevels; }

    GLuint getVertexLODBuffer ( ) { return mBufferId; }
};

