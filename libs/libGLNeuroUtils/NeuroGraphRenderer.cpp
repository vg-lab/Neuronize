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

#include "NeuroGraphRenderer.h"

NeuronGraphRenderer::NeuronGraphRenderer ( void )
{
  mNeuronGraph = NULL;
  mNodeColor = OpenMesh::Vec3f ( 1, 0, 0 );
  mLineColor = OpenMesh::Vec3f ( 0, 0, 0 );
  mIncNode = 0;
}

NeuronGraphRenderer::~NeuronGraphRenderer ( void )
{
}

void NeuronGraphRenderer::setGraphColors ( const OpenMesh::Vec3f &pNodeColor, const OpenMesh::Vec3f &pLineColor )
{
  mNodeColor = pNodeColor;
  mLineColor = pLineColor;
}

void NeuronGraphRenderer::renderNeuronGraph ( )
{
  if ( mNeuronGraph != NULL )
  {
    float lRadius;
    int i = 0;

    glColor3f ( mNodeColor[0], mNodeColor[1], mNodeColor[2] );
    for ( mNeuronGraph->vp = boost::vertices ( mNeuronGraph->NGraph );
          mNeuronGraph->vp.first != mNeuronGraph->vp.second; ++mNeuronGraph->vp.first )
    {
      ++i;
      glPushMatrix ( );
      pos = mNeuronGraph->node_id[*mNeuronGraph->vp.first].OriginalNode.position;

      glTranslatef ( pos[0], pos[1], pos[2] );

      lRadius = mNeuronGraph->node_id[*mNeuronGraph->vp.first].OriginalNode.radius;

      glutSolidSphere ( lRadius + mIncNode, 10, 10 );
      glPopMatrix ( );
    }

    glDisable ( GL_LIGHTING );
    glColor3f ( mLineColor[0], mLineColor[1], mLineColor[2] );
    for ( boost::tie ( mNeuronGraph->ei, mNeuronGraph->ei_end ) = boost::edges ( mNeuronGraph->NGraph );
          mNeuronGraph->ei != mNeuronGraph->ei_end; ++mNeuronGraph->ei )
    {
      pos = mNeuronGraph->node_id[boost::source ( *mNeuronGraph->ei, mNeuronGraph->NGraph )].OriginalNode.position;
      pos2 = mNeuronGraph->node_id[boost::target ( *mNeuronGraph->ei, mNeuronGraph->NGraph )].OriginalNode.position;

      glBegin ( GL_LINES );
      glVertex3f ( pos[0], pos[1], pos[2] );
      glVertex3f ( pos2[0], pos2[1], pos2[2] );
      glEnd ( );
    }
    glEnable ( GL_LIGHTING );
  }
}
