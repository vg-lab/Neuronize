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

#include "DeformableObject.h"
#include "MSSpringConstraint.h"

#include <vector>
#include <list>

using namespace std;

namespace NSMSObject
{
  class MSObject: public NSDeformableObject::DeformableObject
  {
    private:

      //Contenedores de nodos y springs
      vector <MSNode> mNodeContainer;
      vector <MSSpring> mSpringContainer;
      std::list < int > mSelectionContainer;
      vector <MSSpringConstraint> mConstraints;
      vector <OpenMesh::Vec3f> mExternalForces;

      int mIntegrationMethod;
      float mDt;
      bool mUseConstraits;
      float mStiffness;
      float mDumping;

      int mUnCollapseSpringsPercent;

      float mNodeMass;
      float mSystemMass;
      OpenMesh::Vec3f mGravity;

      //Nearly

    public:
      MSObject ( void );

      ~MSObject ( void );

      void generateMSBaseStructure ( );

      void destroyMSBaseStructure ( );

      //Mass Spring seters
      void setMSIntegrator ( int pMethod );

      void setMSDt ( float pDt );

      void setMSConstraints ( bool pUseConstraits );

      void setUnCollapseSprings ( int pUnCollapseSpringsPercent );

      void setMSSpringParams ( float pStiffness, float pDumping );

      void setMSNodeParams ( float pMass, OpenMesh::Vec3f pGravity );

      vector <MSNode> &getNodeContainer ( );

      vector <MSSpring> &getSpringContainer ( );

      std::list < int > &getSelectionContainer ( );

      virtual void update ( );

      virtual void updateMeshPosition ( );

      int getParamIntegrationMethod ( );

      float getParamDt ( );

      bool getParamUseConstraits ( );

      float getParamStiffness ( );

      float getParamDumping ( );

      int getParamUnCollapseSpringsPercent ( );

      float getParamNodeMass ( );

      float getParamSystemMass ( );

      OpenMesh::Vec3f getParamGravity ( );

  };
}
