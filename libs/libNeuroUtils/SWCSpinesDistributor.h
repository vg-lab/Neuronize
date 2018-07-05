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

#ifndef _SWCSPINESDISTRIBUTOR_H_
#define _SWCSPINESDISTRIBUTOR_H_

#include "SWCImporter.h"

using namespace std;
using namespace NSSWCImporter;

namespace NSSWCSpinesDistributor
{
  struct SpineDenNode2
  {
    float SpinesPer_um;
    unsigned int DistToSoma;
    float DistToSomaNormalized;
  };

  struct IncInterval
  {
    float mIniDistance;
    float mFinDistance;
    float mIncrement;
  };

  struct SpinePosOri
  {
    OpenMesh::Vec3f mPosition;
    OpenMesh::Vec3f mOrientation;
  };

  struct NodeSpinesDistrib
  {
    unsigned int mNodeIni;
    unsigned int mNodeFin;
    unsigned int mNumSpines;
    std::vector <SpinePosOri> mSpinesSegmentContainer;
  };

  class SWCSpinesDistributor
  {
    public:
      SWCSpinesDistributor ( );

      ~SWCSpinesDistributor ( );

      void setSpinesDistributionParamas ( SWCImporter *pSWCImporter,
                                          float pSpinesDesp,
                                          float pBProgressionFactor,
                                          float pAProgressionFactor,
                                          float pBasalMinDistance,
                                          float pBasalCteDistance,
                                          float pApicalMinDistance,
                                          float pApicalCteDistance
      );

      void generateDistribution ( );

      void distributeSpinesAlongDendritic ( unsigned int pIdDendritic );

      void distributeSpinesAlongDendriticWellDone ( unsigned int pIdDendritic );

      void exportToFile ( std::string pPath );

      const std::vector <NodeSpinesDistrib> &getSpinesDistributionContainer ( ) const;

      //void SWCSpinesDistributor::exportSpinesInfo(string fileName );

      //void SWCSpinesDistributor::importSpinesInfo(string fileName);

    protected:

    private:
      SWCImporter *mSWCImporter;

      //std::vector<SpineInfo>				mSpinesInfo;
      //float mSpinesIncrementPercent;

      float mSpinesDesp;

      float mBProgressionFactor;

      float mAProgressionFactor;

      float mBasalMinDistance;

      float mBasalCteDistance;

      float mApicalMinDistance;

      float mApicalCteDistance;

      std::vector <NodeSpinesDistrib> mSpinesDistributionContainer;

      std::vector <IncInterval> mIncrementalIntervalsContainer;

      void buildIncrementalIntervals ( );

      float nextSpinePosition ( float pMinDesp, float pIncFactor, unsigned int pIndex );

  };
}

#endif //_SWCSPINESDISTRIBUTOR_H_