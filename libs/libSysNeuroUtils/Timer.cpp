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

#include "Timer.h"

timer::timer ( )
{
  startedAt = 0;
  pausedAt = 0;
  paused = false;
  started = false;
}
bool timer::IsStarted ( )
{
  return started;
}
bool timer::IsStopped ( )
{
  return !started;
}
bool timer::IsPaused ( )
{
  return paused;
}
bool timer::IsActive ( )
{
  return !paused & started;
}
void timer::Pause ( )
{
  if ( paused || !started )
    return;
  paused = true;
  pausedAt = clock ( );
}
void timer::Resume ( )
{
  if ( !paused )
    return;
  paused = false;
  startedAt += clock ( ) - pausedAt;
}
void timer::Stop ( )
{
  started = false;
}
void timer::Start ( )
{
  if ( started )
    return;
  started = true;
  paused = false;
  startedAt = clock ( );
}
void timer::Reset ( )
{
  paused = false;
  startedAt = clock ( );
}
clock_t timer::GetTicks ( )
{
  if ( !started )
    return 0;
  if ( paused )
    return pausedAt - startedAt;
  return clock ( ) - startedAt;
}

clock_t timer::GetElapsedTimeInSeconds ( )
{
  return (( float ) GetTicks ( ))/CLOCKS_PER_SEC;
}


