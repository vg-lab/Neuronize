//
// Created by ivelascog on 17/07/18.
//

#include "LoadFileDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QFile>
#include <SkelGenerator/SkelGeneratorUtil/Neuron.h>
#include <iostream>
#include <fstream>


LoadFileDialog::LoadFileDialog(QWidget *parent): QDialog(parent) {
    traces = new QRadioButton("Trace File: ", this);
    traces->setChecked(true);

    vrmls = new QRadioButton("VRML Files: ", this);

    tracePath = new QLineEdit(this);
    tracePath->setPlaceholderText("Trace file (SWC,ASC)");

    apiPath = new QLineEdit(this);
    apiPath->setPlaceholderText("Apical VRML file");
    apiPath->setEnabled(false);

    basalPath = new QLineEdit(this);
    basalPath->setPlaceholderText("Basal/s VRML file/s");
    basalPath->setEnabled(false);

    okButton = new QPushButton("Ok",this);
    cancelButton = new QPushButton("Cancel",this);

    tracePathButton = new QPushButton("Select File...",this);
    basalPathButton = new QPushButton("Select File...",this);
    basalPathButton->setEnabled(false);
    apiPathButton = new QPushButton("Select File...",this);
    apiPathButton->setEnabled(false);

    auto grid = new QGridLayout();
    grid->setSpacing(6);
    grid->addWidget(traces,0,0);
    grid->addWidget(tracePathButton,0,1);
    grid->addWidget(tracePath,0,2);

    grid->addWidget(vrmls,1,0);
    grid->addWidget(apiPathButton,1,1);
    grid->addWidget(apiPath,1,2);

    grid->addWidget(basalPathButton,2,1);
    grid->addWidget(basalPath,2,2);

    auto buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(10);
    buttonLayout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed));
    buttonLayout->addWidget(okButton,0,Qt::AlignRight);
    buttonLayout->addWidget(cancelButton,0,Qt::AlignRight);


    auto mainLayout = new QVBoxLayout();
    mainLayout->addItem(grid);
    mainLayout->addItem(buttonLayout);


    connect(traces,&QRadioButton::toggled,this,&LoadFileDialog::onRadioChanged);
    connect(tracePathButton, &QPushButton::released,[=](){
        openSelectFileDialog(tracePath,"Select trace file","NeuroMorpho(*.swc);;Neurolucida ASC(*.asc)",false);
    });
    connect(apiPathButton, &QPushButton::released,[=](){
        openSelectFileDialog(apiPath,"Select apical file","Imaris VRML (*.vrml *.wrl)",false);
    });
    connect(basalPathButton, &QPushButton::released,[=](){
        openSelectFileDialog(basalPath,"Select basal file","Imaris VRML (*.vrml *.wrl)",true);
    });

    connect(okButton,&QPushButton::released,this,&LoadFileDialog::onOkPressed);
    connect(cancelButton,&QPushButton::released,[=](){this->close();});


    setLayout(mainLayout);
    resize(600,100);
}

void LoadFileDialog::onRadioChanged(bool b) {
    tracePath->setDisabled(!b);
    tracePathButton->setDisabled(!b);
    apiPath->setDisabled(b);
    apiPathButton->setDisabled(b);
    basalPath->setDisabled(b);
    basalPathButton->setDisabled(b);
}

void LoadFileDialog::openSelectFileDialog(QLineEdit *target,const QString& title, const QString &types,bool multiFile) {
    if (multiFile) {
        auto files = QFileDialog::getOpenFileNames(this,title,QString(),types);
        QString filesString;
        filesString.push_back(files[0]);
        for (int i = 1; i < files.size(); i++) {
            filesString.push_back(";" + files[i]);
        }
        target->setText(filesString);

    } else {
        auto file = QFileDialog::getOpenFileName(this,title,QString(),types);
        target->setText(file);
    }

}

void LoadFileDialog::onOkPressed() {
    if (traces->isChecked()) {
        this->file = tracePath->text().toStdString();
    } else {
        auto basalFiles = basalPath->text().split(";");
        auto apiFile = apiPath->text().toStdString();
        std::vector<std::string> basalFilesStd;
        for (const auto& string: basalFiles) {
            basalFilesStd.push_back(string.toStdString());
        }
        skelgenerator::Neuron neuron (apiFile,basalFilesStd);
        std::ofstream file;
        file.open("temp.swc",std::ios::out);
        file <<neuron.to_swc();
        file.close();
        this->file = "temp.swc";
    }

    accept();


}

const std::string &LoadFileDialog::getFile() const {
    return file;
}
