//
// Created by ivelascog on 19/9/19.
//

#ifndef NEURONIZE_COMPAREMESHWIDGETVIEWER_H
#define NEURONIZE_COMPAREMESHWIDGETVIEWER_H


#include <GL/glew.h>
#include <QGLViewer/qglviewer.h>
#include <QMouseEvent>
#include <libs/libGLNeuroUtils/LightManager.h>
#include <libs/libGLNeuroUtils/MeshRenderer.h>
#include <Eigen/Dense>

class CompareMeshWidgetViewer: public QGLViewer {
    Q_OBJECT

    NSLightManager::LightManager* lights;
    NSMeshRenderer::MeshRenderer* meshRend;
    std::vector<NSMeshRenderer::MeshRenderer*> visualizeMeshRends;
    unsigned int renderMask;
    BaseMesh* mesh;
    int number;
    double maxDist;
    Eigen::Vector3f displacement;

public:
    explicit CompareMeshWidgetViewer ( int number,QWidget* parent = 0);
    void setupViewer();
    void setMesh(const std::string& filename );
    void addVisualizeMesh(const std::string& filename);
    void removeVisualizeMesh(int i);
    void setDisplacement(Eigen::Vector3f displacement);
    Eigen::Vector3f getSceneCenter();
    bool isRendering();
    void removeMesh();
    pair<Eigen::Vector3f, Eigen::Vector3f> getSceneBoundingBox();
    void fitBox(std::pair<Eigen::Vector3f, Eigen::Vector3f> box);


signals:
    void viewChanged( QMouseEvent *mouseMoveEvent, QWheelEvent* whellEvent, QMouseEvent* mousePressEvent, QMouseEvent* mouseReleaseEvent);

public slots:
    void onViewChanged( QMouseEvent *mouseMoveEvent, QWheelEvent* whellEvent, QMouseEvent* mousePressEvent, QMouseEvent* mouseReleaseEvent);
private:

    void initLight();
    std::pair<Eigen::Vector3f,Eigen::Vector3f> getBoundingBox(BaseMesh* mesh);

protected:

    virtual void draw ();

    void mouseMoveEvent(QMouseEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;

    void wheelEvent(QWheelEvent *event) override;
    };


#endif //NEURONIZE_COMPAREMESHWIDGETVIEWER_H
