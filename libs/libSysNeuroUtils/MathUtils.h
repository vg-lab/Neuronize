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
 * @MathUtils
 * @MathUtils.h
 * @author Juan Pedro Brito M�ndez
 * @version 0.1
 */

#ifndef MATHUTILS_H_
#define MATHUTILS_H_

#include "defs.h"

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>

/**
* Utilidades matematicas genericas (Interpolaci�n, transformaciones ...)
* @author Juan Pedro Brito M�ndez
* @version 0.1
*/

/**
 * Realiza una interpolacion simple:
 * 	Dependiendo del discretizador nos quedamos con la correspondiente altura
 * 		Si discretizador == 0 -> Nos quedamos con la altura 1
 * 		Si discretizador == 1 -> Nos quedamos con la altura 2
 * 		En otro caso, nos quedamos con el valor de la interpolacion
 */
REAL Interpolacion ( REAL altura1, REAL altura2, REAL discretizador );

/**
 * Realiza una interpolacion lineal:
 * 	Dependiendo del discretizador nos quedamos con la correspondiente altura
 * 		Si discretizador == 0 -> Nos quedamos con la altura 1
 * 		Si discretizador == 1 -> Nos quedamos con la altura 2
 * 		En otro caso, nos quedamos con el valor de la interpolacion
 */
REAL InterpolacionLineal ( REAL altura1, REAL altura2, REAL discretizador );

/**
 * Realiza una interpolacion cosenoidal a partir de los valores suministrados
 */
REAL InterpolacionCosenoidal ( REAL altura1, REAL altura2 );

/**
 * Realiza una interpolacion cubica a partir de los valores suministrados
 */
REAL InterpolacionCubica ( REAL v0, REAL v1, REAL v2, REAL v3, REAL x );

//Calc the cross product between two vectors
boost::numeric::ublas::vector < float > cross_prod ( boost::numeric::ublas::vector < float > vecA,
                                                     boost::numeric::ublas::vector < float > vecB );

//Generate the transformation matrix for the vertices
void generateTransformationMatrix ( boost::numeric::ublas::matrix < float > &TransMatrix,
                                    const boost::numeric::ublas::vector < float > &DespVector,
                                    const boost::numeric::ublas::vector < float > &TransVector,
                                    boost::numeric::ublas::vector < float > &glb_TransVectorAux,
                                    boost::numeric::ublas::vector < float > &glb_RightVector,
                                    boost::numeric::ublas::vector < float > &glb_UpVector,
                                    boost::numeric::ublas::vector < float > &XVector,
                                    boost::numeric::ublas::vector < float > &YVector
);

bool parallelVectors ( const boost::numeric::ublas::vector < float > &VecA,
                       const boost::numeric::ublas::vector < float > &VecB );

void generateSquareUniformScaleMatrix ( boost::numeric::ublas::matrix < float > &pScaleMatrix,
                                        unsigned int pDimMatrix,
                                        float pScaleFactor );

void generateSquareRotationMatrix ( boost::numeric::ublas::matrix < float > &pRotationMatrix,
                                    float pAngRotX,
                                    float pAngRotY,
                                    float pAngRotZ
);

void generateSquareTraslationMatrix ( boost::numeric::ublas::matrix < float > &pTraslationMatrix,
                                      float pTrasX,
                                      float pTrasY,
                                      float pTrasZ
);

#endif /*MATHUTILS_H_*/
