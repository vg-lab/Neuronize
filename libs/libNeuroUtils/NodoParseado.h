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

#include <string>
#include <vector>
#ifndef NODO_PARSEADO_H
#define NODO_PARSEADO_H
using namespace std;
class NodoParseado
{
  public:
    std::vector < NodoParseado * > ramaIzquierda;
    std::vector < NodoParseado * > ramaDerecha;
    std::string tipoIzquierda;
    std::string tipoDerecha;
    std::vector <string> elementosLista;
    bool entraEnDerecha;    //Por defecto a false,si est� en true empieza a rellenar la rama derecha
    std::string comentario;
};
#endif