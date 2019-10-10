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
    auto gridLayout = new QGridLayout();
    checkBoxs.reserve(neurons.size());

    for(size_t i =0 ; i < neurons.size(); i++) {
        const auto& neuron = neurons[i];
        auto checkBox = new QCheckBox(this);
        checkBoxs.push_back(checkBox);
        gridLayout->addWidget(checkBox,i,0);
        gridLayout->addWidget(new QLabel(QString::fromStdString(neuron)),i,1);
    }
    widget->setLayout(gridLayout);
    scrollArea->setWidget(widget);


    buttons = new QDialogButtonBox(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel);

    auto title = new QLabel("Select neurons for export");
    title->setAlignment(Qt::AlignHCenter);

    auto mainLayout = new QVBoxLayout();
    mainLayout->addWidget(title);
    mainLayout->addWidget(scrollArea);
    mainLayout->addWidget(buttons);

    setLayout(mainLayout);

    connect(buttons,&QDialogButtonBox::accepted,this,&ExportDialog::onOk);
    connect(buttons,&QDialogButtonBox::rejected,[&](){this->reject();});
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
