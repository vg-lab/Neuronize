
#include "FrameConstrictor.h"
#include <QGLViewer/frame.h>

using namespace qglviewer;

void FrameConstrictor::clearSet ( )
{
  mObjects.clear ( );
}

void FrameConstrictor::addObjectToSet ( MSNode *o )
{
  mObjects.append ( o );
}

void FrameConstrictor::constrainTranslation ( qglviewer::Vec &translation, Frame *const )
{
  qglviewer::Frame frame;
  OpenMesh::Vec3f lPos;
  Vec lQtVec ( 0, 0, 0 );

  for ( QList < MSNode * >::iterator it = mObjects.begin ( ), end = mObjects.end ( ); it != end; ++it )
  {
    //Esto es para desplazar el nodo
    //!!!(*it)->frame.translate(translation);

    // update the node position at the new frame location
    //!!!(*it)->updatePos();

    lPos = ( *it )->getPos ( );
    lQtVec = Vec ( lPos[0], lPos[1], lPos[2] );

    frame.setPosition ( lQtVec );
    frame.translate ( translation );

    lPos[0] = frame.position ( ).x;
    lPos[1] = frame.position ( ).y;
    lPos[2] = frame.position ( ).z;

    ( *it )->setPos ( lPos );
  }
}

void FrameConstrictor::constrainRotation ( qglviewer::Quaternion &rotation, Frame *const frame )
{
  // A little bit of math. Easy to understand, hard to guess (tm).
  // rotation is expressed in the frame local coordinates system. Convert it back to world coordinates.
//  const Vec worldAxis = frame->inverseTransformOf(rotation.axis());
//  const Vec pos = frame->position();
//  const float angle = rotation.angle();
//
//#if QT_VERSION < 0x040000
//  for (QPtrList<Object>::iterator it=objects_.begin(), end=objects_.end(); it != end; ++it)
//#else
//    for (QList<Object*>::iterator it=objects_.begin(), end=objects_.end(); it != end; ++it)
//#endif
//      {
//	// Rotation has to be expressed in the object local coordinates system.
//	Quaternion qObject((*it)->frame.transformOf(worldAxis), angle);
//	(*it)->frame.rotate(qObject);
//
//	// Comment these lines only rotate the objects
//	Quaternion qWorld(worldAxis, angle);
//	// Rotation around frame world position (pos)
//	(*it)->frame.setPosition(pos + qWorld.rotate((*it)->frame.position() - pos));
//      }
}
