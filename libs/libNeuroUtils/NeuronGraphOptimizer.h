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
#include "NeuronGraph.h"
#include <boost/graph/copy.hpp>

#include <glm/glm.hpp>
#include <glmSpline.hpp>
#include "imppsimpl.h"

class NeuronGraphOptimizer
    //: public NeuronGraph
{
    NeuronGraph &mOriginalGraph;

    NeuronGraph &mOptimizedGraph;

    NSSpline::Spline < float, glm::vec3 > *mSpliner;

    std::vector < float > mSimplifiedCoords;

    unsigned int mOptimizationMethod;

    unsigned int mIniValue;
    unsigned int mFinalValue;

    //Optimization methods
    int mNPoints;
    float mRadialDistance;
    float mPerpendDistance;
    float mRWDistance;
    float mOpheinMinThreshold;
    float mOpheinMaxThreshold;
    float mLangThreshold;
    float mLangSize;
    float mDPThreshold;
    int mDPModifNumPoints;

    //Enhanded methods


    //Editing methods


    //Rerpair methods


    //From stacks methods


    //Add manual selectionmethods (add ids such as in progresive visualization)
    //Aplicar las transformaciones como en el progressive visualizator


    //Filosof�a, siempre aplicar los criterios sobre el grapho copia para no perder el original, machacar el original a mano cuando los cambios son buenos

  public:

    NeuronGraphOptimizer ( NeuronGraph &pOriginalGraph, NeuronGraph &pOptimizedGraph );

    ~NeuronGraphOptimizer ( void );

    void setSimplificationInterval ( unsigned int pIniVertex, unsigned int pFinalVertex );

    void setOptimizeParams ( int pNPoints,
                             float pRadialDistance,
                             float pPerpendDistance,
                             float pRWDistance,
                             float pOpheinMinThreshold,
                             float pOpheinMaxThreshold,
                             float pLangThreshold,
                             float pLangSize,
                             float pDPThreshold,
                             int pDPModifNumPoints );

    void setOptimizeMethod ( unsigned int pOptMethod ) { mOptimizationMethod = pOptMethod; };

    void optimize ( );

    void directDeleteVertexs ( unsigned int pStep );

    void copyGraph ( );

    void recalculateConectivity ( );

    NeuronGraph &getOptimizedGraph ( ) { return mOptimizedGraph; };

    void applyNodeSplineDistribution ( unsigned int pNumNodesPerSegment );

    //void applyNodeLinearDistribution(unsigned int pNumNodesPerSegment);
    void applyNodeLinearDistribution ( );
};

