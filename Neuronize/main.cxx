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

#include <QApplication>
#include "neuronize.h"


int main ( int argc, char *argv[] )
{
//    skelgenerator::Neuron neuron ("Rata11secc2-capaVb-cel2_Stitchapi.vrml",{"Rata11secc2-capaVb-cel2_Stitch.vrml"},"","",0.05);
//    std::cout << neuron.getReamingSegments() << std::endl;
//    std::ofstream file;
//    file.open ("sal.asc");
//    file << neuron.to_asc();
//    file.close();
//    ASC2SWCV2 parser ("temp.asc", true);
//    parser.toSWC("test.swc");
  QApplication a ( argc, argv );
  Neuronize w;
  w.show ( );
  return a.exec ( );
}
