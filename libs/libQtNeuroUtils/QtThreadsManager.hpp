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

// QThread has static sleep functions; but they're protected (duh).
// So we provide wrapper functions:
//
//    void MyLib::sleep (unsigned long secs)
//    void MyLib::msleep (unsigned long msecs)
//    void MyLib::usleep (unsigned long usecs)

#include <QThread>

namespace NSQtThreadsManager
{
  class QtThreadsManager: protected QThread
  {
    public:
      static void sleep ( unsigned long secs ) { QThread::sleep ( secs ); }
      static void msleep ( unsigned long msecs ) { QThread::msleep ( msecs ); }
      static void usleep ( unsigned long usecs ) { QThread::usleep ( usecs ); }
  };
  void sleep ( unsigned long secs ) { QtThreadsManager::sleep ( secs ); }
  void msleep ( unsigned long msecs ) { QtThreadsManager::msleep ( msecs ); }
  void usleep ( unsigned long usecs ) { QtThreadsManager::usleep ( usecs ); }
} // namespace MyLib
