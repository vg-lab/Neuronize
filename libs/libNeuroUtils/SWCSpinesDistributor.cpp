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
 * @SWCSpinesDistributor
 * @SWCSpinesDistributor.h
 * @author Juan Pedro Brito M�ndez
 * @version 0.1
 */

#include <math.h>

#include "SWCSpinesDistributor.h"
#include "randomUtils.h"

//#define DNEARDISTTOSOMA		10.0
//#define DFARDISTTOSOMA		40.0
//#define SPNPROGFACTOR		100

#define MAXITERATIONS    100

namespace NSSWCSpinesDistributor
{
  SWCSpinesDistributor::SWCSpinesDistributor ( )
  {

  }

  SWCSpinesDistributor::~SWCSpinesDistributor ( )
  {

  }

  void SWCSpinesDistributor::setSpinesDistributionParamas ( SWCImporter *pSWCImporter,
                                                            float pSpinesDesp,
                                                            float pBProgressionFactor,
                                                            float pAProgressionFactor,
                                                            float pBasalMinDistance,
                                                            float pBasalCteDistance,
                                                            float pApicalMinDistance,
                                                            float pApicalCteDistance
  )
  {

    mSWCImporter = pSWCImporter;

    mSpinesDesp = pSpinesDesp;
    mBProgressionFactor = pBProgressionFactor;
    mAProgressionFactor = pAProgressionFactor;
    mBasalMinDistance = pBasalMinDistance;
    mBasalCteDistance = pBasalCteDistance;
    mApicalMinDistance = pApicalMinDistance;
    mApicalCteDistance = pApicalCteDistance;
  }

/*
	void SWCSpinesDistributor::distributeSpinesAlongDendritic(unsigned int pIdDendritic)
	{

		unsigned int						lDendriticSegmentIni	=	mSWCImporter->getDendritics()[pIdDendritic].initialNode;
		unsigned int						lDendriticSegmentFin	=	mSWCImporter->getDendritics()[pIdDendritic].finalNode;

		unsigned int						lParentId				=	0;
		unsigned int						lNumSpinesInSegment		=	0;

		float								lSegmentDistance		=	0.0;

		NodeSpinesDistrib					lNodeSpinesDistrib;
		SpinesPosOri						lSpinesPosOri;

		OpenMesh::Vec3f						lDirection(0,0,0);
		OpenMesh::Vec3f						lAuxVec(0,0,0);
		OpenMesh::Vec3f						lAuxPos(0,0,0);

		float								lSpinesSeparation=0;

		initrand();

		//Incremental distribution
		//if ( (mSWCImporter->getDendritics()[pIdDendritic].type != DendriticType::AXON)		
		//	//&& (mSWCImporter->getDendritics()[pIdDendritic].type != DendriticType::APICAL)		
		//	)

		//Solo apical
		if (
			(mSWCImporter->getDendritics()[pIdDendritic].type == DendriticType::APICAL)		
			)
		{
			//Recooremos todos los segmentos que componen la dendrita
			for (unsigned int i=lDendriticSegmentIni;i<lDendriticSegmentFin;++i)
			{
				lParentId = mSWCImporter->getElementAt(i).parent;

				//Calcular distancia desde el centro del soma al parent
				lSegmentDistance	=	mSWCImporter->getElementAt(lParentId).mDistanceToSoma;

				//Si est� en el intervalo de spinas creciente creamos un segmento de espinas crecientes
				//III: Nota las distancias se evaluan de parent a parent
				if ( 
					(lSegmentDistance > DNEARDISTTOSOMA)
					&& (lSegmentDistance < DFARDISTTOSOMA)
					)
				{
					//Segmentos iniciales y finales de los trozos de dendritas donde 
					//se han de disponer las espinas
					unsigned int lInitialNode	=	i;
					unsigned int lFinalNode		=	0;

					bool lFinalDendriticPiece	=	false;
					unsigned int lAuxParent		=	0;
					float lAuxSegmentDistance	=	0.0;

					//Avanzamos por los nodos para saber hasta donde es creciente
					//Y marcamos todos los nodos que conformaran un segmento creciente de espinas
					while (!lFinalDendriticPiece)
					{
						//Avanzamos por el segmento y marcamos 
						lAuxParent			= mSWCImporter->getElementAt(i).parent;
						lAuxSegmentDistance = mSWCImporter->getElementAt(lAuxParent).mDistanceToSoma;

						if ( 
								(lAuxSegmentDistance>DFARDISTTOSOMA)
								//|| (lAuxCounter>=MAXITERATIONS)
								|| (i>=lDendriticSegmentFin)
							)
						{
							lFinalDendriticPiece = true;

							//Decrementamos el identificado del segmento que se esta tratando
							//(Es la variable iteradora del bucle)
							--i;
						}
						else
						{
							++i;
						}	
					}

					lFinalNode	=	i;

					//Ya tenmos calculados los nodos iniciales y finales de cada trazo de dendrita
					//en la que aplicar una disribucion creciente
					lNodeSpinesDistrib.mNodeIni		=	lInitialNode;
					lNodeSpinesDistrib.mNodeFin		=	lFinalNode;

					lNodeSpinesDistrib.mSpinesSegmentContainer.clear();

					//for (unsigned int j=0;j<lNumSpinesInSegment;++j)
					bool	lFinishSpinesDisp			=	false;
					float	lPieceAdvance				=	0.0;
					float	lPieceAdvanceGlobalAdvance	=	0.0;
					float	lPartialSegmentAdvance		=	0;

					//Calculamos la distancia del segmento para saber cuantas espinas se han de colocar
					lSegmentDistance		=	mSWCImporter->getElementAt(lFinalNode).mDendriticDistanceReal 
												- mSWCImporter->getElementAt(lInitialNode).mDendriticDistanceReal;
					unsigned int lIdx=0;
					while(!lFinishSpinesDisp)
					{						
						//Reset the Disposition
						lSpinesPosOri.mPosition		= OpenMesh::Vec3f(0,0,0);
						lSpinesPosOri.mOrientation	= OpenMesh::Vec3f(0,0,0);

						//Calculate the position
						lPieceAdvance = nextSpinePosition(mSpinesDesp,1.0/SPNPROGFACTOR,lIdx);
						lPieceAdvanceGlobalAdvance+=lPieceAdvance;

						//lAuxPos = mSWCImporter->getElementAt(lInitialNode).position 
						//						+ ( lDirection * (lPieceAdvanceGlobalAdvance) );

						//Hay que calcular el segmento en el que cae, la distancia acumulada y la cantidad 
						//que se desplaza en el segmento en el que cae
						//Ojo, se le da la vuelta para recorrerlo del reves
						unsigned int	lAuxNodeIni	=	lFinalNode;
						unsigned int	lAuxNodeFin	=	mSWCImporter->getElementAt(lAuxNodeIni).parent;
						float			lAuxPInc	=	fabs(mSWCImporter->getElementAt(lFinalNode).mDendriticDistanceReal - (lPieceAdvanceGlobalAdvance));
						bool			lForceStop	=	false;

						while (
								!( ( lAuxPInc < mSWCImporter->getElementAt(lAuxNodeIni).mDendriticDistanceReal )
									&& ( lAuxPInc > mSWCImporter->getElementAt(lAuxNodeFin).mDendriticDistanceReal )
								  )
								&& (!lForceStop)
							  )
						{
						
							lAuxNodeIni	=	lAuxNodeFin;
							lAuxNodeFin	=	mSWCImporter->getElementAt(lAuxNodeIni).parent;;

							//if (lAuxNodeIni	==	lInitialNode)
							//if (lAuxNodeFin	==	lInitialNode)
							//if (lAuxNodeIni	==	lInitialNode)
							//{
							//	lForceStop	=	true;
							//}
						}

						//lPartialSegmentAdvance = (mSWCImporter->getElementAt(lAuxNodeIni).mDendriticDistanceReal + lPieceAdvanceGlobalAdvance) 
						//							- mSWCImporter->getElementAt(lFinalNode).mDendriticDistanceReal;
						unsigned int lAuxCounter	=	0;

						//if (!lForceStop)
						{
							lPartialSegmentAdvance = (mSWCImporter->getElementAt(lAuxNodeIni).mDendriticDistanceReal + lPieceAdvanceGlobalAdvance) 
														- mSWCImporter->getElementAt(lFinalNode).mDendriticDistanceReal;							

							//lPartialSegmentAdvance = 0.2;


							//Calculamos la direccion del segmento donde ha caido
							lDirection = mSWCImporter->getElementAt(lAuxNodeFin).position 
										- mSWCImporter->getElementAt(lAuxNodeIni).position;
							lDirection.normalize();

							//Calculamos la posici�n exacta
							lAuxPos = mSWCImporter->getElementAt(lAuxNodeIni).position 
																//+ ( (lDirection*(-1) ) * (lPieceAdvanceGlobalAdvance) );
																+ ( (lDirection ) * (lPartialSegmentAdvance) );

							//Calculate the orientation
							//----------------------------
							lAuxVec = OpenMesh::Vec3f(randfloat(-1,1),randfloat(-1,1),randfloat(-1,1));
							//lAuxVec = OpenMesh::Vec3f(1,0,0);
							lAuxVec.normalize();

							//El producto vectorial (cross) nos da un vector normal
							lAuxVec = OpenMesh::cross(lAuxPos, lAuxVec);
							lAuxVec.normalize();

							//Set the node values
							lSpinesPosOri.mPosition		=	lAuxPos;
							lSpinesPosOri.mOrientation	=	lAuxVec;

							//Add Spine to node
							lNodeSpinesDistrib.mSpinesSegmentContainer.push_back(lSpinesPosOri);
						}

						//We finish the Incremental Spine Distribution
						//lPieceAdvanceGlobalAdvance+=lPieceAdvance;
						++lIdx;

						if (lPieceAdvanceGlobalAdvance>lSegmentDistance)
						{
							lFinishSpinesDisp = true;	
						}
					}

					//Add all the spines for this node
					mSpinesDistributionContainer.push_back(lNodeSpinesDistrib);
				}
			}

		}

		//Distribuci�n constante

		//No hacer
		//if (false)

		//Hacer ambas
		//if ( (mSWCImporter->getDendritics()[pIdDendritic].type != DendriticType::AXON)		
			//&& (mSWCImporter->getDendritics()[pIdDendritic].type != DendriticType::APICAL)		
		//	)


		//Solo apical
		if (
			(mSWCImporter->getDendritics()[pIdDendritic].type == DendriticType::APICAL)		
			)

		{
			//Si el parent esta a m�s de la distancia m�nima del soma
			//Calcular cuantas espinas van en ese segmento (divisi�n de la distancia entre el deplazamiento)
			for (unsigned int i=lDendriticSegmentIni;i<lDendriticSegmentFin;++i)
			{
				lParentId = mSWCImporter->getElementAt(i).parent;

				//Calcular distancia del parent
				lSegmentDistance	=	mSWCImporter->getElementAt(lParentId).mDistanceToSoma;
			
				if (lSegmentDistance > DFARDISTTOSOMA)
				{
					//Calculamos la distancia del segmento para saber cuantas espinas se han de colocar
					lSegmentDistance		=	mSWCImporter->getElementAt(i).mDendriticDistanceReal 
												- mSWCImporter->getElementAt(lParentId).mDendriticDistanceReal;
					lNumSpinesInSegment		=	lSegmentDistance/mSpinesDesp;

					lDirection = mSWCImporter->getElementAt(i).position - mSWCImporter->getElementAt(lParentId).position;
					lDirection.normalize();

					lNodeSpinesDistrib.mNodeIni		= lParentId;
					lNodeSpinesDistrib.mNodeFin		= i;
					lNodeSpinesDistrib.mNumSpines	= lNumSpinesInSegment;
					lNodeSpinesDistrib.mSpinesSegmentContainer.clear();
					
					lSpinesPosOri.mPosition		= OpenMesh::Vec3f(0,0,0);
					lSpinesPosOri.mOrientation	= OpenMesh::Vec3f(0,0,0);

					for (unsigned int j=0;j<lNumSpinesInSegment;++j)
					{
						//Calculate the position
						lAuxPos = mSWCImporter->getElementAt(lParentId).position 
												+ ( lDirection * (j * mSpinesDesp) );

						//Calculate the orientation
						lAuxVec = OpenMesh::Vec3f(randfloat(-1,1),randfloat(-1,1),randfloat(-1,1));
						lAuxVec.normalize();

						//El producto vectorial (cross) nos da un vector normal
						lAuxVec = OpenMesh::cross(lAuxPos, lAuxVec);
						lAuxVec.normalize();


						//Set the node values
						lSpinesPosOri.mPosition		=	lAuxPos;
						lSpinesPosOri.mOrientation	=	lAuxVec;


						//Add Spine to node
						lNodeSpinesDistrib.mSpinesSegmentContainer.push_back(lSpinesPosOri);
					}

					//Add all the spines for this node
					mSpinesDistributionContainer.push_back(lNodeSpinesDistrib);
				}
			}
		}
	}

*/


  void SWCSpinesDistributor::generateDistribution ( )
  {

    unsigned int lNumDendritics = mSWCImporter->getNumDendritics ( );
    //mSpinesInfo.clear();

    //Restore the dendrititc modificator
    for ( unsigned int i = 0; i < lNumDendritics; ++i )
      //for (unsigned int i=0;i<1;++i)
      //unsigned int i=0;
    {
      //distributeSpinesAlongDendritic(i);
      distributeSpinesAlongDendriticWellDone ( i );
    }
  }

  void SWCSpinesDistributor::distributeSpinesAlongDendritic ( unsigned int pIdDendritic )
  {

    unsigned int lDendriticSegmentIni = mSWCImporter->getDendritics ( )[pIdDendritic].initialNode;
    unsigned int lDendriticSegmentFin = mSWCImporter->getDendritics ( )[pIdDendritic].finalNode;

    unsigned int lParentId = 0;
    unsigned int lNumSpinesInSegment = 0;

    float lSegmentDistance = 0.0;

    NodeSpinesDistrib lNodeSpinesDistrib;
    SpinePosOri lSpinesPosOri;

    OpenMesh::Vec3f lDirection ( 0, 0, 0 );
    OpenMesh::Vec3f lAuxVec ( 0, 0, 0 );
    OpenMesh::Vec3f lAuxPos ( 0, 0, 0 );

    float lSpinesSeparation = 0;
    unsigned int lDendriticType = -1;
    float lMinLimit = 0;
    float lMaxLimit = 0;

    initrand ( );

    //Incremental distribution
    if (( mSWCImporter->getDendritics ( )[pIdDendritic].type != DendriticType::AXON )
      //&& (mSWCImporter->getDendritics()[pIdDendritic].type != DendriticType::APICAL)
      )
    {
      lDendriticType = mSWCImporter->getDendritics ( )[pIdDendritic].type;

      if ( lDendriticType == DendriticType::BASAL )
      {
        lMinLimit = mBasalMinDistance;
        lMaxLimit = mBasalCteDistance;
      }
      else
      {
        lMinLimit = mApicalMinDistance;
        lMaxLimit = mApicalCteDistance;
      }


      //Recooremos todos los segmentos que componen la dendrita
      for ( unsigned int i = lDendriticSegmentIni; i < lDendriticSegmentFin; ++i )
      {
        lParentId = mSWCImporter->getElementAt ( i ).parent;

        //Calcular distancia desde el centro del soma al parent
        lSegmentDistance = mSWCImporter->getElementAt ( lParentId ).mDistanceToSoma;

        //Si est� en el intervalo de spinas creciente creamos un segmento de espinas crecientes
        //III: Nota las distancias se evaluan de parent a parent
        //if (
        //	(lSegmentDistance > DNEARDISTTOSOMA)
        //	&& (lSegmentDistance < DFARDISTTOSOMA)
        //	)

        if (
          ( lSegmentDistance > lMinLimit )
            && ( lSegmentDistance < lMaxLimit )
          )
        {
          //Segmentos iniciales y finales de los trozos de dendritas donde
          //se han de disponer las espinas

          //### Bug de zonas no rellenadas en las bifurcaciones (no estoy seguro de q este arreglada as�)
          //unsigned int lInitialNode	=	i;
          unsigned int lInitialNode = lParentId;
          unsigned int lFinalNode = 0;

          bool lFinalDendriticPiece = false;
          unsigned int lAuxParent = 0;
          float lAuxSegmentDistance = 0.0;

          //Avanzamos por los nodos para saber hasta donde es creciente
          //Y marcamos todos los nodos que conformaran un segmento creciente de espinas
          while ( !lFinalDendriticPiece )
          {
            //Avanzamos por el segmento y marcamos
            lAuxParent = mSWCImporter->getElementAt ( i ).parent;
            lAuxSegmentDistance = mSWCImporter->getElementAt ( lAuxParent ).mDistanceToSoma;

            if (
              ( lAuxSegmentDistance > lMaxLimit )
                //|| (lAuxCounter>=MAXITERATIONS)
                || ( i >= lDendriticSegmentFin )
              //|| (mSWCImporter->isDendriticTermination(i))
              )
            {
              lFinalDendriticPiece = true;

              //Decrementamos el identificado del segmento que se esta tratando
              //(Es la variable iteradora del bucle)
              --i;
            }
            else
            {
              ++i;
            }
          }

          lFinalNode = i;

          //Ya tenmos calculados los nodos iniciales y finales de cada trazo de dendrita
          //en la que aplicar una disribucion creciente

          //Super mega nyapa: PAra el bug q no se disponen espinas en las bifurcaciones grandes no consecutivas
          if ( lInitialNode != lFinalNode )
            lNodeSpinesDistrib.mNodeIni = lInitialNode;

          lNodeSpinesDistrib.mNodeFin = lFinalNode;

          lNodeSpinesDistrib.mSpinesSegmentContainer.clear ( );

          //for (unsigned int j=0;j<lNumSpinesInSegment;++j)
          bool lFinishSpinesDisp = false;
          float lPieceAdvance = 0.0;
          float lPieceAdvanceGlobalAdvance = 0.0;
          float lPartialSegmentAdvance = 0;

          //Calculamos la distancia del segmento para saber cuantas espinas se han de colocar
          lSegmentDistance = mSWCImporter->getElementAt ( lNodeSpinesDistrib.mNodeFin ).mDendriticDistanceReal
            - mSWCImporter->getElementAt ( lNodeSpinesDistrib.mNodeIni ).mDendriticDistanceReal;
          unsigned int lIdx = 0;
          while ( !lFinishSpinesDisp )
          {
            //Reset the Disposition
            lSpinesPosOri.mPosition = OpenMesh::Vec3f ( 0, 0, 0 );
            lSpinesPosOri.mOrientation = OpenMesh::Vec3f ( 0, 0, 0 );


            //Calculate the position
            if ( mSWCImporter->getElementAt ( i ).type == DendriticType::BASAL )
            {
              lPieceAdvance = nextSpinePosition ( mSpinesDesp, 1.0/mBProgressionFactor, lIdx );
            }
            else
            {
              lPieceAdvance = nextSpinePosition ( mSpinesDesp, 1.0/mAProgressionFactor, lIdx );
            }

            lPieceAdvanceGlobalAdvance += lPieceAdvance;

            //lAuxPos = mSWCImporter->getElementAt(lInitialNode).position
            //						+ ( lDirection * (lPieceAdvanceGlobalAdvance) );

            //Hay que calcular el segmento en el que cae, la distancia acumulada y la cantidad
            //que se desplaza en el segmento en el que cae
            //Ojo, se le da la vuelta para recorrerlo del reves
            unsigned int lAuxNodeIni = lFinalNode;
            unsigned int lAuxNodeFin = mSWCImporter->getElementAt ( lAuxNodeIni ).parent;
            float lAuxPInc =
              fabs ( mSWCImporter->getElementAt ( lFinalNode ).mDendriticDistanceReal - ( lPieceAdvanceGlobalAdvance ));
            bool lForceStop = false;

            while (
              !(( lAuxPInc < mSWCImporter->getElementAt ( lAuxNodeIni ).mDendriticDistanceReal )
                && ( lAuxPInc > mSWCImporter->getElementAt ( lAuxNodeFin ).mDendriticDistanceReal )
              )
                && ( !lForceStop )
              )
            {

              lAuxNodeIni = lAuxNodeFin;
              lAuxNodeFin = mSWCImporter->getElementAt ( lAuxNodeIni ).parent;;

              //if (lAuxNodeIni	==	lInitialNode)
              //if (lAuxNodeFin	==	lInitialNode)
              //if (lAuxNodeIni	==	lInitialNode)
              //{
              //	lForceStop	=	true;
              //}
            }

            //lPartialSegmentAdvance = (mSWCImporter->getElementAt(lAuxNodeIni).mDendriticDistanceReal + lPieceAdvanceGlobalAdvance)
            //							- mSWCImporter->getElementAt(lFinalNode).mDendriticDistanceReal;
            unsigned int lAuxCounter = 0;

            //if (!lForceStop)
            {
              lPartialSegmentAdvance =
                ( mSWCImporter->getElementAt ( lAuxNodeIni ).mDendriticDistanceReal + lPieceAdvanceGlobalAdvance )
                  - mSWCImporter->getElementAt ( lFinalNode ).mDendriticDistanceReal;

              //lPartialSegmentAdvance = 0.2;


              //Calculamos la direccion del segmento donde ha caido
              lDirection = mSWCImporter->getElementAt ( lAuxNodeFin ).position
                - mSWCImporter->getElementAt ( lAuxNodeIni ).position;
              lDirection.normalize ( );

              //Calculamos la posici�n exacta
              lAuxPos = mSWCImporter->getElementAt ( lAuxNodeIni ).position
                //+ ( (lDirection*(-1) ) * (lPieceAdvanceGlobalAdvance) );
                + (( lDirection )*( lPartialSegmentAdvance ));

              //Calculate the orientation
              //----------------------------
              lAuxVec = OpenMesh::Vec3f ( randfloat ( -1, 1 ), randfloat ( -1, 1 ), randfloat ( -1, 1 ));
              //lAuxVec = OpenMesh::Vec3f(1,0,0);
              lAuxVec.normalize ( );

              //El producto vectorial (cross) nos da un vector normal
              lAuxVec = OpenMesh::cross ( lAuxPos, lAuxVec );
              lAuxVec.normalize ( );

              //Set the node values
              //lSpinesPosOri.mPosition		=	lAuxPos;

              float lMinRadioDesp = 0;

              if ( mSWCImporter->getElementAt ( i ).radius < mSWCImporter->getElementAt ( lParentId ).radius )
              {
                lMinRadioDesp = mSWCImporter->getElementAt ( i ).radius;
              }
              else
              {
                lMinRadioDesp = mSWCImporter->getElementAt ( lParentId ).radius;
              }

              //Las hundimos un tercio del radio m�nimo.
              lSpinesPosOri.mPosition = lAuxPos + ( lAuxVec*( lMinRadioDesp - ( lMinRadioDesp/3.0 )));
              lSpinesPosOri.mOrientation = lAuxVec;

              //Add Spine to node
              lNodeSpinesDistrib.mSpinesSegmentContainer.push_back ( lSpinesPosOri );
              //mSpinesInfo.push_back(lSpinesPosOri);
            }

            //We finish the Incremental Spine Distribution
            //lPieceAdvanceGlobalAdvance+=lPieceAdvance;
            ++lIdx;

            if (( lPieceAdvanceGlobalAdvance > lSegmentDistance )
              //|| ()
              )
            {
              lFinishSpinesDisp = true;
            }
          }

          //Add all the spines for this node
          mSpinesDistributionContainer.push_back ( lNodeSpinesDistrib );
        }
      }

    }



    //Distribuci�n constante
    //if (false)
    if (( mSWCImporter->getDendritics ( )[pIdDendritic].type != DendriticType::AXON )
      //&& (mSWCImporter->getDendritics()[pIdDendritic].type != DendriticType::APICAL)
      )
    {
      //Si el parent esta a m�s de la distancia m�nima del soma
      //Calcular cuantas espinas van en ese segmento (divisi�n de la distancia entre el deplazamiento)
      for ( unsigned int i = lDendriticSegmentIni; i <= lDendriticSegmentFin; ++i )
      {
        lParentId = mSWCImporter->getElementAt ( i ).parent;

        //Calcular distancia del parent
        lSegmentDistance = mSWCImporter->getElementAt ( lParentId ).mDistanceToSoma;

        if ( lSegmentDistance > lMaxLimit )
        {
          //Calculamos la distancia del segmento para saber cuantas espinas se han de colocar
          lSegmentDistance = mSWCImporter->getElementAt ( i ).mDendriticDistanceReal
            - mSWCImporter->getElementAt ( lParentId ).mDendriticDistanceReal;
          lNumSpinesInSegment = lSegmentDistance/mSpinesDesp;

          lDirection = mSWCImporter->getElementAt ( i ).position - mSWCImporter->getElementAt ( lParentId ).position;
          lDirection.normalize ( );

          lNodeSpinesDistrib.mNodeIni = lParentId;
          lNodeSpinesDistrib.mNodeFin = i;
          lNodeSpinesDistrib.mNumSpines = lNumSpinesInSegment;
          lNodeSpinesDistrib.mSpinesSegmentContainer.clear ( );

          lSpinesPosOri.mPosition = OpenMesh::Vec3f ( 0, 0, 0 );
          lSpinesPosOri.mOrientation = OpenMesh::Vec3f ( 0, 0, 0 );

          for ( unsigned int j = 0; j < lNumSpinesInSegment; ++j )
          {
            //Calculate the position
            lAuxPos = mSWCImporter->getElementAt ( lParentId ).position
              + ( lDirection*( j*mSpinesDesp ));

            //Calculate the orientation
            lAuxVec = OpenMesh::Vec3f ( randfloat ( -1, 1 ), randfloat ( -1, 1 ), randfloat ( -1, 1 ));
            lAuxVec.normalize ( );

            //El producto vectorial (cross) nos da un vector normal
            lAuxVec = OpenMesh::cross ( lAuxPos, lAuxVec );
            lAuxVec.normalize ( );

            //Set the node values
            //lSpinesPosOri.mPosition		=	lAuxPos;

            float lMinRadioDesp = 0;

            if ( mSWCImporter->getElementAt ( i ).radius < mSWCImporter->getElementAt ( lParentId ).radius )
            {
              lMinRadioDesp = mSWCImporter->getElementAt ( i ).radius;
            }
            else
            {
              lMinRadioDesp = mSWCImporter->getElementAt ( lParentId ).radius;
            }

            //lSpinesPosOri.mPosition		=	lAuxPos + lAuxVec*lMinRadioDesp;

            lSpinesPosOri.mPosition = lAuxPos + ( lAuxVec*( lMinRadioDesp - ( lMinRadioDesp/3.0 )));
            lSpinesPosOri.mOrientation = lAuxVec;

            //Add Spine to node
            lNodeSpinesDistrib.mSpinesSegmentContainer.push_back ( lSpinesPosOri );
            //mSpinesInfo.push_back(lSpinesPosOri);
          }

          //Add all the spines for this node
          mSpinesDistributionContainer.push_back ( lNodeSpinesDistrib );
        }
      }
    }

  }

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  void SWCSpinesDistributor::distributeSpinesAlongDendriticWellDone ( unsigned int pIdDendritic )
  {

    unsigned int lDendriticNodeIni = mSWCImporter->getDendritics ( )[pIdDendritic].initialNode;
    unsigned int lDendriticNodeFin = mSWCImporter->getDendritics ( )[pIdDendritic].finalNode;

    unsigned int lParentId = 0;
    int lNumSpinesInSegment = 0;

    float lIniNodeDistanceToSoma = 0.0;
    float lFinNodeDistanceToSoma = 0.0;
    float lSegmentDistance = 0.0;

    NodeSpinesDistrib lNodeSpinesDistrib;
    SpinePosOri lSpinesPosOri;

    OpenMesh::Vec3f lDirection ( 0, 0, 0 );
    OpenMesh::Vec3f lAuxVec ( 0, 0, 0 );
    OpenMesh::Vec3f lAuxPos ( 0, 0, 0 );

    unsigned int lDendriticType = -1;

    std::vector <SpineDenNode2> BasalSpineDensityContainer;
    std::vector <SpineDenNode2> ApicalSpineDensityContainer;

    SpineDenNode2 lAuxNode;

    //#define BASAL_INTERVALS		10
    //#define APICAL_INTERVALS	22

    unsigned int BASAL_INTERVALSAUX = 10;
    unsigned int APICAL_INTERVALSAUX = 22;

    //Generate basal distribution
    lAuxNode.DistToSoma = lAuxNode.SpinesPer_um = 0;
    for ( int i = 0; i <= BASAL_INTERVALSAUX; ++i )
    {
      lAuxNode.DistToSoma += 20;
      BasalSpineDensityContainer.push_back ( lAuxNode );
    }

    //Normalize distance
    for ( int i = 0; i <= BASAL_INTERVALSAUX; ++i )
    {
      BasalSpineDensityContainer[i].DistToSomaNormalized = ( float ) BasalSpineDensityContainer[i].DistToSoma
        /( float ) BasalSpineDensityContainer[BASAL_INTERVALSAUX].DistToSoma;
    }

    BasalSpineDensityContainer.at ( 0 ).SpinesPer_um = 0;    //20
    BasalSpineDensityContainer.at ( 1 ).SpinesPer_um = 0.45;    //40
    BasalSpineDensityContainer.at ( 2 ).SpinesPer_um = 1.1;    //60
    BasalSpineDensityContainer.at ( 3 ).SpinesPer_um = 1.7;    //80
    BasalSpineDensityContainer.at ( 4 ).SpinesPer_um = 2;    //100
    BasalSpineDensityContainer.at ( 5 ).SpinesPer_um = 1.9;    //120
    BasalSpineDensityContainer.at ( 6 ).SpinesPer_um = 1.8;    //140
    BasalSpineDensityContainer.at ( 7 ).SpinesPer_um = 1.85;    //160
    BasalSpineDensityContainer.at ( 8 ).SpinesPer_um = 1.6;    //180
    BasalSpineDensityContainer.at ( 9 ).SpinesPer_um = 1.5;    //200
    BasalSpineDensityContainer.at ( 10 ).SpinesPer_um = 1.55;    //220

    //Nodo muy alejado para impedir que la evaluaci�n del interfalo se salga de rando
    //La tomamos constante a partir de este punto
    lAuxNode.DistToSoma = 5000;
    BasalSpineDensityContainer.push_back ( lAuxNode );
    BasalSpineDensityContainer.at ( 11 ).SpinesPer_um = 1.5;

    //Generate apical distribution
    lAuxNode.DistToSoma = lAuxNode.SpinesPer_um = 0;
    for ( int i = 0; i <= APICAL_INTERVALSAUX; ++i )
    {
      lAuxNode.DistToSoma += 20;
      ApicalSpineDensityContainer.push_back ( lAuxNode );
    }



    //Normalize distance
    for ( int i = 0; i <= APICAL_INTERVALSAUX; ++i )
    {
      ApicalSpineDensityContainer[i].DistToSomaNormalized = ( float ) ApicalSpineDensityContainer[i].DistToSoma
        /( float ) ApicalSpineDensityContainer[APICAL_INTERVALSAUX].DistToSoma;
    }

    ApicalSpineDensityContainer.at ( 0 ).SpinesPer_um = 0;    //20
    ApicalSpineDensityContainer.at ( 1 ).SpinesPer_um = 0.1;    //40
    ApicalSpineDensityContainer.at ( 2 ).SpinesPer_um = 0.2;    //60
    ApicalSpineDensityContainer.at ( 3 ).SpinesPer_um = 0.5;    //80
    ApicalSpineDensityContainer.at ( 4 ).SpinesPer_um = 1.1;    //100
    ApicalSpineDensityContainer.at ( 5 ).SpinesPer_um = 1.5;    //120
    ApicalSpineDensityContainer.at ( 6 ).SpinesPer_um = 1.8;    //140
    ApicalSpineDensityContainer.at ( 7 ).SpinesPer_um = 2.2;    //160
    ApicalSpineDensityContainer.at ( 8 ).SpinesPer_um = 2.3;    //180
    ApicalSpineDensityContainer.at ( 9 ).SpinesPer_um = 2.1;    //200
    ApicalSpineDensityContainer.at ( 10 ).SpinesPer_um = 1.9;    //220
    ApicalSpineDensityContainer.at ( 11 ).SpinesPer_um = 1.8;    //240
    ApicalSpineDensityContainer.at ( 12 ).SpinesPer_um = 1.5;    //260
    ApicalSpineDensityContainer.at ( 13 ).SpinesPer_um = 1.3;    //280
    ApicalSpineDensityContainer.at ( 14 ).SpinesPer_um = 1.35;    //300
    ApicalSpineDensityContainer.at ( 15 ).SpinesPer_um = 1.25;    //320
    ApicalSpineDensityContainer.at ( 16 ).SpinesPer_um = 1.20;    //340
    ApicalSpineDensityContainer.at ( 17 ).SpinesPer_um = 1.22;    //360
    ApicalSpineDensityContainer.at ( 18 ).SpinesPer_um = 0.9;    //380
    ApicalSpineDensityContainer.at ( 19 ).SpinesPer_um = 0.8;    //400
    ApicalSpineDensityContainer.at ( 20 ).SpinesPer_um = 1;    //420
    ApicalSpineDensityContainer.at ( 21 ).SpinesPer_um = 0.8;    //440
    ApicalSpineDensityContainer.at ( 22 ).SpinesPer_um = 0.8;

    //Nodo muy alejado para impedir que la evaluaci�n del interfalo se salga de rando
    //La tomamos constante a partir de este punto
    lAuxNode.DistToSoma = 5000;
    ApicalSpineDensityContainer.push_back ( lAuxNode );
    ApicalSpineDensityContainer.at ( 23 ).SpinesPer_um = 0.8;

    //Por si se quiere reducir
    /*
    if (true)
    {
      for (int k=0;k<BasalSpineDensityContainer.size();++k)
        BasalSpineDensityContainer[k].SpinesPer_um/=5.0;
      for (int k=0;k<ApicalSpineDensityContainer.size();++k)
        ApicalSpineDensityContainer[k].SpinesPer_um/=5.0;
    }
    */

    initrand ( );

    //Posicionamos spinas s�lo si no es el axon.
    if (( mSWCImporter->getDendritics ( )[pIdDendritic].type != DendriticType::AXON ))
    {
      lDendriticType = mSWCImporter->getDendritics ( )[pIdDendritic].type;

      //Recooremos todos los segmentos/nodos que componen la dendrita
      //Se ha de accediendo del padre al hijo
      for ( unsigned int i = lDendriticNodeIni; i <= lDendriticNodeFin; ++i )
      {
        lParentId = mSWCImporter->getElementAt ( i ).parent;
        unsigned int lNodeId = mSWCImporter->getElementAt ( i ).id;


        //Calculamos la direccion del segmento donde ha caido
        lDirection = mSWCImporter->getElementAt ( lNodeId ).position
          - mSWCImporter->getElementAt ( lParentId ).position;
        lDirection.normalize ( );


        //Calcular distancia desde el centro del soma a los nodos y la distancia del segmento
        lIniNodeDistanceToSoma = mSWCImporter->getElementAt ( lParentId ).mDistanceToSoma;
        lFinNodeDistanceToSoma = mSWCImporter->getElementAt ( lNodeId ).mDistanceToSoma;
        lSegmentDistance = lFinNodeDistanceToSoma - lIniNodeDistanceToSoma;


        //Determinar el n�mero de espinas a posicionar en el segmento
        //Calcular la media del segmento (de momento no acumulativo)
        //Calcular intervalo de valores del nodo inicial
        unsigned int k = 0;
        bool lFlagEncontrado = false;
        std::vector <SpineDenNode2> *lContainerOfIntervals;

        if ( mSWCImporter->getDendritics ( )[pIdDendritic].type == DendriticType::BASAL )
          lContainerOfIntervals = &BasalSpineDensityContainer;
        else
          lContainerOfIntervals = &ApicalSpineDensityContainer;

        float lAux = mSWCImporter->getElementAt ( lNodeId ).mDistanceToSoma;

        unsigned int lInitialDistribDivision = 0;
        //#### Cuidado con el +1, se sale de rango
        if ( mSWCImporter->getElementAt ( lParentId ).mDistanceToSoma >= lContainerOfIntervals->at ( k ).DistToSoma )
        {
          while (( !lFlagEncontrado ) && ( k < lContainerOfIntervals->size ( )))
          {
            if (
              ( mSWCImporter->getElementAt ( lParentId ).mDistanceToSoma >= lContainerOfIntervals->at ( k ).DistToSoma )
                && ( mSWCImporter->getElementAt ( lParentId ).mDistanceToSoma
                  <= lContainerOfIntervals->at ( k + 1 ).DistToSoma )
              )
            {
              lFlagEncontrado = true;
            }
            else
              ++k;
          }
        }
        //else
        //{
//					lInitialDistribDivision=lContainerOfIntervals->size()-2;
//				}
        lInitialDistribDivision = k;

        unsigned int lFinalDistribDivision;
        if ( lInitialDistribDivision < lContainerOfIntervals->size ( ))
        {
          lFlagEncontrado = false;
          if ( mSWCImporter->getElementAt ( lNodeId ).mDistanceToSoma >= lContainerOfIntervals->at ( k ).DistToSoma )
          {
            while (( !lFlagEncontrado ) && ( k < lContainerOfIntervals->size ( )))
            {
              if (
                ( mSWCImporter->getElementAt ( lNodeId ).mDistanceToSoma >= lContainerOfIntervals->at ( k ).DistToSoma )
                  && ( mSWCImporter->getElementAt ( lNodeId ).mDistanceToSoma
                    <= lContainerOfIntervals->at ( k + 1 ).DistToSoma )
                )
              {
                lFlagEncontrado = true;
              }
              else
                ++k;
            }
          }
          lFinalDistribDivision = k;
        }
        else
        {
          lInitialDistribDivision = lFinalDistribDivision;
        }

        //Caso de ambos nodos en el mismo intervalo de gr�fica

        if ( lInitialDistribDivision == lFinalDistribDivision )
          if ( lInitialDistribDivision < ( lContainerOfIntervals->size ( ) - 1 ))
            ++lFinalDistribDivision;

        float lMediaDeEspinas = 0;

        /* //original
        float lDiffDistance = lContainerOfIntervals->at(lFinalDistribDivision).DistToSoma
                  - lContainerOfIntervals->at(lInitialDistribDivision).DistToSoma;

        float lValues = lContainerOfIntervals->at(lFinalDistribDivision).SpinesPer_um
                  - lContainerOfIntervals->at(lInitialDistribDivision).SpinesPer_um;

        //Calcular media de espinas para el nodo1
        float lLocalSpinesPer_umN1 = ( (mSWCImporter->getElementAt(lParentId).mDistanceToSoma - lContainerOfIntervals->at(lInitialDistribDivision).DistToSoma)
                      * (lValues)
                      )/lDiffDistance ;

        //Calcular media de espinas para el nodo2
        float lLocalSpinesPer_umN2 = ( (mSWCImporter->getElementAt(lNodeId).mDistanceToSoma - lContainerOfIntervals->at(lFinalDistribDivision).DistToSoma)
                      * (lValues)
                      )/lDiffDistance ;

        */

        //float lDiffDistance = lContainerOfIntervals->at(lFinalDistribDivision).DistToSoma
        //					- lContainerOfIntervals->at(lInitialDistribDivision).DistToSoma;

        //float lValues = lContainerOfIntervals->at(lFinalDistribDivision).SpinesPer_um
        //					- lContainerOfIntervals->at(lInitialDistribDivision).SpinesPer_um;
        //
        ////Calcular media de espinas para el nodo1
        //float lLocalSpinesPer_umN1 = ( (mSWCImporter->getElementAt(lParentId).mDistanceToSoma - lContainerOfIntervals->at(lInitialDistribDivision).DistToSoma)
        //							* (lValues)
        //							)
        //							///lDiffDistance
        //							;

        ////Calcular media de espinas para el nodo2
        //float lLocalSpinesPer_umN2 = ( (mSWCImporter->getElementAt(lNodeId).mDistanceToSoma - lContainerOfIntervals->at(lFinalDistribDivision).DistToSoma)
        //							* (lValues)
        //							)
        //							///lDiffDistance
        //							;



        //Sumar ambos en lMediaDeEspinas
        //lMediaDeEspinas = abs(abs(lLocalSpinesPer_umN1) + abs(lLocalSpinesPer_umN2));
        //lMediaDeEspinas = abs(abs(lLocalSpinesPer_umN1) + abs(lLocalSpinesPer_umN2))/2.0;
        lMediaDeEspinas = ( lContainerOfIntervals->at ( lInitialDistribDivision ).SpinesPer_um
          + lContainerOfIntervals->at ( lFinalDistribDivision ).SpinesPer_um )/2.0
          //+ lValues
          ;

        ////Por si hay que sumar nodos intermedios
        //if ( (lFinalDistribDivision-lInitialDistribDivision) > 1)
        //	for (int j=lInitialDistribDivision;j<lFinalDistribDivision;++j) lMediaDeEspinas+=COntainerDeINtervalos[j];


        //Se calcula la media de espinas (acumular los divisores que sean)
        //lMediaDeEspinas/=2.0;
        //lMediaDeEspinas*=2.0;

        //float lTestValue = abs(ceil( (lSegmentDistance/lMediaDeEspinas) - 0.5)  );
        //float lTestValue = abs(ceil( (lSegmentDistance/lMediaDeEspinas))  );
        float lTestValue = abs ( ceil (( lSegmentDistance*lMediaDeEspinas )));

        lNumSpinesInSegment = ( int ) lTestValue;
        //lNumSpinesInSegment = 10;

        float lAdvanceSpine = lSegmentDistance/lNumSpinesInSegment;

        //if (lNumSpinesInSegment>50) return;

        //Posicionar y orientar espinas
        lNodeSpinesDistrib.mSpinesSegmentContainer.clear ( );

        /*
        float lSpineDesp[10];
        for (int k=0;k<lNumSpinesInSegment;++k)
        {
          //lSpineDesp[k]=randfloat(lSegmentDistance);;
          lSpineDesp[k]=k*(lSegmentDistance/(float)lNumSpinesInSegment);
        }
        */
        if (( lNumSpinesInSegment > 0 )) //(lNumSpinesInSegment<50) &&
          for ( unsigned int j = 1; j <= lNumSpinesInSegment; ++j )
          {
            //Reset the Disposition
            lSpinesPosOri.mPosition = OpenMesh::Vec3f ( 0, 0, 0 );
            lSpinesPosOri.mOrientation = OpenMesh::Vec3f ( 0, 0, 0 );

            //Se calcula la posici�n
            lAuxPos = mSWCImporter->getElementAt ( lParentId ).position
              //+ ( (lDirection )* lMediaDeEspinas * j)
              + (( lDirection )*lAdvanceSpine*j );

            //Calculate the orientation of the spine
            lAuxVec = OpenMesh::Vec3f ( randfloat ( -1, 1 ), randfloat ( -1, 1 ), randfloat ( -1, 1 ));
            lAuxVec.normalize_cond ( );

            //El producto vectorial (cross) nos da un vector normal a ambos
            lAuxVec = OpenMesh::cross ( lAuxPos, lAuxVec );
            lAuxVec.normalize_cond ( );

            //Calculo final de la orientaci�n y la posici�n
            lSpinesPosOri.mPosition = lAuxPos;
            lSpinesPosOri.mOrientation = lAuxVec;

            //Add actual spine
            lNodeSpinesDistrib.mSpinesSegmentContainer.push_back ( lSpinesPosOri );
          }

        lNodeSpinesDistrib.mNodeFin = lNodeId;
        lNodeSpinesDistrib.mNodeIni = lParentId;
        lNodeSpinesDistrib.mNumSpines = lNumSpinesInSegment;

        //Add all the spines for this node
        mSpinesDistributionContainer.push_back ( lNodeSpinesDistrib );
      }
    }

  }



  //////////////////////////////////////////////////////////////////////////////////////////////////////////////

  const std::vector <NodeSpinesDistrib> &SWCSpinesDistributor::getSpinesDistributionContainer ( ) const
  {
    return mSpinesDistributionContainer;
  }

  void SWCSpinesDistributor::exportToFile ( std::string pPath )
  {

  }

  float SWCSpinesDistributor::nextSpinePosition ( float pMinDesp, float pIncFactor, unsigned int pIndex )
  {
    float result;
    result = pMinDesp + pIncFactor*( pIndex*pIndex );
    //result = pMinDesp + pIncFactor*(pIndex*(pIndex/2.0));
    //result = 2*pMinDesp;
    return result;
  }

  //!( ( lAuxPInc < mSWCImporter->getElementAt(lAuxNodeIni).mDendriticDistanceReal )

  //void SWCSpinesDistributor::exportSpinesInfo(string fileName )
  //{
  //	std::ofstream outputFileTXT;
  //	string fileTXT = fileName + ".txt";

  //	//Clean files
  //	std::ofstream(fileTXT.c_str());

  //	//Open files to add data
  //	outputFileTXT.open(fileTXT.c_str(), std::ios::app);

  //	//Head of the file
  //	//File format version
  //	outputFileTXT  << "#Spines of " <<fileTXT.c_str() <<endl;

  //	//No version
  //	//outputFileTXT  << 0.01 <<endl;

  //	//Total of neurons
  //	outputFileTXT  << mSpinesInfo.size() <<endl;

  //	SpineInfo lSpineInfo;
  //	float lAuxPosX,lAuxPosY,lAuxPosZ;


  //	for (int i=0;i<mSpinesInfo.size();++i)
  //	{
  //		lSpineInfo = mSpinesInfo.at(i);

  //		//Text exit
  //		outputFileTXT	<< lSpineInfo.mPosition[0] <<" " << lSpineInfo.mPosition[1] <<" " << lSpineInfo.mPosition[2] <<" "
  //						<< lSpineInfo.mOrientation[0] <<" " << lSpineInfo.mOrientation[1] <<" " << lSpineInfo.mOrientation[2] <<" "
  //						<< lSpineInfo.Id <<" "
  //						<< endl;

  //	}
  //	outputFileTXT.close();
  //}

  //void SWCSpinesDistributor::importSpinesInfo(string fileName)
  //{
  //	mSpinesInfo.clear();

  //	std::ifstream inputFileTXT;
  //	inputFileTXT.open(fileName.c_str(), std::ios::in);
  //	//if (inputFileTXT.fail())
  //	if (!inputFileTXT)
  //	{
  //		cerr << "An error occurred. Unable to read input file." << fileName << endl;
  //		exit(1);
  //	}

  //	SpineInfo lSpineInfo;

  //	char str[2000];
  //	char coment;
  //	//Read all the header
  //	bool continueReading=true;
  //	while (continueReading)
  //	{
  //		inputFileTXT.get(coment);
  //		if (coment!='#')	continueReading = false;
  //		else
  //		{
  //			inputFileTXT.getline(str,2000);
  //		}
  //	}

  //	//Node readed
  //	int pos=0;

  //	if (coment!=' ')
  //	{
  //		++pos;
  //
  //		inputFileTXT >> lSpineInfo.mPosition[0];
  //		inputFileTXT >> lSpineInfo.mPosition[1];
  //		inputFileTXT >> lSpineInfo.mPosition[2];
  //		inputFileTXT >> lSpineInfo.mOrientation[0];
  //		inputFileTXT >> lSpineInfo.mOrientation[1];
  //		inputFileTXT >> lSpineInfo.mOrientation[2];
  //		inputFileTXT >> lSpineInfo.Id;

  //		mSpinesInfo.push_back(lSpineInfo);
  //	}

  //	while (!inputFileTXT.eof())
  //	{
  //		++pos;
  //
  //		inputFileTXT >> lSpineInfo.mPosition[0];
  //		inputFileTXT >> lSpineInfo.mPosition[1];
  //		inputFileTXT >> lSpineInfo.mPosition[2];
  //		inputFileTXT >> lSpineInfo.mOrientation[0];
  //		inputFileTXT >> lSpineInfo.mOrientation[1];
  //		inputFileTXT >> lSpineInfo.mOrientation[2];
  //		inputFileTXT >> lSpineInfo.Id;

  //		mSpinesInfo.push_back(lSpineInfo);
  //	}

  //	//Remove the last element if it is replicated
  //	int tam = mSpinesInfo.size();
  //	lSpineInfo = mSpinesInfo[tam-1];
  //	//if ( (tam-1)!=lSpineInfo.id )	lSpineInfo.pop_back();

  //	//Close the file
  //	inputFileTXT.close();

  //}
  //	&& ( lAuxPInc > mSWCImporter->getElementAt(lAuxNodeFin).mDendriticDistanceReal )
  //  )
  //&& (!lForceStop)

}
