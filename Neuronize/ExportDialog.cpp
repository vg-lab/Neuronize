//
// Created by ivelascog on 9/10/19.
//

#include "ExportDialog.h"
#include "neuronize.h"
#include <QScrollArea>
#include <QFileDialog>

ExportDialog::ExportDialog(QWidget *parent): QDialog(parent) {
    auto scrollArea = new QScrollArea(this);
    neurons = Neuronize::bbdd.getNeuronsNames();

    auto widget = new QWidget();
    auto verticalLayout = new QVBoxLayout();
    checkBoxs.reserve(neurons.size());

    for(const auto & neuron : neurons) {
        auto checkBox = new QCheckBox(QString::fromStdString(neuron),this);
        checkBoxs.push_back(checkBox);
        verticalLayout->addWidget(checkBox);
    }
    widget->setLayout(verticalLayout);
    scrollArea->setWidget(widget);


    buttons = new QDialogButtonBox(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel);

    allCheckBox = new QCheckBox(tr("Select All"), this);

    auto title = new QLabel("Select neurons for export");
    title->setAlignment(Qt::AlignHCenter);

    auto mainLayout = new QVBoxLayout();
    mainLayout->addWidget(title);
    mainLayout->addWidget(allCheckBox);
    mainLayout->addWidget(scrollArea);
    mainLayout->addWidget(buttons);

    setLayout(mainLayout);

    connect(buttons,&QDialogButtonBox::accepted,this,&ExportDialog::onOk);
    connect(buttons,&QDialogButtonBox::rejected,[&](){this->reject();});
    connect(allCheckBox,&QCheckBox::stateChanged,this,&ExportDialog::onAllCheckBox);
}

void ExportDialog::onOk() {
    auto path = QFileDialog::getExistingDirectory(this,"Open directory to export selected Neurons",QString());

    for (size_t i = 0 ; i < checkBoxs.size(); i++) {
        const auto& checkBox = checkBoxs[i];
        if (checkBox->isChecked()) {
            Neuronize::bbdd.exportNeuron(neurons[i],path.toStdString());
        }
    }

    accept();

}

void ExportDialog::onAllCheckBox( int state) {
    for (const auto& checkBox: checkBoxs) {
        checkBox->setCheckState((Qt::CheckState) state);
    }

}
