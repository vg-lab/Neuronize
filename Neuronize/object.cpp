/****************************************************************************

 Copyright (C) 2002-2011 Gilles Debunne. All rights reserved.

 This file is part of the QGLViewer library version 2.3.16.

 http://www.libqglviewer.com - contact@libqglviewer.com

 This file may be used under the terms of the GNU General Public License 
 versions 2.0 or 3.0 as published by the Free Software Foundation and
 appearing in the LICENSE file included in the packaging of this file.
 In addition, as a special exception, Gilles Debunne gives you certain 
 additional rights, described in the file GPL_EXCEPTION in this package.

 libQGLViewer uses dual licensing. Commercial/proprietary software must
 purchase a libQGLViewer Commercial License.

 This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

*****************************************************************************/

#include <qgl.h>
#include "object.h"

using namespace qglviewer;

void Object::draw ( ) const
{
  static GLUquadric *quad = gluNewQuadric ( );

  glPushMatrix ( );
  glMultMatrixd ( frame.matrix ( ));
  //gluSphere(quad, 0.03, 10, 6);
  //gluCylinder(quad, 0.03, 0.0, 0.09, 10, 1);
  glColor3f ( 1, 1, 0 );
  gluSphere ( quad, 1.03, 10, 6 );
  gluCylinder ( quad, 0.03, 0.0, 0.09, 10, 1 );
  //glutSolidSphere(0.1,4,4);
  glPopMatrix ( );
}
