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

class CompareMeshWidgetViewer: public QGLViewer {
    Q_OBJECT
    NSLightManager::LightManager* lights;
    NSMeshRenderer::MeshRenderer* meshRend;
    unsigned int renderMask;
    BaseMesh* mesh;
    int number;
    double maxDist;
public:
    double getMaxDist() const;

    void setMaxDist(double maxDist);

public:
    explicit CompareMeshWidgetViewer ( int number,QWidget* parent = 0);
    void setupViewer();
    void setMesh(const std::string& filename );

signals:
    void viewChanged( QMouseEvent *mouseMoveEvent, QWheelEvent* whellEvent, QMouseEvent* mousePressEvent, QMouseEvent* mouseReleaseEvent);

public slots:
    void onViewChanged( QMouseEvent *mouseMoveEvent, QWheelEvent* whellEvent, QMouseEvent* mousePressEvent, QMouseEvent* mouseReleaseEvent);

private:
    void initLight();

protected:
    virtual void draw ();

    void mouseMoveEvent(QMouseEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;

    void wheelEvent(QWheelEvent *event) override;

};


#endif //NEURONIZE_COMPAREMESHWIDGETVIEWER_H
