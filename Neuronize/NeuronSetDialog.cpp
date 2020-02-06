//
// Created by ivelascog on 10/1/20.
//

#include "NeuronSetDialog.h"
#include "ExportDialog.h"
#include <QFormLayout>
#include <QtWidgets/QToolTip>
#include <QFileDialog>

NeuronSetDialog::NeuronSetDialog(QWidget *parent): QDialog(parent) {
    this->inputDirectoryButton = new QPushButton("Select Input Directory");
    this->inputDirectoryPath = new QLineEdit();

    this->outputDirectoryButton = new QPushButton("Select Output Directory");
    this->outputDirectoryPath = new QLineEdit();

    auto inputLayout = new QGridLayout();
    inputLayout->setSpacing(6);
    inputLayout->addWidget(inputDirectoryButton,0,0);
    inputLayout->addWidget(inputDirectoryPath,0,1);
    inputLayout->addWidget(outputDirectoryButton,1,0);
    inputLayout->addWidget(outputDirectoryPath,1,1);

    this->baseNameLineEdit = new QLineEdit("Piram_");
    this->subdivisionSpinbox = new QSpinBox();
    this->subdivisionSpinbox->setValue(2);
    this->subdivisionSpinbox->setMinimum(0);

    auto formLayout1 = new QFormLayout();
    formLayout1->setSpacing(6);
    formLayout1->addRow("Subdivisions",this->subdivisionSpinbox);

    auto formLayout2 = new QFormLayout();
    formLayout2->setSpacing(6);
    formLayout2->addRow("Base Name",this->baseNameLineEdit);


    auto bottomLayout = new QHBoxLayout();
    bottomLayout->setSpacing(6);
    bottomLayout->addStretch();
    bottomLayout->addItem(formLayout1);
    bottomLayout->addItem(formLayout2);
    bottomLayout->addStretch();

    this->buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    auto mainLayout = new QVBoxLayout();
    mainLayout->addItem(inputLayout);
    mainLayout->addItem(bottomLayout);
    mainLayout->addWidget(buttons);

    setLayout(mainLayout);
    connect(buttons,&QDialogButtonBox::accepted,this,&NeuronSetDialog::onOk);
    connect(buttons,&QDialogButtonBox::rejected,this,&ExportDialog::reject);

    connect(inputDirectoryButton,&QPushButton::released,this,[&](){openFolder(this->inputDirectoryPath,"Select input directory");});
    connect(outputDirectoryButton,&QPushButton::released,this,[&](){openFolder(this->outputDirectoryPath,"Select Output directory");});

}

void NeuronSetDialog::onOk() {
    if (inputDirectoryPath->text().isEmpty()) {
        QToolTip::showText(inputDirectoryPath->mapToGlobal(QPoint(0, 0)), "Need a Input Directory");
        return;
    }

    if (outputDirectoryPath->text().isEmpty()) {
        QToolTip::showText(outputDirectoryPath->mapToGlobal(QPoint(0, 0)), "Need a Output Directory");
        return;
    }

    if (baseNameLineEdit->text().isEmpty()) {
        QToolTip::showText(baseNameLineEdit->mapToGlobal(QPoint(0, 0)), "Need a Base Name");
        return;
    }

    this->inputDir = inputDirectoryPath->text();
    this->outputDir = outputDirectoryPath->text();
    this->subdivisions = this->subdivisionSpinbox->value();
    this->baseName = this->baseNameLineEdit->text();

    this->accept();
}

const QString &NeuronSetDialog::getInputDir() const {
    return inputDir;
}

const QString &NeuronSetDialog::getOutputDir() const {
    return outputDir;
}

const QString &NeuronSetDialog::getBaseName() const {
    return baseName;
}

int NeuronSetDialog::getSubdivisions() const {
    return subdivisions;
}

void NeuronSetDialog::openFolder(QLineEdit *dest, const QString &message) {
    auto folder = QFileDialog::getExistingDirectory(this, message, QString());
    dest->setText(folder);
}