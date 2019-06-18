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
#include <QSettings>
#define ENV "env"
#ifdef _WIN32
#define INSTALL "src/install.bat"
#else
#define INSTALL std::string("src/install.sh")
#endif


int main ( int argc, char *argv[] )
{
    QSettings settings("Neuronize","preferences");
    QString path = QFileInfo(settings.fileName()).absoluteDir().absolutePath();
    QString envPath = path + "/" + ENV;
    std::cout << envPath.toStdString() << std::endl;

    if (!QFileInfo(envPath).exists()) {
        std::string command = INSTALL + " " + envPath.toStdString();
        std::cout << command << std::endl;
        std::system(command.c_str());
    }


  QApplication a ( argc, argv );
  Neuronize w;
  w.show ( );
  return a.exec ( );

}
