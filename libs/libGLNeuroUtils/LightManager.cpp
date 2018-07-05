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

#include "LightManager.h"

#include <iostream>
using namespace std;

/*
 * Proporciona un contrtol sencillo de las luces de Opengl
 * @author Juan Pedro Brito Mendez
 */
namespace NSLightManager
{
  LightManager::LightManager ( )
  {
    //Configuraci�n de los materiales
    Materiales[0].Descripcion = "Esmeralda";
    Materiales[0].Ambiente[0] = 0.0215f;
    Materiales[2].Ambiente[1] = 0.1745f;
    Materiales[2].Ambiente[2] = 0.0215f;
    Materiales[2].Ambiente[3] = 0.55f;
    Materiales[0].Difuso[0] = 0.07568f;
    Materiales[2].Difuso[1] = 0.61424f;
    Materiales[2].Difuso[2] = 0.07568f;
    Materiales[2].Difuso[3] = 0.55f;
    Materiales[0].Especular[0] = 0.633f;
    Materiales[2].Especular[1] = 0.727811f;
    Materiales[2].Especular[2] = 0.633f;
    Materiales[2].Especular[3] = 0.55f;
    Materiales[0].Brillo = 76.8f;

    Materiales[1].Descripcion = "Oro";
    Materiales[1].Ambiente[0] = 0.24725f;
    Materiales[1].Ambiente[1] = 0.2245f;
    Materiales[1].Ambiente[2] = 0.0645f;
    Materiales[1].Ambiente[3] = 1.0;
    Materiales[1].Difuso[0] = 0.34615f;
    Materiales[1].Difuso[1] = 0.3143f;
    Materiales[1].Difuso[2] = 0.0903f;
    Materiales[1].Difuso[3] = 1.0;
    Materiales[1].Especular[0] = 0.797357f;
    Materiales[1].Especular[1] = 0.723991f;
    Materiales[1].Especular[2] = 0.208006f;
    Materiales[1].Especular[3] = 1.0;
    Materiales[1].Brillo = 128.0f;

    Materiales[2].Descripcion = "Plastico negro";
    Materiales[2].Ambiente[0] = 0.0;
    Materiales[0].Ambiente[1] = 0.0;
    Materiales[0].Ambiente[2] = 0.0;
    Materiales[0].Ambiente[3] = 1.0;
    Materiales[2].Difuso[0] = 0.01f;
    Materiales[0].Difuso[1] = 0.01f;
    Materiales[0].Difuso[2] = 0.01f;
    Materiales[0].Difuso[3] = 1.0;
    Materiales[2].Especular[0] = 0.5f;
    Materiales[0].Especular[1] = 0.5f;
    Materiales[0].Especular[2] = 0.5f;
    Materiales[0].Especular[3] = 1.0;
    Materiales[2].Brillo = 32.0f;

    Materiales[3].Descripcion = "Cromo";
    Materiales[3].Ambiente[0] = 0.25f;
    Materiales[3].Ambiente[1] = 0.25f;
    Materiales[3].Ambiente[2] = 0.25f;
    Materiales[3].Ambiente[3] = 1.0;
    Materiales[3].Difuso[0] = 0.4f;
    Materiales[3].Difuso[1] = 0.4f;
    Materiales[3].Difuso[2] = 0.4f;
    Materiales[3].Difuso[3] = 1.0;
    Materiales[3].Especular[0] = 0.774597f;
    Materiales[3].Especular[1] = 0.774597f;
    Materiales[3].Especular[2] = 0.774597f;
    Materiales[3].Especular[3] = 1.0;
    Materiales[3].Brillo = 76.8f;

    Materiales[4].Descripcion = "Total";
    Materiales[4].Ambiente[0] = 1.0f;
    Materiales[4].Ambiente[1] = 1.0f;
    Materiales[4].Ambiente[2] = 1.0f;
    Materiales[4].Ambiente[3] = 1.0;
    Materiales[4].Difuso[0] = 1.0f;
    Materiales[4].Difuso[1] = 1.0f;
    Materiales[4].Difuso[2] = 1.0f;
    Materiales[4].Difuso[3] = 1.0;
    Materiales[4].Especular[0] = 1.0f;
    Materiales[4].Especular[1] = 1.0f;
    Materiales[4].Especular[2] = 1.0f;
    Materiales[4].Especular[3] = 1.0;
    Materiales[4].Brillo = 128.0f;

    Materiales[5].Descripcion = "Nada";
    Materiales[5].Ambiente[0] = 0.0f;
    Materiales[5].Ambiente[1] = 0.0f;
    Materiales[5].Ambiente[2] = 0.0f;
    Materiales[5].Ambiente[3] = 1.0;
    Materiales[5].Difuso[0] = 0.0f;
    Materiales[5].Difuso[1] = 0.0f;
    Materiales[5].Difuso[2] = 0.0f;
    Materiales[5].Difuso[3] = 1.0;
    Materiales[5].Especular[0] = 0.0f;
    Materiales[5].Especular[1] = 0.0f;
    Materiales[5].Especular[2] = 0.0f;
    Materiales[5].Especular[3] = 1.0;
    Materiales[5].Brillo = 1.0f;

    Materiales[6].Descripcion = "Difusion";
    Materiales[6].Ambiente[0] = 0.0f;
    Materiales[6].Ambiente[1] = 0.0f;
    Materiales[6].Ambiente[2] = 0.0f;
    Materiales[6].Ambiente[3] = 1.0;
    Materiales[6].Difuso[0] = 1.0f;
    Materiales[6].Difuso[1] = 1.0f;
    Materiales[6].Difuso[2] = 1.0f;
    Materiales[6].Difuso[3] = 1.0;
    Materiales[6].Especular[0] = 0.0f;
    Materiales[6].Especular[1] = 0.0f;
    Materiales[6].Especular[2] = 0.0f;
    Materiales[6].Especular[3] = 1.0;
    Materiales[6].Brillo = 1.0f;

    Materiales[7].Descripcion = "Ambient";
    Materiales[7].Difuso[0] = 0.0f;
    Materiales[7].Ambiente[1] = 0.0f;
    Materiales[7].Ambiente[2] = 0.0f;
    Materiales[7].Ambiente[3] = 1.0;
    Materiales[7].Ambiente[0] = 1.0f;
    Materiales[7].Difuso[1] = 1.0f;
    Materiales[7].Difuso[2] = 1.0f;
    Materiales[7].Difuso[3] = 1.0;
    Materiales[7].Especular[0] = 0.0f;
    Materiales[7].Especular[1] = 0.0f;
    Materiales[7].Especular[2] = 0.0f;
    Materiales[7].Especular[3] = 1.0;
    Materiales[7].Brillo = 1.0f;

    Materiales[8].Descripcion = "Specular";
    Materiales[8].Ambiente[0] = 0.0f;
    Materiales[8].Ambiente[1] = 0.0f;
    Materiales[8].Ambiente[2] = 0.0f;
    Materiales[8].Ambiente[3] = 1.0;
    Materiales[8].Especular[0] = 1.0f;
    Materiales[8].Difuso[1] = 1.0f;
    Materiales[8].Difuso[2] = 1.0f;
    Materiales[8].Difuso[3] = 1.0;
    Materiales[8].Difuso[0] = 0.0f;
    Materiales[8].Especular[1] = 0.0f;
    Materiales[8].Especular[2] = 0.0f;
    Materiales[8].Especular[3] = 1.0;
    Materiales[8].Brillo = 40.0f;

    //Configuraci�n de la luz
    MaterialLight.Descripcion = "Configuracion Luz";
    MaterialLight.Ambiente[0] = 0.5f;
    MaterialLight.Ambiente[1] = 0.5f;
    MaterialLight.Ambiente[2] = 0.5f;
    MaterialLight.Ambiente[3] = 1.0;
    MaterialLight.Especular[0] = 1.0f;
    MaterialLight.Difuso[1] = 1.0f;
    MaterialLight.Difuso[2] = 1.0f;
    MaterialLight.Difuso[3] = 1.0;
    MaterialLight.Difuso[0] = 1.0f;
    MaterialLight.Especular[1] = 1.0f;
    MaterialLight.Especular[2] = 1.0f;
    MaterialLight.Especular[3] = 1.0;
    MaterialLight.Position[0] = 0.0f;
    MaterialLight.Position[1] = 0.0f;
    MaterialLight.Position[2] = 0.0f;
    MaterialLight.Position[3] = 0.0;

    //ActiveLighting();
  }

  LightManager::~LightManager ( )
  {

  }

  void LightManager::ActiveLighting ( )
  {
    glEnable ( GL_DEPTH_TEST );
    glEnable ( GL_LIGHTING );
    glEnable ( GL_NORMALIZE );
    glEnable ( GL_COLOR_MATERIAL );
    glColorMaterial ( GL_BACK, GL_AMBIENT_AND_DIFFUSE );
  }

  void LightManager::DeActiveLighting ( )
  {
    glDisable ( GL_LIGHTING );
    glDisable ( GL_COLOR_MATERIAL );
  }

  void LightManager::ActiveLight ( int lightId, bool val )
  {

    int selectedLight = 0;
    switch ( lightId )
    {
      case 0: selectedLight = GL_LIGHT0;
        break;
      case 1: selectedLight = GL_LIGHT1;
        break;
      case 2: selectedLight = GL_LIGHT2;
        break;
      case 3: selectedLight = GL_LIGHT3;
        break;
      case 4: selectedLight = GL_LIGHT4;
        break;
      case 5: selectedLight = GL_LIGHT5;
        break;
      case 6: selectedLight = GL_LIGHT6;
        break;
      case 7: selectedLight = GL_LIGHT7;
        break;

    }

    if ( val )
    {

      glLightfv ( selectedLight, GL_AMBIENT, MaterialLight.Ambiente );
      glLightfv ( selectedLight, GL_DIFFUSE, MaterialLight.Difuso );
      glLightfv ( selectedLight, GL_SPECULAR, MaterialLight.Especular );
      glLightfv ( selectedLight, GL_POSITION, MaterialLight.Position );

      glEnable ( selectedLight );

    }
    else
    {
      glDisable ( selectedLight );
    }
  }

  void LightManager::SetLightPosition ( GLfloat posX, GLfloat posY, GLfloat posZ )
  {
    MaterialLight.Position[0] = posX;
    MaterialLight.Position[1] = posY;
    MaterialLight.Position[2] = posZ;
  }

  GLfloat *LightManager::GetLightPosition ( )
  {
    return MaterialLight.Position;
  }

  void LightManager::SetLightType ( int type )
  {
    glMaterialfv ( GL_FRONT, GL_AMBIENT, Materiales[type].Ambiente );
    glMaterialfv ( GL_FRONT, GL_DIFFUSE, Materiales[type].Difuso );
    glMaterialfv ( GL_FRONT, GL_SPECULAR, Materiales[type].Especular );
    glMaterialf ( GL_FRONT, GL_SHININESS, Materiales[type].Brillo );
  }
}
