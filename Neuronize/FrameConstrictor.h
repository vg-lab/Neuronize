/*
 * based on:
 *	manipulatedFrameSetConstraint.h
 *
 *  Created on: 07-feb-2009
 *
 *  Customized select() function that enables the selection of several objects.
 *  Object selection is preformed using the left mouse button. Press Shift to add objects to
 *  the selection, and Alt to remove objects from the selection.
 *  Individual objects (click on them) as well as rectangular regions (click and drag mouse)
 *  can be selected. To do this, the selection region size is modified and the endSelection()
 *  function has been overloaded so that all the objects of the region are taken into account
 *  (the default implementation only selects the closest object).
 *  The selected objects can then be manipulated by pressing the Control key.
 *  Other set operations (parameter edition, deletion...) can also easily be applied
 *  to the selected objects.
 */

#ifndef FRAME_CONSTRICTOR_H_
#define FRAME_CONSTRICTOR_H_

#include <QGLViewer/constraint.h>
#include <MSNode.h>

class FrameConstrictor: public qglviewer::Constraint
{
  public:
    void clearSet ( );
    void addObjectToSet ( MSNode *o );

    virtual void constrainTranslation ( qglviewer::Vec &translation, qglviewer::Frame *const frame );
    virtual void constrainRotation ( qglviewer::Quaternion &rotation, qglviewer::Frame *const frame );

  private :
    QList < MSNode * > mObjects;
    //std::vector<MSNode*> mObjects;
};

#endif /* FRAME_CONSTRICTOR_H_ */
