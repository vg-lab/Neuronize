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
 * @SystemUtils
 * @SystemUtils.h
 * @author Juan Pedro Brito M�ndez
 * @version 0.1
 */

#ifndef SYSTEMUTILS_H_
#define SYSTEMUTILS_H_

#include <stdlib.h>
#include <time.h>

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

/**
* Generic utilities
* @author Juan Pedro Brito M�ndez
* @version 0.1
*/


/*
 * Return string where replace the first string for the second
 */
string changeExps ( string cad, string expSearch, string expRepl );

/**
 * Return actual time
 */
string actTime ( );

/**
 * Write pCad int pFilePath
 */
void strToFile ( string pCad, string pFilePath );

#endif /*SYSTEMUTILS_H_*/
