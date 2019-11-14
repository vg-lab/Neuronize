//
// Created by ivelascog on 19/9/19.
//

#include "CompareMeshWidgetViewer.h"

CompareMeshWidgetViewer::CompareMeshWidgetViewer(int number,QWidget *parent): QGLViewer( parent){
    this->number = number;
    meshRend = nullptr;
    mesh = nullptr;
    extraMesh = nullptr;
    extraMeshRend = nullptr;
    this->displacement = {0,0,0};
    renderMask = 0;
    renderMask = NSMeshRenderer::MeshRenderer::RENDER_SURFACE;
}

void CompareMeshWidgetViewer::setupViewer() {
    camera()->setZClippingCoefficient(50.0);
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

    if (extraMeshRend != nullptr) {
        extraMeshRend->Render();
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

void CompareMeshWidgetViewer::setExtraMesh(const std::string& filename) {
    if (extraMesh != nullptr) {
        delete extraMesh;
    }

    if (extraMeshRend != nullptr) {
        delete extraMeshRend;
    }

    extraMesh = new BaseMesh(filename);
    extraMeshRend = new NSMeshRenderer::MeshRenderer;

    extraMesh->setVertexColor ( extraMesh->getMesh ( )->vertices_begin ( ),
                          extraMesh->getMesh ( )->vertices_end ( ),
                          MeshDef::Color ( 0.5, 0.5, 1.0, 1.0 ));

    boost::numeric::ublas::matrix<float> translationMatrix (4,4);
    generateSquareTraslationMatrix(translationMatrix,-displacement[0],-displacement[1],-displacement[2]);
    extraMesh->applyMatrixTransform(translationMatrix,4);

    extraMeshRend->setMeshToRender(extraMesh);
    extraMeshRend->setRenderOptions(renderMask);
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

void CompareMeshWidgetViewer::setDisplacement(Eigen::Vector3d displacement_){
    if (extraMesh != nullptr) {
        boost::numeric::ublas::matrix<float> inverseTranslationMatrix (4,4);
        generateSquareTraslationMatrix(inverseTranslationMatrix,displacement[0],displacement[1],displacement[2]);

        boost::numeric::ublas::matrix<float> translationMatrix (4,4);
        generateSquareTraslationMatrix(translationMatrix,-displacement_[0],-displacement_[1],-displacement_[2]);

        extraMesh->applyMatrixTransform(inverseTranslationMatrix,4);
        extraMesh->applyMatrixTransform(translationMatrix,4);
    }

    this->displacement = displacement_;
    updateGL();
}



