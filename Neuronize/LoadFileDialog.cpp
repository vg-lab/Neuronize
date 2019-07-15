//
// Created by ivelascog on 17/07/18.
//

#include "LoadFileDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QFile>
#include <iostream>
#include <fstream>
#include <QtConcurrent/QtConcurrent>
#include <QMessageBox>
#include <QInputDialog>
#include <QProgressDialog>
#include <QFutureWatcher>


LoadFileDialog::LoadFileDialog(const std::string &tmpDir,QWidget *parent): QDialog(parent) {

    this->tmpDir = tmpDir;
    traces = new QRadioButton("Trace File: ", this);
    traces->setChecked(true);

    vrmls = new QRadioButton("VRML Files: ", this);

    tracePath = new QLineEdit(this);
    tracePath->setPlaceholderText("Trace file (SWC,ASC)");

    apiPath = new QLineEdit(this);
    apiPath->setPlaceholderText("Apical VRML file (if any)");
    apiPath->setEnabled(false);

    basalPath = new QLineEdit(this);
    basalPath->setPlaceholderText("Basal/s VRML file/s");
    basalPath->setEnabled(false);

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    tracePathButton = new QPushButton("Select File...",this);
    basalPathButton = new QPushButton("Select File...",this);
    basalPathButton->setEnabled(false);
    apiPathButton = new QPushButton("Select File...",this);
    apiPathButton->setEnabled(false);
    saveCheckBox = new QCheckBox("Save Output File",this);
    saveCheckBox->setEnabled(false);

    ascButton = new QPushButton("Select Output File...",this);
    ascButton->setEnabled(false);

    ascPath = new QLineEdit(this);
    ascPath->setPlaceholderText("Export Tracing (ASC)");
    ascPath->setEnabled(false);


    futureWatcher = new QFutureWatcher<void>();
    neuron = nullptr;

    auto grid = new QGridLayout();
    grid->setSpacing(6);
    grid->addWidget(traces,0,0);
    grid->addWidget(tracePathButton,0,1);
    grid->addWidget(tracePath,0,2);

    grid->addWidget(vrmls,1,0);
    grid->addWidget(basalPathButton,1,1);
    grid->addWidget(basalPath,1,2);

    grid->addWidget(apiPathButton,2,1);
    grid->addWidget(apiPath,2,2);

    grid->addWidget(saveCheckBox,3,1);
    grid->addWidget(ascButton,4,1);
    grid->addWidget(ascPath,4,2);




    auto mainLayout = new QVBoxLayout();
    mainLayout->addItem(grid);
    mainLayout->addWidget(buttonBox);


    connect(traces,&QRadioButton::toggled,this,&LoadFileDialog::onRadioChanged);
    connect(saveCheckBox,&QCheckBox::stateChanged,this,&LoadFileDialog::onSaveChanged);
    connect(tracePathButton, &QPushButton::released,[=](){
        openSelectFileDialog(tracePath,"Select trace file","NeuroMorpho(*.swc *.SWC);;Neurolucida ASC(*.asc *.ASC)",false);
    });
    connect(apiPathButton, &QPushButton::released,[=](){
        openSelectFileDialog(apiPath,"Select apical file","Imaris VRML (*.vrml *.wrl)",false);
    });
    connect(basalPathButton, &QPushButton::released,[=](){
        openSelectFileDialog(basalPath,"Select basal file","Imaris VRML (*.vrml *.wrl)",true);
    });

    connect(ascButton, &QPushButton::released,[=](){
        openSaveFileDialog(ascPath,"Select ASC file","Neurolucida (*.asc *.ASC)");
    });

    connect(buttonBox,&QDialogButtonBox::accepted,this,&LoadFileDialog::onOkPressed);
    connect(buttonBox,&QDialogButtonBox::rejected,[=](){this->close();});

    connect(futureWatcher,&QFutureWatcher<void>::finished,this,&LoadFileDialog::onProcessFinish);


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
    saveCheckBox->setDisabled(b);
    if (b) {
        ascButton->setDisabled(b);
        ascPath->setDisabled(b);
    } else {
        bool active = saveCheckBox->checkState() == Qt::Checked;
        ascPath->setDisabled(!active);
        ascButton->setDisabled(!active);
    }
}

void LoadFileDialog::onSaveChanged(int state) {
    bool active = state == Qt::Checked;
    ascPath->setDisabled(!active);
    ascButton->setDisabled(!active);
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

void LoadFileDialog::openSaveFileDialog(QLineEdit *target,const QString& title, const QString &types) {
        auto file = QFileDialog::getSaveFileName(this,title,QString(),types);
        target->setText(file);

}

void LoadFileDialog::onOkPressed() {
    if (traces->isChecked()) {
        this->file = tracePath->text().toStdString();
        accept();
    } else {

        QFuture<void> future = QtConcurrent::run([=]() { processSkel(tmpDir + "/temp.asc"); });
        futureWatcher->setFuture(future);
        progresDialog = new QProgressDialog("Operation in progress", "Cancel", 0, 0, this);
        progresDialog->setValue(0);
        progresDialog->setCancelButton(0);
        progresDialog->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
        progresDialog->exec();
    }
}

const std::string &LoadFileDialog::getFile() const {
    return file;
}

void LoadFileDialog::processSkel(const std::string &fileName){
    auto basalFiles = basalPath->text().split(";");
    auto apiFile = apiPath->text().toStdString();
    std::vector<std::string> basalFilesStd;
    for (const auto& string: basalFiles) {
        basalFilesStd.push_back(string.toStdString());
    }
    auto neuron = new skelgenerator::Neuron (apiFile,basalFilesStd);
    bool ignore = false;
    while (neuron->isIncorrectConecctions() || neuron->getReamingSegments() >0 && !ignore) {
        int newThreshold;
        if (neuron->isIncorrectConecctions()) {
            QMetaObject::invokeMethod(this, "showWarningDialogIncorrectConnections", Qt::BlockingQueuedConnection,
                                      Q_ARG(int &, newThreshold));

        } else {
            if ( neuron->getReamingSegments() > 0 ){
                QMetaObject::invokeMethod(this, "showWarningDialogReaminingSegments", Qt::BlockingQueuedConnection,
                                          Q_ARG(int, neuron->getReamingSegments()),
                                          Q_ARG(int & , newThreshold));
            }
        }

        ignore = newThreshold < 0;
        if (!ignore) {
            delete(neuron);
            neuron = new skelgenerator::Neuron(apiFile, basalFilesStd, newThreshold);
        }

    }


    std::ofstream file;
    file.open(fileName,std::ios::out);
    file << neuron->to_asc();
    file.close();
    this->file = fileName;
    this->neuron = neuron;

}

void LoadFileDialog::onProcessFinish() {
    progresDialog->setMaximum(1);
    progresDialog->setValue(1);
    QMessageBox msgBox(this);
    if (saveCheckBox->checkState() == Qt::Checked && ascPath->text() != nullptr && ascPath->text() != "") {
        std::ofstream file;
        file.open(ascPath->text().toStdString());
        file << this->neuron->to_asc();
        file.close();
    }
    msgBox.setText("Task Finished");
    msgBox.setIcon(QMessageBox::Information);
    msgBox.exec();
    this->accept();
}

void LoadFileDialog::showWarningDialogIncorrectConnections(int &newThreshold) {
    auto *msgBox = new QMessageBox(this);;
    std::string msg = "The neuron maybe has incorrect connections.\t";
    msgBox->setIcon(QMessageBox::Warning);
    msgBox->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    msgBox->setInformativeText(
            "Do you want to process the neuron again changing the \"Connection Threshold\" or ignore the maybe incorrect conections?");
    msgBox->setText(QString::fromStdString(msg));
    QPushButton *changeButton = msgBox->addButton(tr(" Change Threshold "), QMessageBox::NoRole);
    QPushButton *ignoreButton = msgBox->addButton(tr("Ignore"), QMessageBox::NoRole);
    msgBox->setDefaultButton(changeButton);
    msgBox->exec();

    if (msgBox->clickedButton() == changeButton) {
        QInputDialog inputDialog(this);
        inputDialog.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
        inputDialog.setLabelText("New Connection Threshold");
        inputDialog.setTextValue("Insert new value");
        inputDialog.setInputMode(QInputDialog::IntInput);
        inputDialog.setIntRange(0, 40);
        inputDialog.setIntStep(1);
        inputDialog.setIntValue(newThreshold);
        inputDialog.exec();
        newThreshold = inputDialog.intValue();
    } else {
        newThreshold = -1;
    }
}

void LoadFileDialog::showWarningDialogReaminingSegments(int sobrantes, int &newThreshold) {
    QMessageBox *msgBox = new QMessageBox(this);;
    std::string msg = "This neuron has " + std::to_string(sobrantes) +
                      " segments that have not been connected and therefore will be ignored.";
    msgBox->setIcon(QMessageBox::Warning);
    msgBox->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    msgBox->setInformativeText(
            "Do you want to process the neuron again changing the \"Connection Threshold\" or ignore the missing segments?");
    msgBox->setText(QString::fromStdString(msg));
    QPushButton *changeButton = msgBox->addButton(tr(" Change Threshold "), QMessageBox::NoRole);
    QPushButton *ignoreButton = msgBox->addButton(tr("Ignore"), QMessageBox::NoRole);
    msgBox->setDefaultButton(changeButton);
    msgBox->exec();

    if (msgBox->clickedButton() == changeButton) {
        QInputDialog inputDialog;
        inputDialog.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
        inputDialog.setLabelText("New Connection Threshold");
        inputDialog.setTextValue("Insert new value");
        inputDialog.setInputMode(QInputDialog::IntInput);
        inputDialog.setIntRange(0, 40);
        inputDialog.setIntStep(1);
        inputDialog.setIntValue(newThreshold);
        inputDialog.exec();
        newThreshold = inputDialog.intValue();


    } else {
        newThreshold = -1;
    }
}

skelgenerator::Neuron *LoadFileDialog::getNeuron() const {
    return neuron;
}
