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
#include "CompareMeshWidgetViewer.h"

class CompareMeshesWidget: public QDialog {
    Q_OBJECT
    static int const NUMBER_OF_COLORS = 100;
    static int const NUMBER_OF_TEXT = 10;
public:
    explicit CompareMeshesWidget(const std::string& tmpPath_,QWidget* parent = 0);

private:
    QPushButton* selectMesh1Button;
    QPushButton* selectMesh2Button;
    QLineEdit* mesh1Path;
    QLineEdit* mesh2Path;
    QHBoxLayout* render1;
    QHBoxLayout* render2;
    QHBoxLayout* transferGrid1;
    QHBoxLayout* transferGrid2;
    CompareMeshWidgetViewer* viewer1;
    CompareMeshWidgetViewer* viewer2;
    std::string tmpPath;
    std::vector<QLabel*> labels1;
    std::vector<QLabel*> labels2;

    void initUi();
    void generateTransfer();
    void updateLabels(float max1,float max2);
    void initConnections();

private slots:
    void loadFileDialog(QLineEdit* target,const QString& title,const QString& types);
    void initRender();
};


#endif //NEURONIZE_COMPAREMESHESWIDGET_H
