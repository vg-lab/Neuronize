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

#include "MathUtils.h"

REAL Interpolacion ( REAL altura1, REAL altura2, REAL discretizador )
{
  return altura1 + ( altura2 - altura1 )*discretizador*discretizador*( 3 - 2*discretizador );
}

REAL InterpolacionLineal ( REAL altura1, REAL altura2, REAL discretizador )
{
  return altura1*( 1 - discretizador ) + altura2*discretizador;
}

REAL InterpolacionCosenoidal ( REAL altura1, REAL altura2 )
{
  float ft = altura1*PI;
  float f = ( 1 - cos ( ft ))*.5;

  return ( altura1*( 1 - f ) + altura2*f );
}

REAL InterpolacionCubica ( REAL v0, REAL v1, REAL v2, REAL v3, REAL x )
{
  float P = ( v3 - v2 ) - ( v0 - v1 );
  float Q = ( v0 - v1 ) - P;
  float R = v2 - v0;
  float S = v1;

  return (( P*x*x*x ) + ( Q*x*x ) + ( R*x ) + S );
}

//PENDIENTE: Simplificar esta parte del algoritmo
bool parallelVectors ( const boost::numeric::ublas::vector < float > &VecA,
                       const boost::numeric::ublas::vector < float > &VecB )
{

  float auxResult = 0;
  boost::numeric::ublas::vector < float > glb_ZeroVec;
  glb_ZeroVec.resize ( 3 );
  glb_ZeroVec[0] = glb_ZeroVec[1] = glb_ZeroVec[2] = 0;


  //use TOLERANCIA instead hardcoded number ;P ... but we need velocity
  if ( !(( fabs ( VecA[0] - glb_ZeroVec[0] ) < 0.001 )
    && ( fabs ( VecA[1] - glb_ZeroVec[1] ) < 0.001 )
    && ( fabs ( VecA[2] - glb_ZeroVec[2] ) < 0.001 )
  )
    &&
      !(( fabs ( VecB[0] - glb_ZeroVec[0] ) < 0.001 )
        && ( fabs ( VecB[1] - glb_ZeroVec[1] ) < 0.001 )
        && ( fabs ( VecB[2] - glb_ZeroVec[2] ) < 0.001 )
      )
    )
    auxResult = ( inner_prod ( VecA, VecB ))/( norm_2 ( VecA )*norm_2 ( VecB ));

  auxResult = sqrt ( 1. - ( auxResult*auxResult ));

  if ( fabs ( auxResult ) < 0.001 )
    return true;
  else
    return false;

}

boost::numeric::ublas::vector < float > cross_prod ( boost::numeric::ublas::vector < float > vecA,
                                                     boost::numeric::ublas::vector < float > vecB )
{
  boost::numeric::ublas::vector < float > result ( 3 );
  result[0] = ( vecA[1]*vecB[2] ) - ( vecA[2]*vecB[1] );
  result[1] = ( vecA[2]*vecB[0] ) - ( vecA[0]*vecB[2] );
  result[2] = ( vecA[0]*vecB[1] ) - ( vecA[1]*vecB[0] );
  //result[3] = 0;
  return result;
}

//Generate the transformation matrix for the vertices
void generateTransformationMatrix ( boost::numeric::ublas::matrix < float > &TransMatrix,
                                    const boost::numeric::ublas::vector < float > &DespVector,
                                    const boost::numeric::ublas::vector < float > &TransVector,
                                    boost::numeric::ublas::vector < float > &glb_TransVectorAux,
                                    boost::numeric::ublas::vector < float > &glb_RightVector,
                                    boost::numeric::ublas::vector < float > &glb_UpVector,
                                    boost::numeric::ublas::vector < float > &XVector,
                                    boost::numeric::ublas::vector < float > &YVector
)
{

  namespace ublas = boost::numeric::ublas;

  //Forward vector normalization
  float mod = ublas::norm_2 ( TransVector );
  glb_TransVectorAux = TransVector*( 1.0/mod );

  //Right vector
  glb_RightVector = cross_prod ( glb_TransVectorAux, YVector );

  //Up vector
  glb_UpVector = cross_prod ( glb_RightVector, glb_TransVectorAux );


  //Case parallel vectors
  if ( parallelVectors ( glb_TransVectorAux, YVector ))
  {
    //Calculate the other vectors
    //Right vector
    glb_UpVector = cross_prod ( XVector, glb_TransVectorAux );

    //Right vector
    glb_RightVector = cross_prod ( glb_TransVectorAux, glb_UpVector );
  }


  //Up vector
  mod = ublas::norm_2 ( glb_UpVector );
  TransMatrix ( 0, 2 ) = glb_UpVector[0]/mod;
  TransMatrix ( 1, 2 ) = glb_UpVector[1]/mod;
  TransMatrix ( 2, 2 ) = glb_UpVector[2]/mod;

  //Forward vector
  TransMatrix ( 0, 1 ) = glb_TransVectorAux[0];
  TransMatrix ( 1, 1 ) = glb_TransVectorAux[1];
  TransMatrix ( 2, 1 ) = glb_TransVectorAux[2];

  //Right vector
  mod = ublas::norm_2 ( glb_RightVector );
  TransMatrix ( 0, 0 ) = glb_RightVector[0]/mod;
  TransMatrix ( 1, 0 ) = glb_RightVector[1]/mod;
  TransMatrix ( 2, 0 ) = glb_RightVector[2]/mod;

  //Homogeneus Component
  TransMatrix ( 0, 3 )
    = TransMatrix ( 1, 3 )
    = TransMatrix ( 2, 3 )
    = 0;

  TransMatrix ( 3, 0 )
    = TransMatrix ( 3, 1 )
    = TransMatrix ( 3, 2 )
    = 0;

  TransMatrix ( 3, 3 ) = 1.0;

  /*
  //Traslate transformation
  //--->>>Is directly applied adding the ini position (more faster tha this transformation)
  ublas::matrix<float> MoveMatrix(4,4);
  for (unsigned i = 0; i < MoveMatrix.size1(); ++ i)
  {
    for (unsigned j = 0; j < MoveMatrix.size2 (); ++ j)
    {
      if ( i==j )	MoveMatrix(i, j) = 1;
      else		MoveMatrix(i, j) = 0;
    }
  }

  MoveMatrix(0,3) = DespVector[0];
  MoveMatrix(1,3) = DespVector[1];
  MoveMatrix(2,3) = DespVector[2];

  TransMatrix = prod(MoveMatrix, TransMatrix);
  */
}

void generateSquareUniformScaleMatrix ( boost::numeric::ublas::matrix < float > &pScaleMatrix,
                                        unsigned int pDimMatrix,
                                        float pScaleFactor )
{
  for ( unsigned int i = 0; i < pDimMatrix; ++i )
    for ( unsigned int j = 0; j < pDimMatrix; ++j )
    {
      pScaleMatrix ( i, j ) = 0;
    }

  for ( unsigned int i = 0; i < pDimMatrix; ++i )
  {
    pScaleMatrix ( i, i ) = pScaleFactor;
  }

  pScaleMatrix (( pDimMatrix - 1 ), ( pDimMatrix - 1 )) = 1;
}

void generateSquareRotationMatrix ( boost::numeric::ublas::matrix < float > &pRotationMatrix,
                                    float pAngRotX,
                                    float pAngRotY,
                                    float pAngRotZ
)
{
  //pRotationMatrix(0,0) =(cos(pAngRotY)*cos(pAngRotZ));	pRotationMatrix(0,1) =( (-cos(pAngRotX)*sin(pAngRotZ))+(sin(pAngRotX)*sin(pAngRotY)*cos(pAngRotZ)) );	pRotationMatrix(0,2) =((sin(pAngRotX)*sin(pAngRotZ))+(cos(pAngRotX)*sin(pAngRotY)*cos(pAngRotZ)));	pRotationMatrix(0,3) =0;
  //pRotationMatrix(1,0) =(cos(pAngRotY)*sin(pAngRotZ));	pRotationMatrix(1,1) =( (cos(pAngRotX)*cos(pAngRotZ)) + (sin(pAngRotX)*sin(pAngRotY)*sin(pAngRotZ)) );	pRotationMatrix(1,2) =((-sin(pAngRotX)*cos(pAngRotZ))+(cos(pAngRotX)*sin(pAngRotY)*cos(pAngRotZ)));	pRotationMatrix(1,3) =0;
  //pRotationMatrix(2,0) =(-sin(pAngRotY));					pRotationMatrix(2,1) =(sin(pAngRotX)*cos(pAngRotY));													pRotationMatrix(2,2) =(cos(pAngRotX)*cos(pAngRotY));												pRotationMatrix(2,3) =0;
  //pRotationMatrix(3,0) =0;								pRotationMatrix(3,1) =0;																				pRotationMatrix(3,2) =0;																			pRotationMatrix(3,3) =1;

  //Por partes
  namespace ublas      = boost::numeric::ublas;
  ublas::matrix < float > lRotMatrixX ( 4, 4 );
  ublas::matrix < float > lRotMatrixY ( 4, 4 );
  ublas::matrix < float > lRotMatrixZ ( 4, 4 );

  ublas::matrix < float > lRotAuxXY ( 4, 4 );

  for ( int i = 0; i < 4; ++i )
  {
    for ( int j = 0; j < 4; ++j )
    {
      lRotMatrixX ( i, j ) = lRotMatrixY ( i, j ) = lRotMatrixZ ( i, j ) = 0;
    }
  }

  //http://www.cprogramming.com/tutorial/3d/rotationMatrices.html

  //Left Hand System
  //Rotation matrix X
  lRotMatrixX ( 0, 0 ) = lRotMatrixX ( 3, 3 ) = 1;
  lRotMatrixX ( 1, 1 ) = cos ( pAngRotX );
  lRotMatrixX ( 1, 2 ) = -sin ( pAngRotX );
  lRotMatrixX ( 2, 1 ) = sin ( pAngRotX );
  lRotMatrixX ( 2, 2 ) = cos ( pAngRotX );

  //Rotation matrix Y
  lRotMatrixY ( 1, 1 ) = lRotMatrixY ( 3, 3 ) = 1;
  lRotMatrixY ( 0, 0 ) = cos ( pAngRotY );
  lRotMatrixY ( 0, 2 ) = sin ( pAngRotY );
  lRotMatrixY ( 2, 0 ) = -sin ( pAngRotY );
  lRotMatrixY ( 2, 2 ) = cos ( pAngRotY );

  //Rotation matrix Z
  lRotMatrixZ ( 2, 2 ) = lRotMatrixZ ( 3, 3 ) = 1;
  lRotMatrixZ ( 0, 0 ) = cos ( pAngRotZ );
  lRotMatrixZ ( 0, 1 ) = -sin ( pAngRotZ );
  lRotMatrixZ ( 1, 0 ) = sin ( pAngRotZ );
  lRotMatrixZ ( 1, 1 ) = cos ( pAngRotZ );

  ////Right Hand System
  ////Rotation matrix X
  //lRotMatrixX(0,0) = lRotMatrixX(3,3) = 1;
  //lRotMatrixX(1,1) = cos(pAngRotX); lRotMatrixX(1,2) = sin(pAngRotX);
  //lRotMatrixX(2,1) = -sin(pAngRotX); lRotMatrixX(2,2) = cos(pAngRotX);

  ////Rotation matrix Y
  //lRotMatrixY(1,1) = lRotMatrixY(3,3) = 1;
  //lRotMatrixY(0,0) = cos(pAngRotY); lRotMatrixY(0,2)  = -sin(pAngRotY);
  //lRotMatrixY(2,0) = sin(pAngRotY); lRotMatrixY(2,2) = cos(pAngRotY);

  ////Rotation matrix Z
  //lRotMatrixZ(2,2) = lRotMatrixZ(3,3) = 1;
  //lRotMatrixZ(0,0) = cos(pAngRotZ); lRotMatrixZ(0,1) = sin(pAngRotZ);
  //lRotMatrixZ(1,0) = -sin(pAngRotZ); lRotMatrixZ(1,1) = cos(pAngRotZ);

  lRotAuxXY = prod ( lRotMatrixX, lRotMatrixY );
  pRotationMatrix = prod ( lRotAuxXY, lRotMatrixZ );

}

void generateSquareTraslationMatrix ( boost::numeric::ublas::matrix < float > &pTraslationMatrix,
                                      float pTrasX,
                                      float pTrasY,
                                      float pTrasZ
)
{
  //Por partes
  namespace ublas      = boost::numeric::ublas;
  ublas::matrix < float > lTrasMatrix ( 4, 4 );

  for ( int i = 0; i < 4; ++i )
  {
    for ( int j = 0; j < 4; ++j )
    {
      if ( i == j )
        lTrasMatrix ( i, j ) = 1;
      else
        lTrasMatrix ( i, j ) = 0;
    }
  }

  lTrasMatrix ( 0, 3 ) = pTrasX;
  lTrasMatrix ( 1, 3 ) = pTrasY;
  lTrasMatrix ( 2, 3 ) = pTrasZ;

  //lTrasMatrix(3,0)=pTrasX;
  //lTrasMatrix(3,1)=pTrasY;
  //lTrasMatrix(3,2)=pTrasZ;

  pTraslationMatrix = lTrasMatrix;
}





