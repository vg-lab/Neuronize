//
// Created by ivelascog on 19/9/19.
//

#include "CompareMeshesWidget.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMessageBox>
#include <QtConcurrent/QtConcurrent>
#include <QFuture>
#include <QProgressDialog>

CompareMeshesWidget::CompareMeshesWidget(const std::string &tmpPath_, QWidget *parent) : QWidget(parent) {
    this->tmpPath = tmpPath_;
    initUi();
    initConnections();
}

void CompareMeshesWidget::initUi() {
    this->selectMesh1Button = new QPushButton("Select File...",this);
    this->selectMesh2Button = new QPushButton("Select File...",this);

    this->mesh1Path = new QLineEdit(this);
    mesh1Path->setPlaceholderText("Mesh 1");
    this->mesh2Path = new QLineEdit(this);
    mesh2Path->setPlaceholderText("Mesh 2");

    this->addMeshVisualize1 = new QPushButton("Add mesh to view",this);
    this->addMeshVisualize2 = new QPushButton("Add mesh to view",this);
    addMeshVisualize1->setSizePolicy(QSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed));
    addMeshVisualize2->setSizePolicy(QSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed));


    this->removeMeshVisualize1 = new QPushButton("Remove mesh from view",this);
    this->removeMeshVisualize2 = new QPushButton("Remove mesh from view",this);
    removeMeshVisualize1->setSizePolicy(QSizePolicy(QSizePolicy::Minimum,QSizePolicy::Fixed));
    removeMeshVisualize2->setSizePolicy(QSizePolicy(QSizePolicy::Minimum,QSizePolicy::Fixed));

    this->listMeshes1 = new QListWidget(this);
    this->listMeshes2 = new QListWidget(this);
    listMeshes1->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    listMeshes2->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    listMeshes1->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    listMeshes2->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);


    this->comparaMeshesCheckbox = new QCheckBox(this);

    auto mainLayout = new QVBoxLayout(this);

    generateTransfer();

    render1 = new QHBoxLayout(this);
    render2 = new QHBoxLayout(this);

    viewer1 = new CompareMeshWidgetViewer(1,this);
    viewer1->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
    viewer1->setMaximumSize(1000,1000);
    render1->setSpacing(6);
    render1->addWidget(transfer1);
    render1->addWidget(viewer1);

    viewer2 = new CompareMeshWidgetViewer(2,this);
    viewer2->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
    viewer2->setMaximumSize(1000,1000);
    render2->setSpacing(6);
    render2->addWidget(viewer2);
    render2->addWidget(transfer2);

    viewer1->setupViewer();
    viewer2->setupViewer();

    auto buttonLayout1 = new QVBoxLayout(this);
    buttonLayout1->addWidget(addMeshVisualize1);
    buttonLayout1->addWidget(removeMeshVisualize1);
   // buttonLayout1->addStretch();


    auto buttonLayout2 = new QVBoxLayout(this);
    buttonLayout2->addWidget(addMeshVisualize2);
    buttonLayout2->addWidget(removeMeshVisualize2);
    //buttonLayout2->addStretch();

    auto visualizeMeshLayout1 = new QHBoxLayout(this);
    visualizeMeshLayout1->setSpacing(6);
    visualizeMeshLayout1->addItem(buttonLayout1);
    visualizeMeshLayout1->addWidget(listMeshes1);
    auto visualizeMeshLayout2 = new QHBoxLayout(this);
    visualizeMeshLayout2->addItem(buttonLayout2);
    visualizeMeshLayout2->addWidget(listMeshes2);

    auto compareMeshLayout1 = new QHBoxLayout(this);
    compareMeshLayout1->setSpacing(6);
    compareMeshLayout1->addWidget(selectMesh1Button);
    compareMeshLayout1->addWidget(mesh1Path);
    auto compareMeshLayout2 = new QHBoxLayout(this);
    compareMeshLayout2->setSpacing(6);
    compareMeshLayout2->addWidget(selectMesh2Button);
    compareMeshLayout2->addWidget(mesh2Path);


    auto stats1layout = new QHBoxLayout(this);
    auto stats2layout = new QHBoxLayout(this);

    maxLabel1 = new QLabel("Max Dist: 0", this);
    maxLabel2 = new QLabel("Max Dist: 0", this);
    meanLabe1 = new QLabel("Mean Dist: 0", this);
    meanLabel2 = new QLabel("Mean Dist: 0", this);
    minLabel1 = new QLabel("Min Dist: 0", this);
    minLabel2 = new QLabel("Min Dist: 0", this);

    stats1layout->addWidget(maxLabel1);
    stats1layout->addWidget(minLabel1);
    stats1layout->addWidget(meanLabe1);
    stats2layout->addWidget(maxLabel2);
    stats2layout->addWidget(minLabel2);
    stats2layout->addWidget(meanLabel2);

    auto line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    auto checkboxLayout = new QHBoxLayout(this);
    checkboxLayout->setSpacing(6);
    checkboxLayout->addWidget(comparaMeshesCheckbox);
    checkboxLayout->addWidget(new QLabel("Meshes to be compare",this));
    checkboxLayout->addStretch();

    auto gridLayout = new QGridLayout(this);
    gridLayout->setSpacing(8);
    gridLayout->addWidget(new QLabel("Meshes to be viewed:",this),0,0,1,2);
    gridLayout->addItem(visualizeMeshLayout1,1,0);
    gridLayout->addItem(visualizeMeshLayout2,1,1);
    gridLayout->addWidget(line,2,0,1,2);
    gridLayout->addItem(checkboxLayout,3,0,1,2);
    gridLayout->addItem(compareMeshLayout1,4,0);
    gridLayout->addItem(compareMeshLayout2,4,1);
    gridLayout->addItem(stats1layout, 5, 0);
    gridLayout->addItem(stats2layout, 5, 1);
    gridLayout->addItem(render1, 6, 0);
    gridLayout->addItem(render2, 6, 1);

    onColorsChange(Qt::Unchecked);
    mainLayout->addItem(gridLayout);

    setLayout(mainLayout);
}

void CompareMeshesWidget::initConnections() {
    connect(selectMesh1Button, &QPushButton::released, this,
            [=]() { loadFileDialog(mesh1Path, "Select Mesh File", "Mesh(*.obj *.off *.ply *.stl)"); });
    connect(selectMesh2Button, &QPushButton::released, this,
            [=]() { loadFileDialog(mesh2Path, "Select Mesh File", "Mesh(*.obj *.off *.ply *.stl)"); });
    connect(addMeshVisualize1,&QPushButton::released,this,[=](){ addMesh(listMeshes1,viewer1);});
    connect(addMeshVisualize2,&QPushButton::released,this,[=](){ addMesh(listMeshes2,viewer2);});
    connect(removeMeshVisualize1,&QPushButton::released,this,[=](){removeMesh(listMeshes1,viewer1);});
    connect(removeMeshVisualize2,&QPushButton::released,this,[=](){removeMesh(listMeshes2,viewer2);});
    connect(comparaMeshesCheckbox,&QCheckBox::stateChanged,this,&CompareMeshesWidget::onColorsChange);

    connect(viewer1,&CompareMeshWidgetViewer::viewChanged,viewer2,&CompareMeshWidgetViewer::onViewChanged);
    connect(viewer2,&CompareMeshWidgetViewer::viewChanged,viewer1,&CompareMeshWidgetViewer::onViewChanged);
}

void CompareMeshesWidget::loadFileDialog(QLineEdit *target, const QString &title, const QString &types) {
    auto file = QFileDialog::getOpenFileName(this,title,QString(),types);
    target->setText(file);
    if (!mesh1Path->text().isEmpty() && !mesh2Path->text().isEmpty()) {
       initRender();
    }

}

void CompareMeshesWidget::initRender() {
    auto mesh1text = mesh1Path->text();
    auto mes2text = mesh2Path->text();
    MeshVCG mesh1 (mesh1text.toStdString());
    MeshVCG mesh2 (mes2text.toStdString());

    QFuture<HausdorffRet> future = QtConcurrent::run([&](){return mesh1.hausdorffDistance(mesh2, tmpPath);});
    QFutureWatcher<HausdorffRet> watcher;
    watcher.setFuture(future);

    QProgressDialog progress(this);
    connect(&watcher, SIGNAL(finished()), &progress, SLOT(close()));
    progress.setLabelText("calculating the distances between the meshes");
    progress.setCancelButton(0);
    progress.setMaximum(0);
    progress.setMinimum(0);
    progress.exec();

    auto hausdorffResult = future.result();
    if (hausdorffResult.max1 > 1.0f) {
        auto messageBox = new QMessageBox(this);
        messageBox->setText("A too-large distance has been found");
        messageBox->setInformativeText(
                "This may be because the meshes are not in the same place, do you want to center them?");
        messageBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        messageBox->setDefaultButton(QMessageBox::Yes);
        auto buttton = messageBox->exec();

        if (buttton == QMessageBox::Yes) {
            mesh1.center();
            mesh2.center();
            hausdorffResult = mesh1.hausdorffDistance(mesh2, tmpPath);
        }
    }


    QFileInfo fi (mesh1text);
    auto mesh1Filename = fi.fileName().toStdString();
    QFileInfo fi2(mes2text);
    auto mesh2Filemane = fi2.fileName().toStdString();

    viewer1->setMesh(tmpPath + "/" + mesh1Filename );
    viewer2->setMesh(tmpPath + "/" + mesh2Filemane );
    auto center = viewer1->getSceneCenter();
    viewer1->setDisplacement(center);
    viewer2->setDisplacement(center);

    updateLabels(hausdorffResult);
}

void CompareMeshesWidget::generateTransfer() {
    transfer1 = new QWidget(this);
    transfer2 = new QWidget(this);

    auto transferGrid1 = new QHBoxLayout(transfer1);
    auto transferGrid2 = new QHBoxLayout(transfer2);

    auto transferLayout1 = new QVBoxLayout(transfer1);
    auto transferLayout2 = new QVBoxLayout(transfer2);

    float inc = 1.0f/ NUMBER_OF_COLORS;
    float value = 0.0f;
    for (size_t i=0 ; i< NUMBER_OF_COLORS; i++) {
        auto widget1 = new QWidget(transfer1);
        auto widget2 = new QWidget(transfer2);
        auto color = MeshVCG::getColor(value);
        widget1->setStyleSheet("background-color:" + color.name() + ";" );
        widget2->setStyleSheet("background-color:" + color.name() + ";");
        value += inc;
        widget1->setMinimumSize(10,0);
        widget1->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Minimum));
        widget2->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Minimum));
        widget2->setMinimumSize(10, 0);
        transferLayout1->addWidget(widget1);
        transferLayout2->addWidget(widget2);
    }

    auto textLayout1 = new QVBoxLayout(transfer1);
    auto textLayout2 = new QVBoxLayout(transfer2);

    for (int i =0 ; i< NUMBER_OF_TEXT - 1; i++) {
        auto label1 = new QLabel(transfer1);
        label1->setSizePolicy(QSizePolicy(QSizePolicy::Maximum,QSizePolicy::Minimum));
        label1->setText("0");
        auto label2 = new QLabel(transfer2);
        label2->setSizePolicy(QSizePolicy(QSizePolicy::Maximum,QSizePolicy::Minimum));
        label2->setText("0");

        labels1.push_back(label1);
        labels2.push_back(label2);

        textLayout1->addWidget(label1);
        textLayout1->addStretch();
        textLayout2->addWidget(label2);
        textLayout2->addStretch();
    }

    auto label1 = new QLabel(transfer1);
    label1->setText("0");
    auto label2 = new QLabel(transfer2);
    label2->setText("0");

    labels1.push_back(label1);
    labels2.push_back(label2);

    textLayout1->addWidget(label1);
    textLayout2->addWidget(label2);

    transferGrid1->addItem(transferLayout1);
    transferGrid1->addItem(textLayout1);
    transferGrid2->addItem(textLayout2);
    transferGrid2->addItem(transferLayout2);

    transfer1->setLayout(transferGrid1);
    transfer2->setLayout(transferGrid2);

}

void CompareMeshesWidget::updateLabels(HausdorffRet dists) {

    double inc1 = (dists.max1 - dists.min1) / (NUMBER_OF_TEXT - 1);
    double inc2 = (dists.max2 - dists.min2) / (NUMBER_OF_TEXT - 1);
    double value1 = dists.max1;
    double value2 = dists.max2;
    for (int i = 0; i < NUMBER_OF_TEXT; i++) {
        labels1[i]->setText(QString::number(value1));
        labels2[i]->setText(QString::number(value2));
        value1 -= inc1;
        value2 -= inc2;
    }

    maxLabel1->setText(tr("Max Dist: ") + QString::number(dists.max1));
    minLabel1->setText(tr("Min Dist: ") + QString::number(dists.min1));
    meanLabe1->setText(tr("Mean Dist: ") + QString::number(dists.mean1));

    maxLabel2->setText(tr("Max Dist: ") + QString::number(dists.max2));
    minLabel2->setText(tr("Min Dist: ") + QString::number(dists.min2));
    meanLabel2->setText(tr("Mean Dist: ") + QString::number(dists.mean2));
}

void CompareMeshesWidget::addMesh(QListWidget *list,CompareMeshWidgetViewer* viewer) {
    auto file = QFileDialog::getOpenFileName(this,"Open mesh",QString(),"Mesh(*.obj *.off *.ply *.stl)");
    if (!file.isEmpty()) {
        QFileInfo fileInfo (file);
        list->addItem(fileInfo.fileName());
        viewer->addVisualizeMesh(file.toStdString());
        auto center = viewer->getSceneCenter();
        viewer1->setDisplacement(center);
        viewer2->setDisplacement(center);

    }

}

void CompareMeshesWidget::removeMesh(QListWidget *list,CompareMeshWidgetViewer* viewer) {
    auto selectedItems = list->selectedItems();
    for (const auto& item : selectedItems) {
        int index = list->row(item);
        list->takeItem(index);
        viewer->removeVisualizeMesh(index);
    }
}

void CompareMeshesWidget::onColorsChange(int state) {
    bool visible = state == Qt::Checked;
    mesh1Path->setVisible(visible);
    mesh2Path->setVisible(visible);
    selectMesh1Button->setVisible(visible);
    selectMesh2Button->setVisible(visible);
    maxLabel1->setVisible(visible);
    maxLabel2->setVisible(visible);
    minLabel1->setVisible(visible);
    minLabel2->setVisible(visible);
    meanLabe1->setVisible(visible);
    meanLabel2->setVisible(visible);
    transfer1->setVisible(visible);
    transfer2->setVisible(visible);
    if (visible) {
        QMessageBox::information(this,tr("Neuronize"),tr("Please note that mesh comparison is"
                                                         " intended for the comparison of two individual objects"));
    }
}




