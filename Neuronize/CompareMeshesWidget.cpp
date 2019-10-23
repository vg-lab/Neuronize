//
// Created by ivelascog on 19/9/19.
//

#include "CompareMeshesWidget.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMessageBox>

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

    auto mainLayout = new QVBoxLayout(this);

    generateTransfer();

    render1 = new QHBoxLayout(this);
    render2 = new QHBoxLayout(this);

    viewer1 = new CompareMeshWidgetViewer(1,this);
    viewer1->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
    render1->setSpacing(6);
    render1->addItem(transferGrid1);
    render1->addWidget(viewer1);

    viewer2 = new CompareMeshWidgetViewer(2,this);
    viewer2->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
    render2->setSpacing(6);
    render2->addWidget(viewer2);
    render2->addItem(transferGrid2);

    viewer1->setupViewer();
    viewer2->setupViewer();


    auto header1layout = new QHBoxLayout(this);
    header1layout->setSpacing(6);
    header1layout->addWidget(selectMesh1Button);
    header1layout->addWidget(mesh1Path);

    auto header2layout = new QHBoxLayout(this);
    header2layout->setSpacing(6);
    header2layout->addWidget(selectMesh2Button);
    header2layout->addWidget(mesh2Path);

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

    auto gridLayout = new QGridLayout(this);
    gridLayout->setSpacing(8);
    gridLayout->addItem(header1layout,0,0);
    gridLayout->addItem(header2layout,0,1);
    gridLayout->addItem(stats1layout, 1, 0);
    gridLayout->addItem(stats2layout, 1, 1);
    gridLayout->addItem(render1, 2, 0);
    gridLayout->addItem(render2, 2, 1);

    mainLayout->addItem(gridLayout);

    setLayout(mainLayout);
}

void CompareMeshesWidget::initConnections() {
    connect(selectMesh1Button, &QPushButton::released, this,
            [=]() { loadFileDialog(mesh1Path, "Select Mesh File", "Mesh(*.obj *.off *.ply *.stl)"); });
    connect(selectMesh2Button, &QPushButton::released, this,
            [=]() { loadFileDialog(mesh2Path, "Select Mesh File", "Mesh(*.obj *.off *.ply *.stl)"); });

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
    auto hausdorffResult = mesh1.hausdorffDistance(mesh2, tmpPath);


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

    updateLabels(hausdorffResult);
}

void CompareMeshesWidget::generateTransfer() {
    transferGrid1 = new QHBoxLayout(this);
    transferGrid2 = new QHBoxLayout(this);

    auto transferLayout1 = new QVBoxLayout(this);
    auto transferLayout2 = new QVBoxLayout(this);

    float inc = 1.0f/ NUMBER_OF_COLORS;
    float value = 0.0f;
    for (int i=0 ; i< NUMBER_OF_COLORS; i++) {
        auto widget1 = new QWidget(this);
        auto widget2 = new QWidget(this);
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

    auto textLayout1 = new QVBoxLayout(this);
    auto textLayout2 = new QVBoxLayout(this);

    for (int i =0 ; i< NUMBER_OF_TEXT - 1; i++) {
        auto label1 = new QLabel(this);
        label1->setSizePolicy(QSizePolicy(QSizePolicy::Maximum,QSizePolicy::Minimum));
        label1->setText("0");
        auto label2 = new QLabel(this);
        label2->setSizePolicy(QSizePolicy(QSizePolicy::Maximum,QSizePolicy::Minimum));
        label2->setText("0");

        labels1.push_back(label1);
        labels2.push_back(label2);

        textLayout1->addWidget(label1);
        textLayout1->addStretch();
        textLayout2->addWidget(label2);
        textLayout2->addStretch();
    }

    auto label1 = new QLabel(this);
    label1->setText("0");
    auto label2 = new QLabel(this);
    label2->setText("0");

    labels1.push_back(label1);
    labels2.push_back(label2);

    textLayout1->addWidget(label1);
    textLayout2->addWidget(label2);

    transferGrid1->addItem(transferLayout1);
    transferGrid1->addItem(textLayout1);
    transferGrid2->addItem(textLayout2);
    transferGrid2->addItem(transferLayout2);

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
