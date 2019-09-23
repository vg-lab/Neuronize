//
// Created by ivelascog on 19/9/19.
//

#include "CompareMeshWidgetViewer.h"

CompareMeshWidgetViewer::CompareMeshWidgetViewer(int number,QWidget *parent): QGLViewer( parent){
    this->number = number;
    meshRend = nullptr;
    mesh = nullptr;
    renderMask = 0;
    renderMask = NSMeshRenderer::MeshRenderer::RENDER_SURFACE;
}

void CompareMeshWidgetViewer::setupViewer() {

    camera ()->setZClippingCoefficient(500.0);
    initLight();
    updateGL();

}

void CompareMeshWidgetViewer::draw() {
    // Place light at camera position
    const qglviewer::Vec cameraPos = camera ( )->position ( );
    const GLfloat pos[4] = { ( float ) cameraPos[0], ( float ) cameraPos[1], ( float ) cameraPos[2], 1.0 };
    glLightfv ( GL_LIGHT1, GL_POSITION, pos );

    // Orientate light along view direction
    glLightfv ( GL_LIGHT1, GL_SPOT_DIRECTION, camera ( )->viewDirection ( ));

    if ( meshRend != nullptr ) {
        meshRend->Render ( );
    }
}



void CompareMeshWidgetViewer::initLight() {
    glDisable ( GL_LIGHT0 );
    glShadeModel ( GL_SMOOTH );              // Enable Smooth Shading
    lights = new NSLightManager::LightManager ( );
    lights->ActiveLighting ( );

    lights->SetLightPosition ( 0.0f, 0.0f, 0.0f );
    lights->ActiveLight ( 1, true );

    lights->SetLightType ( 0 );
}

void CompareMeshWidgetViewer::setMesh(const std::string& filename) {
    if (mesh != nullptr) {
        delete mesh;
    }

    if (meshRend != nullptr) {
        delete meshRend;
    }

    mesh = new BaseMesh(filename);
    meshRend = new NSMeshRenderer::MeshRenderer();

    meshRend->setMeshToRender(mesh);
    meshRend->setRenderOptions(renderMask);
    updateGL();
}

void CompareMeshWidgetViewer::onViewChanged(QMouseEvent *mouseMoveEvent, QWheelEvent* whellEvent, QMouseEvent* mousePressEvent, QMouseEvent* mouseReleaseEvent) {
    if (mouseMoveEvent != nullptr) {
        QGLViewer::mouseMoveEvent(mouseMoveEvent);
    }

    if (whellEvent != nullptr) {
        QGLViewer::wheelEvent(whellEvent);
    }

    if (mousePressEvent != nullptr) {
        QGLViewer::mousePressEvent(mousePressEvent);
    }

    if (mouseReleaseEvent != nullptr) {
        QGLViewer::mouseReleaseEvent(mouseReleaseEvent);
    }

    updateGL();
}

void CompareMeshWidgetViewer::mouseMoveEvent(QMouseEvent *event) {
    QGLViewer::mouseMoveEvent(event);
    emit viewChanged(event, nullptr, nullptr, nullptr);
}

void CompareMeshWidgetViewer::wheelEvent(QWheelEvent *event) {
    QGLViewer::wheelEvent(event);
    emit viewChanged(nullptr,event, nullptr, nullptr);
}

void CompareMeshWidgetViewer::mousePressEvent(QMouseEvent *event) {
    QGLViewer::mousePressEvent(event);
    emit viewChanged(nullptr, nullptr, event, nullptr);
}

void CompareMeshWidgetViewer::mouseReleaseEvent(QMouseEvent *event) {
    QGLViewer::mouseReleaseEvent(event);
    emit viewChanged(nullptr, nullptr, nullptr,event);
}

double CompareMeshWidgetViewer::getMaxDist() const {
    return maxDist;
}

void CompareMeshWidgetViewer::setMaxDist(double maxDist) {
    CompareMeshWidgetViewer::maxDist = maxDist;
}

