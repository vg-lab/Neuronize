//
// Created by ivelascog on 19/9/19.
//
#include <limits>
#include "CompareMeshWidgetViewer.h"

CompareMeshWidgetViewer::CompareMeshWidgetViewer(int number,QWidget *parent): QGLViewer( parent){
    this->number = number;
    meshRend = nullptr;
    mesh = nullptr;
    this->displacement = {0,0,0};
    renderMask = 0;
    renderMask = NSMeshRenderer::MeshRenderer::RENDER_SURFACE;
}

void CompareMeshWidgetViewer::setupViewer() {
    camera()->setZClippingCoefficient(50.0f);
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

    for (const auto& renderer:visualizeMeshRends){
        renderer->Render();
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

void CompareMeshWidgetViewer::removeMesh() {
    delete mesh;
    mesh = nullptr;
    delete meshRend;
    meshRend = nullptr;
    updateGL();
}

void CompareMeshWidgetViewer::setMesh(const std::string& filename) {
    if (mesh != nullptr) {
        delete mesh;
    }

    if (meshRend != nullptr) {
        delete meshRend;
    }

    mesh = new BaseMesh(filename);

    boost::numeric::ublas::matrix<float> translationMatrix (4,4);
    generateSquareTraslationMatrix(translationMatrix,-displacement[0],-displacement[1],-displacement[2]);
    mesh->applyMatrixTransform(translationMatrix,4);

    meshRend = new NSMeshRenderer::MeshRenderer();

    meshRend->setMeshToRender(mesh);
    meshRend->setRenderOptions(renderMask);
    updateGL();
}

void CompareMeshWidgetViewer::addVisualizeMesh(const std::string& filename) {

    auto mesh = new BaseMesh(filename);
    mesh->getUnprotectedMesh()->delete_isolated_vertices();
    mesh->getUnprotectedMesh()->garbage_collection();
    mesh->updateBaseMesh();
    auto renderer = new NSMeshRenderer::MeshRenderer;

    mesh->setVertexColor ( mesh->getMesh ( )->vertices_begin ( ),
                           mesh->getMesh ( )->vertices_end ( ),
                          MeshDef::Color ( 0.5, 0.5, 1.0, 1.0 ));

    boost::numeric::ublas::matrix<float> translationMatrix (4,4);
    generateSquareTraslationMatrix(translationMatrix,-displacement[0],-displacement[1],-displacement[2]);
    mesh->applyMatrixTransform(translationMatrix,4);

    renderer->setMeshToRender(mesh);
    renderer->setRenderOptions(renderMask);
    visualizeMeshRends.push_back(renderer);
    auto center = getSceneCenter();
    setDisplacement(center);
    updateGL();
}

void CompareMeshWidgetViewer::removeVisualizeMesh(int i) {
    visualizeMeshRends.erase(visualizeMeshRends.begin() + i);
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

void CompareMeshWidgetViewer::setDisplacement(Eigen::Vector3f displacement_){
    boost::numeric::ublas::matrix<float> inverseTranslationMatrix (4,4);
    generateSquareTraslationMatrix(inverseTranslationMatrix,displacement[0],displacement[1],displacement[2]);

    boost::numeric::ublas::matrix<float> translationMatrix (4,4);
    generateSquareTraslationMatrix(translationMatrix,-displacement_[0],-displacement_[1],-displacement_[2]);

    if (meshRend != nullptr && meshRend->getBaseMesh() != nullptr) {
        meshRend->getBaseMesh()->applyMatrixTransform(inverseTranslationMatrix,4);
        meshRend->getBaseMesh()->applyMatrixTransform(translationMatrix,4);
    }

   for (const auto& renderer:visualizeMeshRends) {
        renderer->getBaseMesh()->applyMatrixTransform(inverseTranslationMatrix,4);
        renderer->getBaseMesh()->applyMatrixTransform(translationMatrix,4);
    }



   this->displacement = displacement_;
   updateGL();
}

bool CompareMeshWidgetViewer::isRendering() {
    return meshRend != nullptr || !visualizeMeshRends.empty();
}

Eigen::Vector3f CompareMeshWidgetViewer::getSceneCenter(){
   auto boundingBox = getSceneBoundingBox();
   auto min = boundingBox.first;
   auto max = boundingBox.second;
   return (max - min) / 2 + min + displacement;
}

std::pair<Eigen::Vector3f, Eigen::Vector3f > CompareMeshWidgetViewer::getSceneBoundingBox() {
    if ( (meshRend == nullptr || meshRend->getBaseMesh() == nullptr) && visualizeMeshRends.empty()) {
        return {{0,0,0},{0,0,0}};
    }

    Eigen::Vector3f min = {std::numeric_limits<float>::max(),std::numeric_limits<float>::max(),std::numeric_limits<float>::max()};
    Eigen::Vector3f max = {-std::numeric_limits<float>::max(),-std::numeric_limits<float>::max(),-std::numeric_limits<float>::max()};
    if (meshRend != nullptr && meshRend->getBaseMesh() != nullptr) {
        auto boundingBox = getBoundingBox(meshRend->getBaseMesh());
        min = boundingBox.first;
        max = boundingBox.second;
    }

    for (const auto& rend : visualizeMeshRends) {
        auto boundingBox = getBoundingBox(rend->getBaseMesh());
        min = min.cwiseMin(boundingBox.first);
        max = max.cwiseMax(boundingBox.second);
    }

    return {min,max};
}

std::pair<Eigen::Vector3f, Eigen::Vector3f> CompareMeshWidgetViewer::getBoundingBox(BaseMesh* baseMesh) {
    Eigen::Vector3f min = {std::numeric_limits<float>::max(),std::numeric_limits<float>::max(),std::numeric_limits<float>::max()};
    Eigen::Vector3f max = {-std::numeric_limits<float>::max(),-std::numeric_limits<float>::max(),-std::numeric_limits<float>::max()};

    for (const auto& vertex:baseMesh->getMesh()->vertices()) {
        auto p = baseMesh->getMesh()->point(vertex);
        min[0] = std::min(min[0], p[0]);
        min[1] = std::min(min[1], p[1]);
        min[2] = std::min(min[2], p[2]);

        max[0] = std::max(max[0], p[0]);
        max[1] = std::max(max[1], p[1]);
        max[2] = std::max(max[2], p[2]);

    }
    return {min,max};
}

void CompareMeshWidgetViewer::fitBox(std::pair<Eigen::Vector3f, Eigen::Vector3f> box) {
    qglviewer::Vec min = {box.first.x(),box.first.y(),box.first.z()};
    qglviewer::Vec max = {box.second.x(),box.second.y(),box.second.z()};
    camera()->setSceneBoundingBox(min,max);
    camera()->fitBoundingBox(min,max);
    updateGL();
}




