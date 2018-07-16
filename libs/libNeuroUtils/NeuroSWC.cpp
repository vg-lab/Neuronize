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

/*
Ojo, la variable mTessellationControl introduce unos v�rtices de control para el shader pnTriangles
para indexar de manera correcta se ha de desactivar.
*/

#include <iostream>
#include <algorithm>

#include "NeuroSWC.h"
#include <libs/libSysNeuroUtils/defs.h>

#define NEURODESP    1
#define RANDRANGE    1.0
#define HOLENUMVERTEX  3
#define PERCENTEXTTUBE  0.0
#define SWE_VER      0
#define VERTRATESOMASEW  2.0
#define ANGBADSEW    145.0

namespace NSNeuroSWC
{

  NeuroSWC::NeuroSWC ( ): BaseMesh ( )
  {
    HResolTube = 0;
    VResolTube = 0;
    HResolSoma = 0;
    VResolSoma = 0;

    fileName = "empty";

    importer = NULL;

    mSomaId = -1;
    mSomaNumFaces = 0;

    mPiramidalSoma = false;

    glb_tubeVHandle = NULL;
    importer = NULL;
    glb_meshVHandle = NULL;
    mSomaContainer = NULL;

    mSharpEnd = true;

    //mTessellControlSew	=	true;
    mTessellControlSew = false;
  }

  NeuroSWC::NeuroSWC ( string SWCName, int horResolTube, int verResolTube, int horResolSoma, int verResolSoma )
  {
    mPiramidalSoma = false;

    glb_tubeVHandle = NULL;
    importer = NULL;
    glb_meshVHandle = NULL;
    mSomaContainer = NULL;

    generateNeuron ( SWCName, horResolTube, verResolTube, horResolSoma, verResolSoma );
  }

  NeuroSWC::~NeuroSWC ( )
  {
    if ( glb_tubeVHandle != NULL )
      delete[] glb_tubeVHandle;
    //if (glb_meshVHandle != NULL) delete [] glb_meshVHandle;
    if ( importer != NULL )
      delete importer;
    //if (mSomaContainer	!= NULL) delete mSomaContainer;

  }

  //Add a spheric object to model
  void NeuroSWC::generateSphere ( const OpenMesh::Vec3f &center, float radio, int width, int height )
  {
    float theta, phi;
    int i, j, t;

    const int nvec = ( height - 2 )*width + 2;

    MeshDef::VertexHandle *vhandle = new MeshDef::VertexHandle[nvec];

    for ( t = 0, j = 1; j < height - 1; j++ )
    {
      for ( i = 0; i < width; i++ )
      {
        theta = float ( j )/( height - 1 )*PI;
        phi = float ( i )/( width )*PI*2;
        vhandle[t] = Mesh->add_vertex ( MeshDef::Point ( center[0] + radio*(( sinf ( theta )*cosf ( phi ))),
                                                         center[1] + radio*(( cosf ( theta ))),
                                                         center[2] + radio*(( -sinf ( theta )*sinf ( phi )))
                                        )
        );

        glb_meshVHandle[meshVHandleTotal] = vhandle[t];
        meshVHandleTotal++;

        t++;
      }
    }

    //Extreme points
    vhandle[t] = Mesh->add_vertex ( MeshDef::Point ( center[0], center[1] + radio, center[2]
                                    )
    );

    glb_meshVHandle[meshVHandleTotal] = vhandle[t];
    meshVHandleTotal++;

    t++;

    vhandle[t] = Mesh->add_vertex ( MeshDef::Point ( center[0], center[1] - radio, center[2]
                                    )
    );

    glb_meshVHandle[meshVHandleTotal] = vhandle[t];
    meshVHandleTotal++;

    t++;

    //Index faces construct
    std::vector <MeshDef::VertexHandle> face_vhandles;

    for ( t = 0, j = 0; j < height - 3; j++ )
    {
      for ( i = 0; i < width - 1; i++ )
      {
        //Uper face
        face_vhandles.clear ( );
        face_vhandles.push_back ( vhandle[(( j )*width + i )] );
        face_vhandles.push_back ( vhandle[(( j + 1 )*width + i + 1 )] );
        face_vhandles.push_back ( vhandle[(( j )*width + i + 1 )] );
        Mesh->add_face ( face_vhandles );

        ////Down face
        face_vhandles.clear ( );
        face_vhandles.push_back ( vhandle[(( j )*width + i )] );
        face_vhandles.push_back ( vhandle[(( j + 1 )*width + i )] );
        face_vhandles.push_back ( vhandle[(( j + 1 )*width + i + 1 )] );
        Mesh->add_face ( face_vhandles );
      }

      //Last Uper face
      face_vhandles.clear ( );
      face_vhandles.push_back ( vhandle[(( j )*width + i )] );
      face_vhandles.push_back ( vhandle[(( j + 1 )*width )] );
      face_vhandles.push_back ( vhandle[(( j )*width )] );
      Mesh->add_face ( face_vhandles );

      ////Last Down face
      face_vhandles.clear ( );
      face_vhandles.push_back ( vhandle[(( j )*width + i )] );
      face_vhandles.push_back ( vhandle[(( j + 1 )*width + i )] );
      face_vhandles.push_back ( vhandle[(( j + 1 )*width )] );
      Mesh->add_face ( face_vhandles );
    }

    for ( i = 0; i < width - 1; i++ )
    {
      //Superior circle
      face_vhandles.clear ( );
      face_vhandles.push_back ( vhandle[(( height - 2 )*width )] );
      face_vhandles.push_back ( vhandle[( i )] );
      face_vhandles.push_back ( vhandle[( i + 1 )] );
      Mesh->add_face ( face_vhandles );

      //Inferior circle
      face_vhandles.clear ( );
      face_vhandles.push_back ( vhandle[(( height - 2 )*width + 1 )] );
      face_vhandles.push_back ( vhandle[(( height - 3 )*width + i + 1 )] );
      face_vhandles.push_back ( vhandle[(( height - 3 )*width + i )] );
      Mesh->add_face ( face_vhandles );
    }

    //Las triangle Superior circle
    face_vhandles.clear ( );
    face_vhandles.push_back ( vhandle[(( height - 2 )*width )] );
    face_vhandles.push_back ( vhandle[( width - 1 )] );
    face_vhandles.push_back ( vhandle[( 0 )] );
    Mesh->add_face ( face_vhandles );

    //Last triangle Inferior circle
    face_vhandles.clear ( );
    face_vhandles.push_back ( vhandle[(( height - 2 )*width + 1 )] );
    face_vhandles.push_back ( vhandle[(( height - 3 )*width )] );
    face_vhandles.push_back ( vhandle[(( height - 3 )*width + ( width - 1 ))] );
    Mesh->add_face ( face_vhandles );

    delete[] vhandle;

    updateBaseMesh ( );
  }

  //Add a spheric object to model
  void NeuroSWC::generateLinearTube ( const OpenMesh::Vec3f &ini,
                                      const OpenMesh::Vec3f &fin,
                                      float radioIni,
                                      float radioFin,
                                      int nodeId,
                                      int nodeParent )
  {
    //Counters vars
    int i, j, t;
    float lPercent = 0;

    //Desp for no consecutive tubes
    float lExtraDespIni = 0;
    float lExtraDespFin = 0;

    glb_forward = ( fin - ini );

    glb_modulo = glb_forward.length ( );

    //lPercent = ( (PERCENTEXTTUBE * glb_modulo )/100.0);
    lPercent = 0;

    //Caso est�ndar, tubo q no es bifurcacion ni terminaci�n
    lExtraDespFin = lExtraDespIni = glb_modulo/( VResolTube );

    /*
		Recordar que cosemos desde el parent hasta el hijo nodeId
		*/

    //C�lculo de los desplazamientos de los  slices para el correcto cosido de las bifurcaciones
    //Los par�metros dependientes son lPercent y lExtraDespIni (Parte inicial de los tubos)

    // -> lPercetn = es el desplazamiento del Slice 0
    // -> lExtraDespIni = Es el desplazamiento del slice 1
    if ( importer->isDendriticBifurcation ( nodeParent ))
    {
      //Caso de bifurcaci�n y nodo consecutivo.
      if (( nodeId - nodeParent ) == 1 )
      {
        lPercent = 0;

        //Si e radio del parent es menor que le m�dulo del segmento, quiere decir que se puede desplazar ese radio el segundo segmento
        //Del tugo, no traspasaremos el tubo.
        if ( importer->getElementAt ( nodeParent ).radius < glb_modulo )
          lExtraDespIni = importer->getElementAt ( nodeParent ).radius;

          //Si no desplazamos todo lo posible, que es el total del bubo excepto un 10% (que es para el resto de los slices)
        else
          lExtraDespIni = glb_modulo - (( glb_modulo*10.0/100.0 ));
      }
        //Caso bifurcacion y nodo no consecutivo.
      else
      {
        //Desplazamos el nodo bifurcaci�n, el radio m�s un porcentaje que es PERCENTEXTTUBE
        //Para evitar la interpenetraci�n

        lPercent = importer->getElementAt ( nodeParent ).radius
          + (( PERCENTEXTTUBE*importer->getElementAt ( nodeParent ).radius )/100 );
        if ( lPercent < 0 )
          lPercent = 0;

        //El resto de desplazamientos, no se tendr� en cuenta el lPercent inicial

        //lExtraDespIni = glb_modulo /  (VResolTube-1);

        //El resto de slices se han de repartir en una longitud que es:
        //la longitudo del segmento - lPercet (que es el desplazamiento para sacarlo) / entre la resoluci�n - 1 ya que el uno va a ir al 0
        lExtraDespIni = ( glb_modulo - lPercent )/( VResolTube - 1 );
      }
    }
    else
    {


      //Desplazamiento inicial para lograr suavidad en la curva
      //Tiene l problema de que deja parte del volumen de la efera fuera
      //lPercent = importer->getElementAt(nodeParent).radius
      //+ ( (PERCENTEXTTUBE*importer->getElementAt(nodeParent).radius)/100)
      //;
      //lExtraDespIni = (glb_modulo /  (VResolTube-1)) ;

      //No hay desplazamiento inicial
      lPercent = 0;
      if ( importer->isDendriticBifurcation ( nodeId ))
      {
        lExtraDespIni = (( glb_modulo )/( VResolTube - 1 ));
      }
      else
      {
        lExtraDespIni = (( glb_modulo )/( VResolTube ));
      }
    }


    //Calculo del desplazamiento final
    if ( importer->isDendriticTermination ( nodeId ))
    {
      lExtraDespFin = 0;
      //Caso de 2 slices, se lleva el inicial hasta el final
      if ( VResolTube == 2 )
        lExtraDespIni *= 2;
    }
    else
    {

      //Si deseamos desplazar la parte final del tubo, el radio de la esfera a la que llegua
      /*
			if ( importer->getElementAt(nodeId).radius < glb_modulo)	lExtraDespFin =  (importer->getElementAt(nodeId).radius);
			else														lExtraDespFin =  (glb_modulo -( (glb_modulo*10.0/100.0) ) );
			*/

      /*
			Para el desplazamiento final diferenciamos entre si es un nodo bifurcacion, en cuyo casolo llevamos hasta un poco menos del radio
			o si es un nodo normal, en cuyo caso hacemos una reparticion equiespacial de los slices
			*/
      if ( importer->isDendriticBifurcation ( nodeId ))
      {
        if ( importer->getElementAt ( nodeId ).radius < glb_modulo )
          lExtraDespFin = ( importer->getElementAt ( nodeId ).radius );
        else
          lExtraDespFin = ( glb_modulo - (( glb_modulo*10.0/100.0 )));
      }
      else
      {
        //Desplazamiento final constante (no se acerca al punto de deficnion el radio de este)
        //if ( importer->getElementAt(nodeId).radius < glb_modulo)
        lExtraDespFin = ( glb_modulo/( VResolTube ));
        //else														lExtraDespFin =  (glb_modulo - ( (glb_modulo*10.0/100.0) ) );

      }
      //No contemplamos el desplazamiento final, para que al sumar 0 en el c�lculo de lso incrementos de bien
      //lExtraDespFin = 0;
    }

    /*

		//Desplazamiento inicial para los tubos no consecutivos
		//As� evitamos que se introduzcan dentro en las bifurcaciones
		if (	( (nodeParent+1)!=nodeId)
				&& (nodeParent!=mSomaId)
			)
		{
			lExtraDespIni = glb_modulo /  (VResolTube);

			//lPercent = ( (PERCENTEXTTUBE * (glb_modulo-(lExtraDespIni + lExtraDespFin)) )/100.0);

			//Esta es la mejor opcion, solo desplaza el radio dle padre, as� no hay posibilidad de que se cruze
			//Al menos eso parece.
			lPercent = importer->getElementAt(nodeParent).radius
						+ ( (PERCENTEXTTUBE*importer->getElementAt(nodeParent).radius)/100)
						;

			if (lPercent<0)	lPercent=0;

		}

		*/

    //-->>Formas de generacion de los tubos
    //1)Equidistant distance
    //glb_incremet	= glb_modulo/(VResolTube);

    //2)Generate little slices in the extremes of the tube
    glb_incremet = ( glb_modulo - ( lExtraDespIni + lExtraDespFin + lPercent ))/( VResolTube - 2 );

    //3)Generate little slices in the extrmees of the tube
    //glb_incremet	= ( glb_modulo-(2*lPercent) )/(VResolTube-1);


    glb_incRadio = ( radioFin - radioIni )/( VResolTube );

    //Calcular matriz de transformacion
    /*
			1ro:Calcular direcci�n del segmento, q conformara uno de los vectores de la matriz
			2do:pro vectorial con el eje y para sacar el vector right
				si es cero o muy proximo, utilizar el eje X
			3ro: Con otro producto vectorial obtendremos el vector up
				-> Ojo con la colocaci�n y el orden de los vectores
			--> Con esto ya esta calculada la matriz de rotaci�n
			--> Luego aplicar la traslado

			-->Multiplicar cada vertice por la matriz de transformaci�n
		*/

    //Forward vector normalized
    glb_forwardVec[0] = glb_forward[0]/glb_modulo;
    glb_forwardVec[1] = glb_forward[1]/glb_modulo;
    glb_forwardVec[2] = glb_forward[2]/glb_modulo;


    //Copy the vector for product
    glb_DespVec[0] = ini[0];
    glb_DespVec[1] = ini[1];
    glb_DespVec[2] = ini[2];


    //glb_forwardVec_1Slice = glb_forwardVec;

    generateTransformationMatrix ( glb_mat,
                                   glb_DespVec,
                                   glb_forwardVec,
                                   glb_TransVectorAux,
                                   glb_RightVector,
                                   glb_UpVector,
                                   XVector,
                                   YVector
    );

    //Generate the transformation matrix for the first slice
    ////////////////////////////////////////////////////////////////////////////////////////////////

    OpenMesh::Vec3f lPrevVector;

    if ( nodeParent != mSomaId )
    {
      if (
        ( importer->isDendriticBifurcation ( nodeParent ))
          && ( nodeId - nodeParent > 1 )
        )
      {

        //No se puede hacer una comparaci�n de cercan�a porque le tubo a�n no esta sintetizado
        /*
				int index = 0;
				int indexCandidate = 0;
				unsigned int	globalDesp	=	HResolTube*VResolTube;
				MeshDef::Point	lBariCenter(0,0,0);
				MeshDef::Point	ltmpVertex(0,0,0);
				OpenMesh::Vec3f ltmpVertexResult(0,0,0);
				float dis=0;
				MeshDef::VertexHandle   vhandle;

				//Recorremos los vertices a atar y nos quedamos con la normal del m�s cercano
				///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				//->Calcular el centro de los v�rtices del nodo
				for(j=0; j<HResolTube; j++)
				{
					//Calc the index of the vertex
					index		=	( (nodeId-NEURODESP-1)*(globalDesp) + j);
					index		+=	glb_nVerticesSoma;
					lBariCenter	+=	Mesh->points()[(index)];
				}

				lBariCenter	/=	(float)(HResolTube);

				//->Calcular el v�rtice m�s cercano del parent al centro del tubo calculado
				indexCandidate		=	( ( ((nodeParent-NEURODESP)*(globalDesp))) );
				indexCandidate		+=	glb_nVerticesSoma;

				//Utilizamos el opossite para salirnos del tubo (no estar por detr�s del tri�ngulo)
				ltmpVertexResult	=	OpenMesh::Vec3f(lBariCenter
														- Mesh->points()[(indexCandidate)]);
				dis=ltmpVertexResult.length();

				bool lNearestTest=true;
				//////////////////////////////////////////////////////////////////////////////////////////

				//Calculate the vectors
				OpenMesh::Vec3f lVecAB2	=	OpenMesh::Vec3f(
															  importer->getElementAt(nodeId).position
															  - importer->getElementAt(nodeParent).position
															);

				OpenMesh::Vec3f lVecAC2	=	OpenMesh::Vec3f(
															importer->getElementAt(nodeParent+1).position
															- importer->getElementAt(nodeParent).position
															);

				//Cross product == producto vectorial
				lVecAB2.normalize();
				lVecAC2.normalize();

				float lAuxDot2	= OpenMesh::dot(lVecAB2,lVecAC2);

				float lAuxAngle2 = acos(lAuxDot2);
				float lAuxAngleInGrads = fabs(OpenMesh::rad_to_deg(lAuxAngle2));

				if ( (lAuxAngleInGrads>ANGBADSEW)
					)
				{
					lNearestTest = false;
				}

				//////////////////////////////////////////////////////////////////////////////////////////

				//Select the nearest vertex
				if (lNearestTest)
				{
					for(j=0; j<(HResolTube); j++)
					{
						index				=	( ( ((nodeParent-NEURODESP)*(globalDesp))) + j);
						index				+=	glb_nVerticesSoma;

						ltmpVertex			=	Mesh->points()[(index)];
						ltmpVertexResult	=	OpenMesh::Vec3f(lBariCenter - ltmpVertex);

						if (ltmpVertexResult.length()< dis)
						{
							indexCandidate	=	index;
							dis				=	ltmpVertexResult.length();
						}
					}
				}
				//Select the most extern vertes
				else
				{
					for(j=0; j<(HResolTube); j++)
					{
						index				=	( ( ((nodeParent-NEURODESP)*(globalDesp))) + j);
						index				+=	glb_nVerticesSoma;

						ltmpVertex			=	Mesh->points()[(index)];
						ltmpVertexResult	=	OpenMesh::Vec3f(lBariCenter - ltmpVertex);

						if (ltmpVertexResult.length()> dis)
						{
							indexCandidate	=	index;
							dis				=	ltmpVertexResult.length();
						}
					}
				}


				//Ahora que tenemos el vertice m�s cercano

				//Select the vertex handler
				vhandle		= Mesh->vertex_handle(indexCandidate);
				lPrevVector = Mesh->normal(vhandle);
				*/

        /*
				int index = 0;
				unsigned int	globalDesp	=	HResolTube*VResolTube;
				MeshDef::Point	lBariCenter(0,0,0);

				//Recorremos los vertices a atar y nos quedamos con la normal del m�s cercano
				///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				//->Calcular el centro de los v�rtices del nodo
				for(j=0; j<HResolTube; j++)
				{
					//Calc the index of the vertex
					index		=	( (nodeParent-NEURODESP-1)*(globalDesp) + j);
					index		+=	glb_nVerticesSoma;
					lBariCenter	+=	Mesh->points()[(index)];
				}

				lBariCenter	/=	(float)(HResolTube);

				lPrevVector = importer->getElementAt(nodeId).position - lBariCenter;
				*/


        /*
				//Funciona mas o menos, pero la orientaci�n del eslice esta limitada por la geometr�a de la malla
				int index = 0;
				int indexCandidate = 0;
				unsigned int	globalDesp	=	HResolTube*VResolTube;
				MeshDef::Point	ltmpVertex(0,0,0);
				OpenMesh::Vec3f ltmpVertexResult(0,0,0);
				float dis, disAux;
				MeshDef::VertexHandle   vhandle;

				//Recorremos los vertices a atar y nos quedamos con la normal del m�s cercano
				//QUe ser� la orientaci�n del primer del slice bifurcaci�n que no sea el primer hijo
				///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

				index		=	( ( ((nodeParent-NEURODESP)*(globalDesp))) );
				index		+=	glb_nVerticesSoma;
				dis = (Mesh->points()[(index)] - importer->getElementAt(nodeId).position).length();
				lPrevVector = Mesh->vertex_normals()[index];

				for(j=1; j<HResolTube; j++)
				{
					//Calc the index of the vertex
					index		=	( (nodeParent-NEURODESP)*(globalDesp) + j);
					index		+=	glb_nVerticesSoma;
					disAux = (Mesh->points()[(index)] - importer->getElementAt(nodeId).position).length();

					if ( disAux<dis )
					{
						dis=disAux;
						//vhandle = Mesh->vertex_handle(index);
						lPrevVector = Mesh->vertex_normals()[index];
					}
				}
				*/

        /*
				//suma de todos los vectores normalizados en ese nodo
				MeshDef::Point	ltmpVertex(0,0,0);
				MeshDef::Point	ltmpVertex2(0,0,0);

				ltmpVertex =	importer->getElementAt(nodeParent).position
								- importer->getElementAt(nodeParent-1).position;
				ltmpVertex /= ltmpVertex.length();

				ltmpVertex2 =	importer->getElementAt(nodeParent+1).position
								- importer->getElementAt(nodeParent).position;
				ltmpVertex2 /= ltmpVertex2.length();

				lPrevVector = importer->getElementAt(nodeId).position
							  - importer->getElementAt(nodeParent).position;
				lPrevVector /= lPrevVector.length();

				lPrevVector = (lPrevVector+ltmpVertex+ltmpVertex2);
				*/

        /*
				OpenMesh::Vec3f ltmpVertexResult(0,0,0);
				OpenMesh::Vec3f ltmpVertexResult2(0,0,0);

				ltmpVertexResult =	importer->getElementAt(nodeParent).position
									//Menos el parent del parent
									- importer->getElementAt(importer->getElementAt(nodeParent).parent).position
									;

				ltmpVertexResult2 =	importer->getElementAt(nodeParent+1).position
									//Menos el parent del parent
									- importer->getElementAt(nodeParent).position
									;
				*/
        /*
				float lmod = ltmpVertexResult.length();
				ltmpVertexResult/=lmod;

				lmod = ltmpVertexResult2.length();
				ltmpVertexResult2/=lmod;

				lPrevVector = ltmpVertexResult
								+ ltmpVertexResult2;
				*/

        /*
				float lmod = ltmpVertexResult.length();

				boost::numeric::ublas::vector<float> ltmpVertexResultboost;
				ltmpVertexResultboost.resize(3);
				ltmpVertexResultboost[0]=ltmpVertexResult[0]/lmod;
				ltmpVertexResultboost[1]=ltmpVertexResult[1]/lmod;
				ltmpVertexResultboost[2]=ltmpVertexResult[2]/lmod;

				lmod = ltmpVertexResult2.length();

				boost::numeric::ublas::vector<float> ltmpVertexResultboost2;
				ltmpVertexResultboost2.resize(3);
				ltmpVertexResultboost2[0]=ltmpVertexResult2[0]/lmod;
				ltmpVertexResultboost2[1]=ltmpVertexResult2[1]/lmod;
				ltmpVertexResultboost2[2]=ltmpVertexResult2[2]/lmod;

				ltmpVertexResultboost+=ltmpVertexResultboost2;
				*/

        /*
				//Esto es el standart
				lPrevVector =	importer->getElementAt(nodeParent).position
								//Menos el parent del parent
								- importer->getElementAt(nodeParent+1).position
								;
				*/

        /*
				//Esto es el standart
				lPrevVector =	importer->getElementAt(nodeParent).position
								//Menos el parent del parent
								- importer->getElementAt(importer->getElementAt(nodeParent).parent).position
								;
				*/
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      }
      else
      {
        lPrevVector = importer->getElementAt ( nodeParent ).position
          //Menos el parent del parent
          - importer->getElementAt ( importer->getElementAt ( nodeParent ).parent ).position;
      }
    }
    else
    {
      lPrevVector = importer->getElementAt ( nodeId ).position;
    }

    float lmod = lPrevVector.length ( );

    glb_AuxDir_1Slice[0] = lPrevVector[0]/lmod;
    glb_AuxDir_1Slice[1] = lPrevVector[1]/lmod;
    glb_AuxDir_1Slice[2] = lPrevVector[2]/lmod;

    //Los 2 vectores a sumar est�n normaliazos (Ok)
    if (
      ( importer->isDendriticBifurcation ( nodeParent ))
        && ( nodeId - nodeParent > 1 )
      )
    {
      ////Si e suan bifurcacion no consecutiva, la direcci�n es la ponderaci�n entre la normal del vertice seleccionado
      ////y la direcci�n del segmento
      //glb_forwardVec_1Slice = glb_AuxDir_1Slice
      //						+glb_forwardVec
      //						;

      glb_forwardVec_1Slice = glb_forwardVec
        //+ glb_AuxDir_1Slice
        ;

    }
    else
    {
      glb_forwardVec_1Slice = glb_AuxDir_1Slice
        + glb_forwardVec;
    }


    //Forward vector normalization
    lmod = boost::numeric::ublas::norm_2 ( glb_forwardVec_1Slice );

    glb_forwardVec_1Slice[0] /= lmod;
    glb_forwardVec_1Slice[1] /= lmod;
    glb_forwardVec_1Slice[2] /= lmod;

    generateTransformationMatrix ( glb_mat_1Slice,
                                   glb_DespVec,
                                   glb_forwardVec_1Slice,
                                   glb_TransVectorAux_1Slice,
                                   glb_RightVector_1Slice,
                                   glb_UpVector_1Slice,
                                   XVector,
                                   YVector
    );


    ////////////////////////////////////////////////////////////////////////////////////////////////

    float lRadioDesp = 0;
    float lRadioDiff = ( radioFin - radioIni );
    float lRadioFactor = 0;

    for ( t = 0, j = 0; j < VResolTube; j++ )
    {
      for ( i = 0; i < HResolTube; i++ )
      {
        ////1)Equidistant distance

        //!!!Previous version
        //glb_AuxVec[1]=(j*glb_incremet);
        //glb_AuxVec[1]=(j*glb_incremet) + lExtraDesp;


        //1) y 2) Generate little slices in the extrmees of the tube
        switch ( j )
        {
          case 0: glb_AuxVec[1] = lPercent;
            break;

          case 1: glb_AuxVec[1] = lExtraDespIni + lPercent;
            break;

          default: glb_AuxVec[1] = (( lExtraDespIni + lPercent ) + ( j - 1 )*glb_incremet );
        }

        ////2)Generate no connected tubes (Minimal separation)
        //switch (j)
        //{
        //	case 0:
        //		glb_AuxVec[1]	=	lPercent;
        //		break;

        //	default:
        //		glb_AuxVec[1]=(j*glb_incremet);
        //}


        ////3)Generate no connected tubes (Minimal separation)
        //switch (j)
        //{
        //	case 0:
        //		glb_AuxVec[1] =	lPercent;
        //		break;

        //	case 1:
        //		glb_AuxVec[1] = 2*lPercent;
        //		break;

        //	default:
        //		glb_AuxVec[1]=(j*glb_incremet) + 2*lPercent;
        //}


        //Interpolate from relativa value desp in segment
        //This is the methos to avoid the smooth
        //////////////////////////////////////////////////////////////
        lRadioFactor = glb_AuxVec[1]/glb_modulo;
        lRadioDesp = ( radioIni + ( lRadioFactor*lRadioDiff ));

        glb_AuxVec[0] = lRadioDesp*( CoSinAngles ( i ));

        glb_AuxVec[2] = lRadioDesp*( SinAngles ( i ));
        glb_AuxVec[3] = 1.0;

        //Producto por matriz

        //Caso de primer slice (reorientaci�n fina)
        if (
          ( j == 0 ) //Estamos en el primer slice
          //&& ( !importer->isDendriticBifurcation(nodeParent) ) //Y no es una bifurcacion
          )
        {
          glb_AuxVec = prod ( glb_mat_1Slice, glb_AuxVec );
        }
        else
        {
          glb_AuxVec = prod ( glb_mat, glb_AuxVec );
        }

        //Explicit translate
        //Muchmore faster than apply the trnaslate matrix
        glb_AuxVec[0] += ini[0];
        glb_AuxVec[1] += ini[1];
        glb_AuxVec[2] += ini[2];

        glb_tubeVHandle[t] = Mesh->add_vertex ( MeshDef::Point ( glb_AuxVec[0], glb_AuxVec[1], glb_AuxVec[2]
                                                )
        );

        glb_meshVHandle[meshVHandleTotal] = glb_tubeVHandle[t];
        meshVHandleTotal++;

        t++;
      }
    }


    //El primer slice ha de hacerse por cercan�a, ya que la reorientaci�n del eslice hace que se rote.
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //Calcular el desplazamiento a realizar por cercan�a del primer slice
    //la rotaci�n del slice hace que rote sobre su eje, hay que solventarlo

    //Calc the nearest vertex
    //Indice inicial de la zona A (principio del primer slice)
    unsigned int lIdxA;
    MeshDef::Point lInitPointA ( 0, 0, 0 );

    //Indice inicial de la zona B (principio del siguiente slice)
    unsigned int lIdxB;
    MeshDef::Point lInitPointB ( 0, 0, 0 );

    OpenMesh::Vec3f lTmpVecResult ( 0, 0, 0 );

    //Distancia m�nima entre v�rtices
    float dis = 10E10;

    //Desplazamiento con los vertices a coser
    //Hay que comenzar desde los dos v�rtices con distancia m�nima
    unsigned int despA = 0;
    unsigned int despTmpA = 0;

    unsigned int despB = 0;
    unsigned int despTmpB = 0;

    for ( int i = 0; i < HResolTube; i++ )
    {
      lIdxA = i;
      lInitPointA = Mesh->points ( )[glb_tubeVHandle[lIdxA].idx ( )];

      ////Indices con los que coser
      for ( j = 0; j < HResolTube; j++ )
      {
        lIdxB = ( HResolTube + j );
        lInitPointB = Mesh->points ( )[glb_tubeVHandle[lIdxB].idx ( )];;

        lTmpVecResult = OpenMesh::Vec3f ( lInitPointA - lInitPointB );

        if ( lTmpVecResult.length ( ) < dis )
        {
          dis = lTmpVecResult.length ( );

          despB = despTmpB;
          despA = despTmpA;
        }
        ++despTmpB;
      }

      ++despTmpA;
      despTmpB = 0;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    for ( i = 0; i < HResolTube; i++ )
    {
      glb_tubeFace_VHandles.clear ( );
      glb_tubeFace_VHandles.push_back ( glb_tubeVHandle[(( i + despA )%HResolTube )] );
      glb_tubeFace_VHandles.push_back ( glb_tubeVHandle[(( i + despB + 1 )%HResolTube + HResolTube )] );
      glb_tubeFace_VHandles.push_back ( glb_tubeVHandle[(( i + despA + 1 )%HResolTube )] );
      Mesh->add_face ( glb_tubeFace_VHandles );

      glb_tubeFace_VHandles.clear ( );
      glb_tubeFace_VHandles.push_back ( glb_tubeVHandle[(( i + despA )%HResolTube )] );
      glb_tubeFace_VHandles.push_back ( glb_tubeVHandle[(( i + despB )%HResolTube + HResolTube )] );
      glb_tubeFace_VHandles.push_back ( glb_tubeVHandle[(( i + despB + 1 )%HResolTube + HResolTube )] );
      Mesh->add_face ( glb_tubeFace_VHandles );
    }

    for ( j = 1; j < VResolTube - 1; j++ )
    {
      for ( i = 0; i < HResolTube - 1; i++ )
      {
        glb_tubeFace_VHandles.clear ( );
        glb_tubeFace_VHandles.push_back ( glb_tubeVHandle[(( j )*HResolTube + i )] );
        glb_tubeFace_VHandles.push_back ( glb_tubeVHandle[(( j + 1 )*HResolTube + i + 1 )] );
        glb_tubeFace_VHandles.push_back ( glb_tubeVHandle[(( j )*HResolTube + i + 1 )] );
        Mesh->add_face ( glb_tubeFace_VHandles );

        glb_tubeFace_VHandles.clear ( );
        glb_tubeFace_VHandles.push_back ( glb_tubeVHandle[(( j )*HResolTube + i )] );
        glb_tubeFace_VHandles.push_back ( glb_tubeVHandle[(( j + 1 )*HResolTube + i )] );
        glb_tubeFace_VHandles.push_back ( glb_tubeVHandle[(( j + 1 )*HResolTube + i + 1 )] );
        Mesh->add_face ( glb_tubeFace_VHandles );
      }

      //Last two faces
      glb_tubeFace_VHandles.clear ( );
      glb_tubeFace_VHandles.push_back ( glb_tubeVHandle[( j*HResolTube + i )] );
      glb_tubeFace_VHandles.push_back ( glb_tubeVHandle[( j*HResolTube + i + 1 )] );
      glb_tubeFace_VHandles.push_back ( glb_tubeVHandle[( j*HResolTube )] );
      Mesh->add_face ( glb_tubeFace_VHandles );

      glb_tubeFace_VHandles.clear ( );
      glb_tubeFace_VHandles.push_back ( glb_tubeVHandle[( j*HResolTube + i )] );
      glb_tubeFace_VHandles.push_back ( glb_tubeVHandle[(( j + 1 )*HResolTube + i )] );
      glb_tubeFace_VHandles.push_back ( glb_tubeVHandle[( j*HResolTube + i + 1 )] );
      Mesh->add_face ( glb_tubeFace_VHandles );
    }




    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /*
		for(j=0; j<VResolTube-1; j++)
		{
			for(i=0; i<HResolTube-1; i++)
			{
				glb_tubeFace_VHandles.clear();
					glb_tubeFace_VHandles.push_back(glb_tubeVHandle[((j  )*HResolTube + i)]);
					glb_tubeFace_VHandles.push_back(glb_tubeVHandle[((j+1)*HResolTube + i+1)]);
					glb_tubeFace_VHandles.push_back(glb_tubeVHandle[((j  )*HResolTube + i+1)]);
				Mesh->add_face(glb_tubeFace_VHandles);

				glb_tubeFace_VHandles.clear();
					glb_tubeFace_VHandles.push_back(glb_tubeVHandle[((j  )*HResolTube + i )]);
					glb_tubeFace_VHandles.push_back(glb_tubeVHandle[((j+1)*HResolTube + i)]);
					glb_tubeFace_VHandles.push_back(glb_tubeVHandle[((j+1)*HResolTube + i+1)]);
				Mesh->add_face(glb_tubeFace_VHandles);
			}

			//Last two faces
			glb_tubeFace_VHandles.clear();
				glb_tubeFace_VHandles.push_back(glb_tubeVHandle[(j*HResolTube + i)]);
				glb_tubeFace_VHandles.push_back(glb_tubeVHandle[(j*HResolTube + i+1)]);
				glb_tubeFace_VHandles.push_back(glb_tubeVHandle[(j*HResolTube)]);
			Mesh->add_face(glb_tubeFace_VHandles);

			glb_tubeFace_VHandles.clear();
				glb_tubeFace_VHandles.push_back(glb_tubeVHandle[(j*HResolTube + i)]);
				glb_tubeFace_VHandles.push_back(glb_tubeVHandle[((j+1)*HResolTube + i)]);
				glb_tubeFace_VHandles.push_back(glb_tubeVHandle[(j*HResolTube + i+1)]);
			Mesh->add_face(glb_tubeFace_VHandles);
		}
		*/
    updateBaseMesh ( );
  }

  void NeuroSWC::generateLinearTubeWithControlSews ( const OpenMesh::Vec3f &ini,
                                                     const OpenMesh::Vec3f &fin,
                                                     float radioIni,
                                                     float radioFin,
                                                     int nodeId,
                                                     int nodeParent )
  {
    //Counters vars
    int i, j, t;
    float lPercent = 0;

    //Desp for no consecutive tubes
    float lExtraDespIni = 0;
    float lExtraDespFin = 0;

    glb_forward = ( fin - ini );

    glb_modulo = glb_forward.length ( );

    //lPercent = ( (PERCENTEXTTUBE * glb_modulo )/100.0);
    lPercent = 0;

    if ( importer->isDendriticBifurcation ( nodeParent ))
    {
      //Caso de bifurcaci�n y nodo no consecutivo.
      if (( nodeId - nodeParent ) != 1 )
      {
        lPercent = importer->getElementAt ( nodeParent ).radius;
      }
    }



/*
		//Caso est�ndar, tubo q no es bifurcacion ni terminaci�n
		lExtraDespFin = lExtraDespIni = glb_modulo /  (VResolTube);


		//Recordar que cosemos desde el parent hasta el hijo nodeId


		//C�lculo de los desplazamientos de los  slices para el correcto cosido de las bifurcaciones
		//Los par�metros dependientes son lPercent y lExtraDespIni (Parte inicial de los tubos)

		// -> lPercetn = es el desplazamiento del Slice 0
		// -> lExtraDespIni = Es el desplazamiento del slice 1
		if (importer->isDendriticBifurcation(nodeParent))
		{
			//Caso de bifurcaci�n y nodo consecutivo.
			if ( (nodeId-nodeParent)==1)
			{
				lPercent = 0;

				//Si e radio del parent es menor que el m�dulo del segmento, quiere decir que se puede desplazar ese radio el segundo segmento
				//Del tubo, no traspasaremos el tubo.
				if ( importer->getElementAt(nodeParent).radius < glb_modulo)	lExtraDespIni = importer->getElementAt(nodeParent).radius;

				//Si no desplazamos todo lo posible, que es el total del bubo excepto un 10% (que es para el resto de los slices)
				else															lExtraDespIni = glb_modulo -( (glb_modulo*10.0/100.0) );
			}
			//Caso bifurcacion y nodo no consecutivo.
			else
			{
				//Desplazamos el nodo bifurcaci�n, el radio m�s un porcentaje que es PERCENTEXTTUBE
				//Para evitar la interpenetraci�n

				lPercent = importer->getElementAt(nodeParent).radius
							+ ( (PERCENTEXTTUBE*importer->getElementAt(nodeParent).radius)/100);
				if (lPercent<0)	lPercent=0;

				//El resto de desplazamientos, no se tendr� en cuenta el lPercent inicial

				//lExtraDespIni = glb_modulo /  (VResolTube-1);

				//El resto de slices se han de repartir en una longitud que es:
				//la longitudo del segmento - lPercet (que es el desplazamiento para sacarlo) / entre la resoluci�n - 1 ya que el uno va a ir al 0
				//lExtraDespIni = (glb_modulo-lPercent) /  (VResolTube-1);
				lExtraDespIni = lPercent + importer->getElementAt(nodeParent).radius;
			}
		}
		else
		{


			//Desplazamiento inicial para lograr suavidad en la curva
			//Tiene l problema de que deja parte del volumen de la efera fuera
			//lPercent = importer->getElementAt(nodeParent).radius
			//+ ( (PERCENTEXTTUBE*importer->getElementAt(nodeParent).radius)/100)
			//;
			//lExtraDespIni = (glb_modulo /  (VResolTube-1)) ;

			//No hay desplazamiento inicial
			lPercent = 0;
			lExtraDespIni = importer->getElementAt(nodeParent).radius;
			//if (importer->isDendriticBifurcation(nodeId))
			//{
			//	lExtraDespIni = ( (glb_modulo) /  (VResolTube-1)) ;
			//}
			//else
			//{
			//	lExtraDespIni = ( (glb_modulo) /  (VResolTube)) ;
			//}
		}


		//Calculo del desplazamiento final
		if (importer->isDendriticTermination(nodeId))
		{
			lExtraDespFin=0;
			//Caso de 2 slices, se lleva el inicial hasta el final
			if (VResolTube==2) lExtraDespIni *=2;
		}
		else
		{

			//Si deseamos desplazar la parte final del tubo, el radio de la esfera a la que llegua
			//if ( importer->getElementAt(nodeId).radius < glb_modulo)	lExtraDespFin =  (importer->getElementAt(nodeId).radius);
			//else														lExtraDespFin =  (glb_modulo -( (glb_modulo*10.0/100.0) ) );

			//Para el desplazamiento final diferenciamos entre si es un nodo bifurcacion, en cuyo casolo llevamos hasta un poco menos del radio
			//o si es un nodo normal, en cuyo caso hacemos una reparticion equiespacial de los slices

			//if (importer->isDendriticBifurcation(nodeId))
			{
				if ( importer->getElementAt(nodeId).radius < glb_modulo)
					lExtraDespFin =  (importer->getElementAt(nodeId).radius);
				else
					lExtraDespFin =  (glb_modulo -( (glb_modulo*10.0/100.0) ) );
			}
			//else
			//{
			//	//Desplazamiento final constante (no se acerca al punto de deficnion el radio de este)
			//	//if ( importer->getElementAt(nodeId).radius < glb_modulo)
			//		lExtraDespFin =  (glb_modulo /  (VResolTube));
			//	//else														lExtraDespFin =  (glb_modulo - ( (glb_modulo*10.0/100.0) ) );
			//
			//}
			//No contemplamos el desplazamiento final, para que al sumar 0 en el c�lculo de lso incrementos de bien
			//lExtraDespFin = 0;
		}


		////Desplazamiento inicial para los tubos no consecutivos
		////As� evitamos que se introduzcan dentro en las bifurcaciones
		//if (	( (nodeParent+1)!=nodeId)
		//		&& (nodeParent!=mSomaId)
		//	)
		//{
		//	lExtraDespIni = glb_modulo /  (VResolTube);

		//	//lPercent = ( (PERCENTEXTTUBE * (glb_modulo-(lExtraDespIni + lExtraDespFin)) )/100.0);

		//	//Esta es la mejor opcion, solo desplaza el radio dle padre, as� no hay posibilidad de que se cruze
		//	//Al menos eso parece.
		//	lPercent = importer->getElementAt(nodeParent).radius
		//				+ ( (PERCENTEXTTUBE*importer->getElementAt(nodeParent).radius)/100)
		//				;

		//	if (lPercent<0)	lPercent=0;
		//
		//}
*/


    //-->>Formas de generacion de los tubos
    //1)Equidistant distance
    //glb_incremet	= glb_modulo/(VResolTube);

    //2)Generate little slices in the extremes of the tube
    //!!!glb_incremet	= ( glb_modulo-(lExtraDespIni + lExtraDespFin + lPercent))/(VResolTube-2);

    //3)Generate little slices in the extrmees of the tube
    //glb_incremet	= ( glb_modulo-(2*lPercent) )/(VResolTube-1);

    if ( importer->isDendriticTermination ( nodeId ))
    {
      glb_incRadio = ( radioFin - radioIni )/( VResolTube - 1 );
      glb_incremet = ( glb_modulo )/( VResolTube - 1 );
    }
    else
    {
      glb_incRadio = ( radioFin - radioIni )/( VResolTube );
      glb_incremet = ( glb_modulo )/( VResolTube );
    }
    //Calcular matriz de transformacion
    /*
			1ro:Calcular direcci�n del segmento, q conformara uno de los vectores de la matriz
			2do:pro vectorial con el eje y para sacar el vector right
				si es cero o muy proximo, utilizar el eje X
			3ro: Con otro producto vectorial obtendremos el vector up
				-> Ojo con la colocaci�n y el orden de los vectores
			--> Con esto ya esta calculada la matriz de rotaci�n
			--> Luego aplicar la traslado

			-->Multiplicar cada vertice por la matriz de transformaci�n
		*/

    //Forward vector normalized
    glb_forwardVec[0] = glb_forward[0]/glb_modulo;
    glb_forwardVec[1] = glb_forward[1]/glb_modulo;
    glb_forwardVec[2] = glb_forward[2]/glb_modulo;


    //Copy the vector for product
    glb_DespVec[0] = ini[0];
    glb_DespVec[1] = ini[1];
    glb_DespVec[2] = ini[2];


    //glb_forwardVec_1Slice = glb_forwardVec;

    generateTransformationMatrix ( glb_mat,
                                   glb_DespVec,
                                   glb_forwardVec,
                                   glb_TransVectorAux,
                                   glb_RightVector,
                                   glb_UpVector,
                                   XVector,
                                   YVector
    );

    //Generate the transformation matrix for the first slice
    ////////////////////////////////////////////////////////////////////////////////////////////////

    OpenMesh::Vec3f lPrevVector;

    if ( nodeParent != mSomaId )
    {
      if (
        ( importer->isDendriticBifurcation ( nodeParent ))
          && ( nodeId - nodeParent > 1 )
        )
      {

        //No se puede hacer una comparaci�n de cercan�a porque le tubo a�n no esta sintetizado
        /*
				int index = 0;
				int indexCandidate = 0;
				unsigned int	globalDesp	=	HResolTube*VResolTube;
				MeshDef::Point	lBariCenter(0,0,0);
				MeshDef::Point	ltmpVertex(0,0,0);
				OpenMesh::Vec3f ltmpVertexResult(0,0,0);
				float dis=0;
				MeshDef::VertexHandle   vhandle;

				//Recorremos los vertices a atar y nos quedamos con la normal del m�s cercano
				///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				//->Calcular el centro de los v�rtices del nodo
				for(j=0; j<HResolTube; j++)
				{
					//Calc the index of the vertex
					index		=	( (nodeId-NEURODESP-1)*(globalDesp) + j);
					index		+=	glb_nVerticesSoma;
					lBariCenter	+=	Mesh->points()[(index)];
				}

				lBariCenter	/=	(float)(HResolTube);

				//->Calcular el v�rtice m�s cercano del parent al centro del tubo calculado
				indexCandidate		=	( ( ((nodeParent-NEURODESP)*(globalDesp))) );
				indexCandidate		+=	glb_nVerticesSoma;

				//Utilizamos el opossite para salirnos del tubo (no estar por detr�s del tri�ngulo)
				ltmpVertexResult	=	OpenMesh::Vec3f(lBariCenter
														- Mesh->points()[(indexCandidate)]);
				dis=ltmpVertexResult.length();

				bool lNearestTest=true;
				//////////////////////////////////////////////////////////////////////////////////////////

				//Calculate the vectors
				OpenMesh::Vec3f lVecAB2	=	OpenMesh::Vec3f(
															  importer->getElementAt(nodeId).position
															  - importer->getElementAt(nodeParent).position
															);

				OpenMesh::Vec3f lVecAC2	=	OpenMesh::Vec3f(
															importer->getElementAt(nodeParent+1).position
															- importer->getElementAt(nodeParent).position
															);

				//Cross product == producto vectorial
				lVecAB2.normalize();
				lVecAC2.normalize();

				float lAuxDot2	= OpenMesh::dot(lVecAB2,lVecAC2);

				float lAuxAngle2 = acos(lAuxDot2);
				float lAuxAngleInGrads = fabs(OpenMesh::rad_to_deg(lAuxAngle2));

				if ( (lAuxAngleInGrads>ANGBADSEW)
					)
				{
					lNearestTest = false;
				}

				//////////////////////////////////////////////////////////////////////////////////////////

				//Select the nearest vertex
				if (lNearestTest)
				{
					for(j=0; j<(HResolTube); j++)
					{
						index				=	( ( ((nodeParent-NEURODESP)*(globalDesp))) + j);
						index				+=	glb_nVerticesSoma;

						ltmpVertex			=	Mesh->points()[(index)];
						ltmpVertexResult	=	OpenMesh::Vec3f(lBariCenter - ltmpVertex);

						if (ltmpVertexResult.length()< dis)
						{
							indexCandidate	=	index;
							dis				=	ltmpVertexResult.length();
						}
					}
				}
				//Select the most extern vertes
				else
				{
					for(j=0; j<(HResolTube); j++)
					{
						index				=	( ( ((nodeParent-NEURODESP)*(globalDesp))) + j);
						index				+=	glb_nVerticesSoma;

						ltmpVertex			=	Mesh->points()[(index)];
						ltmpVertexResult	=	OpenMesh::Vec3f(lBariCenter - ltmpVertex);

						if (ltmpVertexResult.length()> dis)
						{
							indexCandidate	=	index;
							dis				=	ltmpVertexResult.length();
						}
					}
				}


				//Ahora que tenemos el vertice m�s cercano

				//Select the vertex handler
				vhandle		= Mesh->vertex_handle(indexCandidate);
				lPrevVector = Mesh->normal(vhandle);
				*/

        /*
				int index = 0;
				unsigned int	globalDesp	=	HResolTube*VResolTube;
				MeshDef::Point	lBariCenter(0,0,0);

				//Recorremos los vertices a atar y nos quedamos con la normal del m�s cercano
				///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				//->Calcular el centro de los v�rtices del nodo
				for(j=0; j<HResolTube; j++)
				{
					//Calc the index of the vertex
					index		=	( (nodeParent-NEURODESP-1)*(globalDesp) + j);
					index		+=	glb_nVerticesSoma;
					lBariCenter	+=	Mesh->points()[(index)];
				}

				lBariCenter	/=	(float)(HResolTube);

				lPrevVector = importer->getElementAt(nodeId).position - lBariCenter;
				*/


        /*
				//Funciona mas o menos, pero la orientaci�n del eslice esta limitada por la geometr�a de la malla
				int index = 0;
				int indexCandidate = 0;
				unsigned int	globalDesp	=	HResolTube*VResolTube;
				MeshDef::Point	ltmpVertex(0,0,0);
				OpenMesh::Vec3f ltmpVertexResult(0,0,0);
				float dis, disAux;
				MeshDef::VertexHandle   vhandle;

				//Recorremos los vertices a atar y nos quedamos con la normal del m�s cercano
				//QUe ser� la orientaci�n del primer del slice bifurcaci�n que no sea el primer hijo
				///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

				index		=	( ( ((nodeParent-NEURODESP)*(globalDesp))) );
				index		+=	glb_nVerticesSoma;
				dis = (Mesh->points()[(index)] - importer->getElementAt(nodeId).position).length();
				lPrevVector = Mesh->vertex_normals()[index];

				for(j=1; j<HResolTube; j++)
				{
					//Calc the index of the vertex
					index		=	( (nodeParent-NEURODESP)*(globalDesp) + j);
					index		+=	glb_nVerticesSoma;
					disAux = (Mesh->points()[(index)] - importer->getElementAt(nodeId).position).length();

					if ( disAux<dis )
					{
						dis=disAux;
						//vhandle = Mesh->vertex_handle(index);
						lPrevVector = Mesh->vertex_normals()[index];
					}
				}
				*/

        /*
				//suma de todos los vectores normalizados en ese nodo
				MeshDef::Point	ltmpVertex(0,0,0);
				MeshDef::Point	ltmpVertex2(0,0,0);

				ltmpVertex =	importer->getElementAt(nodeParent).position
								- importer->getElementAt(nodeParent-1).position;
				ltmpVertex /= ltmpVertex.length();

				ltmpVertex2 =	importer->getElementAt(nodeParent+1).position
								- importer->getElementAt(nodeParent).position;
				ltmpVertex2 /= ltmpVertex2.length();

				lPrevVector = importer->getElementAt(nodeId).position
							  - importer->getElementAt(nodeParent).position;
				lPrevVector /= lPrevVector.length();

				lPrevVector = (lPrevVector+ltmpVertex+ltmpVertex2);
				*/

        /*
				OpenMesh::Vec3f ltmpVertexResult(0,0,0);
				OpenMesh::Vec3f ltmpVertexResult2(0,0,0);

				ltmpVertexResult =	importer->getElementAt(nodeParent).position
									//Menos el parent del parent
									- importer->getElementAt(importer->getElementAt(nodeParent).parent).position
									;

				ltmpVertexResult2 =	importer->getElementAt(nodeParent+1).position
									//Menos el parent del parent
									- importer->getElementAt(nodeParent).position
									;
				*/
        /*
				float lmod = ltmpVertexResult.length();
				ltmpVertexResult/=lmod;

				lmod = ltmpVertexResult2.length();
				ltmpVertexResult2/=lmod;

				lPrevVector = ltmpVertexResult
								+ ltmpVertexResult2;
				*/

        /*
				float lmod = ltmpVertexResult.length();

				boost::numeric::ublas::vector<float> ltmpVertexResultboost;
				ltmpVertexResultboost.resize(3);
				ltmpVertexResultboost[0]=ltmpVertexResult[0]/lmod;
				ltmpVertexResultboost[1]=ltmpVertexResult[1]/lmod;
				ltmpVertexResultboost[2]=ltmpVertexResult[2]/lmod;

				lmod = ltmpVertexResult2.length();

				boost::numeric::ublas::vector<float> ltmpVertexResultboost2;
				ltmpVertexResultboost2.resize(3);
				ltmpVertexResultboost2[0]=ltmpVertexResult2[0]/lmod;
				ltmpVertexResultboost2[1]=ltmpVertexResult2[1]/lmod;
				ltmpVertexResultboost2[2]=ltmpVertexResult2[2]/lmod;

				ltmpVertexResultboost+=ltmpVertexResultboost2;
				*/

        /*
				//Esto es el standart
				lPrevVector =	importer->getElementAt(nodeParent).position
								//Menos el parent del parent
								- importer->getElementAt(nodeParent+1).position
								;
				*/

        /*
				//Esto es el standart
				lPrevVector =	importer->getElementAt(nodeParent).position
								//Menos el parent del parent
								- importer->getElementAt(importer->getElementAt(nodeParent).parent).position
								;
				*/
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      }
      else
      {
        lPrevVector = importer->getElementAt ( nodeParent ).position
          //Menos el parent del parent
          - importer->getElementAt ( importer->getElementAt ( nodeParent ).parent ).position;
      }
    }
    else
    {
      lPrevVector = importer->getElementAt ( nodeId ).position;
    }

    float lmod = lPrevVector.length ( );

    glb_AuxDir_1Slice[0] = lPrevVector[0]/lmod;
    glb_AuxDir_1Slice[1] = lPrevVector[1]/lmod;
    glb_AuxDir_1Slice[2] = lPrevVector[2]/lmod;

    //Los 2 vectores a sumar est�n normaliazos (Ok)
    if (
      ( importer->isDendriticBifurcation ( nodeParent ))
        && ( nodeId - nodeParent > 1 )
      )
    {
      ////Si e suan bifurcacion no consecutiva, la direcci�n es la ponderaci�n entre la normal del vertice seleccionado
      ////y la direcci�n del segmento
      //glb_forwardVec_1Slice = glb_AuxDir_1Slice
      //						+glb_forwardVec
      //						;

      glb_forwardVec_1Slice = glb_forwardVec
        //+ glb_AuxDir_1Slice
        ;

    }
    else
    {
      glb_forwardVec_1Slice = glb_AuxDir_1Slice
        + glb_forwardVec;
    }


    //Forward vector normalization
    lmod = boost::numeric::ublas::norm_2 ( glb_forwardVec_1Slice );

    glb_forwardVec_1Slice[0] /= lmod;
    glb_forwardVec_1Slice[1] /= lmod;
    glb_forwardVec_1Slice[2] /= lmod;

    generateTransformationMatrix ( glb_mat_1Slice,
                                   glb_DespVec,
                                   glb_forwardVec_1Slice,
                                   glb_TransVectorAux_1Slice,
                                   glb_RightVector_1Slice,
                                   glb_UpVector_1Slice,
                                   XVector,
                                   YVector
    );


    ////////////////////////////////////////////////////////////////////////////////////////////////

    float lRadioDesp = 0;
    float lRadioDiff = ( radioFin - radioIni );
    float lRadioFactor = 0;

    for ( t = 0, j = 0; j < VResolTube; j++ )
    {
      for ( i = 0; i < HResolTube; i++ )
      {
        ////1)Equidistant distance

        //!!!Previous version
        //glb_AuxVec[1]=(j*glb_incremet);
        //glb_AuxVec[1]=(j*glb_incremet) + lExtraDesp;


        //1) y 2) Generate little slices in the extrmees of the tube
        switch ( j )
        {
          case 0: glb_AuxVec[1] = lPercent;
            break;

            //case 1:
            //	glb_AuxVec[1] = lExtraDespIni + lPercent;
            //	break;

          default:
            //glb_AuxVec[1]=( (lExtraDespIni + lPercent) + (j-1)*glb_incremet);
            glb_AuxVec[1] = (( j )*glb_incremet );
        }

        ////2)Generate no connected tubes (Minimal separation)
        //switch (j)
        //{
        //	case 0:
        //		glb_AuxVec[1]	=	lPercent;
        //		break;

        //	default:
        //		glb_AuxVec[1]=(j*glb_incremet);
        //}


        ////3)Generate no connected tubes (Minimal separation)
        //switch (j)
        //{
        //	case 0:
        //		glb_AuxVec[1] =	lPercent;
        //		break;

        //	case 1:
        //		glb_AuxVec[1] = 2*lPercent;
        //		break;

        //	default:
        //		glb_AuxVec[1]=(j*glb_incremet) + 2*lPercent;
        //}


        //Interpolate from relativa value desp in segment
        //This is the methos to avoid the smooth
        //////////////////////////////////////////////////////////////
        lRadioFactor = glb_AuxVec[1]/glb_modulo;
        lRadioDesp = ( radioIni + ( lRadioFactor*lRadioDiff ));

        glb_AuxVec[0] = lRadioDesp*( CoSinAngles ( i ));

        glb_AuxVec[2] = lRadioDesp*( SinAngles ( i ));
        glb_AuxVec[3] = 1.0;

        //Producto por matriz

        //Caso de primer slice (reorientaci�n fina)
        if (
          ( j == 0 ) //Estamos en el primer slice
          //&& ( !importer->isDendriticBifurcation(nodeParent) ) //Y no es una bifurcacion
          )
        {
          glb_AuxVec = prod ( glb_mat_1Slice, glb_AuxVec );
        }
        else
        {
          glb_AuxVec = prod ( glb_mat, glb_AuxVec );
        }

        //Explicit translate
        //Muchmore faster than apply the trnaslate matrix
        glb_AuxVec[0] += ini[0];
        glb_AuxVec[1] += ini[1];
        glb_AuxVec[2] += ini[2];

        glb_tubeVHandle[t] = Mesh->add_vertex ( MeshDef::Point ( glb_AuxVec[0], glb_AuxVec[1], glb_AuxVec[2]
                                                )
        );

        glb_meshVHandle[meshVHandleTotal] = glb_tubeVHandle[t];
        meshVHandleTotal++;

        t++;
      }
    }


    //El primer slice ha de hacerse por cercan�a, ya que la reorientaci�n del eslice hace que se rote.
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //Calcular el desplazamiento a realizar por cercan�a del primer slice
    //la rotaci�n del slice hace que rote sobre su eje, hay que solventarlo

    //Calc the nearest vertex
    //Indice inicial de la zona A (principio del primer slice)
    unsigned int lIdxA;
    MeshDef::Point lInitPointA ( 0, 0, 0 );

    //Indice inicial de la zona B (principio del siguiente slice)
    unsigned int lIdxB;
    MeshDef::Point lInitPointB ( 0, 0, 0 );

    OpenMesh::Vec3f lTmpVecResult ( 0, 0, 0 );

    //Distancia m�nima entre v�rtices
    float dis = 10E10;

    //Desplazamiento con los vertices a coser
    //Hay que comenzar desde los dos v�rtices con distancia m�nima
    unsigned int despA = 0;
    unsigned int despTmpA = 0;

    unsigned int despB = 0;
    unsigned int despTmpB = 0;

    for ( int i = 0; i < HResolTube; i++ )
    {
      lIdxA = i;
      lInitPointA = Mesh->points ( )[glb_tubeVHandle[lIdxA].idx ( )];

      ////Indices con los que coser
      for ( j = 0; j < HResolTube; j++ )
      {
        lIdxB = ( HResolTube + j );
        lInitPointB = Mesh->points ( )[glb_tubeVHandle[lIdxB].idx ( )];;

        lTmpVecResult = OpenMesh::Vec3f ( lInitPointA - lInitPointB );

        if ( lTmpVecResult.length ( ) < dis )
        {
          dis = lTmpVecResult.length ( );

          despB = despTmpB;
          despA = despTmpA;
        }
        ++despTmpB;
      }

      ++despTmpA;
      despTmpB = 0;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    for ( i = 0; i < HResolTube; i++ )
    {
      glb_tubeFace_VHandles.clear ( );
      glb_tubeFace_VHandles.push_back ( glb_tubeVHandle[(( i + despA )%HResolTube )] );
      glb_tubeFace_VHandles.push_back ( glb_tubeVHandle[(( i + despB + 1 )%HResolTube + HResolTube )] );
      glb_tubeFace_VHandles.push_back ( glb_tubeVHandle[(( i + despA + 1 )%HResolTube )] );
      Mesh->add_face ( glb_tubeFace_VHandles );

      glb_tubeFace_VHandles.clear ( );
      glb_tubeFace_VHandles.push_back ( glb_tubeVHandle[(( i + despA )%HResolTube )] );
      glb_tubeFace_VHandles.push_back ( glb_tubeVHandle[(( i + despB )%HResolTube + HResolTube )] );
      glb_tubeFace_VHandles.push_back ( glb_tubeVHandle[(( i + despB + 1 )%HResolTube + HResolTube )] );
      Mesh->add_face ( glb_tubeFace_VHandles );
    }

    for ( j = 1; j < VResolTube - 1; j++ )
    {
      for ( i = 0; i < HResolTube - 1; i++ )
      {
        glb_tubeFace_VHandles.clear ( );
        glb_tubeFace_VHandles.push_back ( glb_tubeVHandle[(( j )*HResolTube + i )] );
        glb_tubeFace_VHandles.push_back ( glb_tubeVHandle[(( j + 1 )*HResolTube + i + 1 )] );
        glb_tubeFace_VHandles.push_back ( glb_tubeVHandle[(( j )*HResolTube + i + 1 )] );
        Mesh->add_face ( glb_tubeFace_VHandles );

        glb_tubeFace_VHandles.clear ( );
        glb_tubeFace_VHandles.push_back ( glb_tubeVHandle[(( j )*HResolTube + i )] );
        glb_tubeFace_VHandles.push_back ( glb_tubeVHandle[(( j + 1 )*HResolTube + i )] );
        glb_tubeFace_VHandles.push_back ( glb_tubeVHandle[(( j + 1 )*HResolTube + i + 1 )] );
        Mesh->add_face ( glb_tubeFace_VHandles );
      }

      //Last two faces
      glb_tubeFace_VHandles.clear ( );
      glb_tubeFace_VHandles.push_back ( glb_tubeVHandle[( j*HResolTube + i )] );
      glb_tubeFace_VHandles.push_back ( glb_tubeVHandle[( j*HResolTube + i + 1 )] );
      glb_tubeFace_VHandles.push_back ( glb_tubeVHandle[( j*HResolTube )] );
      Mesh->add_face ( glb_tubeFace_VHandles );

      glb_tubeFace_VHandles.clear ( );
      glb_tubeFace_VHandles.push_back ( glb_tubeVHandle[( j*HResolTube + i )] );
      glb_tubeFace_VHandles.push_back ( glb_tubeVHandle[(( j + 1 )*HResolTube + i )] );
      glb_tubeFace_VHandles.push_back ( glb_tubeVHandle[( j*HResolTube + i + 1 )] );
      Mesh->add_face ( glb_tubeFace_VHandles );
    }




    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /*
		for(j=0; j<VResolTube-1; j++)
		{
			for(i=0; i<HResolTube-1; i++)
			{
				glb_tubeFace_VHandles.clear();
					glb_tubeFace_VHandles.push_back(glb_tubeVHandle[((j  )*HResolTube + i)]);
					glb_tubeFace_VHandles.push_back(glb_tubeVHandle[((j+1)*HResolTube + i+1)]);
					glb_tubeFace_VHandles.push_back(glb_tubeVHandle[((j  )*HResolTube + i+1)]);
				Mesh->add_face(glb_tubeFace_VHandles);

				glb_tubeFace_VHandles.clear();
					glb_tubeFace_VHandles.push_back(glb_tubeVHandle[((j  )*HResolTube + i )]);
					glb_tubeFace_VHandles.push_back(glb_tubeVHandle[((j+1)*HResolTube + i)]);
					glb_tubeFace_VHandles.push_back(glb_tubeVHandle[((j+1)*HResolTube + i+1)]);
				Mesh->add_face(glb_tubeFace_VHandles);
			}

			//Last two faces
			glb_tubeFace_VHandles.clear();
				glb_tubeFace_VHandles.push_back(glb_tubeVHandle[(j*HResolTube + i)]);
				glb_tubeFace_VHandles.push_back(glb_tubeVHandle[(j*HResolTube + i+1)]);
				glb_tubeFace_VHandles.push_back(glb_tubeVHandle[(j*HResolTube)]);
			Mesh->add_face(glb_tubeFace_VHandles);

			glb_tubeFace_VHandles.clear();
				glb_tubeFace_VHandles.push_back(glb_tubeVHandle[(j*HResolTube + i)]);
				glb_tubeFace_VHandles.push_back(glb_tubeVHandle[((j+1)*HResolTube + i)]);
				glb_tubeFace_VHandles.push_back(glb_tubeVHandle[(j*HResolTube + i+1)]);
			Mesh->add_face(glb_tubeFace_VHandles);
		}
		*/
    updateBaseMesh ( );
  }

  void NeuroSWC::generateNeuron ( string SWCName,
                                  int horResolTube,
                                  int verResolTube,
                                  int horResolSoma,
                                  int verResolSoma,
                                  unsigned int pSomaModel )
  {
    BaseMesh *tmpMesh;

    HResolTube = horResolTube;
    VResolTube = verResolTube;
    HResolSoma = horResolSoma;
    VResolSoma = verResolSoma;

    //glb_nVerticesSoma	= (HResolSoma*VResolSoma)	+ 2;

    //!!!->Aki hay que calcular los v�rtices del soma desde un modelo y las facetas
    if (( pSomaModel == -1 ) || ( mSomaContainer->getContainer ( ).size ( ) == 0 ))
    {
      glb_nVerticesSoma = ( VResolSoma - 2 )*HResolSoma + 2;
      //mSomaNumFaces		= HResolSoma*(VResolSoma-2)*2;
      mSomaNumFaces = HResolSoma*( VResolSoma )*2;
    }
    else
    {
      //tmpMesh = new BaseMesh();

      //tmpMesh->loadModel(pSomaModel);

      //glb_nVerticesSoma	= tmpMesh->getNumVertex();
      //mSomaNumFaces		= tmpMesh->getNumFaces();
      ////delete tmpMesh;

      //tmpMesh = &mSomaContainer->getElementAt(0);
      //tmpMesh =  mSomaContainer->getElementAt(pSomaModel);
      tmpMesh = new BaseMesh ( );

      tmpMesh->JoinBaseMesh ( mSomaContainer->getElementAt ( pSomaModel ));

      glb_nVerticesSoma = tmpMesh->getNumVertex ( );
      mSomaNumFaces = tmpMesh->getNumFaces ( );

      //Una vez que se ha unido
      //delete tmpMesh;
    }

    fileName = SWCName;
    importer = new SWCImporter ( fileName );
    glb_nVerticesTube = HResolTube*VResolTube;

    //Vertex tmp container
    glb_tubeVHandle = new MeshDef::VertexHandle[glb_nVerticesTube];

    meshVHandleTotal = 0;

    if ( mTessellControlSew )
    {
      glb_meshVHandle = new MeshDef::VertexHandle[
      glb_nVerticesSoma                  //Soma numVertexs
        + glb_nVerticesTube*( importer->getNumNodes ( ) - 1 )  //TubeNumVertex
          //!!! +	importer->getNumDendriticTermination() 			//Dendritic terminations
        + ( HResolTube + 1 )*importer->getNumDendriticTermination ( )      //Dendritic terminations
      ];
    }
    else
    {
      glb_meshVHandle = new MeshDef::VertexHandle[
      glb_nVerticesSoma                  //Soma numVertexs
        + glb_nVerticesTube*( importer->getNumNodes ( ) - 1 )  //TubeNumVertex
        + importer->getNumDendriticTermination ( )      //Dendritic terminations
      ];
    }

    glb_mat.resize ( 4, 4 );
    glb_forwardVec.resize ( 3 );
    glb_DespVec.resize ( 3 );
    glb_AuxVec.resize ( 4 );

    //Precalculate angles
    CoSinAngles.resize ( HResolTube );
    SinAngles.resize ( HResolTube );
    for ( int i = 0; i < HResolTube; i++ )
    {
      glb_angle = ( PI*2 )*(( float ) i/( float ) HResolTube );
      CoSinAngles ( i ) = cos ( glb_angle );
      SinAngles ( i ) = sin ( glb_angle );
    }

    XVector.resize ( 3 );
    XVector[0] = 1;
    XVector[1] = 0;
    XVector[2] = 0;

    YVector.resize ( 3 );
    YVector[0] = 0;
    YVector[1] = 1;
    YVector[2] = 0;

    glb_ZeroVec.resize ( 3 );
    glb_ZeroVec[0] = glb_ZeroVec[1] = glb_ZeroVec[2] = 0;

    glb_UpVector.resize ( 3 );
    glb_RightVector.resize ( 3 );
    glb_TransVectorAux.resize ( 3 );


    //Inicializaci�n de los objetos necesarios para la orientaci�n de primer slice
    glb_mat_1Slice.resize ( 4, 4 );
    glb_forwardVec_1Slice.resize ( 3 );
    glb_UpVector_1Slice.resize ( 3 );
    glb_RightVector_1Slice.resize ( 3 );
    glb_TransVectorAux_1Slice.resize ( 3 );
    glb_AuxDir_1Slice.resize ( 3 );

    int numNodes = importer->getNumNodes ( );
    //int somaId		=	-1;

    SWCNode nodeAux;
    SWCNode parentNodeAux;

    for ( int i = 1; i < numNodes; ++i )
      //for (int i=1;i<(numNodes-1);++i)
      //for (int i=(numNodes-200);i<(numNodes-1);++i)
      //for (int i=1;i<50;++i)
    {
      nodeAux = importer->getElementAt ( i );

      //Case of Soma
      if ( nodeAux.parent == -1 )
      {
        mSomaId = i;
        if (( pSomaModel == -1 ) || ( mSomaContainer->getContainer ( ).size ( ) == 0 ))
        {
          generateSphere ( OpenMesh::Vec3f ( nodeAux.position[0], nodeAux.position[1], nodeAux.position[2] ),
                           nodeAux.radius,
                           HResolSoma,
                           VResolSoma );
        }
        else
        {
          //Hacer que la maya sea el soma
          tmpMesh->scaleBaseMesh ( nodeAux.radius );
          JoinBaseMesh ( tmpMesh );
          meshVHandleTotal += glb_nVerticesSoma;

          delete tmpMesh;
        }
      }
      else
      {
        parentNodeAux = importer->getElementAt ( nodeAux.parent );

        //float auxRadius =parentNodeAux.radius;
        //if (parentNodeAux.id==somaId)	auxRadius/=2.0;

        float auxRadius = parentNodeAux.radius;
        //if (parentNodeAux.id==somaId)	auxRadius/=2.0;

        if ( parentNodeAux.id == mSomaId )
        {
          auxRadius = nodeAux.radius;

          //Calculate the desplacement of the segmente to the center of the Soma

          OpenMesh::Vec3f lOrigin ( parentNodeAux.position[0], parentNodeAux.position[1], parentNodeAux.position[2] );
          OpenMesh::Vec3f lFin ( nodeAux.position[0], nodeAux.position[1], nodeAux.position[2] );

          //if (lFin.length()<lOrigin.length())
          //{
          //	OpenMesh::Vec3f lAuxVec =	lOrigin;
          //	lOrigin					=	lFin;
          //	lFin					=	lAuxVec;
          //}

          OpenMesh::Vec3f lVecDir = lFin - lOrigin;
          lVecDir.normalize ( );


          //Desplazamientos de los tubos
          //Condicion para los somas modelados con mass spring (el inicio del tubo no es sobre la superficie d ela esfera, sino cerca del otro punto del tubo)
          if ( mPiramidalSoma )
          {
            lVecDir = lVecDir*( nodeAux.position.length ( ) - (( nodeAux.position.length ( )*VResolTube )/100.0 ));
          }
          else
          {
            lVecDir = lVecDir*( parentNodeAux.radius );
          }

          parentNodeAux.position[0] += lVecDir[0];
          parentNodeAux.position[1] += lVecDir[1];
          parentNodeAux.position[2] += lVecDir[2];
        }

        if ( mTessellControlSew )
        {
          generateLinearTubeWithControlSews ( OpenMesh::Vec3f ( parentNodeAux.position ),
                                              OpenMesh::Vec3f ( nodeAux.position ),
                                              auxRadius,
                                              nodeAux.radius,
                                              nodeAux.id,
                                              nodeAux.parent
          );
        }
        else
        {
          generateLinearTube ( OpenMesh::Vec3f ( parentNodeAux.position ),
                               OpenMesh::Vec3f ( nodeAux.position ),
                               auxRadius,
                               nodeAux.radius,
                               nodeAux.id,
                               nodeAux.parent
          );
        }

      }
    }

    ////Build neuron connections
    connectConsecutiveTubes ( );

    //connectNoConsecutiveTubes();
    connectNoConsecutiveTubes2 ( );

    if ( mPiramidalSoma )
    {
      connectPiramidalSoma ( );
    }
    else
    {
      connectSoma ( );
    }


    //connectPiramidalSoma();

    ////Set neuron colors
    generateDendriticTerminations ( );

    //Set colors to vertex
    setColorsFromSWC ( importer );

    delete[] glb_tubeVHandle;
    delete[] glb_meshVHandle;
    //delete importer;

    glb_tubeVHandle = NULL;
    //importer		= NULL;

  }

  void NeuroSWC::setColorsFromSWC ( const SWCImporter *pImporter )
  {
    int i = 0;
    MeshDef::ConstVertexIter iniLimit = Mesh->vertices_begin ( );
    MeshDef::ConstVertexIter finLimit = Mesh->vertices_begin ( );

    //Pintar vertices soma
    for ( ; i < glb_nVerticesSoma; ++i )
      finLimit++;

    setVertexColor ( iniLimit, finLimit, MeshDef::Color ( 1.0, 0, 0, 0 ));

    unsigned int lNumNodes = pImporter->getNumNodes ( );
    SWCNode lAuxNode;

    iniLimit = finLimit;

    for ( int j = 2; j < lNumNodes; ++j )
    {
      lAuxNode = pImporter->getElementAt ( j );

      for ( i = 0; i < glb_nVerticesTube; ++i )
        finLimit++;

      switch ( lAuxNode.type )
      {
        case 2: setVertexColor ( iniLimit, finLimit, MeshDef::Color ( 0, 0, 1.0, 0 ));
          break;

        case 3: setVertexColor ( iniLimit, finLimit, MeshDef::Color ( 0, 1.0, 0, 0 ));
          break;

        case 4: setVertexColor ( iniLimit, finLimit, MeshDef::Color ( 1.0, 0, 1.0, 0 ));
          break;

        default: setVertexColor ( iniLimit, finLimit, MeshDef::Color ( 1.0, 1.0, 1.0, 0 ));
      }

      iniLimit = finLimit;
    }

    //Pintar parte final de las dendirtas
    finLimit = Mesh->vertices_end ( );
    setVertexColor ( iniLimit, finLimit, MeshDef::Color ( 0.0, 1.0, 0.0, 0 ));
/*
		//Pintar vertices axon
		SWCNode lAuxNode;
		bool lAxonStart	=false;
		bool lAxonFinish=false;
		unsigned int lAxonNodes=0;

		i=0;
		while (!lAxonStart)
		{
			lAuxNode = importer->getElementAt(i);
			if (lAuxNode.type==2) lAxonStart=true;
			++i;
		}

		while (!lAxonFinish)
		{
			lAuxNode = importer->getElementAt(i);
			if (lAuxNode.type!=2)
			{
				lAxonFinish=true;
			}
			++lAxonNodes;
			++i;
		}

		iniLimit = finLimit;
		lAxonNodes *= glb_nVerticesTube;
		for (i=0;i<lAxonNodes;++i) finLimit++;
		setVertexColor(iniLimit, finLimit, MeshDef::Color(0,0,255));

		//Pintar resto de vertices
		setVertexColor(finLimit, Mesh->vertices_end(), MeshDef::Color(0,255,0));
*/

  }

  /*
	//Generate the dendritic terminations
	void NeuroSWC::generateDendriticTerminations()
	{
		int numDenTer		= importer->getNumDendriticTermination();
		int			idTer	=0;
		SWCNode		nodeAux;

		int index			= 0;

		unsigned int globalDesp	=	HResolTube*VResolTube;
		MeshDef::VertexHandle vhandle;

		for (int i=0;i<numDenTer; ++i)
		{
			idTer=importer->getDendriticTermination(i);
			nodeAux = importer->getElementAt(idTer);

			//Terminaci�n en punta
			if (mSharpEnd)
			{
				SWCNode		nodeAuxParent;
				nodeAuxParent = importer->getElementAt(nodeAux.parent);
				OpenMesh::Vec3f lSharpDirection =	nodeAux.position - nodeAuxParent.position;
				lSharpDirection.normalize();

				nodeAux.position += lSharpDirection*nodeAux.radius;
			}



			//Add vertex termination
			vhandle = Mesh->add_vertex(MeshDef::Point(nodeAux.position[0]
													,nodeAux.position[1]
													,nodeAux.position[2]
													)
										);

			glb_meshVHandle[meshVHandleTotal] = vhandle;
			meshVHandleTotal++;

			int j=0;
			//Add fan faces
			for(j=0; j<(HResolTube-1); j++ )
			{
				//Superior circle
				glb_tubeFace_VHandles.clear();
					index = ( ( ((nodeAux.id-NEURODESP)*(globalDesp))) - HResolTube + j);
					index+=glb_nVerticesSoma;
					glb_tubeFace_VHandles.push_back(glb_meshVHandle[index]);

					index = (meshVHandleTotal-1);
					glb_tubeFace_VHandles.push_back(glb_meshVHandle[index]);

					index = ( ( ((nodeAux.id-NEURODESP)*(globalDesp))) - HResolTube + j + 1);
					index+=glb_nVerticesSoma;
					glb_tubeFace_VHandles.push_back(glb_meshVHandle[index]);
				Mesh->add_face(glb_tubeFace_VHandles);
			}

			glb_tubeFace_VHandles.clear();
				index = ( ( ((nodeAux.id-NEURODESP)*(globalDesp))) - HResolTube + j);
				index+=glb_nVerticesSoma;
				glb_tubeFace_VHandles.push_back(glb_meshVHandle[index]);

				index = (meshVHandleTotal-1);
				glb_tubeFace_VHandles.push_back(glb_meshVHandle[index]);

				index = ( ( ((nodeAux.id-NEURODESP)*(globalDesp))) - HResolTube);
				index+=glb_nVerticesSoma;
				glb_tubeFace_VHandles.push_back(glb_meshVHandle[index]);
			Mesh->add_face(glb_tubeFace_VHandles);
		}
		updateBaseMesh();
	}
	*/

  void NeuroSWC::generateDendriticTerminations ( )
  {
    int numDenTer = importer->getNumDendriticTermination ( );
    int idTer = 0;
    SWCNode nodeAux;

    int index = 0;

    unsigned int globalDesp = HResolTube*VResolTube;
    MeshDef::VertexHandle vhandle;

    for ( int i = 0; i < numDenTer; ++i )
    {
      idTer = importer->getDendriticTermination ( i );
      nodeAux = importer->getElementAt ( idTer );

      SWCNode nodeAuxParent;
      nodeAuxParent = importer->getElementAt ( nodeAux.parent );
      OpenMesh::Vec3f lSharpDirection = nodeAux.position - nodeAuxParent.position;
      //Sacamos la direccion del segmento
      lSharpDirection.normalize ( );

      //Costura peque�a de contenci�n para el shader de teselaci�n
      if ( mTessellControlSew )
      {
        int j = 0;
        for ( j = 0; j < HResolTube; ++j )
        {
          index = (((( nodeAux.id - NEURODESP )*( globalDesp ))) - HResolTube + j );
          index += glb_nVerticesSoma;

          MeshDef::Point lNewVertexPoint = Mesh->points ( )[( index )];
          lNewVertexPoint += lSharpDirection*( nodeAux.radius/2.0 );

          //Add control sew vertexs
          vhandle = Mesh->add_vertex ( MeshDef::Point ( lNewVertexPoint[0], lNewVertexPoint[1], lNewVertexPoint[2]
                                       )
          );

          glb_meshVHandle[meshVHandleTotal] = vhandle;
          meshVHandleTotal++;
        }

        //Coser las nuevas costuras
        //////////////////////////////////////////////////////////////////////////////
        j = 0;
        for ( j = 0; j < ( HResolTube - 1 ); j++ )
          //for(j=0; j<1; j++)
        {
          glb_tubeFace_VHandles.clear ( );
          //index = ( ( ((nodeAux.id-NEURODESP)*(globalDesp))-HResolTube ) + ((j + despA)%HResolTube))		;
          index = (((( nodeAux.id - NEURODESP )*( globalDesp )) - HResolTube ) + j );
          index += glb_nVerticesSoma;
          glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

          //index = ( (nodeAux.id-NEURODESP)*(globalDesp) + ((j + despB)%HResolTube));
          index = ( meshVHandleTotal ) - HResolTube + j;
          glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

          //index = ( (nodeAux.id-NEURODESP)*(globalDesp) + ((j+1	+ despB)%HResolTube));
          index = ( meshVHandleTotal ) - HResolTube + j + 1;
          glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );
          Mesh->add_face ( glb_tubeFace_VHandles );

          glb_tubeFace_VHandles.clear ( );
          //index = ( ( ((nodeAux.id-NEURODESP)*(globalDesp) )-HResolTube ) + ((j + despA)%HResolTube) )		;
          index = (((( nodeAux.id - NEURODESP )*( globalDesp )) - HResolTube ) + j );
          index += glb_nVerticesSoma;
          glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

          //index = ( ((nodeAux.id-NEURODESP)*(globalDesp) ) + ((j+1	+ despB)%HResolTube));
          index = ( meshVHandleTotal ) - HResolTube + j + 1;
          glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

          //index = ( ( ((nodeAux.id-NEURODESP)*(globalDesp) )-HResolTube ) + ((j + 1 + despA)%HResolTube) )	;
          index = (((( nodeAux.id - NEURODESP )*( globalDesp )) - HResolTube ) + j + 1 );
          index += glb_nVerticesSoma;
          glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );
          Mesh->add_face ( glb_tubeFace_VHandles );
        }

        //Cerrado de las puntas
        glb_tubeFace_VHandles.clear ( );
        //index = ( ( ((nodeAux.id-NEURODESP)*(globalDesp))-HResolTube ) + ((j + despA)%HResolTube) )		;
        index = (((( nodeAux.id - NEURODESP )*( globalDesp )) - HResolTube ));
        index += glb_nVerticesSoma;
        glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

        //index = ( (nodeAux.id-NEURODESP)*(globalDesp) + ((j + despB)%HResolTube) );
        index = ( meshVHandleTotal - 1 );
        glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

        //index = ( (nodeAux.id-NEURODESP)*(globalDesp) + ((despB)%HResolTube) );
        index = ( meshVHandleTotal - 1 ) - HResolTube + 1;
        glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );
        Mesh->add_face ( glb_tubeFace_VHandles );

        glb_tubeFace_VHandles.clear ( );
        //index = ( ( ((nodeAux.id-NEURODESP)*(globalDesp) )-HResolTube ) + ((j + despA)%HResolTube) );
        index = (((( nodeAux.id - NEURODESP )*( globalDesp )) - HResolTube ) + j );
        index += glb_nVerticesSoma;
        glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

        //index = ( ((nodeAux.id-NEURODESP)*(globalDesp) ) + ((despB)%HResolTube) );
        index = ( meshVHandleTotal - 1 );
        glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

        //index = ( ( ((nodeAux.id-NEURODESP)*(globalDesp) )-HResolTube ) + ((despA)%HResolTube) );
        index = (((( nodeAux.id - NEURODESP )*( globalDesp )) - HResolTube ));
        index += glb_nVerticesSoma;
        glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );
        Mesh->add_face ( glb_tubeFace_VHandles );

        //////////////////////////////////////////////////////////////////////////////


        /*
				for(j=0; j<(HResolTube-1); j++)
				{
					glb_tubeFace_VHandles.clear();
						index = ( ( ((nodeAux.id-NEURODESP)*(globalDesp))-HResolTube ) + ((j + despA)%HResolTube))		;
						index+=glb_nVerticesSoma;
						glb_tubeFace_VHandles.push_back(glb_meshVHandle[index]);

						index = ( (nodeAux.id-NEURODESP)*(globalDesp) + ((j + despB)%HResolTube));
						index+=glb_nVerticesSoma;
						glb_tubeFace_VHandles.push_back(glb_meshVHandle[index]);

						index = ( (nodeAux.id-NEURODESP)*(globalDesp) + ((j+1	+ despB)%HResolTube));
						index+=glb_nVerticesSoma;
						glb_tubeFace_VHandles.push_back(glb_meshVHandle[index]);
					Mesh->add_face(glb_tubeFace_VHandles);

					glb_tubeFace_VHandles.clear();
						index = ( ( ((nodeAux.id-NEURODESP)*(globalDesp) )-HResolTube ) + ((j + despA)%HResolTube) )		;
						index+=glb_nVerticesSoma;
						glb_tubeFace_VHandles.push_back(glb_meshVHandle[index]);

						index = ( ((nodeAux.id-NEURODESP)*(globalDesp) ) + ((j+1	+ despB)%HResolTube));
						index+=glb_nVerticesSoma;
						glb_tubeFace_VHandles.push_back(glb_meshVHandle[index]);

						index = ( ( ((nodeAux.id-NEURODESP)*(globalDesp) )-HResolTube ) + ((j + 1 + despA)%HResolTube) )	;
						index+=glb_nVerticesSoma;
						glb_tubeFace_VHandles.push_back(glb_meshVHandle[index]);
					Mesh->add_face(glb_tubeFace_VHandles);
				}


				glb_tubeFace_VHandles.clear();
					index = ( ( ((nodeAux.id-NEURODESP)*(globalDesp))-HResolTube ) + ((j + despA)%HResolTube) )		;
					index+=glb_nVerticesSoma;
					glb_tubeFace_VHandles.push_back(glb_meshVHandle[index]);

					index = ( (nodeAux.id-NEURODESP)*(globalDesp) + ((j + despB)%HResolTube) );
					index+=glb_nVerticesSoma;
					glb_tubeFace_VHandles.push_back(glb_meshVHandle[index]);

					index = ( (nodeAux.id-NEURODESP)*(globalDesp) + ((despB)%HResolTube) );
					index+=glb_nVerticesSoma;
					glb_tubeFace_VHandles.push_back(glb_meshVHandle[index]);
				Mesh->add_face(glb_tubeFace_VHandles);

				glb_tubeFace_VHandles.clear();
					index = ( ( ((nodeAux.id-NEURODESP)*(globalDesp) )-HResolTube ) + ((j + despA)%HResolTube) );
					index+=glb_nVerticesSoma;
					glb_tubeFace_VHandles.push_back(glb_meshVHandle[index]);

					index = ( ((nodeAux.id-NEURODESP)*(globalDesp) ) + ((despB)%HResolTube) );
					index+=glb_nVerticesSoma;
					glb_tubeFace_VHandles.push_back(glb_meshVHandle[index]);

					index = ( ( ((nodeAux.id-NEURODESP)*(globalDesp) )-HResolTube ) + ((despA)%HResolTube) );
					index+=glb_nVerticesSoma;
					glb_tubeFace_VHandles.push_back(glb_meshVHandle[index]);
				Mesh->add_face(glb_tubeFace_VHandles);
				*/

        //Terminaci�n en punta
        if ( mSharpEnd )
        {
          nodeAux.position += lSharpDirection*nodeAux.radius;
        }

        //Add vertex termination
        vhandle = Mesh->add_vertex ( MeshDef::Point ( nodeAux.position[0], nodeAux.position[1], nodeAux.position[2]
                                     )
        );

        glb_meshVHandle[meshVHandleTotal] = vhandle;
        meshVHandleTotal++;

        //Add fan faces to close the dendrite over the control sew
        for ( int j = 1; j < ( HResolTube ); j++ )
        {
          //Superior circle
          glb_tubeFace_VHandles.clear ( );
          index = ( meshVHandleTotal - 1 );
          glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

          index = (( meshVHandleTotal - 1 ) - ( j ));
          glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

          index = (( meshVHandleTotal - 1 ) - ( j + 1 ));
          glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );
          Mesh->add_face ( glb_tubeFace_VHandles );
        }

        //Close fan
        glb_tubeFace_VHandles.clear ( );
        index = ( meshVHandleTotal - 1 );
        glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

        index = (( meshVHandleTotal - 1 ) - ( HResolTube ));
        glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

        index = (( meshVHandleTotal - 1 ) - ( 1 ));
        glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );
        Mesh->add_face ( glb_tubeFace_VHandles );

      }
      else
      {
        //Terminaci�n en punta
        if ( mSharpEnd )
        {
          nodeAux.position += lSharpDirection*nodeAux.radius;
        }

        //Add vertex termination
        vhandle = Mesh->add_vertex ( MeshDef::Point ( nodeAux.position[0], nodeAux.position[1], nodeAux.position[2]
                                     )
        );

        glb_meshVHandle[meshVHandleTotal] = vhandle;
        meshVHandleTotal++;

        int j = 0;
        //Add fan faces to close the dendrite
        for ( j = 0; j < ( HResolTube - 1 ); j++ )
        {
          //Superior circle
          glb_tubeFace_VHandles.clear ( );
          index = (((( nodeAux.id - NEURODESP )*( globalDesp ))) - HResolTube + j );
          index += glb_nVerticesSoma;
          glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

          index = ( meshVHandleTotal - 1 );
          glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

          index = (((( nodeAux.id - NEURODESP )*( globalDesp ))) - HResolTube + j + 1 );
          index += glb_nVerticesSoma;
          glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );
          Mesh->add_face ( glb_tubeFace_VHandles );
        }

        glb_tubeFace_VHandles.clear ( );
        index = (((( nodeAux.id - NEURODESP )*( globalDesp ))) - HResolTube + j );
        index += glb_nVerticesSoma;
        glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

        index = ( meshVHandleTotal - 1 );
        glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

        index = (((( nodeAux.id - NEURODESP )*( globalDesp ))) - HResolTube );
        index += glb_nVerticesSoma;
        glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );
        Mesh->add_face ( glb_tubeFace_VHandles );
      }
    }
    updateBaseMesh ( );
  }

  /*
		Solo hay q a�adir facetas al finalsegun los parents: desde el parent -> al nodo act desechando los q se conectan al soma
		calcular indice del nodo parent, crear un vertex handle y conectar la faceta.
	*/
  void NeuroSWC::connectConsecutiveTubes ( )
  {
    int numConnections = importer->getNumConnection ( );

    //int somaId=-1;

    SWCConnection nodeConnect;
    SWCNode nodeAux;
    SWCNode parentNodeAux;

    unsigned int globalDesp = HResolTube*VResolTube;
    unsigned int index;

    for ( int i = 0; i < ( numConnections - 1 ); ++i )
    {
      nodeConnect = importer->getConnectionAt ( i );

      if ( !nodeConnect.mTermination )
      {

        nodeAux = importer->getElementAt ( nodeConnect.fin );
        parentNodeAux = importer->getElementAt ( nodeConnect.ini );

        int j = 0;

        //////////////////////////////////////////////////////////////////////////////
        //Conecciones hacia adelante (Las normales de toda la vida)
        //Hemos de tener en cuenta los giros de los tubos

        //Calc the nearest vertex
        //Indice inicial de la zona A (final del tubo anterior)
        unsigned int lIdxA;
        MeshDef::Point lInitPointA ( 0, 0, 0 );

        //Indice inicial de la zona B (principio del tubo siguiente)
        unsigned int lIdxB;
        MeshDef::Point lInitPointB ( 0, 0, 0 );

        OpenMesh::Vec3f lTmpVecResult ( 0, 0, 0 );

        //Distancia m�nima entre v�rtices
        float dis = 10E10;

        //Desplazamiento con los vertices a coser
        //Hay que comenzar desde los dos v�rtices con distancia m�nima
        unsigned int despA = 0;
        unsigned int despTmpA = 0;

        unsigned int despB = 0;
        unsigned int despTmpB = 0;

        for ( int i = 0; i < ( HResolTube - 1 ); i++ )
        {
          lIdxA = (((( nodeAux.id - NEURODESP )*( globalDesp )) - HResolTube ) + i );
          lIdxA += glb_nVerticesSoma;

          lInitPointA = Mesh->points ( )[( lIdxA )];

          ////Indices con los que coser
          for ( j = 0; j < ( HResolTube - 1 ); j++ )
          {
            lIdxB = (( nodeAux.id - NEURODESP )*( globalDesp ) + j );
            lIdxB += glb_nVerticesSoma;

            lInitPointB = Mesh->points ( )[( lIdxB )];
            lTmpVecResult = OpenMesh::Vec3f ( lInitPointA - lInitPointB );

            if ( lTmpVecResult.length ( ) < dis )
            {
              dis = lTmpVecResult.length ( );

              despB = despTmpB;
              despA = despTmpA;
            }
            ++despTmpB;
          }

          ++despTmpA;
          despTmpB = 0;
        }


        //Costura
        for ( j = 0; j < ( HResolTube - 1 ); j++ )
        {
          glb_tubeFace_VHandles.clear ( );
          index = (((( nodeAux.id - NEURODESP )*( globalDesp )) - HResolTube ) + (( j + despA )%HResolTube ));
          index += glb_nVerticesSoma;
          glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

          index = (( nodeAux.id - NEURODESP )*( globalDesp ) + (( j + despB )%HResolTube ));
          index += glb_nVerticesSoma;
          glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

          index = (( nodeAux.id - NEURODESP )*( globalDesp ) + (( j + 1 + despB )%HResolTube ));
          index += glb_nVerticesSoma;
          glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );
          Mesh->add_face ( glb_tubeFace_VHandles );

          glb_tubeFace_VHandles.clear ( );
          index = (((( nodeAux.id - NEURODESP )*( globalDesp )) - HResolTube ) + (( j + despA )%HResolTube ));
          index += glb_nVerticesSoma;
          glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

          index = ((( nodeAux.id - NEURODESP )*( globalDesp )) + (( j + 1 + despB )%HResolTube ));
          index += glb_nVerticesSoma;
          glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

          index = (((( nodeAux.id - NEURODESP )*( globalDesp )) - HResolTube ) + (( j + 1 + despA )%HResolTube ));
          index += glb_nVerticesSoma;
          glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );
          Mesh->add_face ( glb_tubeFace_VHandles );
        }

        glb_tubeFace_VHandles.clear ( );
        index = (((( nodeAux.id - NEURODESP )*( globalDesp )) - HResolTube ) + (( j + despA )%HResolTube ));
        index += glb_nVerticesSoma;
        glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

        index = (( nodeAux.id - NEURODESP )*( globalDesp ) + (( j + despB )%HResolTube ));
        index += glb_nVerticesSoma;
        glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

        index = (( nodeAux.id - NEURODESP )*( globalDesp ) + (( despB )%HResolTube ));
        index += glb_nVerticesSoma;
        glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );
        Mesh->add_face ( glb_tubeFace_VHandles );

        glb_tubeFace_VHandles.clear ( );
        index = (((( nodeAux.id - NEURODESP )*( globalDesp )) - HResolTube ) + (( j + despA )%HResolTube ));
        index += glb_nVerticesSoma;
        glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

        index = ((( nodeAux.id - NEURODESP )*( globalDesp )) + (( despB )%HResolTube ));
        index += glb_nVerticesSoma;
        glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

        index = (((( nodeAux.id - NEURODESP )*( globalDesp )) - HResolTube ) + (( despA )%HResolTube ));
        index += glb_nVerticesSoma;
        glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );
        Mesh->add_face ( glb_tubeFace_VHandles );

        //////////////////////////////////////////////////////////////////////////////
      }
    }

    updateBaseMesh ( );
  }

  /*
	Old version
	*/
//
//	void NeuroSWC::connectNoConsecutiveTubes()
//	{
//		int numConnections=importer->getNumConnection();
//
//
//		SWCConnection	nodeConnect;
//		SWCNode			nodeAux;
//		SWCNode			parentNodeAux;
//
//		unsigned int globalDesp=HResolTube*VResolTube;
//		unsigned int index;
//
//		for (int i=0;i<(numConnections-1);++i)
//		{
//			nodeConnect		= importer->getConnectionAt(i);
//			nodeAux			= importer->getElementAt(nodeConnect.fin);
//			parentNodeAux	= importer->getElementAt(nodeConnect.ini);
//
//			int j=0;
//			unsigned int consecValues	=nodeAux.id-nodeAux.parent;
//			float dis					= 10E10;
//			unsigned int indexCandidate	=0;
//			MeshDef::Point	lBariCenter(0,0,0);
//			MeshDef::Point	ltmpVertex(0,0,0);
//
//			//eliminar faceta del indexCandidate
//			MeshDef::VertexHandle   vhandle;
//			MeshDef::HalfedgeHandle hehandle;
//			MeshDef::FaceHandle		fhandle;
//
//			MeshDef::FaceHandle		fCandHandle;
//			MeshDef::VertexFaceIter	lVFIter;
//
//			float lAuxDis	=	10E10;
//
//			//No consecutive segments
//			//Requiere connection at inition
//			if ( (consecValues>1)
//				  || (nodeAux.parent==mSomaId)
//				  //|| (nodeConnect.mTermination)
//				  )
//			{
//
//				//Calcular el centro de los v�rtices del nodo
//				for(j=0; j<(HResolTube-1); j++)
//				{
//					//Calc the index of the vertex
//					index = ( (nodeAux.id-NEURODESP-1)*(globalDesp) + j);
//					index+=glb_nVerticesSoma;
//
//					lBariCenter+= Mesh->points()[(index)];
//				}
//
//				lBariCenter/=(float)(HResolTube-1);
//
//
//				//Conexion de tubos normales
//				if (nodeAux.parent!=mSomaId)
//				{
//					//Conexiones Problematicas en el inicio de la conexion ;P
//
//					//Calc the index of the vertex
//					//!!!
//					//Necesita reservar mas memoria si se va a utilizar
//					//Add baricentgric for debug mode
//					//MeshDef::VertexHandle	vhandle = Mesh->add_vertex(lBariCenter);
//					//glb_meshVHandle[meshVHandleTotal] = vhandle;
//					//meshVHandleTotal++;
//
//					//Calcular el m�s cercano del parent al centro calculado
//					indexCandidate = ( ( ((parentNodeAux.id-NEURODESP)*(globalDesp))) );
//					indexCandidate+=glb_nVerticesSoma;
//
//					OpenMesh::Vec3f ltmpVertexResult(lBariCenter
//													 - Mesh->points()[(indexCandidate)]);
//
//					dis=ltmpVertexResult.length();
//
//					for(j=1; j<(HResolTube-1); j++)
//					{
//						index = ( ( ((parentNodeAux.id-NEURODESP)*(globalDesp))) + j);
//						index+=glb_nVerticesSoma;
//
//						ltmpVertex = Mesh->points()[(index)];
//						ltmpVertexResult = OpenMesh::Vec3f(lBariCenter - ltmpVertex);
//
//						if (ltmpVertexResult.length()< dis)
//						{
//							indexCandidate = index;
//							dis=ltmpVertexResult.length();
//						}
//					}
//
//					//////////////////////////////////////////////////////////////////////////////////////////////////////////
//					//Ahora que tenemos el vertice m�s cercano, hay que calcular la
//					//faceta cuyo centro es el mas cercano al baricentro, eliminarla
//					//y soldar con los vertices subyacentes.
//					//!!!Para el soma tb deber�a funcionar de esta manera, pero no es asi
//					////////////////////////////////////////////////////////////////////////////////////////////////////
//
//					vhandle		= Mesh->vertex_handle(indexCandidate);
//					hehandle	= Mesh->halfedge_handle(vhandle);
//					fhandle		= Mesh->face_handle(hehandle);
//
//					//Iterate over the faces conected to tis vertex for selec the rearest face
//					//Prueba, borrar las facetas para saber que son las buenas las que se estan recorriendo
//
//					fCandHandle = fhandle;
//					hehandle	= Mesh->halfedge_handle(fhandle);
//
//					Mesh->calc_face_centroid(fhandle,ltmpVertex);
//					dis				=	OpenMesh::Vec3f(ltmpVertex - lBariCenter).length();
//					//lAuxDis	=	10E10;
//
//					// circulate around the current vertex
//					for (lVFIter=Mesh->vf_iter(vhandle); lVFIter.is_valid(); ++lVFIter)
//					{
//
//						fCandHandle = Mesh->face_handle(lVFIter);
//						Mesh->calc_face_centroid(fCandHandle,ltmpVertex);
//						lAuxDis = OpenMesh::Vec3f(ltmpVertex - lBariCenter).length();
//
//						if ( (lAuxDis<dis) && (fCandHandle.is_valid()) )
//						{
//							fhandle		= fCandHandle;
//							hehandle	= Mesh->halfedge_handle(fhandle);
//							dis			= lAuxDis;
//						}
//					}
//
//				//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//				}
//				//Conexion con el soma
//				else
//				{
//
//					//Calcular el vertice mas cercano de los q conforma el soma
//					//Ponerlo como candidate para eliminar la faceta y q se haga la costura
//					indexCandidate =	0;
//					//indexCandidate+=glb_nVerticesSoma;
//
//					OpenMesh::Vec3f ltmpVertexResult(lBariCenter
//													 - Mesh->points()[(indexCandidate)]);
//					dis				=	ltmpVertexResult.length();
//
//					//for(j=1; j<glb_nVerticesSoma; j++)
//					//{
//					//	index = j;
//
//					//	ltmpVertex = Mesh->points()[(index)];
//					//	ltmpVertexResult = OpenMesh::Vec3f(lBariCenter - ltmpVertex);
//
//					//	if (ltmpVertexResult.length()< dis)
//					//	{
//					//		indexCandidate = index;
//					//		dis=ltmpVertexResult.length();
//					//	}
//					//}
//
//					//Circulate around the faces
//					for (int lVFIter=0; lVFIter<mSomaNumFaces; ++lVFIter)
//					{
//						fCandHandle = Mesh->face_handle(lVFIter);
//						Mesh->calc_face_centroid(fCandHandle,ltmpVertex);
//						lAuxDis = OpenMesh::Vec3f(ltmpVertex - lBariCenter).length();
//						if ( (lAuxDis<dis) && (fCandHandle.is_valid()) )
//						{
//							fhandle		= fCandHandle;
//							hehandle	= Mesh->halfedge_handle(fhandle);
//							dis			= lAuxDis;
//						}
//					}
//
//					//continue;
//
//				}
//
//				MeshDef::VertexHandle  vIdx[3];//, vIdx2, vIdx3;
//
//				//Ojo con el orden
//				vIdx[0]		= Mesh->to_vertex_handle(hehandle);
//				hehandle	= Mesh->next_halfedge_handle(hehandle);
//				vIdx[2]		= Mesh->to_vertex_handle(hehandle);
//				hehandle	= Mesh->next_halfedge_handle(hehandle);
//				vIdx[1]		= Mesh->to_vertex_handle(hehandle);
//
//				if (fhandle.is_valid())
//				{
//					Mesh->delete_face(fhandle ,false);
//				}
//				else
//				{
//					//Si no es valida, es que ya ha sido marcada para borrar por otro anterior para coser,
//					//Hay que circular alrededor de ella para detrminar la m�s cecana
//				}
//
//
///*
//				//Calc the signed area to determintate the order to sew
//				//-----------------------------------------------------------------------
//				OpenMesh::Vec3f lVertex[3];
//
//				for (int i = 0; i<3; ++i)
//				{
//					index = ( ( ((parentNodeAux.id-NEURODESP)*(globalDesp))) + i);
//					if (nodeAux.parent==mSomaId)
//					{
//						index+=glb_nVerticesSoma;
//					}
//
//					lVertex[i] = OpenMesh::Vec3f(Mesh->points()[(index)]);
//				}
//
//				float lSigAreaTube = SignedArea(lVertex[0],lVertex[1],lVertex[2]);
//
//				lVertex[0] = OpenMesh::Vec3f(Mesh->points()[(vIdx[0].idx())]);
//				lVertex[1] = OpenMesh::Vec3f(Mesh->points()[(vIdx[1].idx())]);
//				lVertex[2] = OpenMesh::Vec3f(Mesh->points()[(vIdx[2].idx())]);
//
//				float lSigAreaTriang = SignedArea(lVertex[0],lVertex[1],lVertex[2]);
//
//				//!!!
//				if ( (lSigAreaTube*lSigAreaTriang)<0)
//				{
//					//MeshDef::VertexHandle  lVHandleTmp = vIdx[1];
//					//vIdx[1] = vIdx[2];
//					//vIdx[2] = lVHandleTmp;
//				}
//				//-----------------------------------------------------------------------
//*/
//
//				//Calc de desps in sections A and B to determinate de sew
//				//-----------------------------------------------------------------------
//				unsigned int lIdxA;
//				MeshDef::Point	lInitPointA(0,0,0);
//
//				//Indice inicial de la zona B (principio del tubo siguiente)
//				unsigned int lIdxB;
//				MeshDef::Point lInitPointB (0,0,0);
//
//				OpenMesh::Vec3f lTmpVecResult(0,0,0);
//
//				//Distancia m�nima entre v�rtices
//				dis			=10E10;
//
//				//Desplazamiento con los vertices a coser
//				//Hay que comenzar desde los dos v�rtices con distancia m�nima
//				unsigned int despA		=0;
//				unsigned int despTmpA	=0;
//
//				unsigned int despB		=0;
//				unsigned int despTmpB	=0;
//
//				for (int i=0;i<3; i++)
//				{
//					//lIdxA = ( ( ((nodeAux.id-NEURODESP)*(globalDesp))-HResolTube ) + i);
//					//lIdxA +=glb_nVerticesSoma;
//					lIdxA = vIdx[i].idx();
//
//					lInitPointA = Mesh->points()[(lIdxA)];
//
//					////Indices con los que coser
//					for(j=0; j<(HResolTube-1); j++)
//					{
//						lIdxB = ( (nodeAux.id-NEURODESP)*(globalDesp) + j		);
//						lIdxB+=glb_nVerticesSoma;
//
//						lInitPointB = Mesh->points()[(lIdxB)];
//						lTmpVecResult = OpenMesh::Vec3f(lInitPointA - lInitPointB);
//
//						if (lTmpVecResult.length() < dis)
//						{
//							dis			=lTmpVecResult.length();
//
//							despB		= despTmpB;
//							despA		= despTmpA;
//						}
//						++despTmpB;
//					}
//
//					++despTmpA;
//					despTmpB	= 0;
//				}
//				//-----------------------------------------------------------------------
//
//				//Costura
//				//-----------------------------------------------------------------------------------------
//				//Primeras dos costuras
//				j=0;
//				{
//					glb_tubeFace_VHandles.clear();
//						glb_tubeFace_VHandles.push_back(glb_meshVHandle[vIdx[( (j+despA)%HOLENUMVERTEX) ].idx()]);
//
//						index = ( (nodeAux.id-NEURODESP-1)*(globalDesp) + j + despB);
//						index+=glb_nVerticesSoma;
//						glb_tubeFace_VHandles.push_back(glb_meshVHandle[index]);
//
//						index = ( (nodeAux.id-NEURODESP-1)*(globalDesp) + j+1	+ despB);
//						index+=glb_nVerticesSoma;
//						glb_tubeFace_VHandles.push_back(glb_meshVHandle[index]);
//					Mesh->add_face(glb_tubeFace_VHandles);
//
//					glb_tubeFace_VHandles.clear();
//						glb_tubeFace_VHandles.push_back(glb_meshVHandle[vIdx[( (j+despA)%HOLENUMVERTEX) ].idx()]);
//
//						index = ( (nodeAux.id-NEURODESP-1)*(globalDesp) + j + 1 + despB);
//						index+=glb_nVerticesSoma;
//						glb_tubeFace_VHandles.push_back(glb_meshVHandle[index]);
//
//
//						glb_tubeFace_VHandles.push_back(glb_meshVHandle[vIdx[( (j+1+despA)%HOLENUMVERTEX) ].idx()]);
//					Mesh->add_face(glb_tubeFace_VHandles);
//				}
//
//				//Costuras fijadas al v�rtice m�s alejado
//				unsigned int fixVextex=( (j + 1 + despA)%HOLENUMVERTEX);
//
//				for(; j<(HResolTube-1); j++)
//				{
//					glb_tubeFace_VHandles.clear();
//						glb_tubeFace_VHandles.push_back(glb_meshVHandle[vIdx[fixVextex].idx()]);
//
//						index = ( (nodeAux.id-NEURODESP-1)*(globalDesp) + ( (j + despB)%HResolTube) );
//						index+=glb_nVerticesSoma;
//						glb_tubeFace_VHandles.push_back(glb_meshVHandle[index]);
//
//						index = ( (nodeAux.id-NEURODESP-1)*(globalDesp) + ( (j + 1	+ despB)%HResolTube) );
//						index+=glb_nVerticesSoma;
//						glb_tubeFace_VHandles.push_back(glb_meshVHandle[index]);
//					Mesh->add_face(glb_tubeFace_VHandles);
//				}
//
//				//Costuras de cierre (3 Costuras)
//				glb_tubeFace_VHandles.clear();
//
//					glb_tubeFace_VHandles.push_back(glb_meshVHandle[vIdx[fixVextex].idx()]);
//
//					index = ( (nodeAux.id-NEURODESP-1)*(globalDesp) + ( (j + despB)%HResolTube) );
//					index+=glb_nVerticesSoma;
//					glb_tubeFace_VHandles.push_back(glb_meshVHandle[index]);
//
//					glb_tubeFace_VHandles.push_back(glb_meshVHandle[vIdx[ ( (fixVextex+1)%HOLENUMVERTEX ) ].idx()]);
//
//				Mesh->add_face(glb_tubeFace_VHandles);
//
//				glb_tubeFace_VHandles.clear();
//
//					glb_tubeFace_VHandles.push_back(glb_meshVHandle[vIdx[ ( (fixVextex+1)%HOLENUMVERTEX ) ].idx()]);
//
//					index = ( (nodeAux.id-NEURODESP-1)*(globalDesp) + ( (j + despB)%HResolTube) );
//					index+=glb_nVerticesSoma;
//					glb_tubeFace_VHandles.push_back(glb_meshVHandle[index]);
//
//					glb_tubeFace_VHandles.push_back(glb_meshVHandle[vIdx[ ( (fixVextex+2)%HOLENUMVERTEX ) ].idx()]);
//
//				Mesh->add_face(glb_tubeFace_VHandles);
//
//				glb_tubeFace_VHandles.clear();
//
//					glb_tubeFace_VHandles.push_back(glb_meshVHandle[vIdx[ ( (fixVextex+2)%HOLENUMVERTEX ) ].idx()]);
//
//					index = ( (nodeAux.id-NEURODESP-1)*(globalDesp) + ( (j + despB)%HResolTube) );
//					index+=glb_nVerticesSoma;
//					glb_tubeFace_VHandles.push_back(glb_meshVHandle[index]);
//
//					index = ( (nodeAux.id-NEURODESP-1)*(globalDesp) + ( (j + 1 + despB)%HResolTube) );
//					index+=glb_nVerticesSoma;
//					glb_tubeFace_VHandles.push_back(glb_meshVHandle[index]);
//
//				Mesh->add_face(glb_tubeFace_VHandles);
//				//-----------------------------------------------------------------------------------------
//
//			}
//		}
//
//		//Free the memory of deleted objects
//		Mesh->garbage_collection();
//
//		updateBaseMesh();
//	}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  void NeuroSWC::connectNoConsecutiveTubes ( )
  {
    int numConnections = importer->getNumConnection ( );

    SWCConnection nodeConnect;
    SWCNode nodeAux;
    SWCNode parentNodeAux;

    unsigned int globalDesp = HResolTube*VResolTube;
    unsigned int index;

    int j = 0;
    unsigned int consecValues;
    unsigned int indexCandidate;
    float dis;

    MeshDef::Point lBariCenter ( 0, 0, 0 );
    MeshDef::Point lOpositeBariCenter ( 0, 0, 0 );

    MeshDef::Point ltmpVertex ( 0, 0, 0 );
    OpenMesh::Vec3f ltmpVertexResult ( 0, 0, 0 );

    MeshDef::VertexHandle vhandle;
    MeshDef::HalfedgeHandle hehandle;
    MeshDef::FaceHandle fhandle;

    MeshDef::VertexHandle vIdx[3];
    float lAuxDis = 10E10;

    OpenMesh::Vec3f lVertex[3];
    MeshDef::FaceHandle fCandHandle;
    MeshDef::VertexFaceIter lVFIter;

    float lSigAreaTriang = 0;
    float lSigAreaTube = 0;

    unsigned int lIdxA;
    unsigned int lIdxB;

    MeshDef::Point lInitPointA ( 0, 0, 0 );
    MeshDef::Point lInitPointB ( 0, 0, 0 );
    OpenMesh::Vec3f lTmpVecResult ( 0, 0, 0 );

    unsigned int despA = 0;
    unsigned int despB = 0;
    unsigned int despTmpA = 0;
    unsigned int despTmpB = 0;

    for ( int i = 0; i < ( numConnections - 1 ); ++i )
    {
      //Inicializations the local vars of the segment to sew
      nodeConnect = importer->getConnectionAt ( i );
      nodeAux = importer->getElementAt ( nodeConnect.fin );
      parentNodeAux = importer->getElementAt ( nodeConnect.ini );

      j = 0;
      consecValues = nodeAux.id - nodeAux.parent;
      dis = 10E10;
      indexCandidate = 0;

      lBariCenter = MeshDef::Point ( 0, 0, 0 );
      ltmpVertex = MeshDef::Point ( 0, 0, 0 );

      //No consecutive segments, requiere connection at inition
      //and no conected to the soma
      if (( consecValues > 1 )
        && ( nodeAux.parent != mSomaId )
        )
      {

        //->Calcular el centro de los v�rtices del nodo
        for ( j = 0; j < HResolTube; j++ )
        {
          //Calc the index of the vertex
          //Se resta uno para retroceder en la malla una distancia global desp ya que estamos en una bifurcaci�n
          index = (( nodeAux.id - NEURODESP - 1 )*( globalDesp ) + j );
          index += glb_nVerticesSoma;
          lBariCenter += Mesh->points ( )[( index )];
        }

        lBariCenter /= ( float ) ( HResolTube );

        //->Utilizamos el oposite para aseguranos de que estamos fuera del tubo
        for ( j = 0; j < HResolTube; j++ )
        {
          //Calc the index of the vertex
          index = (( nodeAux.id - NEURODESP )*( globalDesp ) - ( HResolTube ) + j );
          index += glb_nVerticesSoma;
          lOpositeBariCenter += Mesh->points ( )[( index )];
        }

        lOpositeBariCenter /= ( float ) ( HResolTube );

        //->Calcular el v�rtice m�s cercano del parent al centro del tubo calculado
        indexCandidate = (((( parentNodeAux.id - NEURODESP )*( globalDesp ))));
        indexCandidate += glb_nVerticesSoma;

        //Utilizamos el opossite para salirnos del tubo (no estar por detr�s del tri�ngulo)
        ltmpVertexResult = OpenMesh::Vec3f ( lOpositeBariCenter
                                               - Mesh->points ( )[( indexCandidate )] );
        dis = ltmpVertexResult.length ( );

        for ( j = 0; j < ( HResolTube ); j++ )
        {
          index = (((( parentNodeAux.id - NEURODESP )*( globalDesp ))) + j );
          index += glb_nVerticesSoma;

          ltmpVertex = Mesh->points ( )[( index )];
          ltmpVertexResult = OpenMesh::Vec3f ( lOpositeBariCenter - ltmpVertex );

          if ( ltmpVertexResult.length ( ) < dis )
          {
            indexCandidate = index;
            dis = ltmpVertexResult.length ( );
          }
        }

        //Ahora que tenemos el vertice m�s cercano, hay que calcular la
        //faceta cuyo centro es el mas cercano al baricentro, eliminarla
        //y soldar con los vertices subyacentes.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        //Eliminar faceta del indexCandidate
        vhandle = Mesh->vertex_handle ( indexCandidate );

///////////////////////////////////////////////////////////////////////////////////////////////////////////

//New sew version

        MeshDef::FaceHandle lFaceHandleCandidate;
        OpenMesh::Vec3f lNormalFace ( 0, 0, 0 );
        OpenMesh::Vec3f lVecBarCent ( 0, 0, 0 );

        switch ( SWE_VER )
        {
          case 0:
          {
            //Select the nearest face
            MeshDef::ConstVertexFaceIter CVFIter;
            MeshDef::FaceHandle lAuxFaceHandle;

            unsigned int lTmpIniIndexTube = (((( nodeAux.id - NEURODESP - 1 )*( globalDesp ))));
            lTmpIniIndexTube += glb_nVerticesSoma;
            unsigned int lTmpNextIndexTube = (((( nodeAux.id - NEURODESP - 1 )*( globalDesp ))) + ( 1 ));
            lTmpNextIndexTube += glb_nVerticesSoma;

            //Calculate the vectors
            OpenMesh::Vec3f lVecBarIni = OpenMesh::Vec3f (

              MeshDef::Point ( lBariCenter[0], lBariCenter[1], lBariCenter[2] )
                - Mesh->points ( )[lTmpIniIndexTube]
            );


            //Calculate the vectors
            //OpenMesh::Vec3f	lVecBarNext	=	OpenMesh::Vec3f(
            //													Mesh->points()[lTmpNextIndexTube]
            //													- MeshDef::Point(lBariCenter[0]
            //																	,lBariCenter[1]
            //																	,lBariCenter[2])
            //												);

            OpenMesh::Vec3f lVecIniNext = OpenMesh::Vec3f (
              Mesh->points ( )[lTmpNextIndexTube]
                - Mesh->points ( )[lTmpIniIndexTube]
            );


            //OpenMesh::Vec3f lCrossTube = OpenMesh::cross(lVecBarNext, lVecBarIni);
            OpenMesh::Vec3f lCrossTube = OpenMesh::cross ( lVecIniNext, lVecBarIni );

            lCrossTube.normalize ( );

            dis = 10E10;
            CVFIter = Mesh->cvf_iter ( vhandle );
            lFaceHandleCandidate = Mesh->face_handle ( CVFIter->idx ( ));

            for ( ; CVFIter.is_valid ( ); ++CVFIter )
            {
              lAuxFaceHandle = Mesh->face_handle ( CVFIter->idx ( ));
              Mesh->calc_face_centroid ( lAuxFaceHandle, ltmpVertex );

              ltmpVertexResult = OpenMesh::Vec3f ( lBariCenter - ltmpVertex );

              if ( ltmpVertexResult.length ( ) < dis )
              {
                ////Test the normal of the face
                lNormalFace = Mesh->normal ( lAuxFaceHandle );
                lNormalFace.normalize ( );
                //float lAuxDot	= OpenMesh::dot(lCrossTube,lNormalFace);

                lVecBarCent = OpenMesh::Vec3f (
                  MeshDef::Point ( lBariCenter[0], lBariCenter[1], lBariCenter[2] )
                    - ltmpVertex
                );

                lVecBarCent.normalize ( );

                //float lAuxDot	= OpenMesh::dot(lVecBarCent,lNormalFace);
                float lAuxDot = OpenMesh::dot ( lCrossTube, lNormalFace );

                float lAuxAngle = acos ( lAuxDot );

                //-->>De momento se necesita que se coja siempre la mas cercana
                if ( fabs ( RADTODEG ( lAuxAngle )) < 90 )
                {
                  lFaceHandleCandidate = lAuxFaceHandle;
                  dis = ltmpVertexResult.length ( );
                }
              }
            }

            ////Recoger todos los vertices de esa faceta
            //MeshDef::ConstFaceVertexIter		fvIt	=	Mesh->cfv_iter(lFaceHandleCandidate);
            //vIdx[0]		= fvIt.handle(); ++fvIt;
            //vIdx[1]		= fvIt.handle(); ++fvIt;
            //vIdx[2]		= fvIt.handle();;

            //Recoger todos los vertices de esa faceta
            //MeshDef::ConstFaceVertexIter	CFVIter;
            //unsigned int t=0;
            //for (CFVIter=Mesh->cfv_iter(lFaceHandleCandidate);CFVIter;++CFVIter)
            //{
            //	vIdx[t]		= Mesh->vertex_handle(CFVIter);

            //	++t;
            //}

            MeshDef::ConstFaceHalfedgeIter lCFHIter;
            unsigned int t = 0;
            lCFHIter = Mesh->cfh_iter ( lFaceHandleCandidate );

            vIdx[0] = Mesh->to_vertex_handle ( *lCFHIter );
            ++lCFHIter;
            vIdx[2] = Mesh->to_vertex_handle ( *lCFHIter );
            ++lCFHIter;
            vIdx[1] = Mesh->to_vertex_handle ( *lCFHIter );

            //for (;lCFHIter;++lCFHIter)
            //{
            //	vIdx[t]		= Mesh->to_vertex_handle(*lCFHIter);
            //	++t;
            //}

            lNormalFace = Mesh->normal ( lFaceHandleCandidate );
            lNormalFace.normalize ( );

            if ( lFaceHandleCandidate.is_valid ( ))
              Mesh->delete_face ( lFaceHandleCandidate, false );
            break;
          }

///////////////////////////////////////////////////////////////////////////////////////////////////////////


            //Old selection vertices
          case 1:
          {
            hehandle = Mesh->halfedge_handle ( vhandle );
            fhandle = Mesh->face_handle ( hehandle );

            //Iterate over the faces conected to this vertex for selec the rearest face
            //Prueba, borrar las facetas para saber que son las buenas las que se estan recorriendo

            fCandHandle = fhandle;

            Mesh->calc_face_centroid ( fhandle, ltmpVertex );
            dis = OpenMesh::Vec3f ( ltmpVertex - lBariCenter ).length ( );
            lAuxDis = 10E10;

            //Circulate around the faces of the current vertex
            //to found the nearest face
            for ( lVFIter = Mesh->vf_iter ( vhandle ); lVFIter.is_valid ( ); ++lVFIter )
            {
              //---fCandHandle		=	Mesh->face_handle(lVFIter);
              fCandHandle = *lVFIter;
              Mesh->calc_face_centroid ( fCandHandle, ltmpVertex );
              lAuxDis = OpenMesh::Vec3f ( ltmpVertex - lBariCenter ).length ( );
              if ( lAuxDis < dis )
              {
                fhandle = fCandHandle;
                hehandle = Mesh->halfedge_handle ( fhandle );
              }
            }

            //Ojo con el orden
            vIdx[0] = Mesh->to_vertex_handle ( hehandle );
            hehandle = Mesh->next_halfedge_handle ( hehandle );
            vIdx[2] = Mesh->to_vertex_handle ( hehandle );
            hehandle = Mesh->next_halfedge_handle ( hehandle );
            vIdx[1] = Mesh->to_vertex_handle ( hehandle );

            if ( fhandle.is_valid ( ))
              Mesh->delete_face ( fhandle, false );

            break;
          }
        }
///////////////////////////////////////////////////////////////////////////////////////////////////////////

        //Determinate the order to sew
        //----------------------------
        //Calcular la distancia m�nima entre los dos conjuntos de v�rtices a coser
        //Todos los de uno (3) contra todos los del otro (HResol*VerResol)
        //Almacenando los desplazamientos parciles de cada uno para la costura

        //Indice inicial del tiangulo suprimido
        lInitPointA = MeshDef::Point ( 0, 0, 0 );

        //Indice inicial de la zona B (principio del tubo siguiente)
        lInitPointB = MeshDef::Point ( 0, 0, 0 );

        lTmpVecResult = OpenMesh::Vec3f ( 0, 0, 0 );

        //Distancia m�nima entre v�rtices
        dis = 10E10;

        //Desplazamiento con los vertices a coser
        //Hay que comenzar desde los dos v�rtices con distancia m�nima
        despA = despTmpA = despB = despTmpB = 0;

        for ( int i = 0; i < 3; i++ )
        {
          lIdxA = vIdx[i].idx ( );
          lInitPointA = Mesh->points ( )[( lIdxA )];

          //Indices con los que coser
          for ( j = 0; j < HResolTube; j++ )
          {
            lIdxB = (( nodeAux.id - NEURODESP )*( globalDesp ) + j );
            lIdxB += glb_nVerticesSoma;

            lInitPointB = Mesh->points ( )[( lIdxB )];
            lTmpVecResult = OpenMesh::Vec3f ( lInitPointA - lInitPointB );

            if ( lTmpVecResult.length ( ) < dis )
            {
              dis = lTmpVecResult.length ( );
              despB = despTmpB;
              despA = despTmpA;
            }
            ++despTmpB;
          }

          ++despTmpA;
          despTmpB = 0;
        }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //Old version
        //!!!Determinar orden de cosido de los vertices
        //Determinar si se invierten los indices de los vertices a coser
/*
				unsigned int lIndexCand = ( (nodeAux.id-NEURODESP-1)*(globalDesp) + 1 + despB);
				lIndexCand				+= glb_nVerticesSoma;

				unsigned int lNextIndexCand = ( (1 + despB)%HOLENUMVERTEX);
				unsigned int lPrevIndexCand = ( ((HOLENUMVERTEX-1) + despB)%HOLENUMVERTEX);

				float distCCW	= OpenMesh::Vec3f(Mesh->points()[lIndexCand]
									- Mesh->points()[lNextIndexCand]).length();


				float distCW	= OpenMesh::Vec3f(Mesh->points()[lIndexCand]
									- Mesh->points()[lPrevIndexCand]).length();

				if (distCW<distCCW)
				{
					//MeshDef::VertexHandle lAuxVHandle;
					//lAuxVHandle						=	vIdx[(despB+1)%HOLENUMVERTEX];
					//vIdx[(despB+1)%HOLENUMVERTEX]	=	vIdx[(despB+2)%HOLENUMVERTEX];
					//vIdx[(despB+2)%HOLENUMVERTEX]	=	lAuxVHandle;
				}
*/
        //Costura

        //Angle method evaluation
        //Index Vertex int the tube
        unsigned int lIndexTube = (((( nodeAux.id - NEURODESP - 1 )*( globalDesp ))) + (( despB )%HResolTube ));
        lIndexTube += glb_nVerticesSoma;
        unsigned int lNextIndexTube = (((( nodeAux.id - NEURODESP - 1 )*( globalDesp ))) + (( 1 + despB )%HResolTube ));
        lNextIndexTube += glb_nVerticesSoma;
        unsigned int lPrevIndexTube =
          (((( nodeAux.id - NEURODESP - 1 )*( globalDesp ))) + ((( HResolTube - 1 ) + despB )%HResolTube ));
        lPrevIndexTube += glb_nVerticesSoma;

        unsigned int lIndexHole = vIdx[(( despA )%HOLENUMVERTEX )].idx ( );
        unsigned int lNextIndexHole = vIdx[(( 1 + despA )%HOLENUMVERTEX )].idx ( );
        unsigned int lPrevIndexHole = vIdx[(( HOLENUMVERTEX - 1 ) + despA )%HOLENUMVERTEX].idx ( );

        //Calculate the vectors
        OpenMesh::Vec3f lVecAB = OpenMesh::Vec3f (
          Mesh->points ( )[lNextIndexTube]
            - Mesh->points ( )[lIndexTube]
        );

        OpenMesh::Vec3f lVecAC = OpenMesh::Vec3f (
          Mesh->points ( )[lPrevIndexTube]
            - Mesh->points ( )[lIndexTube]
        );

        //Cross product == producto vectorial
        OpenMesh::Vec3f lCrossAB_AC = OpenMesh::cross ( lVecAB, lVecAC );

        //Calculate the vectors
        OpenMesh::Vec3f lVecDE = OpenMesh::Vec3f (
          Mesh->points ( )[lNextIndexHole]
            - Mesh->points ( )[lIndexHole]
        );

        OpenMesh::Vec3f lVecDF = OpenMesh::Vec3f (
          Mesh->points ( )[lPrevIndexHole]
            - Mesh->points ( )[lIndexHole]
        );

        OpenMesh::Vec3f lCrossDE_DF = OpenMesh::cross ( lVecDE, lVecDF );

        lCrossAB_AC.normalize ( );
        lCrossDE_DF.normalize ( );

        float lAuxDot = OpenMesh::dot ( lCrossAB_AC, lCrossDE_DF );
        float lAuxAngle = acos ( lAuxDot );

        //Mesh->calc_face_centroid(lFaceHandleCandidate,ltmpVertex);
        //lVecBarCent	=	OpenMesh::Vec3f(
        //								MeshDef::Point(lBariCenter[0]
        //												,lBariCenter[1]
        //												,lBariCenter[2])
        //								- ltmpVertex
        //								);
        //lVecBarCent.normalize();

        //float lAuxDot	= OpenMesh::dot(lVecBarCent,lNormalFace);
        //float lAuxAngle = acos(lAuxDot);

        if ( fabs ( RADTODEG ( lAuxAngle )) > 90 )
        {
          //MeshDef::VertexHandle lAuxVHandle;
          //lAuxVHandle						=	vIdx[(despA+1)%HOLENUMVERTEX];
          //vIdx[(despA+1)%HOLENUMVERTEX]	=	vIdx[(despA+2)%HOLENUMVERTEX];
          //vIdx[(despA+2)%HOLENUMVERTEX]	=	lAuxVHandle;
        }

///////////////////////////////////////////////////////////////////////////////////////////////////////////
//Old sew version
/*
				//Primeras dos costuras
				j=0;
				{
					glb_tubeFace_VHandles.clear();
						glb_tubeFace_VHandles.push_back(glb_meshVHandle[vIdx[( (j+despA)%HOLENUMVERTEX) ].idx()]);

						index = ( (nodeAux.id-NEURODESP-1)*(globalDesp) + j + despB);
						index+=glb_nVerticesSoma;
						glb_tubeFace_VHandles.push_back(glb_meshVHandle[index]);

						index = ( (nodeAux.id-NEURODESP-1)*(globalDesp) + j+1	+ despB);
						index+=glb_nVerticesSoma;
						glb_tubeFace_VHandles.push_back(glb_meshVHandle[index]);
					Mesh->add_face(glb_tubeFace_VHandles);

					glb_tubeFace_VHandles.clear();
						glb_tubeFace_VHandles.push_back(glb_meshVHandle[vIdx[( (j+despA)%HOLENUMVERTEX) ].idx()]);

						index = ( (nodeAux.id-NEURODESP-1)*(globalDesp) + j + 1 + despB);
						index+=glb_nVerticesSoma;
						glb_tubeFace_VHandles.push_back(glb_meshVHandle[index]);


						glb_tubeFace_VHandles.push_back(glb_meshVHandle[vIdx[( (j+1+despA)%HOLENUMVERTEX) ].idx()]);
					Mesh->add_face(glb_tubeFace_VHandles);
				}

				//Costuras fijadas al v�rtice m�s alejado
				unsigned int fixVextex=( (j + 1 + despA)%HOLENUMVERTEX);

				for(; j<(HResolTube-1); j++)
				{
					glb_tubeFace_VHandles.clear();
						glb_tubeFace_VHandles.push_back(glb_meshVHandle[vIdx[fixVextex].idx()]);

						index = ( (nodeAux.id-NEURODESP-1)*(globalDesp) + ( (j + despB)%HResolTube) );
						index+=glb_nVerticesSoma;
						glb_tubeFace_VHandles.push_back(glb_meshVHandle[index]);

						index = ( (nodeAux.id-NEURODESP-1)*(globalDesp) + ( (j + 1	+ despB)%HResolTube) );
						index+=glb_nVerticesSoma;
						glb_tubeFace_VHandles.push_back(glb_meshVHandle[index]);
					Mesh->add_face(glb_tubeFace_VHandles);
				}

				//Costuras de cierre (3 Costuras)
				glb_tubeFace_VHandles.clear();

					glb_tubeFace_VHandles.push_back(glb_meshVHandle[vIdx[fixVextex].idx()]);

					index = ( (nodeAux.id-NEURODESP-1)*(globalDesp) + ( (j + despB)%HResolTube) );
					index+=glb_nVerticesSoma;
					glb_tubeFace_VHandles.push_back(glb_meshVHandle[index]);

					glb_tubeFace_VHandles.push_back(glb_meshVHandle[vIdx[ ( (fixVextex+1)%HOLENUMVERTEX ) ].idx()]);

				Mesh->add_face(glb_tubeFace_VHandles);

				glb_tubeFace_VHandles.clear();

					glb_tubeFace_VHandles.push_back(glb_meshVHandle[vIdx[ ( (fixVextex+1)%HOLENUMVERTEX ) ].idx()]);

					index = ( (nodeAux.id-NEURODESP-1)*(globalDesp) + ( (j + despB)%HResolTube) );
					index+=glb_nVerticesSoma;
					glb_tubeFace_VHandles.push_back(glb_meshVHandle[index]);

					glb_tubeFace_VHandles.push_back(glb_meshVHandle[vIdx[ ( (fixVextex+2)%HOLENUMVERTEX ) ].idx()]);

				Mesh->add_face(glb_tubeFace_VHandles);

				glb_tubeFace_VHandles.clear();

					glb_tubeFace_VHandles.push_back(glb_meshVHandle[vIdx[ ( (fixVextex+2)%HOLENUMVERTEX ) ].idx()]);

					index = ( (nodeAux.id-NEURODESP-1)*(globalDesp) + ( (j + despB)%HResolTube) );
					index+=glb_nVerticesSoma;
					glb_tubeFace_VHandles.push_back(glb_meshVHandle[index]);

					index = ( (nodeAux.id-NEURODESP-1)*(globalDesp) + ( (j + 1 + despB)%HResolTube) );
					index+=glb_nVerticesSoma;
					glb_tubeFace_VHandles.push_back(glb_meshVHandle[index]);

				Mesh->add_face(glb_tubeFace_VHandles);
*/
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//New sew version

        unsigned int lNumVertsHole = 3;
        unsigned int lNumVertsConse = ( unsigned int ) ( HResolTube )/( lNumVertsHole );

        int k = 0;
        for ( j = 0; j < ( lNumVertsHole ); ++j )
        {
          for ( k = ( j*lNumVertsConse ); k < (( j + 1 )*lNumVertsConse ); ++k )
          {
            glb_tubeFace_VHandles.clear ( );
            index = vIdx[(( j + despA )%HOLENUMVERTEX )].idx ( );
            glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

            index = (( nodeAux.id - NEURODESP - 1 )*( globalDesp ) + (( k + despB )%HResolTube ));
            index += glb_nVerticesSoma;
            glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

            index = (( nodeAux.id - NEURODESP - 1 )*( globalDesp ) + (( k + 1 + despB )%HResolTube ));
            index += glb_nVerticesSoma;
            glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );
            Mesh->add_face ( glb_tubeFace_VHandles );
          }

          //Close the sew
          glb_tubeFace_VHandles.clear ( );
          glb_tubeFace_VHandles.push_back ( glb_meshVHandle[vIdx[(( j + despA )%HOLENUMVERTEX )].idx ( )] );

          //index = ( (nodeAux.id-NEURODESP-1)*(globalDesp) + ( (k + 1 + despB)%HResolTube) );
          index = (( nodeAux.id - NEURODESP - 1 )*( globalDesp ) + (( k + despB )%HResolTube ));
          index += glb_nVerticesSoma;
          glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

          glb_tubeFace_VHandles.push_back ( glb_meshVHandle[vIdx[(( j + 1 + despA )%HOLENUMVERTEX )].idx ( )] );
          Mesh->add_face ( glb_tubeFace_VHandles );
        }

        //sew the rest of the vertexs
        if (( lNumVertsConse*lNumVertsHole ) < HResolTube )
        {
          for ( k = ( lNumVertsConse*lNumVertsHole ); k < HResolTube; ++k )
          {
            //Ultimo vertice del hole contra los restantes
            glb_tubeFace_VHandles.clear ( );

            glb_tubeFace_VHandles.push_back ( glb_meshVHandle[vIdx[(( j + despA )%HOLENUMVERTEX )].idx ( )] );

            //index = ( (nodeAux.id-NEURODESP-1)*(globalDesp) + ( (k + 1 + despB)%HResolTube) );
            index = (( nodeAux.id - NEURODESP - 1 )*( globalDesp ) + (( k + despB )%HResolTube ));
            index += glb_nVerticesSoma;
            glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

            index = (( nodeAux.id - NEURODESP - 1 )*( globalDesp ) + (( k + 1 + despB )%HResolTube ));
            index += glb_nVerticesSoma;
            glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

            Mesh->add_face ( glb_tubeFace_VHandles );
          }
        }
///////////////////////////////////////////////////////////////////////////////////////////////////////////

      } //-> fin if

    } //-> fin for

    //Free the memory of deleted objects
    Mesh->garbage_collection ( );

    updateBaseMesh ( );
  }

  void NeuroSWC::connectNoConsecutiveTubes2 ( )
  {
    int numConnections = importer->getNumConnection ( );

    SWCConnection nodeConnect;
    SWCNode nodeAux;
    SWCNode parentNodeAux;

    unsigned int globalDesp = HResolTube*VResolTube;
    unsigned int index;

    int j = 0;
    unsigned int consecValues;
    unsigned int indexCandidate;
    float dis;

    MeshDef::Point lBariCenter ( 0, 0, 0 );
    MeshDef::Point ltmpVertex ( 0, 0, 0 );
    OpenMesh::Vec3f ltmpVertexResult ( 0, 0, 0 );

    MeshDef::VertexHandle vhandle;
    MeshDef::HalfedgeHandle hehandle;
    MeshDef::FaceHandle fhandle;

    MeshDef::VertexHandle vIdx[3];
    float lAuxDis = 10E10;

    OpenMesh::Vec3f lVertex[3];
    MeshDef::FaceHandle fCandHandle;
    MeshDef::VertexFaceIter lVFIter;

    float lSigAreaTriang = 0;
    float lSigAreaTube = 0;

    unsigned int lIdxA;
    unsigned int lIdxB;

    MeshDef::Point lInitPointA ( 0, 0, 0 );
    MeshDef::Point lInitPointB ( 0, 0, 0 );
    OpenMesh::Vec3f lTmpVecResult ( 0, 0, 0 );

    unsigned int despA = 0;
    unsigned int despB = 0;
    unsigned int despTmpA = 0;
    unsigned int despTmpB = 0;

    std::vector <MeshDef::VertexHandle> vecVertexHoleHandle;
    std::vector <MeshDef::VertexHandle> vecIsolatedVertex;

    std::vector < unsigned int > vecVertexHoleHandleIds;

    std::vector <MeshDef::FaceHandle> vecFaceHoleHandle;
    std::vector <MeshDef::FaceHandle> totalVecFaceHolesHandles;
    std::vector < unsigned int > vecFaceHoleHandleIds;

    unsigned int lNumSews = 0;

    for ( int i = 0; i < ( numConnections - 1 ); ++i )
    {
      //Inicializations the local vars of the segment to sew
      nodeConnect = importer->getConnectionAt ( i );
      nodeAux = importer->getElementAt ( nodeConnect.fin );
      parentNodeAux = importer->getElementAt ( nodeConnect.ini );

      j = 0;
      consecValues = nodeAux.id - nodeAux.parent;
      dis = 10E10;
      indexCandidate = 0;

      lBariCenter = MeshDef::Point ( 0, 0, 0 );
      ltmpVertex = MeshDef::Point ( 0, 0, 0 );

      //No consecutive segments, requiere connection at inition
      //and no conected to the soma
      if (( consecValues > 1 )
        && ( nodeAux.parent != mSomaId )
        )
      {

        //->Calcular el centro de los v�rtices del nodo
        for ( j = 0; j < HResolTube; j++ )
        {
          //Calc the index of the vertex
          index = (( nodeAux.id - NEURODESP - 1 )*( globalDesp ) + j );
          index += glb_nVerticesSoma;
          lBariCenter += Mesh->points ( )[( index )];
        }

        lBariCenter /= ( float ) ( HResolTube );

        //->Calcular el v�rtice m�s cercano del parent al centro del tubo calculado
        indexCandidate = (((( parentNodeAux.id - NEURODESP )*( globalDesp ))));
        indexCandidate += glb_nVerticesSoma;

        //Utilizamos el opossite para salirnos del tubo (no estar por detr�s del tri�ngulo)
        ltmpVertexResult = OpenMesh::Vec3f ( lBariCenter
                                               - Mesh->points ( )[( indexCandidate )] );
        dis = ltmpVertexResult.length ( );

        bool lNearestTest = true;
        //////////////////////////////////////////////////////////////////////////////////////////

        //Calculate the vectors
        OpenMesh::Vec3f lVecAB2 = OpenMesh::Vec3f (
          nodeAux.position
            - importer->getElementAt ( nodeAux.parent ).position
        );

        OpenMesh::Vec3f lVecAC2 = OpenMesh::Vec3f (
          importer->getElementAt ( nodeAux.parent + 1 ).position
            - importer->getElementAt ( nodeAux.parent ).position
        );

        //Cross product == producto vectorial
        lVecAB2.normalize ( );
        lVecAC2.normalize ( );

        float lAuxDot2 = OpenMesh::dot ( lVecAB2, lVecAC2 );

        float lAuxAngle2 = acos ( lAuxDot2 );
        float lAuxAngleInGrads = fabs ( OpenMesh::rad_to_deg ( lAuxAngle2 ));

        if (( lAuxAngleInGrads > ANGBADSEW )
          )
        {
          lNearestTest = false;
        }



        //////////////////////////////////////////////////////////////////////////////////////////


        //Select the nearest vertex
        if ( lNearestTest )
        {
          for ( j = 0; j < ( HResolTube ); j++ )
          {
            index = (((( parentNodeAux.id - NEURODESP )*( globalDesp ))) + j );
            index += glb_nVerticesSoma;

            ltmpVertex = Mesh->points ( )[( index )];
            ltmpVertexResult = OpenMesh::Vec3f ( lBariCenter - ltmpVertex );

            if ( ltmpVertexResult.length ( ) < dis )
            {
              indexCandidate = index;
              dis = ltmpVertexResult.length ( );
            }
          }
        }
          //Select the most extern vertes
        else
        {
          for ( j = 0; j < ( HResolTube ); j++ )
          {
            index = (((( parentNodeAux.id - NEURODESP )*( globalDesp ))) + j );
            index += glb_nVerticesSoma;

            ltmpVertex = Mesh->points ( )[( index )];
            ltmpVertexResult = OpenMesh::Vec3f ( lBariCenter - ltmpVertex );

            if ( ltmpVertexResult.length ( ) > dis )
            {
              indexCandidate = index;
              dis = ltmpVertexResult.length ( );
            }
          }
        }


        //Ahora que tenemos el vertice m�s cercano
        //Desplazamos el resto de v�rtices d emanera que la apertura de un extremo a otro del agujero
        //sea el radio del nodo (as� evitamos pliegues.)
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////


        //Da el desplazamiento hasta el slice a coser
        int lInitialDesp = (((( parentNodeAux.id - NEURODESP )*( globalDesp ))))
          + glb_nVerticesSoma;

        //V�rtice con el que se va a coser
        int lRelativeDesp = indexCandidate
          - lInitialDesp;

        //Primer v�rtice a llado del candidato
        int lNextVertex = lInitialDesp
          + (( lRelativeDesp + ( 1 ))%HResolTube );

        int lPrevVertex = lInitialDesp
          + (( lRelativeDesp + ( HResolTube - 1 ))%HResolTube );

        //Calculate the the vector of the NextVertex
        OpenMesh::Vec3f lNextVector = (
          Mesh->points ( )[lNextVertex]
            - parentNodeAux.position
        );
        lNextVector.normalize ( );

        //Calculate the the vector of the PrevVertex
        OpenMesh::Vec3f lPrevVector = (
          Mesh->points ( )[lPrevVertex]
            - parentNodeAux.position
        );
        lPrevVector.normalize ( );

        //Calculate the vectors of the segments
        OpenMesh::Vec3f lIdxActVector = (
          Mesh->points ( )[indexCandidate]
            - parentNodeAux.position
        );
        lIdxActVector.normalize ( );

        OpenMesh::Vec3f ltmpNodeVector = ( nodeAux.position
          - parentNodeAux.position );


        //The up vector of the Slice
        OpenMesh::Vec3f lSliceUpVector = OpenMesh::cross ( lNextVector, lIdxActVector );
        lSliceUpVector.normalize ( );


        //Vector Right del Slice
        OpenMesh::Vec3f lSliceRightVector = OpenMesh::cross ( ltmpNodeVector, lSliceUpVector );
        //OpenMesh::Vec3f		lSliceRightVector	= OpenMesh::cross(lSliceUpVector, ltmpNodeVector);
        lSliceRightVector.normalize ( );

        //Vector de rotaci�n que se utilizar� para los Quaternios
        OpenMesh::Vec3f lNextVectorForQuat = lSliceRightVector*parentNodeAux.radius;

        float lScaleFactor = 1;

        //Vector right global, que da la apartura del nodo

        //Posici�n final del NextVertex
        OpenMesh::Vec3f lFinalPosVector = ( lSliceRightVector*parentNodeAux.radius*lScaleFactor )
          + parentNodeAux.position;

        MeshDef::ConstVertexIter v_it = getVertexIterators ( ).at ( lNextVertex );
        Mesh->set_point ( *v_it, MeshDef::Point ( lFinalPosVector[0], lFinalPosVector[1], lFinalPosVector[2] ));

        //Posici�n final del PrevVertex
        OpenMesh::Vec3f lSliceLeftVector = -lSliceRightVector;
        lFinalPosVector = ( lSliceLeftVector*parentNodeAux.radius*lScaleFactor )
          + parentNodeAux.position;
        v_it = getVertexIterators ( ).at ( lPrevVertex );
        Mesh->set_point ( *v_it, MeshDef::Point ( lFinalPosVector[0], lFinalPosVector[1], lFinalPosVector[2] ));

        unsigned int ltmpIndex;

        //Preveemos divisi�n por 0
        if ( HResolTube > 3 )
        {
          //Direcci�n del QUaternion
          float IncAng;
          if ( HResolTube == 4 )
            IncAng = -DEGTORAD ( 90 );
            //Hay que dividir por un elemento m�s que los 3 vertices pertenecientes al slice que se encuentran el el agujero abierto
            // ya que si no el �ltimo v�rtice a rotar se colapsa con el PrevVertex del agujero
          else
            IncAng = -DEGTORAD ( 180/(( HResolTube - 3 ) + 1 ));

          float angAct = IncAng;
          float angMed = angAct/2;

          for ( int i = 0; i < ( HResolTube - 3 ); ++i )
          {
            //Movimiento circular salt�ndote elementos (el index ylos dos laterales)
            ltmpIndex = ( lInitialDesp
              + (( lRelativeDesp + ( 2 ) + i )%HResolTube ));

            {
              boost::math::quaternion < float > lQuat ( cos ( angMed ),
                                                        ( sin ( angMed ))*lSliceUpVector[0],
                                                        ( sin ( angMed ))*lSliceUpVector[1],
                                                        ( sin ( angMed ))*lSliceUpVector[2]
              );

              boost::math::quaternion < float > lQuat2 ( cos ( angMed ),
                                                         ( -sin ( angMed ))*lSliceUpVector[0],
                                                         ( -sin ( angMed ))*lSliceUpVector[1],
                                                         ( -sin ( angMed ))*lSliceUpVector[2]
              );

              boost::math::quaternion < float >
                lPQuat ( 0, lNextVectorForQuat[0], lNextVectorForQuat[1], lNextVectorForQuat[2]
              );

              boost::math::quaternion < float > lQuatResult = lQuat*lPQuat*lQuat2;

              lFinalPosVector[0] = lQuatResult.R_component_2 ( );
              lFinalPosVector[1] = lQuatResult.R_component_3 ( );
              lFinalPosVector[2] = lQuatResult.R_component_4 ( );

              lFinalPosVector += parentNodeAux.position;

              v_it = getVertexIterators ( ).at ( ltmpIndex );
              Mesh->set_point ( *v_it, MeshDef::Point ( lFinalPosVector[0], lFinalPosVector[1], lFinalPosVector[2] ));
            }

            angAct += IncAng;
            angMed = angAct/2;
          }
        }

        /*
				for (int i=0;	i<HResolTube;	++i)
				{
					ltmpIndex = (lInitialDesp
								  + i);

					if (
							(ltmpIndex!=indexCandidate)
							&& (ltmpIndex!=lNextVertex)
							&& (ltmpIndex!=lPrevVertex)
						)
					{
						//Solo los impulsa hacia afuera

						lFinalPosVector	= Mesh->vertex_normals()[ltmpIndex];
						lFinalPosVector.normalize();
						lFinalPosVector	*=lScaleFactor;
						lFinalPosVector	+= parentNodeAux.position;

						v_it = getVertexIterators().at(ltmpIndex);
						Mesh->set_point( *v_it, MeshDef::Point(lFinalPosVector[0],lFinalPosVector[1],lFinalPosVector[2]) );


					}
				}

				*/

        /*
				//int lOpositeIndexCandidate;

				//lOpositeIndexCandidate = lInitialDesp
				//						 + ( (lRelativeDesp + (HResolTube/2)) %HResolTube);

				//Next Point
				OpenMesh::Vec3f		lNextVector		=  Mesh->vertex_normals()[lNextVertex];
				OpenMesh::Vec3f		lDiretionVector	=  Mesh->vertex_normals()[indexCandidate];
				//Up vector of the slice
				OpenMesh::Vec3f		lSliceUpVector	=  OpenMesh::cross(lNextVector,lDiretionVector);

				lNextVector	=  OpenMesh::cross(lDiretionVector,lSliceUpVector);
				lNextVector.normalize();
				lNextVector *= parentNodeAux.radius;
				lNextVector += parentNodeAux.position;

				MeshDef::ConstVertexIter v_it = getVertexIterators().at(lNextVertex);
				Mesh->set_point( *v_it, MeshDef::Point(lNextVector[0],lNextVector[1],lNextVector[2]) );


				//Point 2
				lNextVertex = lInitialDesp
								  + ( (lRelativeDesp - (1)) %HResolTube);

				lNextVector		=  Mesh->vertex_normals()[lNextVertex];
				//OpenMesh::Vec3f		lDiretionVector	=  Mesh->vertex_normals()[indexCandidate];
				//OpenMesh::Vec3f		lUpVector	=  OpenMesh::cross(lNextVector,lDiretionVector);

				lNextVector	=  OpenMesh::cross(lDiretionVector,-lUpVector);
				lNextVector.normalize();
				lNextVector *= parentNodeAux.radius;
				lNextVector += parentNodeAux.position;

				v_it = getVertexIterators().at(lNextVertex);
				Mesh->set_point( *v_it, MeshDef::Point(lNextVector[0],lNextVector[1],lNextVector[2]) );
				*/

        //Se podr�a calcular el vector derecha como el producto vectorial del vector direcci�n dle padre y del segmentoe bifurcacion
        /*
				//Recolocaci�n de todos los v�rtices menos el pivote
				//OpenMesh::Vec3f		lMainVec= Mesh->vertex_normals()[lOpositeIndexCandidate];
				OpenMesh::Vec3f		lMainVec	=  Mesh->points()[lOpositeIndexCandidate]
													-parentNodeAux.position
													;
				lMainVec.normalize();

				OpenMesh::Vec3f		lAuxDirVec;
				MeshDef::ConstVertexIter v_it = getVertexIterators().at(lInitialDesp);

				OpenMesh::Vec3f		tmpVec;

				for(j=0; j<(HResolTube); j++)
				{
					index	=	( ( ((parentNodeAux.id-NEURODESP)*(globalDesp))) + j);
					index	+=	glb_nVerticesSoma;

					if (index!=indexCandidate)
					{
						//lAuxDirVec	=  parentNodeAux.position;
						//lAuxDirVec	=	Mesh->vertex_normals()[(index)];
						lAuxDirVec	=	Mesh->points()[index]
										-parentNodeAux.position
										;
						lAuxDirVec.normalize();
						lAuxDirVec	+=	lMainVec;
						lAuxDirVec.normalize();

						tmpVec		= lAuxDirVec - parentNodeAux.position;
						tmpVec.normalize();

						lAuxDirVec = parentNodeAux.position + (tmpVec*parentNodeAux.radius);

						//Mesh->set_point( *v_it, MeshDef::Point(lAuxDirVec[0],lAuxDirVec[1],lAuxDirVec[2]) );

						lAuxDirVec	=	Mesh->points()[lOpositeIndexCandidate];
						Mesh->set_point( *v_it, MeshDef::Point(lAuxDirVec[0],lAuxDirVec[1],lAuxDirVec[2]) );
					}
					++v_it;
				}
				*/



        ///////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////

        //Select the vertex handler
        vhandle = Mesh->vertex_handle ( indexCandidate );
        vecIsolatedVertex.push_back ( vhandle );

        //Store all the incidente vertex handles
        MeshDef::ConstVertexVertexIter CVVIter;
        MeshDef::VertexHandle lAuxVertexHandle;
        for ( CVVIter = Mesh->cvv_iter ( vhandle ); CVVIter.is_valid ( ); ++CVVIter )
        {
          lAuxVertexHandle = Mesh->vertex_handle ( CVVIter->idx ( ));
          vecVertexHoleHandle.push_back ( lAuxVertexHandle );
        }

        //Store the ids of the selected Vertexs
        for ( unsigned int i = 0; i < vecVertexHoleHandle.size ( ); ++i )
        {
          vecVertexHoleHandleIds.push_back ( vecVertexHoleHandle.at ( i ).idx ( ));
        }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
				//Delete vertex and incident faces
				//Delete the vertex and his incident faces
				//!!!   Se deberian borrar las facetas una a una para no borrar le vertice y perder los indices
				if (vhandle.is_valid()) Mesh->delete_vertex(vhandle);

				//Luego es necesarios

				//!!! Ordenar los indices de forma CW o CCW
				//Determinar distancias para costura minima
*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////

        //Store all the incidente faces handles
        MeshDef::ConstVertexFaceIter CVFIter;
        MeshDef::FaceHandle lAuxFaceHandle;

        for ( CVFIter = Mesh->cvf_iter ( vhandle ); CVFIter.is_valid ( ); ++CVFIter )
        {
          lAuxFaceHandle = Mesh->face_handle ( CVFIter->idx ( ));
          vecFaceHoleHandle.push_back ( lAuxFaceHandle );
        }

        //Store the ids of the selected Faces
        for ( unsigned int i = 0; i < vecFaceHoleHandle.size ( ); ++i )
        {
          vecFaceHoleHandleIds.push_back ( vecFaceHoleHandle.at ( i ).idx ( ));
        }

        //Delete the selected faces
        for ( unsigned int i = 0; i < vecFaceHoleHandle.size ( ); ++i )
        {
          if ( vecFaceHoleHandle.at ( i ).is_valid ( )
            && !isFaceHandleInContainer ( vecFaceHoleHandle.at ( i ), totalVecFaceHolesHandles )
            )
            Mesh->delete_face ( vecFaceHoleHandle.at ( i ), false );
        }

        //Add the handle to the faces handles
        for ( CVFIter = Mesh->cvf_iter ( vhandle ); CVFIter.is_valid ( ); ++CVFIter )
        {
          lAuxFaceHandle = Mesh->face_handle ( CVFIter->idx ( ));
          totalVecFaceHolesHandles.push_back ( lAuxFaceHandle );
        }


        ////!!! Para que no pete en modo debug, el borrado de las facetas peta la costura, hay q actualizarlo antes
        ////Free the memory of deleted objects
        //Mesh->garbage_collection();

        ////Update the mesh
        //updateBaseMesh();

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

        //Desplazamiento con los vertices a coser
        //Hay que comenzar desde los dos v�rtices con distancia m�nima
        despA = 0;
        despTmpA = 0;

        despB = 0;
        despTmpB = 0;

        dis = 10E10;

        unsigned int lNumVertToSew = vecVertexHoleHandle.size ( );

        //Calculamos la distancia m�nima entre cualquier vertice de la dendrita y los del agujero en la otra dendrita
        //Las variables despA y despB contiend cuanto nos tenemos que desplazar para que la distancia
        //entre el vertice inicial a coser en ambos agujero sea m�nima
        for ( int i = 0; i < HResolTube; i++ )
        {
          lIdxA = (((( nodeAux.id - NEURODESP - 1 )*( globalDesp ))) + i );
          lIdxA += glb_nVerticesSoma;

          lInitPointA = Mesh->points ( )[( lIdxA )];

          //Elementos del agujero en la dendrita
          for ( j = 0; j < lNumVertToSew; j++ )
          {
            lIdxB = vecVertexHoleHandle.at ( j ).idx ( );

            lInitPointB = Mesh->points ( )[( lIdxB )];
            lTmpVecResult = OpenMesh::Vec3f ( lInitPointA - lInitPointB );

            if ( lTmpVecResult.length ( ) < dis )
            {
              dis = lTmpVecResult.length ( );

              despB = despTmpB;
              despA = despTmpA;
            }
            ++despTmpB;
          }

          ++despTmpA;
          despTmpB = 0;
        }

        //Cuando se cruzan, este desplazamiento lo suele arreglar
        //despB++;


/*
				//Distance method evaluation
				//Determinar si se invierten los indices de los vertices a coser
				unsigned int lIndexTube = ( ( ((nodeAux.id-NEURODESP-1)*(globalDesp) ) ) + ((1 + despA)%HResolTube) );
				lIndexTube				+= glb_nVerticesSoma;

				unsigned int lNextIndexSoma = ( (1 + despB)%lNumVertToSew);
				unsigned int lPrevIndexSoma = ( ((lNumVertToSew-1) + despB)%lNumVertToSew);

				float distCCW	= OpenMesh::Vec3f(Mesh->points()[lIndexTube]
									- Mesh->points()[lNextIndexSoma]).length();


				float distCW	= OpenMesh::Vec3f(Mesh->points()[lIndexTube]
									- Mesh->points()[lPrevIndexSoma]).length();


				//Invertir vector
				int lOrder=1;
				unsigned int limit=HResolTube;
				if (vecVertexHoleHandle.size()<limit)
					limit=vecVertexHoleHandle.size();
*/

        //Angle method evaluation
        //Index Vertex int the tube
        unsigned int lIndexTube = (((( nodeAux.id - NEURODESP - 1 )*( globalDesp ))) + (( despA )%HResolTube ));
        lIndexTube += glb_nVerticesSoma;
        unsigned int lNextIndexTube = (((( nodeAux.id - NEURODESP - 1 )*( globalDesp ))) + (( 1 + despA )%HResolTube ));
        lNextIndexTube += glb_nVerticesSoma;
        unsigned int lPrevIndexTube =
          (((( nodeAux.id - NEURODESP - 1 )*( globalDesp ))) + ((( HResolTube - 1 ) + despA )%HResolTube ));
        lPrevIndexTube += glb_nVerticesSoma;

        //Index in the temp vextes of the container
        unsigned int lIndexHole = vecVertexHoleHandle.at (( despB )%lNumVertToSew ).idx ( );
        unsigned int lNextIndexHole = vecVertexHoleHandle.at (( 1 + despB )%lNumVertToSew ).idx ( );
        unsigned int lPrevIndexHole = vecVertexHoleHandle.at ((( lNumVertToSew - 1 ) + despB )%lNumVertToSew ).idx ( );

        //Calculate the vectors
        OpenMesh::Vec3f lVecAB = OpenMesh::Vec3f (
          Mesh->points ( )[lNextIndexTube]
            - Mesh->points ( )[lIndexTube]
        );

        OpenMesh::Vec3f lVecAC = OpenMesh::Vec3f (
          Mesh->points ( )[lPrevIndexTube]
            - Mesh->points ( )[lIndexTube]
        );

        //Cross product == producto vectorial
        OpenMesh::Vec3f lCrossAB_AC = OpenMesh::cross ( lVecAB, lVecAC );


        //Calculate the vectors
        OpenMesh::Vec3f lVecDE = OpenMesh::Vec3f (
          Mesh->points ( )[lNextIndexHole]
            - Mesh->points ( )[lIndexHole]
        );

        OpenMesh::Vec3f lVecDF = OpenMesh::Vec3f (
          Mesh->points ( )[lPrevIndexHole]
            - Mesh->points ( )[lIndexHole]
        );

        OpenMesh::Vec3f lCrossDE_DF = OpenMesh::cross ( lVecDE, lVecDF );

        lCrossAB_AC.normalize ( );
        lCrossDE_DF.normalize ( );

        float lAuxDot = OpenMesh::dot ( lCrossAB_AC, lCrossDE_DF );
        float lAuxAngle = acos ( lAuxDot );

        int lOrder = 1;
        unsigned int limit = HResolTube;
        if ( vecVertexHoleHandle.size ( ) < limit )
          limit = vecVertexHoleHandle.size ( );


        //lOrder=-1;
        //if (distCW>distCCW)

        //if (RADTODEG(lAuxAngle)>90)		lOrder=-1;

        //if (true)
        //if (fabs(RADTODEG(lAuxAngle))<=90)

        if ( fabs ( RADTODEG ( lAuxAngle )) > 90 )
          lOrder = -1;


        //Cosemos siempre hacia el mismo lado
        //parece que tiea bien ;P

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////

        unsigned int lNumCanVertexInSoma = vecVertexHoleHandleIds.size ( );
        unsigned int lNumCanVertexInDend = HResolTube;
        float lVertexRate = ( float ) lNumCanVertexInSoma
          /( float ) lNumCanVertexInDend;

        if ( lVertexRate > VERTRATESOMASEW )
          //if (true)
        {
          int k = 0;
          lOrder = 1;
          //Recorremos los v�rtices del agujero en la dendrita
          for ( j = 0; j < lNumCanVertexInDend; ++j )
          {
            //Avanzamos pr el agujero en el soma
            for ( k = ( j*lVertexRate ); k < (( j + 1 )*lVertexRate ); ++k )
            {
              glb_tubeFace_VHandles.clear ( );
              index = (((( nodeAux.id - NEURODESP - 1 )*( globalDesp ))) + (( j + despA )%HResolTube ));
              index += glb_nVerticesSoma;
              glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

              index = abs ( int ((( lOrder*k ) + 1 + despB )%lNumVertToSew ));
              glb_tubeFace_VHandles.push_back ( vecVertexHoleHandle.at ( index ));

              index = abs ( int ((( lOrder*k ) + despB )%lNumVertToSew ));
              glb_tubeFace_VHandles.push_back ( vecVertexHoleHandle.at ( index ));
              Mesh->add_face ( glb_tubeFace_VHandles );

            }

            --k;
            glb_tubeFace_VHandles.clear ( );
            index = (((( nodeAux.id - NEURODESP - 1 )*( globalDesp ))) + (( j + despA )%HResolTube ));
            index += glb_nVerticesSoma;
            glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

            index = (((( nodeAux.id - NEURODESP - 1 )*( globalDesp ))) + (( j + 1 + despA )%HResolTube ));
            index += glb_nVerticesSoma;
            glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

            index = abs ( int ((( lOrder*k ) + 1 + despB )%lNumVertToSew ));
            glb_tubeFace_VHandles.push_back ( vecVertexHoleHandle.at ( index ));
            Mesh->add_face ( glb_tubeFace_VHandles );
          }

          //sew the rest of the vertexs
          --j;

          //Ultimo vertice del hole contra los restantes
          //!!! -> Tal vez hay que iterar, no tengo claro que s�lo asi funcione
          glb_tubeFace_VHandles.clear ( );

          index = (((( nodeAux.id - NEURODESP - 1 )*( globalDesp ))) + (( j + despA )%HResolTube ));
          index += glb_nVerticesSoma;
          glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

          index = (((( nodeAux.id - NEURODESP - 1 )*( globalDesp ))) + (( j + 1 + despA )%HResolTube ));
          index += glb_nVerticesSoma;
          glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

          index = abs ( int (( despB )%lNumVertToSew ));
          glb_tubeFace_VHandles.push_back ( vecVertexHoleHandle.at ( index ));

          Mesh->add_face ( glb_tubeFace_VHandles );
        }
        else
        {
          //if (lOrder>0)
          if ( true )
            //if (false)
          {
            lOrder = 1; //Cuidado, esto peta el algoritmo si se kita!!!


            //Costura hacia adelante
            for ( j = 0; j < ( limit ); j++ )
            {
              glb_tubeFace_VHandles.clear ( );
              index = (((( nodeAux.id - NEURODESP - 1 )*( globalDesp ))) + (( j + despA )%HResolTube ));
              index += glb_nVerticesSoma;
              glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

              index = abs ( int ((( lOrder*j ) + 1 + despB )%lNumVertToSew ));
              glb_tubeFace_VHandles.push_back ( vecVertexHoleHandle.at ( index ));

              index = abs ( int ((( lOrder*j ) + despB )%lNumVertToSew ));
              glb_tubeFace_VHandles.push_back ( vecVertexHoleHandle.at ( index ));
              Mesh->add_face ( glb_tubeFace_VHandles );

              glb_tubeFace_VHandles.clear ( );
              index = (((( nodeAux.id - NEURODESP - 1 )*( globalDesp ))) + (( j + despA )%HResolTube ));
              index += glb_nVerticesSoma;
              glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

              index = (((( nodeAux.id - NEURODESP - 1 )*( globalDesp ))) + (( j + 1 + despA )%HResolTube ));
              index += glb_nVerticesSoma;
              glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

              index = abs ( int ((( lOrder*j ) + 1 + despB )%lNumVertToSew ));
              glb_tubeFace_VHandles.push_back ( vecVertexHoleHandle.at ( index ));
              Mesh->add_face ( glb_tubeFace_VHandles );
            }

            //Sew diferent number of vertex
            if ( vecVertexHoleHandle.size ( ) > HResolTube )
            {
              int i = 0;
              for ( i = HResolTube; i < vecVertexHoleHandle.size ( ); ++i )
              {
                glb_tubeFace_VHandles.clear ( );

                index = (((( nodeAux.id - NEURODESP - 1 )*( globalDesp ))) + (( j + despA )%HResolTube ));
                index += glb_nVerticesSoma;
                glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

                index = abs ( int ((( lOrder*i ) + 1 + despB )%lNumVertToSew ));
                glb_tubeFace_VHandles.push_back ( vecVertexHoleHandle.at ( index ));

                index = abs ( int ((( lOrder*i ) + despB )%lNumVertToSew ));
                glb_tubeFace_VHandles.push_back ( vecVertexHoleHandle.at ( index ));

                Mesh->add_face ( glb_tubeFace_VHandles );
              }

              glb_tubeFace_VHandles.clear ( );

              index = (((( nodeAux.id - NEURODESP - 1 )*( globalDesp ))) + (( despA )%HResolTube ));
              index += glb_nVerticesSoma;
              glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

              index = 0;
              glb_tubeFace_VHandles.push_back ( vecVertexHoleHandle.at ( index ));

              index = abs ( int ((( lOrder*i ) + despB )%lNumVertToSew ));
              glb_tubeFace_VHandles.push_back ( vecVertexHoleHandle.at ( index ));

              Mesh->add_face ( glb_tubeFace_VHandles );
            }
            else
            {
              //!!! Menos vertices en el agujero
              //Fijar ultimo vertice del agujero y fan al tubo
              int i = 0;
              for ( i = vecVertexHoleHandle.size ( ); i < HResolTube; ++i )
              {
                glb_tubeFace_VHandles.clear ( );
                index = (((( nodeAux.id - NEURODESP - 1 )*( globalDesp ))) + (( i + despA )%HResolTube ));
                index += glb_nVerticesSoma;
                glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

                index = (((( nodeAux.id - NEURODESP - 1 )*( globalDesp ))) + (( i + 1 + despA )%HResolTube ));
                index += glb_nVerticesSoma;
                glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

                index = abs ( int ((( lOrder*j ) + despB )%lNumVertToSew ));
                glb_tubeFace_VHandles.push_back ( vecVertexHoleHandle.at ( index ));
                Mesh->add_face ( glb_tubeFace_VHandles );
              }  //for
            } //else -> if (vecVertexHoleHandle.size()>HResolTube)
          } //if (distCW>distCCW)
          else
          {
            lOrder = -1;
            //!!!Costura inversa, de momento se obviaran hasta que se detecten de manera correcta
            for ( j = 0; j < ( limit ); j++ )
              //for(j=0; j<3; j++)
            {
              glb_tubeFace_VHandles.clear ( );
              index = (((( nodeAux.id - NEURODESP - 1 )*( globalDesp ))) + (( j + despA )%HResolTube ));
              index += glb_nVerticesSoma;
              glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

              index = abs ( int ((( lOrder*j ) + despB )%lNumVertToSew ));
              glb_tubeFace_VHandles.push_back ( vecVertexHoleHandle.at ( index ));

              index = abs ( int ((( lOrder*j ) + lOrder*1 + despB )%lNumVertToSew ));
              glb_tubeFace_VHandles.push_back ( vecVertexHoleHandle.at ( index ));

              Mesh->add_face ( glb_tubeFace_VHandles );

              glb_tubeFace_VHandles.clear ( );

              index = (((( nodeAux.id - NEURODESP - 1 )*( globalDesp ))) + (( j + despA )%HResolTube ));
              index += glb_nVerticesSoma;
              glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

              index = abs ( int ((( lOrder*j ) + lOrder*1 + despB )%lNumVertToSew ));
              glb_tubeFace_VHandles.push_back ( vecVertexHoleHandle.at ( index ));

              index = (((( nodeAux.id - NEURODESP - 1 )*( globalDesp ))) + abs ( int (( j + 1 + despA ))%HResolTube ));
              index += glb_nVerticesSoma;
              glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

              Mesh->add_face ( glb_tubeFace_VHandles );
            }

          } //else -> if (distCW>distCCW)
        }
      }

      //Clean the aux containers
      vecVertexHoleHandle.clear ( );
      vecVertexHoleHandleIds.clear ( );

      vecFaceHoleHandle.clear ( );
      vecFaceHoleHandleIds.clear ( );

      ////Free the memory of deleted objects
      //Mesh->garbage_collection();

      ////Update the mesh
      //updateBaseMesh();
    }
    //Free the memory of deleted objects
    Mesh->garbage_collection ( );

    //Update the mesh
    updateBaseMesh ( );
  }

  bool NeuroSWC::isFaceHandleInContainer ( MeshDef::FaceHandle pFHandle,
                                           std::vector <MeshDef::FaceHandle> pFaceHandleContainer )
  {
    for ( int i = 0; i < pFaceHandleContainer.size ( ); ++i )
    {
      if ( pFaceHandleContainer.at ( i ) == pFHandle )
        return true;
    }
    return false;
  }

  bool NeuroSWC::isVertexHandleInContainer ( MeshDef::VertexHandle pVHandle,
                                             std::vector <MeshDef::VertexHandle> pVertexHandleContainer )
  {
    for ( int i = 0; i < pVertexHandleContainer.size ( ); ++i )
    {
      if ( pVertexHandleContainer.at ( i ) == pVHandle )
        return true;
    }
    return false;

  }


  //Old version of Soma Sew ( valid to 27.07.2011)
//	void NeuroSWC::connectSoma()
//	{
//		int numConnections=importer->getNumConnection();
//
//		SWCConnection	nodeConnect;
//		SWCNode			nodeAux;
//		SWCNode			parentNodeAux;
//
//		unsigned int	globalDesp	=	HResolTube*VResolTube;
//		unsigned int	index;
//
//		int j=0;
//		unsigned int	consecValues;
//		unsigned int	indexCandidate;
//		float			dis;
//
//		MeshDef::Point	lBariCenter(0,0,0);
//		MeshDef::Point	ltmpVertex(0,0,0);
//		OpenMesh::Vec3f ltmpVertexResult(0,0,0);
//
//		MeshDef::VertexHandle   vhandle;
//		MeshDef::HalfedgeHandle hehandle;
//		MeshDef::FaceHandle		fhandle;
//
//		MeshDef::VertexHandle  vIdx[3];
//		float lAuxDis			=	10E10;
//
//		OpenMesh::Vec3f			lVertex[3];
//		MeshDef::FaceHandle		fCandHandle;
//		MeshDef::VertexFaceIter	lVFIter;
//
//		float lSigAreaTriang	= 0;
//		float lSigAreaTube		= 0;
//
//		unsigned int lIdxA;
//		unsigned int lIdxB;
//
//		MeshDef::Point			lInitPointA (0,0,0);
//		MeshDef::Point			lInitPointB (0,0,0);
//		OpenMesh::Vec3f			lTmpVecResult(0,0,0);
//
//		unsigned int despA		=0;
//		unsigned int despB		=0;
//		unsigned int despTmpA	=0;
//		unsigned int despTmpB	=0;
//
//		std::vector<MeshDef::VertexHandle>  vecVertexHoleHandle;
//		std::vector<MeshDef::VertexHandle>  vecIsolatedVertex;
//
//		std::vector<unsigned int>			vecVertexHoleHandleIds;
//
//		std::vector<MeshDef::FaceHandle>	vecFaceHoleHandle;
//		std::vector<MeshDef::FaceHandle>	totalVecFaceHolesHandles;
//		std::vector<unsigned int>			vecFaceHoleHandleIds;
//
//		//Comprobar que no se halla seleccionado ya para borrar su facetas
//		//de lo contrario no tendra facetas asociadas, hay q escoger otro
//
//		//Problema!!!! en la costura, hay menos elementos en el std vector que en la seccion del tubo, por eso se sale de rango en la costura
//
//		unsigned int lNumSews=0;
//
//		for (int i=0;i<(numConnections-1);++i)
//		{
//			//Inicializations the local vars of the segment to sew
//			nodeConnect		=	importer->getConnectionAt(i);
//			nodeAux			=	importer->getElementAt(nodeConnect.fin);
//			parentNodeAux	=	importer->getElementAt(nodeConnect.ini);
//
//			j=0;
//			consecValues	=	nodeAux.id-nodeAux.parent;
//			dis				=	10E10;
//			indexCandidate	=	0;
//
//			lBariCenter		=	MeshDef::Point(0,0,0);
//			ltmpVertex		=	MeshDef::Point(0,0,0);
//
//			//No consecutive segments, requiere connection at inition
//			if (nodeAux.parent==mSomaId)
//			{
//
//				//Calcular el baricentro del segmento inicial de los v�rtices del nodo
//				for(j=0; j<(HResolTube-1); j++)
//				{
//					//Calc the index of the vertex
//					index		=	( (nodeAux.id-NEURODESP-1)*(globalDesp) + j);
//					index		+=	glb_nVerticesSoma;
//					lBariCenter	+=	Mesh->points()[(index)];
//				}
//
//				lBariCenter	/=	(float)(HResolTube-1);
//
//				//Calcular el vertice mas cercano de los q conforma el soma
//				//Ponerlo como candidate para eliminar la faceta y q se haga la costura
//				indexCandidate		= 0;
//				ltmpVertexResult	=	OpenMesh::Vec3f (lBariCenter
//														- Mesh->points()[(indexCandidate)]);
//				dis					=	ltmpVertexResult.length();
//
//				//Escogemos el vertice mas cercano del soma que no halla sido borrado
//				for(j=1; j<glb_nVerticesSoma; j++)
//				{
//					index				=	j;
//					ltmpVertex			=	Mesh->points()[(index)];
//					ltmpVertexResult	=	OpenMesh::Vec3f(lBariCenter - ltmpVertex);
//
//					if ( (ltmpVertexResult.length()< dis)
//						 && (!isVertexHandleInContainer( Mesh->vertex_handle(index), vecIsolatedVertex))
//						)
//					{
//						indexCandidate	=	index;
//						dis				=	ltmpVertexResult.length();
//					}
//				}
//
//				//Select the vertex handler
//				vhandle		= Mesh->vertex_handle(indexCandidate);
//				vecIsolatedVertex.push_back(vhandle);
//
//				//Store all the incidente vertex handles
//				MeshDef::ConstVertexVertexIter	CVVIter;
//				MeshDef::VertexHandle			lAuxVertexHandle;
//
//				for (CVVIter=Mesh->cvv_iter(vhandle);CVVIter.is_valid();++CVVIter)
//				{
//					lAuxVertexHandle = Mesh->vertex_handle(CVVIter->idx());
//					vecVertexHoleHandle.push_back(lAuxVertexHandle);
//				}
//
//				//!!! Ordenar los indices de forma CW o CCW
//				//Determinar distancias para costura minima
//				for (unsigned int  i=0;i<vecVertexHoleHandle.size();++i)
//				{
//					vecVertexHoleHandleIds.push_back(vecVertexHoleHandle.at(i).idx());
//				}
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///*
//				//Delete vertex and incident faces
//				//Delete the vertex and his incident faces
//				//!!!   Se deberian borrar las facetas una a una para no borrar le vertice y perder los indices
//				if (vhandle.is_valid()) Mesh->delete_vertex(vhandle);
//*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//				//Store all the incidente vertex handles
//				MeshDef::ConstVertexFaceIter	CVFIter;
//				MeshDef::FaceHandle				lAuxFaceHandle;
//
//				for (CVFIter=Mesh->cvf_iter(vhandle);CVFIter.is_valid();++CVFIter)
//				{
//					lAuxFaceHandle = Mesh->face_handle(CVFIter->idx());
//					vecFaceHoleHandle.push_back(lAuxFaceHandle);
//				}
//
//				//!!! Ordenar los indices de forma CW o CCW
//				//Determinar distancias para costura minima
//				for (unsigned int  i=0;i<vecFaceHoleHandle.size();++i)
//				{
//					vecFaceHoleHandleIds.push_back(vecFaceHoleHandle.at(i).idx());
//				}
//
//				for (unsigned int i=0; i<vecFaceHoleHandle.size();++i)
//				{
//					if (vecFaceHoleHandle.at(i).is_valid()
//						&& !isFaceHandleInContainer(vecFaceHoleHandle.at(i),totalVecFaceHolesHandles )
//						)
//						Mesh->delete_face(vecFaceHoleHandle.at(i), false);
//				}
//
//				//Anyadido tras le borrado
//				for (CVFIter=Mesh->cvf_iter(vhandle);CVFIter.is_valid();++CVFIter)
//				{
//					lAuxFaceHandle = Mesh->face_handle(CVFIter->idx());
//					totalVecFaceHolesHandles.push_back(lAuxFaceHandle);
//				}
//
//
//				////!!! Para que no pete en modo debug, el borrado de las facetas peta la costura, hay q actualizarlo antes
//				////Free the memory of deleted objects
//				//Mesh->garbage_collection();
//
//				////Update the mesh
//				//updateBaseMesh();
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//				//Desplazamiento con los vertices a coser
//				//Hay que comenzar desde los dos v�rtices con distancia m�nima
//				despA		=0;
//				despTmpA	=0;
//
//				despB		=0;
//				despTmpB	=0;
//
//				dis	= 10E10;
//
//				unsigned int lNumVertToSew = vecVertexHoleHandle.size();
//
//				//Elementos de la dendrita con las que se va a coser
//				for (int i=0;i<(HResolTube-1); i++)
//				{
//					//lIdxA = ( ( ((nodeAux.id-NEURODESP)*(globalDesp))-HResolTube ) + i);
//					//lIdxA +=glb_nVerticesSoma;
//
//					lIdxA = ( ( ((nodeAux.id-NEURODESP-1)*(globalDesp) ) ) + i );
//					lIdxA +=glb_nVerticesSoma;
//
//					lInitPointA = Mesh->points()[(lIdxA)];
//
//					//Indices con los que coser
//					//Elementos del soma
//					for(j=0; j<(lNumVertToSew-1); j++)
//					{
//						lIdxB = vecVertexHoleHandle.at(j).idx();
//
//						lInitPointB = Mesh->points()[(lIdxB)];
//						lTmpVecResult = OpenMesh::Vec3f(lInitPointA - lInitPointB);
//
//						if (lTmpVecResult.length() < dis)
//						{
//							dis			=lTmpVecResult.length();
//
//							despB		= despTmpB;
//							despA		= despTmpA;
//						}
//						++despTmpB;
//					}
//
//					++despTmpA;
//					despTmpB = 0;
//				}
//
///*
//				//Distance method evaluation
//				//Determinar si se invierten los indices de los vertices a coser
//				unsigned int lIndexTube = ( ( ((nodeAux.id-NEURODESP-1)*(globalDesp) ) ) + ((1 + despA)%HResolTube) );
//				lIndexTube				+= glb_nVerticesSoma;
//
//				unsigned int lNextIndexSoma = ( (1 + despB)%lNumVertToSew);
//				unsigned int lPrevIndexSoma = ( ((lNumVertToSew-1) + despB)%lNumVertToSew);
//
//				float distCCW	= OpenMesh::Vec3f(Mesh->points()[lIndexTube]
//									- Mesh->points()[lNextIndexSoma]).length();
//
//
//				float distCW	= OpenMesh::Vec3f(Mesh->points()[lIndexTube]
//									- Mesh->points()[lPrevIndexSoma]).length();
//
//
//				//Invertir vector
//				int lOrder=1;
//				unsigned int limit=HResolTube;
//				if (vecVertexHoleHandle.size()<limit)
//					limit=vecVertexHoleHandle.size();
//*/
//
//				//Angle method evaluation
//				//Index Vertex int the tube
//				unsigned int lIndexTube = ( ( ((nodeAux.id-NEURODESP-1)*(globalDesp) ) ) + ((despA)%HResolTube) );
//				lIndexTube				+= glb_nVerticesSoma;
//				unsigned int lNextIndexTube = ( ( ((nodeAux.id-NEURODESP-1)*(globalDesp) ) ) + ((1 + despA)%HResolTube) );
//				lNextIndexTube				+= glb_nVerticesSoma;
//				unsigned int lPrevIndexTube = ( ( ((nodeAux.id-NEURODESP-1)*(globalDesp) ) ) + (((HResolTube - 1) + despA)%HResolTube) );
//				lPrevIndexTube				+= glb_nVerticesSoma;
//
//				//Index in the temp vextes of the container
//				unsigned int lIndexSoma		= vecVertexHoleHandle.at( (despB)%lNumVertToSew).idx();
//				unsigned int lNextIndexSoma = vecVertexHoleHandle.at( (1 + despB)%lNumVertToSew).idx();
//				unsigned int lPrevIndexSoma = vecVertexHoleHandle.at( ((lNumVertToSew-1) + despB)%lNumVertToSew).idx();
//
//				//Calculate the vectors
//				OpenMesh::Vec3f lVecAB	=	OpenMesh::Vec3f(
//															Mesh->points()[lNextIndexTube]
//															- Mesh->points()[lIndexTube]
//															);
//
//				OpenMesh::Vec3f lVecAC	=	OpenMesh::Vec3f(
//															Mesh->points()[lPrevIndexTube]
//															- Mesh->points()[lIndexTube]
//															);
//
//				//Cross product == producto vectorial
//				OpenMesh::Vec3f lCrossAB_AC = OpenMesh::cross(lVecAB, lVecAC);
//
//
//				//Calculate the vectors
//				OpenMesh::Vec3f lVecDE	=	OpenMesh::Vec3f(
//															Mesh->points()[lNextIndexSoma]
//															- Mesh->points()[lIndexSoma]
//															);
//
//				OpenMesh::Vec3f lVecDF	=	OpenMesh::Vec3f(
//															Mesh->points()[lPrevIndexSoma]
//															- Mesh->points()[lIndexSoma]
//															);
//
//				OpenMesh::Vec3f lCrossDE_DF = OpenMesh::cross(lVecDE, lVecDF);
//
//				lCrossAB_AC.normalize();
//				lCrossDE_DF.normalize();
//
//				float lAuxDot	= OpenMesh::dot(lCrossAB_AC,lCrossDE_DF);
//				float lAuxAngle = acos(lAuxDot);
//
//				int lOrder=1;
//				unsigned int limit=HResolTube;
//				if (vecVertexHoleHandle.size()<limit)
//					limit=vecVertexHoleHandle.size();
//
//
//				//lOrder=-1;
//				//if (distCW>distCCW)
//
//				//if (RADTODEG(lAuxAngle)>90)		lOrder=-1;
//
//				//if (true)
//				//if (fabs(RADTODEG(lAuxAngle))<=90)
//
//				if (fabs(RADTODEG(lAuxAngle))>90) lOrder=-1;
//
//				//if (lOrder>0)
//				if (true)
//				{
//					//Costura hacia adelante
//					for(j=0; j<(limit); j++)
//					{
//						glb_tubeFace_VHandles.clear();
//							index = ( ( ((nodeAux.id-NEURODESP-1)*(globalDesp) ) ) + ((j + despA)%HResolTube) );
//							index += glb_nVerticesSoma;
//							glb_tubeFace_VHandles.push_back(glb_meshVHandle[index]);
//
//							index = abs( int( ( (lOrder*j) + 1 + despB)%lNumVertToSew) );
//							glb_tubeFace_VHandles.push_back(vecVertexHoleHandle.at(index));
//
//							index = abs( int( ( (lOrder*j) + despB)%lNumVertToSew) );
//							glb_tubeFace_VHandles.push_back(vecVertexHoleHandle.at(index));
//						Mesh->add_face(glb_tubeFace_VHandles);
//
//						glb_tubeFace_VHandles.clear();
//							index = ( ( ((nodeAux.id-NEURODESP-1)*(globalDesp) ) ) + ((j + despA)%HResolTube) );
//							index += glb_nVerticesSoma;
//							glb_tubeFace_VHandles.push_back(glb_meshVHandle[index]);
//
//							index = ( ( ((nodeAux.id-NEURODESP-1)*(globalDesp) ) ) + ((j + 1 + despA)%HResolTube) );
//							index += glb_nVerticesSoma;
//							glb_tubeFace_VHandles.push_back(glb_meshVHandle[index]);
//
//							index = abs( int ( ( (lOrder*j) + 1 + despB)%lNumVertToSew) );
//							glb_tubeFace_VHandles.push_back(vecVertexHoleHandle.at(index));
//						Mesh->add_face(glb_tubeFace_VHandles);
//					}
//
//					//Sew diferent number of vertex
//					if (vecVertexHoleHandle.size()>HResolTube)
//					{
//						int i=0;
//						for (i=HResolTube;i<vecVertexHoleHandle.size();++i)
//						{
//							glb_tubeFace_VHandles.clear();
//
//								index = ( ( ((nodeAux.id-NEURODESP-1)*(globalDesp) ) ) + ((j + despA)%HResolTube) );
//								index += glb_nVerticesSoma;
//								glb_tubeFace_VHandles.push_back(glb_meshVHandle[index]);
//
//								index = abs( int ( ( (lOrder*i) + 1 + despB)%lNumVertToSew) );
//								glb_tubeFace_VHandles.push_back(vecVertexHoleHandle.at(index));
//
//								index = abs( int ( ( (lOrder*i) + despB)%lNumVertToSew) );
//								glb_tubeFace_VHandles.push_back(vecVertexHoleHandle.at(index));
//
//							Mesh->add_face(glb_tubeFace_VHandles);
//						}
//
//						glb_tubeFace_VHandles.clear();
//
//							index = ( ( ( (nodeAux.id-NEURODESP-1)*(globalDesp) ) ) + ((despA)%HResolTube) );
//							index += glb_nVerticesSoma;
//							glb_tubeFace_VHandles.push_back(glb_meshVHandle[index]);
//
//							index = 0;
//							glb_tubeFace_VHandles.push_back(vecVertexHoleHandle.at(index));
//
//							index = abs( int ( ( (lOrder*i) + despB)%lNumVertToSew) );
//							glb_tubeFace_VHandles.push_back(vecVertexHoleHandle.at(index));
//
//						Mesh->add_face(glb_tubeFace_VHandles);
//					}
//					else
//					{
//						//!!! Menos vertices en el agujero
//						//Fijar ultimo vertice del agujero y fan al tubo
//						int i=0;
//						for (i=vecVertexHoleHandle.size();i<HResolTube;++i)
//						{
//							glb_tubeFace_VHandles.clear();
//								index = ( ( ((nodeAux.id-NEURODESP-1)*(globalDesp) ) ) + ((i + despA)%HResolTube) );
//								index += glb_nVerticesSoma;
//								glb_tubeFace_VHandles.push_back(glb_meshVHandle[index]);
//
//								index = ( ( ((nodeAux.id-NEURODESP-1)*(globalDesp) ) ) + ((i + 1 + despA)%HResolTube) );
//								index += glb_nVerticesSoma;
//								glb_tubeFace_VHandles.push_back(glb_meshVHandle[index]);
//
//								index = abs( int ( ( (lOrder*j) + despB)%lNumVertToSew) );
//								glb_tubeFace_VHandles.push_back(vecVertexHoleHandle.at(index));
//							Mesh->add_face(glb_tubeFace_VHandles);
//						}	//for
//					} //else -> if (vecVertexHoleHandle.size()>HResolTube)
//				} //if (distCW>distCCW)
//				else
//				{
//					lOrder = -1;
//					//!!!Costura inversa, de momento se obviaran hasta que se detecten de manera correcta
//					for(j=0; j<(limit); j++)
//					//for(j=0; j<3; j++)
//					{
//						glb_tubeFace_VHandles.clear();
//							index = ( ( ((nodeAux.id-NEURODESP-1)*(globalDesp) ) ) + ((j + despA)%HResolTube) );
//							index += glb_nVerticesSoma;
//							glb_tubeFace_VHandles.push_back(glb_meshVHandle[index]);
//
//							index = abs( int( ( (lOrder*j) + despB)%lNumVertToSew) );
//							glb_tubeFace_VHandles.push_back(vecVertexHoleHandle.at(index));
//
//							index = abs( int( ( (lOrder*j) + lOrder*1 + despB)%lNumVertToSew) );
//							glb_tubeFace_VHandles.push_back(vecVertexHoleHandle.at(index));
//
//						Mesh->add_face(glb_tubeFace_VHandles);
//
//						glb_tubeFace_VHandles.clear();
//
//							index = ( ( ((nodeAux.id-NEURODESP-1)*(globalDesp) ) ) + ((j + despA)%HResolTube) );
//							index += glb_nVerticesSoma;
//							glb_tubeFace_VHandles.push_back(glb_meshVHandle[index]);
//
//							index = abs( int( ( (lOrder*j) + lOrder*1 + despB)%lNumVertToSew) );
//							glb_tubeFace_VHandles.push_back(vecVertexHoleHandle.at(index));
//
//							index = ( ( ((nodeAux.id-NEURODESP-1)*(globalDesp) ) ) + abs( int((j + 1 + despA) )%HResolTube) );
//							index += glb_nVerticesSoma;
//							glb_tubeFace_VHandles.push_back(glb_meshVHandle[index]);
//
//						Mesh->add_face(glb_tubeFace_VHandles);
//					}
//
//				} //else -> if (distCW>distCCW)
//			}
//			//Clean the aux containers
//			vecVertexHoleHandle.clear();
//			vecVertexHoleHandleIds.clear();
//
//			vecFaceHoleHandle.clear();
//			vecFaceHoleHandleIds.clear();
//
//			////Free the memory of deleted objects
//			//Mesh->garbage_collection();
//
//			////Update the mesh
//			//updateBaseMesh();
//		}
//		//Free the memory of deleted objects
//		Mesh->garbage_collection();
//
//		//Update the mesh
//		updateBaseMesh();
//	}
//

  void NeuroSWC::connectSoma ( )
  {
    int numConnections = importer->getNumConnection ( );

    SWCConnection nodeConnect;
    SWCNode nodeAux;
    SWCNode parentNodeAux;

    unsigned int globalDesp = HResolTube*VResolTube;
    unsigned int index;

    int j = 0;
    unsigned int consecValues;
    unsigned int indexCandidate;
    float dis;

    MeshDef::Point lBariCenter ( 0, 0, 0 );
    MeshDef::Point ltmpVertex ( 0, 0, 0 );
    OpenMesh::Vec3f ltmpVertexResult ( 0, 0, 0 );

    MeshDef::VertexHandle vhandle;
    MeshDef::HalfedgeHandle hehandle;
    MeshDef::FaceHandle fhandle;

    MeshDef::VertexHandle vIdx[3];
    float lAuxDis = 10E10;

    OpenMesh::Vec3f lVertex[3];
    MeshDef::FaceHandle fCandHandle;
    MeshDef::VertexFaceIter lVFIter;

    float lSigAreaTriang = 0;
    float lSigAreaTube = 0;

    unsigned int lIdxA;
    unsigned int lIdxB;

    MeshDef::Point lInitPointA ( 0, 0, 0 );
    MeshDef::Point lInitPointB ( 0, 0, 0 );
    OpenMesh::Vec3f lTmpVecResult ( 0, 0, 0 );

    unsigned int despA = 0;
    unsigned int despB = 0;
    unsigned int despTmpA = 0;
    unsigned int despTmpB = 0;

    std::vector <MeshDef::VertexHandle> vecVertexHoleHandle;
    std::vector <MeshDef::VertexHandle> vecIsolatedVertex;

    std::vector < unsigned int > vecVertexHoleHandleIds;

    std::vector <MeshDef::FaceHandle> vecFaceHoleHandle;
    std::vector <MeshDef::FaceHandle> totalVecFaceHolesHandles;
    std::vector < unsigned int > vecFaceHoleHandleIds;

    //Comprobar que no se halla seleccionado ya para borrar su facetas
    //de lo contrario no tendra facetas asociadas, hay q escoger otro

    //Problema!!!! en la costura, hay menos elementos en el std vector que en la seccion del tubo,
    //por eso se sale de rango en la costura

    unsigned int lNumSews = 0;

    for ( int i = 0; i < ( numConnections - 1 ); ++i )
    {
      //Inicializations the local vars of the segment to sew
      nodeConnect = importer->getConnectionAt ( i );
      nodeAux = importer->getElementAt ( nodeConnect.fin );
      parentNodeAux = importer->getElementAt ( nodeConnect.ini );

      j = 0;
      consecValues = nodeAux.id - nodeAux.parent;
      dis = 10E10;
      indexCandidate = 0;

      lBariCenter = MeshDef::Point ( 0, 0, 0 );
      ltmpVertex = MeshDef::Point ( 0, 0, 0 );

      //No consecutive segments, requiere connection at inition
      if ( nodeAux.parent == mSomaId )
      {
        //Calcular el baricentro del segmento inicial de los v�rtices del nodo
        for ( j = 0; j < ( HResolTube ); j++ )
        {
          //Calc the index of the vertex
          index = (( nodeAux.id - NEURODESP - 1 )*( globalDesp ) + j );
          index += glb_nVerticesSoma;
          lBariCenter += Mesh->points ( )[( index )];
        }

        lBariCenter /= ( float ) ( HResolTube );

        //Calcular el vertice mas cercano de los q conforma el soma
        //Ponerlo como candidate para eliminar la faceta y q se haga la costura
        indexCandidate = 0;
        ltmpVertexResult = OpenMesh::Vec3f ( lBariCenter
                                               - Mesh->points ( )[( indexCandidate )] );
        dis = ltmpVertexResult.length ( );

        //Escogemos el vertice mas cercano del soma que no halla sido borrado
        for ( j = 1; j < glb_nVerticesSoma; j++ )
        {
          index = j;
          ltmpVertex = Mesh->points ( )[( index )];
          ltmpVertexResult = OpenMesh::Vec3f ( lBariCenter - ltmpVertex );

          if (( ltmpVertexResult.length ( ) < dis )
            && ( !isVertexHandleInContainer ( Mesh->vertex_handle ( index ), vecIsolatedVertex ))
            )
          {
            indexCandidate = index;
            dis = ltmpVertexResult.length ( );
          }
        }

        //Select the vertex handler
        vhandle = Mesh->vertex_handle ( indexCandidate );
        vecIsolatedVertex.push_back ( vhandle );

        //Store all the incidente vertex handles
        MeshDef::ConstVertexVertexIter CVVIter;
        MeshDef::VertexHandle lAuxVertexHandle;

        for ( CVVIter = Mesh->cvv_iter ( vhandle ); CVVIter.is_valid ( ); ++CVVIter )
        {
          lAuxVertexHandle = Mesh->vertex_handle ( CVVIter->idx ( ));
          vecVertexHoleHandle.push_back ( lAuxVertexHandle );
        }


        //Store the ids of the selected Vertexs
        for ( unsigned int i = 0; i < vecVertexHoleHandle.size ( ); ++i )
        {
          vecVertexHoleHandleIds.push_back ( vecVertexHoleHandle.at ( i ).idx ( ));
        }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
				//Delete vertex and incident faces
				//Delete the vertex and his incident faces
				//!!!   Se deberian borrar las facetas una a una para no borrar le vertice y perder los indices
				if (vhandle.is_valid()) Mesh->delete_vertex(vhandle);

				//Luego es necesarios

				//!!! Ordenar los indices de forma CW o CCW
				//Determinar distancias para costura minima
*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////

        //Store all the incidente faces handles
        MeshDef::ConstVertexFaceIter CVFIter;
        MeshDef::FaceHandle lAuxFaceHandle;

        for ( CVFIter = Mesh->cvf_iter ( vhandle ); CVFIter.is_valid ( ); ++CVFIter )
        {
          lAuxFaceHandle = Mesh->face_handle ( CVFIter->idx ( ));
          vecFaceHoleHandle.push_back ( lAuxFaceHandle );
        }

        //Store the ids of the selected Faces
        for ( unsigned int i = 0; i < vecFaceHoleHandle.size ( ); ++i )
        {
          vecFaceHoleHandleIds.push_back ( vecFaceHoleHandle.at ( i ).idx ( ));
        }

        //Delete the selected faces
        for ( unsigned int i = 0; i < vecFaceHoleHandle.size ( ); ++i )
        {
          if ( vecFaceHoleHandle.at ( i ).is_valid ( )
            && !isFaceHandleInContainer ( vecFaceHoleHandle.at ( i ), totalVecFaceHolesHandles )
            )
            Mesh->delete_face ( vecFaceHoleHandle.at ( i ), false );
        }

        //Add the handle to the faces handles
        for ( CVFIter = Mesh->cvf_iter ( vhandle ); CVFIter.is_valid ( ); ++CVFIter )
        {
          lAuxFaceHandle = Mesh->face_handle ( CVFIter->idx ( ));
          totalVecFaceHolesHandles.push_back ( lAuxFaceHandle );
        }


        ////!!! Para que no pete en modo debug, el borrado de las facetas peta la costura, hay q actualizarlo antes
        ////Free the memory of deleted objects
        //Mesh->garbage_collection();

        ////Update the mesh
        //updateBaseMesh();

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

        //Desplazamiento con los vertices a coser
        //Hay que comenzar desde los dos v�rtices con distancia m�nima
        despA = 0;
        despTmpA = 0;

        despB = 0;
        despTmpB = 0;

        dis = 10E10;

        unsigned int lNumVertToSew = vecVertexHoleHandle.size ( );

        //Calculamos la distancia m�nima entre cualquier vertice de la dendrita y los del soma
        //Las variables despA y despB contiend cuanto nos tenemos que desplazar para que la distancia
        //entre el vertice inicial a coser de soma y dendrita sea minima
        for ( int i = 0; i < HResolTube; i++ )
        {
          lIdxA = (((( nodeAux.id - NEURODESP - 1 )*( globalDesp ))) + i );
          lIdxA += glb_nVerticesSoma;

          lInitPointA = Mesh->points ( )[( lIdxA )];

          //Elementos del soma
          for ( j = 0; j < lNumVertToSew; j++ )
          {
            lIdxB = vecVertexHoleHandle.at ( j ).idx ( );

            lInitPointB = Mesh->points ( )[( lIdxB )];
            lTmpVecResult = OpenMesh::Vec3f ( lInitPointA - lInitPointB );

            if ( lTmpVecResult.length ( ) < dis )
            {
              dis = lTmpVecResult.length ( );

              despB = despTmpB;
              despA = despTmpA;
            }
            ++despTmpB;
          }

          ++despTmpA;
          despTmpB = 0;
        }

/*
				//Distance method evaluation
				//Determinar si se invierten los indices de los vertices a coser
				unsigned int lIndexTube = ( ( ((nodeAux.id-NEURODESP-1)*(globalDesp) ) ) + ((1 + despA)%HResolTube) );
				lIndexTube				+= glb_nVerticesSoma;

				unsigned int lNextIndexSoma = ( (1 + despB)%lNumVertToSew);
				unsigned int lPrevIndexSoma = ( ((lNumVertToSew-1) + despB)%lNumVertToSew);

				float distCCW	= OpenMesh::Vec3f(Mesh->points()[lIndexTube]
									- Mesh->points()[lNextIndexSoma]).length();


				float distCW	= OpenMesh::Vec3f(Mesh->points()[lIndexTube]
									- Mesh->points()[lPrevIndexSoma]).length();


				//Invertir vector
				int lOrder=1;
				unsigned int limit=HResolTube;
				if (vecVertexHoleHandle.size()<limit)
					limit=vecVertexHoleHandle.size();
*/

        //Angle method evaluation
        //Index Vertex int the tube
        unsigned int lIndexTube = (((( nodeAux.id - NEURODESP - 1 )*( globalDesp ))) + (( despA )%HResolTube ));
        lIndexTube += glb_nVerticesSoma;
        unsigned int lNextIndexTube = (((( nodeAux.id - NEURODESP - 1 )*( globalDesp ))) + (( 1 + despA )%HResolTube ));
        lNextIndexTube += glb_nVerticesSoma;
        unsigned int lPrevIndexTube =
          (((( nodeAux.id - NEURODESP - 1 )*( globalDesp ))) + ((( HResolTube - 1 ) + despA )%HResolTube ));
        lPrevIndexTube += glb_nVerticesSoma;

        //Index in the temp vextes of the container
        unsigned int lIndexSoma = vecVertexHoleHandle.at (( despB )%lNumVertToSew ).idx ( );
        unsigned int lNextIndexSoma = vecVertexHoleHandle.at (( 1 + despB )%lNumVertToSew ).idx ( );
        unsigned int lPrevIndexSoma = vecVertexHoleHandle.at ((( lNumVertToSew - 1 ) + despB )%lNumVertToSew ).idx ( );

        //Calculate the vectors
        OpenMesh::Vec3f lVecAB = OpenMesh::Vec3f (
          Mesh->points ( )[lNextIndexTube]
            - Mesh->points ( )[lIndexTube]
        );

        OpenMesh::Vec3f lVecAC = OpenMesh::Vec3f (
          Mesh->points ( )[lPrevIndexTube]
            - Mesh->points ( )[lIndexTube]
        );

        //Cross product == producto vectorial
        OpenMesh::Vec3f lCrossAB_AC = OpenMesh::cross ( lVecAB, lVecAC );


        //Calculate the vectors
        OpenMesh::Vec3f lVecDE = OpenMesh::Vec3f (
          Mesh->points ( )[lNextIndexSoma]
            - Mesh->points ( )[lIndexSoma]
        );

        OpenMesh::Vec3f lVecDF = OpenMesh::Vec3f (
          Mesh->points ( )[lPrevIndexSoma]
            - Mesh->points ( )[lIndexSoma]
        );

        OpenMesh::Vec3f lCrossDE_DF = OpenMesh::cross ( lVecDE, lVecDF );

        lCrossAB_AC.normalize ( );
        lCrossDE_DF.normalize ( );

        float lAuxDot = OpenMesh::dot ( lCrossAB_AC, lCrossDE_DF );
        float lAuxAngle = acos ( lAuxDot );

        int lOrder = 1;
        unsigned int limit = HResolTube;
        if ( vecVertexHoleHandle.size ( ) < limit )
          limit = vecVertexHoleHandle.size ( );


        //lOrder=-1;
        //if (distCW>distCCW)

        //if (RADTODEG(lAuxAngle)>90)		lOrder=-1;

        //if (true)
        //if (fabs(RADTODEG(lAuxAngle))<=90)

        if ( fabs ( RADTODEG ( lAuxAngle )) > 90 )
          lOrder = -1;


        //Cosemos siempre hacia el mismo lado
        //parece que tiea bien ;P

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////

        unsigned int lNumCanVertexInSoma = vecVertexHoleHandleIds.size ( );
        unsigned int lNumCanVertexInDend = HResolTube;
        float lVertexRate = ( float ) lNumCanVertexInSoma
          /( float ) lNumCanVertexInDend;

        if ( lVertexRate > VERTRATESOMASEW )
        {
          int k = 0;
          //Recorremos los v�rtices del agujero en la dendrita
          for ( j = 0; j < lNumCanVertexInDend; ++j )
          {
            //Avanzamos pr el agujero en el soma
            for ( k = ( j*lVertexRate ); k < (( j + 1 )*lVertexRate ); ++k )
            {
              glb_tubeFace_VHandles.clear ( );
              index = (((( nodeAux.id - NEURODESP - 1 )*( globalDesp ))) + (( j + despA )%HResolTube ));
              index += glb_nVerticesSoma;
              glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

              index = abs ( int ((( lOrder*k ) + 1 + despB )%lNumVertToSew ));
              glb_tubeFace_VHandles.push_back ( vecVertexHoleHandle.at ( index ));

              index = abs ( int ((( lOrder*k ) + despB )%lNumVertToSew ));
              glb_tubeFace_VHandles.push_back ( vecVertexHoleHandle.at ( index ));
              Mesh->add_face ( glb_tubeFace_VHandles );

            }

            --k;
            glb_tubeFace_VHandles.clear ( );
            index = (((( nodeAux.id - NEURODESP - 1 )*( globalDesp ))) + (( j + despA )%HResolTube ));
            index += glb_nVerticesSoma;
            glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

            index = (((( nodeAux.id - NEURODESP - 1 )*( globalDesp ))) + (( j + 1 + despA )%HResolTube ));
            index += glb_nVerticesSoma;
            glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

            index = abs ( int ((( lOrder*k ) + 1 + despB )%lNumVertToSew ));
            glb_tubeFace_VHandles.push_back ( vecVertexHoleHandle.at ( index ));
            Mesh->add_face ( glb_tubeFace_VHandles );
          }

          //sew the rest of the vertexs
          --j;

          //Ultimo vertice del hole contra los restantes
          //!!! -> Tal vez hay que iterar, no tengo claro que s�lo asi funcione
          glb_tubeFace_VHandles.clear ( );

          index = (((( nodeAux.id - NEURODESP - 1 )*( globalDesp ))) + (( j + despA )%HResolTube ));
          index += glb_nVerticesSoma;
          glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

          index = (((( nodeAux.id - NEURODESP - 1 )*( globalDesp ))) + (( j + 1 + despA )%HResolTube ));
          index += glb_nVerticesSoma;
          glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

          index = abs ( int (( despB )%lNumVertToSew ));
          glb_tubeFace_VHandles.push_back ( vecVertexHoleHandle.at ( index ));

          Mesh->add_face ( glb_tubeFace_VHandles );
        }
        else
        {
          //if (lOrder>0)
          if ( true )
          {
            //Costura hacia adelante
            for ( j = 0; j < ( limit ); j++ )
            {
              glb_tubeFace_VHandles.clear ( );
              index = (((( nodeAux.id - NEURODESP - 1 )*( globalDesp ))) + (( j + despA )%HResolTube ));
              index += glb_nVerticesSoma;
              glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

              index = abs ( int ((( lOrder*j ) + 1 + despB )%lNumVertToSew ));
              glb_tubeFace_VHandles.push_back ( vecVertexHoleHandle.at ( index ));

              index = abs ( int ((( lOrder*j ) + despB )%lNumVertToSew ));
              glb_tubeFace_VHandles.push_back ( vecVertexHoleHandle.at ( index ));
              Mesh->add_face ( glb_tubeFace_VHandles );

              glb_tubeFace_VHandles.clear ( );
              index = (((( nodeAux.id - NEURODESP - 1 )*( globalDesp ))) + (( j + despA )%HResolTube ));
              index += glb_nVerticesSoma;
              glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

              index = (((( nodeAux.id - NEURODESP - 1 )*( globalDesp ))) + (( j + 1 + despA )%HResolTube ));
              index += glb_nVerticesSoma;
              glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

              index = abs ( int ((( lOrder*j ) + 1 + despB )%lNumVertToSew ));
              glb_tubeFace_VHandles.push_back ( vecVertexHoleHandle.at ( index ));
              Mesh->add_face ( glb_tubeFace_VHandles );
            }

            //Sew diferent number of vertex
            if ( vecVertexHoleHandle.size ( ) > HResolTube )
            {
              int i = 0;
              for ( i = HResolTube; i < vecVertexHoleHandle.size ( ); ++i )
              {
                glb_tubeFace_VHandles.clear ( );

                index = (((( nodeAux.id - NEURODESP - 1 )*( globalDesp ))) + (( j + despA )%HResolTube ));
                index += glb_nVerticesSoma;
                glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

                index = abs ( int ((( lOrder*i ) + 1 + despB )%lNumVertToSew ));
                glb_tubeFace_VHandles.push_back ( vecVertexHoleHandle.at ( index ));

                index = abs ( int ((( lOrder*i ) + despB )%lNumVertToSew ));
                glb_tubeFace_VHandles.push_back ( vecVertexHoleHandle.at ( index ));

                Mesh->add_face ( glb_tubeFace_VHandles );
              }

              glb_tubeFace_VHandles.clear ( );

              index = (((( nodeAux.id - NEURODESP - 1 )*( globalDesp ))) + (( despA )%HResolTube ));
              index += glb_nVerticesSoma;
              glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

              index = 0;
              glb_tubeFace_VHandles.push_back ( vecVertexHoleHandle.at ( index ));

              index = abs ( int ((( lOrder*i ) + despB )%lNumVertToSew ));
              glb_tubeFace_VHandles.push_back ( vecVertexHoleHandle.at ( index ));

              Mesh->add_face ( glb_tubeFace_VHandles );
            }
            else
            {
              //!!! Menos vertices en el agujero
              //Fijar ultimo vertice del agujero y fan al tubo
              int i = 0;
              for ( i = vecVertexHoleHandle.size ( ); i < HResolTube; ++i )
              {
                glb_tubeFace_VHandles.clear ( );
                index = (((( nodeAux.id - NEURODESP - 1 )*( globalDesp ))) + (( i + despA )%HResolTube ));
                index += glb_nVerticesSoma;
                glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

                index = (((( nodeAux.id - NEURODESP - 1 )*( globalDesp ))) + (( i + 1 + despA )%HResolTube ));
                index += glb_nVerticesSoma;
                glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

                index = abs ( int ((( lOrder*j ) + despB )%lNumVertToSew ));
                glb_tubeFace_VHandles.push_back ( vecVertexHoleHandle.at ( index ));
                Mesh->add_face ( glb_tubeFace_VHandles );
              }  //for
            } //else -> if (vecVertexHoleHandle.size()>HResolTube)
          } //if (distCW>distCCW)
          else
          {
            lOrder = -1;
            //!!!Costura inversa, de momento se obviaran hasta que se detecten de manera correcta
            for ( j = 0; j < ( limit ); j++ )
              //for(j=0; j<3; j++)
            {
              glb_tubeFace_VHandles.clear ( );
              index = (((( nodeAux.id - NEURODESP - 1 )*( globalDesp ))) + (( j + despA )%HResolTube ));
              index += glb_nVerticesSoma;
              glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

              index = abs ( int ((( lOrder*j ) + despB )%lNumVertToSew ));
              glb_tubeFace_VHandles.push_back ( vecVertexHoleHandle.at ( index ));

              index = abs ( int ((( lOrder*j ) + lOrder*1 + despB )%lNumVertToSew ));
              glb_tubeFace_VHandles.push_back ( vecVertexHoleHandle.at ( index ));

              Mesh->add_face ( glb_tubeFace_VHandles );

              glb_tubeFace_VHandles.clear ( );

              index = (((( nodeAux.id - NEURODESP - 1 )*( globalDesp ))) + (( j + despA )%HResolTube ));
              index += glb_nVerticesSoma;
              glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

              index = abs ( int ((( lOrder*j ) + lOrder*1 + despB )%lNumVertToSew ));
              glb_tubeFace_VHandles.push_back ( vecVertexHoleHandle.at ( index ));

              index = (((( nodeAux.id - NEURODESP - 1 )*( globalDesp ))) + abs ( int (( j + 1 + despA ))%HResolTube ));
              index += glb_nVerticesSoma;
              glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

              Mesh->add_face ( glb_tubeFace_VHandles );
            }

          } //else -> if (distCW>distCCW)
        }
      }

      //////////////////////////////////////////////////////////////////////////////////////////////////////////////




      //Clean the aux containers
      vecVertexHoleHandle.clear ( );
      vecVertexHoleHandleIds.clear ( );

      vecFaceHoleHandle.clear ( );
      vecFaceHoleHandleIds.clear ( );

      ////Free the memory of deleted objects
      //Mesh->garbage_collection();

      ////Update the mesh
      //updateBaseMesh();
    }
    //Free the memory of deleted objects
    Mesh->garbage_collection ( );

    //Update the mesh
    updateBaseMesh ( );
  }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  void NeuroSWC::connectPiramidalSoma ( )
  {
    int numConnections = importer->getNumConnection ( );

    SWCConnection nodeConnect;
    SWCNode nodeAux;
    SWCNode parentNodeAux;

    unsigned int globalDesp = HResolTube*VResolTube;
    unsigned int index;

    int j = 0;
    unsigned int consecValues;
    unsigned int indexCandidate;
    float dis;

    MeshDef::Point lBariCenter ( 0, 0, 0 );
    MeshDef::Point ltmpVertex ( 0, 0, 0 );
    OpenMesh::Vec3f ltmpVertexResult ( 0, 0, 0 );

    MeshDef::VertexHandle vhandle;
    MeshDef::HalfedgeHandle hehandle;
    MeshDef::FaceHandle fhandle;

    MeshDef::VertexHandle vIdx[3];
    float lAuxDis = 10E10;

    OpenMesh::Vec3f lVertex[3];
    MeshDef::FaceHandle fCandHandle;
    MeshDef::VertexFaceIter lVFIter;

    float lSigAreaTriang = 0;
    float lSigAreaTube = 0;

    unsigned int lIdxA;
    unsigned int lIdxB;

    MeshDef::Point lInitPointA ( 0, 0, 0 );
    MeshDef::Point lInitPointB ( 0, 0, 0 );
    OpenMesh::Vec3f lTmpVecResult ( 0, 0, 0 );

    unsigned int despA = 0;
    unsigned int despB = 0;
    unsigned int despTmpA = 0;
    unsigned int despTmpB = 0;

    std::vector <MeshDef::VertexHandle> vecVertexHoleHandle;
    std::vector <MeshDef::VertexHandle> vecIsolatedVertex;

    std::vector < unsigned int > vecVertexHoleHandleIds;

    std::vector <MeshDef::FaceHandle> vecFaceHoleHandle;
    std::vector <MeshDef::FaceHandle> totalVecFaceHolesHandles;
    std::vector <MeshDef::FaceHandle> totalVecFaceTubeDelHandles;
    totalVecFaceTubeDelHandles.clear ( );

    std::vector < unsigned int > vecFaceHoleHandleIds;

    //Comprobar que no se halla seleccionado ya para borrar su facetas
    //de lo contrario no tendra facetas asociadas, hay q escoger otro

    //Problema!!!! en la costura, hay menos elementos en el std vector que en la seccion del tubo,
    //por eso se sale de rango en la costura

    unsigned int lNumSews = 0;

    for ( int i = 0; i < ( numConnections - 1 ); ++i )
    {
      //Inicializations the local vars of the segment to sew
      nodeConnect = importer->getConnectionAt ( i );
      nodeAux = importer->getElementAt ( nodeConnect.fin );
      parentNodeAux = importer->getElementAt ( nodeConnect.ini );

      j = 0;
      consecValues = nodeAux.id - nodeAux.parent;
      dis = 10E10;
      indexCandidate = 0;

      lBariCenter = MeshDef::Point ( 0, 0, 0 );
      ltmpVertex = MeshDef::Point ( 0, 0, 0 );

      //No consecutive segments, requiere connection at inition
      if ( nodeAux.parent == mSomaId )
      {
        //Calcular el baricentro del segmento inicial de los v�rtices del nodo
        for ( j = 0; j < ( HResolTube ); j++ )
        {
          //Calc the index of the vertex
          index = (( nodeAux.id - NEURODESP - 1 )*( globalDesp ) + j );
          index += glb_nVerticesSoma;
          lBariCenter += Mesh->points ( )[( index )];
        }

        lBariCenter /= ( float ) ( HResolTube );

        //Calcular el vertice mas cercano de los q conforma el soma
        //Ponerlo como candidate para eliminar la faceta y q se haga la costura
        indexCandidate = 0;
        ltmpVertexResult = OpenMesh::Vec3f ( lBariCenter
                                               - Mesh->points ( )[( indexCandidate )] );
        dis = ltmpVertexResult.length ( );

        //Escogemos el vertice mas cercano del soma que no halla sido borrado
        for ( j = 1; j < glb_nVerticesSoma; j++ )
        {
          index = j;
          ltmpVertex = Mesh->points ( )[( index )];
          ltmpVertexResult = OpenMesh::Vec3f ( lBariCenter - ltmpVertex );

          if (( ltmpVertexResult.length ( ) < dis )
            && ( !isVertexHandleInContainer ( Mesh->vertex_handle ( index ), vecIsolatedVertex ))
            )
          {
            indexCandidate = index;
            dis = ltmpVertexResult.length ( );
          }
        }

        //Select the vertex handler
        vhandle = Mesh->vertex_handle ( indexCandidate );
        vecIsolatedVertex.push_back ( vhandle );

        //Store all the incidente vertex handles
        MeshDef::ConstVertexVertexIter CVVIter;
        MeshDef::VertexHandle lAuxVertexHandle;

        for ( CVVIter = Mesh->cvv_iter ( vhandle ); CVVIter.is_valid ( ); ++CVVIter )
        {
          lAuxVertexHandle = Mesh->vertex_handle ( CVVIter->idx ( ));
          vecVertexHoleHandle.push_back ( lAuxVertexHandle );
        }


        //Store the ids of the selected Vertexs
        for ( unsigned int i = 0; i < vecVertexHoleHandle.size ( ); ++i )
        {
          vecVertexHoleHandleIds.push_back ( vecVertexHoleHandle.at ( i ).idx ( ));
        }

        //Store all the incidente faces handles
        MeshDef::ConstVertexFaceIter CVFIter;
        MeshDef::FaceHandle lAuxFaceHandle;

        for ( CVFIter = Mesh->cvf_iter ( vhandle ); CVFIter.is_valid ( ); ++CVFIter )
        {
          lAuxFaceHandle = Mesh->face_handle ( CVFIter->idx ( ));
          vecFaceHoleHandle.push_back ( lAuxFaceHandle );
        }

        //Store the ids of the selected Faces
        for ( unsigned int i = 0; i < vecFaceHoleHandle.size ( ); ++i )
        {
          vecFaceHoleHandleIds.push_back ( vecFaceHoleHandle.at ( i ).idx ( ));
        }

        //Delete the selected faces (from Soma)
        for ( unsigned int i = 0; i < vecFaceHoleHandle.size ( ); ++i )
        {
          if ( vecFaceHoleHandle.at ( i ).is_valid ( )
            && !isFaceHandleInContainer ( vecFaceHoleHandle.at ( i ), totalVecFaceHolesHandles )
            )
            Mesh->delete_face ( vecFaceHoleHandle.at ( i ), false );
        }

        //Add the handle to the faces handles
        for ( CVFIter = Mesh->cvf_iter ( vhandle ); CVFIter.is_valid ( ); ++CVFIter )
        {
          lAuxFaceHandle = Mesh->face_handle ( CVFIter->idx ( ));
          totalVecFaceHolesHandles.push_back ( lAuxFaceHandle );
        }

        //Truco para la costura con somas piramidales

        //Eliminar facetas del tubo inicial de la dendrita (todas las facetas adyacentes a los vertices que lo conforman)
        std::vector <MeshDef::FaceHandle> vecFaceTubeFaceDelHandle;
        vecFaceTubeFaceDelHandle.clear ( );
        MeshDef::ConstVertexFaceIter CVFIterFaceDel;
        MeshDef::FaceHandle lAuxFaceHandleFaceDel;

        MeshDef::VertexHandle vhandleFaceDel;

        std::vector <MeshDef::FaceHandle> tmpVecFaceTubeDelHandles;
        tmpVecFaceTubeDelHandles.clear ( );

        for ( unsigned int k = 0; k < ( HResolTube*VResolTube ); ++k )
        {
          //Seleccionar vertice
          unsigned int lIdx = (( nodeAux.id - NEURODESP - 1 )*( globalDesp ) + k );
          lIdx += glb_nVerticesSoma;

          vhandleFaceDel = Mesh->vertex_handle ( lIdx );

          //Recorrer todas sus facetas adyacentes y borrarlas
          for ( CVFIterFaceDel = Mesh->cvf_iter ( vhandleFaceDel ); CVFIterFaceDel.is_valid ( ); ++CVFIterFaceDel )
          {
            lAuxFaceHandleFaceDel = Mesh->face_handle ( CVFIterFaceDel->idx ( ));
            tmpVecFaceTubeDelHandles.push_back ( lAuxFaceHandleFaceDel );
          }

          for ( unsigned int i = 0; i < tmpVecFaceTubeDelHandles.size ( ); ++i )
          {
            if ( tmpVecFaceTubeDelHandles.at ( i ).is_valid ( )
              && !isFaceHandleInContainer ( tmpVecFaceTubeDelHandles.at ( i ), totalVecFaceTubeDelHandles )
              )
            {
              Mesh->delete_face ( tmpVecFaceTubeDelHandles.at ( i ), false );
              totalVecFaceTubeDelHandles.push_back ( tmpVecFaceTubeDelHandles.at ( i ));
            }
          }

        }

        //Cogemos el siguiente nodo
        nodeConnect = importer->getConnectionAt ( i + 1 );
        nodeAux = importer->getElementAt ( nodeConnect.fin );

        //Desplazamiento con los vertices a coser
        //Hay que comenzar desde los dos v�rtices con distancia m�nima
        despA = 0;
        despTmpA = 0;

        despB = 0;
        despTmpB = 0;

        dis = 10E10;

        unsigned int lNumVertToSew = vecVertexHoleHandle.size ( );

        //Calculamos la distancia m�nima entre cualquier vertice de la dendrita y los del soma
        //Las variables despA y despB contiend cuanto nos tenemos que desplazar para que la distancia
        //entre el vertice inicial a coser de soma y dendrita sea minima
        for ( int i = 0; i < HResolTube; i++ )
        {
          lIdxA = (((( nodeAux.id - NEURODESP - 1 )*( globalDesp ))) + i );
          lIdxA += glb_nVerticesSoma;

          lInitPointA = Mesh->points ( )[( lIdxA )];

          //Elementos del soma
          for ( j = 0; j < lNumVertToSew; j++ )
          {
            lIdxB = vecVertexHoleHandle.at ( j ).idx ( );

            lInitPointB = Mesh->points ( )[( lIdxB )];
            lTmpVecResult = OpenMesh::Vec3f ( lInitPointA - lInitPointB );

            if ( lTmpVecResult.length ( ) < dis )
            {
              dis = lTmpVecResult.length ( );

              despB = despTmpB;
              despA = despTmpA;
            }
            ++despTmpB;
          }

          ++despTmpA;
          despTmpB = 0;
        }

        //Angle method evaluation
        //Index Vertex int the tube
        unsigned int lIndexTube = (((( nodeAux.id - NEURODESP - 1 )*( globalDesp ))) + (( despA )%HResolTube ));
        lIndexTube += glb_nVerticesSoma;
        unsigned int lNextIndexTube = (((( nodeAux.id - NEURODESP - 1 )*( globalDesp ))) + (( 1 + despA )%HResolTube ));
        lNextIndexTube += glb_nVerticesSoma;
        unsigned int lPrevIndexTube =
          (((( nodeAux.id - NEURODESP - 1 )*( globalDesp ))) + ((( HResolTube - 1 ) + despA )%HResolTube ));
        lPrevIndexTube += glb_nVerticesSoma;

        //Index in the temp vextes of the container
        unsigned int lIndexSoma = vecVertexHoleHandle.at (( despB )%lNumVertToSew ).idx ( );
        unsigned int lNextIndexSoma = vecVertexHoleHandle.at (( 1 + despB )%lNumVertToSew ).idx ( );
        unsigned int lPrevIndexSoma = vecVertexHoleHandle.at ((( lNumVertToSew - 1 ) + despB )%lNumVertToSew ).idx ( );

        //Calculate the vectors
        OpenMesh::Vec3f lVecAB = OpenMesh::Vec3f (
          Mesh->points ( )[lNextIndexTube]
            - Mesh->points ( )[lIndexTube]
        );

        OpenMesh::Vec3f lVecAC = OpenMesh::Vec3f (
          Mesh->points ( )[lPrevIndexTube]
            - Mesh->points ( )[lIndexTube]
        );

        //Cross product == producto vectorial
        OpenMesh::Vec3f lCrossAB_AC = OpenMesh::cross ( lVecAB, lVecAC );


        //Calculate the vectors
        OpenMesh::Vec3f lVecDE = OpenMesh::Vec3f (
          Mesh->points ( )[lNextIndexSoma]
            - Mesh->points ( )[lIndexSoma]
        );

        OpenMesh::Vec3f lVecDF = OpenMesh::Vec3f (
          Mesh->points ( )[lPrevIndexSoma]
            - Mesh->points ( )[lIndexSoma]
        );

        OpenMesh::Vec3f lCrossDE_DF = OpenMesh::cross ( lVecDE, lVecDF );

        lCrossAB_AC.normalize ( );
        lCrossDE_DF.normalize ( );

        float lAuxDot = OpenMesh::dot ( lCrossAB_AC, lCrossDE_DF );
        float lAuxAngle = acos ( lAuxDot );

        int lOrder = 1;
        unsigned int limit = HResolTube;
        if ( vecVertexHoleHandle.size ( ) < limit )
          limit = vecVertexHoleHandle.size ( );

        //!!!El test de direcci�n ya no es valido, ahora el siguiente segmento
        //no tiene porque estar dirigo hacia el soma
        //if (fabs(RADTODEG(lAuxAngle))>90) lOrder=-1;

        unsigned int lNumCanVertexInSoma = vecVertexHoleHandleIds.size ( );
        unsigned int lNumCanVertexInDend = HResolTube;
        float lVertexRate = ( float ) lNumCanVertexInSoma
          /( float ) lNumCanVertexInDend;

        if ( lVertexRate >= VERTRATESOMASEW )
        {
          int k = 0;
          //Recorremos los v�rtices del agujero en la dendrita
          for ( j = 0; j < lNumCanVertexInDend; ++j )
          {
            //Avanzamos pr el agujero en el soma
            for ( k = ( j*lVertexRate ); k < (( j + 1 )*lVertexRate ); ++k )
            {
              glb_tubeFace_VHandles.clear ( );
              index = (((( nodeAux.id - NEURODESP - 1 )*( globalDesp ))) + (( j + despA )%HResolTube ));
              index += glb_nVerticesSoma;
              glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

              index = abs ( int ((( lOrder*k ) + 1 + despB )%lNumVertToSew ));
              glb_tubeFace_VHandles.push_back ( vecVertexHoleHandle.at ( index ));

              index = abs ( int ((( lOrder*k ) + despB )%lNumVertToSew ));
              glb_tubeFace_VHandles.push_back ( vecVertexHoleHandle.at ( index ));
              Mesh->add_face ( glb_tubeFace_VHandles );

            }

            --k;
            glb_tubeFace_VHandles.clear ( );
            index = (((( nodeAux.id - NEURODESP - 1 )*( globalDesp ))) + (( j + despA )%HResolTube ));
            index += glb_nVerticesSoma;
            glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

            index = (((( nodeAux.id - NEURODESP - 1 )*( globalDesp ))) + (( j + 1 + despA )%HResolTube ));
            index += glb_nVerticesSoma;
            glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

            index = abs ( int ((( lOrder*k ) + 1 + despB )%lNumVertToSew ));
            glb_tubeFace_VHandles.push_back ( vecVertexHoleHandle.at ( index ));
            Mesh->add_face ( glb_tubeFace_VHandles );
          }

          //sew the rest of the vertexs
          --j;

          //Ultimo vertice del hole contra los restantes
          //!!! -> Tal vez hay que iterar, no tengo claro que s�lo asi funcione
          glb_tubeFace_VHandles.clear ( );

          index = (((( nodeAux.id - NEURODESP - 1 )*( globalDesp ))) + (( j + despA )%HResolTube ));
          index += glb_nVerticesSoma;
          glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

          index = (((( nodeAux.id - NEURODESP - 1 )*( globalDesp ))) + (( j + 1 + despA )%HResolTube ));
          index += glb_nVerticesSoma;
          glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

          index = abs ( int (( despB )%lNumVertToSew ));
          glb_tubeFace_VHandles.push_back ( vecVertexHoleHandle.at ( index ));

          Mesh->add_face ( glb_tubeFace_VHandles );
        }
        else
        {
          //if (lOrder>0)
          if ( true )
          {
            //Costura hacia adelante
            for ( j = 0; j < ( limit ); j++ )
            {
              glb_tubeFace_VHandles.clear ( );
              index = (((( nodeAux.id - NEURODESP - 1 )*( globalDesp ))) + (( j + despA )%HResolTube ));
              index += glb_nVerticesSoma;
              glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

              index = abs ( int ((( lOrder*j ) + 1 + despB )%lNumVertToSew ));
              glb_tubeFace_VHandles.push_back ( vecVertexHoleHandle.at ( index ));

              index = abs ( int ((( lOrder*j ) + despB )%lNumVertToSew ));
              glb_tubeFace_VHandles.push_back ( vecVertexHoleHandle.at ( index ));
              Mesh->add_face ( glb_tubeFace_VHandles );

              glb_tubeFace_VHandles.clear ( );
              index = (((( nodeAux.id - NEURODESP - 1 )*( globalDesp ))) + (( j + despA )%HResolTube ));
              index += glb_nVerticesSoma;
              glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

              index = (((( nodeAux.id - NEURODESP - 1 )*( globalDesp ))) + (( j + 1 + despA )%HResolTube ));
              index += glb_nVerticesSoma;
              glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

              index = abs ( int ((( lOrder*j ) + 1 + despB )%lNumVertToSew ));
              glb_tubeFace_VHandles.push_back ( vecVertexHoleHandle.at ( index ));
              Mesh->add_face ( glb_tubeFace_VHandles );
            }

            //Sew diferent number of vertex
            if ( vecVertexHoleHandle.size ( ) > HResolTube )
            {
              int i = 0;
              for ( i = HResolTube; i < vecVertexHoleHandle.size ( ); ++i )
              {
                glb_tubeFace_VHandles.clear ( );

                index = (((( nodeAux.id - NEURODESP - 1 )*( globalDesp ))) + (( j + despA )%HResolTube ));
                index += glb_nVerticesSoma;
                glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

                index = abs ( int ((( lOrder*i ) + 1 + despB )%lNumVertToSew ));
                glb_tubeFace_VHandles.push_back ( vecVertexHoleHandle.at ( index ));

                index = abs ( int ((( lOrder*i ) + despB )%lNumVertToSew ));
                glb_tubeFace_VHandles.push_back ( vecVertexHoleHandle.at ( index ));

                Mesh->add_face ( glb_tubeFace_VHandles );
              }

              glb_tubeFace_VHandles.clear ( );

              index = (((( nodeAux.id - NEURODESP - 1 )*( globalDesp ))) + (( despA )%HResolTube ));
              index += glb_nVerticesSoma;
              glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

              index = 0;
              glb_tubeFace_VHandles.push_back ( vecVertexHoleHandle.at ( index ));

              index = abs ( int ((( lOrder*i ) + despB )%lNumVertToSew ));
              glb_tubeFace_VHandles.push_back ( vecVertexHoleHandle.at ( index ));

              Mesh->add_face ( glb_tubeFace_VHandles );
            }
            else
            {
              //!!! Menos vertices en el agujero
              //Fijar ultimo vertice del agujero y fan al tubo
              int i = 0;
              for ( i = vecVertexHoleHandle.size ( ); i < HResolTube; ++i )
              {
                glb_tubeFace_VHandles.clear ( );
                index = (((( nodeAux.id - NEURODESP - 1 )*( globalDesp ))) + (( i + despA )%HResolTube ));
                index += glb_nVerticesSoma;
                glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

                index = (((( nodeAux.id - NEURODESP - 1 )*( globalDesp ))) + (( i + 1 + despA )%HResolTube ));
                index += glb_nVerticesSoma;
                glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

                index = abs ( int ((( lOrder*j ) + despB )%lNumVertToSew ));
                glb_tubeFace_VHandles.push_back ( vecVertexHoleHandle.at ( index ));
                Mesh->add_face ( glb_tubeFace_VHandles );
              }  //for
            } //else -> if (vecVertexHoleHandle.size()>HResolTube)
          } //if (distCW>distCCW)
          else
          {
            lOrder = -1;
            //!!!Costura inversa, de momento se obviaran hasta que se detecten de manera correcta
            for ( j = 0; j < ( limit ); j++ )
              //for(j=0; j<3; j++)
            {
              glb_tubeFace_VHandles.clear ( );
              index = (((( nodeAux.id - NEURODESP - 1 )*( globalDesp ))) + (( j + despA )%HResolTube ));
              index += glb_nVerticesSoma;
              glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

              index = abs ( int ((( lOrder*j ) + despB )%lNumVertToSew ));
              glb_tubeFace_VHandles.push_back ( vecVertexHoleHandle.at ( index ));

              index = abs ( int ((( lOrder*j ) + lOrder*1 + despB )%lNumVertToSew ));
              glb_tubeFace_VHandles.push_back ( vecVertexHoleHandle.at ( index ));

              Mesh->add_face ( glb_tubeFace_VHandles );

              glb_tubeFace_VHandles.clear ( );

              index = (((( nodeAux.id - NEURODESP - 1 )*( globalDesp ))) + (( j + despA )%HResolTube ));
              index += glb_nVerticesSoma;
              glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

              index = abs ( int ((( lOrder*j ) + lOrder*1 + despB )%lNumVertToSew ));
              glb_tubeFace_VHandles.push_back ( vecVertexHoleHandle.at ( index ));

              index = (((( nodeAux.id - NEURODESP - 1 )*( globalDesp ))) + abs ( int (( j + 1 + despA ))%HResolTube ));
              index += glb_nVerticesSoma;
              glb_tubeFace_VHandles.push_back ( glb_meshVHandle[index] );

              Mesh->add_face ( glb_tubeFace_VHandles );
            }

          } //else -> if (distCW>distCCW)
        }
      }

      //Clean the aux containers
      vecVertexHoleHandle.clear ( );
      vecVertexHoleHandleIds.clear ( );

      vecFaceHoleHandle.clear ( );
      vecFaceHoleHandleIds.clear ( );

      ////Free the memory of deleted objects
      //Mesh->garbage_collection();

      ////Update the mesh
      //updateBaseMesh();
    }
    //Free the memory of deleted objects
    Mesh->garbage_collection ( );

    //Update the mesh
    updateBaseMesh ( );
  }


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  unsigned int NeuroSWC::getSomaNumVertex ( )
  {
    return ( glb_nVerticesSoma );
  }

  unsigned int NeuroSWC::getTubeNumVertex ( )
  {
    return ( glb_nVerticesTube );
  }

  float NeuroSWC::SignedArea ( OpenMesh::Vec3f pPoint1, OpenMesh::Vec3f pPoint2, OpenMesh::Vec3f pPoint3 )
  {
    //float val = (pPoint1[2]*pPoint2[1]*pPoint3[0]) - (pPoint1[0]*pPoint2[1]*pPoint3[2]);

    //float val =  (
    //				( (pPoint1[2]*pPoint2[1]*pPoint3[0]) + (pPoint2[2]*pPoint3[1]) + (pPoint3[2]*pPoint1[0]) + (pPoint1[1]*pPoint2[0]) )
    //				- ( (pPoint1[0]*pPoint2[1]*pPoint3[2]) + (pPoint1[1]*pPoint2[2]) + (pPoint1[2]*pPoint3[0]) + (pPoint2[0]*pPoint3[1]) )
    //			);


    //float val =  (
    //				( (pPoint3[0]*pPoint2[1]*pPoint1[2]) +  (pPoint3[1]*pPoint2[2]*pPoint1[0]) + (pPoint3[2]*pPoint2[0]*pPoint1[1]) )
    //				- ( (pPoint1[0]*pPoint2[1]*pPoint3[2]) +  (pPoint1[1]*pPoint2[2]*pPoint3[0]) + (pPoint1[2]*pPoint2[0]*pPoint3[1]) )
    //			);

    float lVal = Determinant3x3 ( pPoint1, pPoint2 );
    lVal += Determinant3x3 ( pPoint2, pPoint3 );
    lVal += Determinant3x3 ( pPoint3, pPoint1 );

    return lVal;
  }

  float NeuroSWC::Determinant3x3 ( OpenMesh::Vec3f pPoint1, OpenMesh::Vec3f pPoint2 )
  {

    float val = ((( pPoint1[2]*pPoint2[1] ) + ( pPoint2[2]*pPoint1[0] ) + ( pPoint1[1]*pPoint2[0] ))
      - (( pPoint1[0]*pPoint2[1] ) + ( pPoint2[0]*pPoint1[2] ) + ( pPoint1[1]*pPoint2[2] ))
    );

    return val;

  }

  void NeuroSWC::setAllParams ( int horResolTube, int verResolTube, int horResolSoma, int verResolSoma )
  {
    HResolTube = horResolTube;
    VResolTube = verResolTube;
    HResolSoma = horResolSoma;
    VResolSoma = verResolSoma;

    glb_nVerticesSoma = ( VResolSoma - 2 )*HResolSoma + 2;
    glb_nVerticesTube = HResolTube*VResolTube;

    //Recorrer los vertices con los iteradores e ir almacenando la cantidad
    meshVHandleTotal = 0;

    //mSomaNumFaces = HResolSoma*(VResolSoma-2)*2;
    mSomaNumFaces = HResolSoma*( VResolSoma )*2;

    for ( MeshDef::VertexIter v_it = Mesh->vertices_begin ( );
          v_it != Mesh->vertices_end ( );
          ++v_it )
    {
      ++meshVHandleTotal;
    }

    //meshVHandleTotal*=2;
    //meshVHandleTotal--;
  }

  void NeuroSWC::JoinSpines ( const BaseMesh *pBaseMesh )
  {
    unsigned int lOldNumVertex = getNumVertex ( );
    unsigned int lNumFacesToDel = mFusionCandidateFace_VHandles.size ( );
    unsigned int index = 0;

    MeshDef::VertexHandle vIdx[3];//, vIdx2, vIdx3;
    MeshDef::HalfedgeHandle hehandle;
    MeshDef::FaceHandle lAuxFaceHandle;

    JoinBaseMesh ( pBaseMesh );

    for ( int i = 0; i < lNumFacesToDel; i++ )
    {
      lAuxFaceHandle = mFusionCandidateFace_VHandles.at ( i );

      //Sacar los tres v�rtices de la faceta ()
      hehandle = Mesh->halfedge_handle ( lAuxFaceHandle );

      //Ojo con el orden
      vIdx[0] = Mesh->to_vertex_handle ( hehandle );
      hehandle = Mesh->next_halfedge_handle ( hehandle );
      vIdx[1] = Mesh->to_vertex_handle ( hehandle );
      hehandle = Mesh->next_halfedge_handle ( hehandle );
      vIdx[2] = Mesh->to_vertex_handle ( hehandle );

      //Borrar la faceta para poder coser
      if ( lAuxFaceHandle.is_valid ( ))
        Mesh->delete_face ( lAuxFaceHandle, false );

      //Coser los v�rtices de la base con los de la faceta

/*
			glb_tubeFace_VHandles.clear();
				//glb_tubeFace_VHandles.push_back(glb_meshVHandle[vIdx[( (j+despA)%HOLENUMVERTEX) ].idx()]);

				//index = ( (nodeAux.id-NEURODESP-1)*(globalDesp) + j + despB);
				//index+=glb_nVerticesSoma;
				//glb_tubeFace_VHandles.push_back(glb_meshVHandle[index]);

				//index = ( (nodeAux.id-NEURODESP-1)*(globalDesp) + j+1	+ despB);
				//index+=glb_nVerticesSoma;
				//glb_tubeFace_VHandles.push_back(glb_meshVHandle[index]);

				glb_tubeFace_VHandles.push_back(vIdx[1]);

				glb_tubeFace_VHandles.push_back(vIdx[0]);

				index = i*(19 + 2);
				index+=lOldNumVertex;
				glb_tubeFace_VHandles.push_back(Mesh->vertex_handle(index));

				//index = i*(19);
				//index+=lOldNumVertex;
				//glb_tubeFace_VHandles.push_back(Mesh->vertex_handle(index));

			Mesh->add_face(glb_tubeFace_VHandles);
*/

    }

    mFusionCandidateFace_VHandles.clear ( );

    Mesh->garbage_collection ( );

    updateBaseMesh ( );
  }

  void NeuroSWC::addFaceFusionCandidate ( MeshDef::FaceHandle pFaceCandidate )
  {
    mFusionCandidateFace_VHandles.push_back ( pFaceCandidate );
  }

  void NeuroSWC::freeFaceFusionCandidates ( )
  {
    mFusionCandidateFace_VHandles.clear ( );
  }

  void NeuroSWC::applyNoiseToSoma ( float range, bool pCteNoise )
  {
    applyNoiseToIntervall ( range, pCteNoise, 0, glb_nVerticesSoma );
  }

  void NeuroSWC::applyNoiseToDendritics ( float range, bool pCteNoise )
  {
    applyNoiseToIntervall ( range, pCteNoise, glb_nVerticesSoma, getNumVertex ( ));
  }

  unsigned int NeuroSWC::getSomaNumFaces ( )
  {
    return mSomaNumFaces;
  }

  SWCImporter *NeuroSWC::getImporter ( )
  {
    return importer;
  }

  void NeuroSWC::setImporter ( SWCImporter *pImporter )
  {
    importer = pImporter;
  }

  //BaseMeshContainer * NeuroSWC::getSomaContainer()
  //{
  //	return mSomaContainer;
  //}

  //void NeuroSWC::setSomaContainer(BaseMeshContainer * pContainer)
  //{
  //	mSomaContainer = pContainer;
  //}

  //BaseMeshContainer * NeuroSWC::getSomaContainer()
  //{
  //	return mSomaContainer;
  //}

  void NeuroSWC::setSomaContainer ( BaseMeshContainer *pContainer )
  {
    mSomaContainer = pContainer;
  }

  void NeuroSWC::setPiramidalSoma ( bool pPiramSoma )
  {
    mPiramidalSoma = pPiramSoma;
  }

  void NeuroSWC::calcPreciseBifurcation ( )
  {

    //recorrer todos los nodos q no sean ni bifurcaci�n ni finalizaci�n
    //indexar primer slice, montar la matriz utilizando como vector forward el del segmento, como up el porduvectorial del forward con
    //con el segmento anterior, y como right, el producto vectorial del forward con el Up.


  }

}
