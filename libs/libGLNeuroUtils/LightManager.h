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

#ifndef LIGHTMANAGER_H_
#define LIGHTMANAGER_H_

/**
 * @LightManager
 * @LightManager.h
 * @author Juan Pedro Brito M�ndez
		   Basado en tutorial de iluminacion de Black Byte: http://black-byte.com/general/superfices-e-iluminacion-en-opengl/
 * @version 0.1
 */

#include <glneuroutils/api.h>

#include <string>
#include <iostream>

#include <GL/glut.h>

using namespace std;


struct MaterialConfig
{
  GLfloat Ambiente[4];
  GLfloat Difuso[4];
  GLfloat Especular[4];
  GLfloat Brillo;
  string Descripcion;
};

struct LightConfig
{
  GLfloat Ambiente[4];
  GLfloat Difuso[4];
  GLfloat Especular[4];
  GLfloat Position[4];
  string Descripcion;
};

namespace NSLightManager
{
  class GLNEUROUTILS_API LightManager
  {
      //Parametros de la luz
      LightConfig MaterialLight;

      //Vector con todos los tipos de materiales
      MaterialConfig Materiales[9];

    public:

      LightManager ( );

      ~LightManager ( );

      void ActiveLighting ( );

      void DeActiveLighting ( );

      void ActiveLight ( int lightId, bool val );

      void SetLightPosition ( GLfloat posX, GLfloat posY, GLfloat posZ );

      GLfloat *GetLightPosition ( );

      void SetLightType ( int type );

  };
}

#endif /*LIGHTMANAGER*/
