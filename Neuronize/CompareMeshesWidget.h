//
// Created by ivelascog on 19/9/19.
//

#ifndef NEURONIZE_COMPAREMESHESWIDGET_H
#define NEURONIZE_COMPAREMESHESWIDGET_H


#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <libs/libNeuroUtils/MeshVCG.h>
#include <QListWidget>
#include <QCheckBox>
#include "CompareMeshWidgetViewer.h"

class CompareMeshesWidget : public QWidget {
    Q_OBJECT
    static int const NUMBER_OF_COLORS = 100;
    static int const NUMBER_OF_TEXT = 10;
public:
    explicit CompareMeshesWidget(const std::string& tmpPath_,QWidget* parent = 0);

private:
    QPushButton* selectMesh1Button;
    QPushButton* selectMesh2Button;
    QPushButton* addMeshVisualize1;
    QPushButton* removeMeshVisualize1;
    QPushButton* addMeshVisualize2;
    QPushButton* removeMeshVisualize2;
    QListWidget* listMeshes1;
    QListWidget* listMeshes2;
    QLineEdit* mesh1Path;
    QLineEdit* mesh2Path;
    QCheckBox* comparaMeshesCheckbox;
    QHBoxLayout* render1;
    QHBoxLayout* render2;
    QWidget* transfer1;
    QWidget* transfer2;
    CompareMeshWidgetViewer* viewer1;
    CompareMeshWidgetViewer* viewer2;
    std::string tmpPath;
    QLabel *maxLabel1;
    QLabel *meanLabe1;
    QLabel *minLabel1;
    QLabel *maxLabel2;
    QLabel *meanLabel2;
    QLabel *minLabel2;
    std::vector<QLabel*> labels1;
    std::vector<QLabel*> labels2;

    void initUi();
    void generateTransfer();

    void updateLabels(HausdorffRet dists);
    void initConnections();

private slots:
    void loadFileDialog(QLineEdit* target,const QString& title,const QString& types);
    void initRender();
    void addMesh(QListWidget *list,CompareMeshWidgetViewer* viewer);
    void removeMesh(QListWidget *list,CompareMeshWidgetViewer* viewer);
    void onColorsChange(int state);
};


#endif //NEURONIZE_COMPAREMESHESWIDGET_H
