//
// Created by ivelascog on 31/1/20.
//

#include "SelectSpinesDialog.h"
#include <QVBoxLayout>
#include <QFileDialog>
#include <QLabel>
#include <QMessageBox>

SelectSpinesDialog::SelectSpinesDialog(bool hasSpines,QWidget *parent): QDialog(parent) {
    this->thisSpinesRadio = new QRadioButton("Spines included in this file");
    this->defaultSpinesRadio = new QRadioButton("Spines from a default dataset");
    this->otherSpinesRadio = new QRadioButton ("Spines from a new file (the spines must be at the same spatial coordinates as the loaded dendrite)");

    this->buttons = new QDialogButtonBox(QDialogButtonBox::Ok);

    auto text = new QLabel("A neuron from filament tracer has been loaded. Please select the spines you want to load: ");

    auto layout = new QVBoxLayout();
    layout->setSpacing(6);
    layout->addWidget(text);
    layout->addWidget(thisSpinesRadio);
    layout->addWidget(defaultSpinesRadio);
    layout->addWidget(otherSpinesRadio);
    layout->addWidget(buttons);

    connect(buttons,&QDialogButtonBox::accepted,this,&SelectSpinesDialog::onOk);

    if (!hasSpines) {
        this->thisSpinesRadio->hide();
    }

    setLayout(layout);
}

void SelectSpinesDialog::onOk() {
    if (this->thisSpinesRadio->isChecked()) {
        option = THIS_SPINES;
        accept();
    } else if (this->defaultSpinesRadio->isChecked()) {
        option = DEFAULT_SPINES;
        accept();
    } else if (this->otherSpinesRadio->isChecked()) {
        option = NEW_FILE_SPINES;
        newFilePath = QFileDialog::getOpenFileName(this,"Select spines file",QString(),"VRML(*.vrml *.VRML *.wrl *.WRL)");
        if (!newFilePath.isEmpty()) {
            accept();
        }
    } else {
        QMessageBox::warning(this,"No option selected","Please select an option to continue");
    }
}

const QString &SelectSpinesDialog::getNewFilePath() const {
    return newFilePath;
}

SelectSpinesDialog::OPTIONS SelectSpinesDialog::getOption() const {
    return option;
}
