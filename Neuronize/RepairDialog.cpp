//
// Created by ivelascog on 6/06/19.
//

#include <QGridLayout>
#include <QProcess>
#include <QFileDialog>
#include <QToolTip>
#include <QtConcurrent/QtConcurrent>
#include <QMessageBox>
#include <QSpacerItem>
#include <QSizePolicy>
#include <QFormLayout>
#include <iostream>
#include "RepairDialog.h"

RepairDialog::RepairDialog(QWidget *parent):QDialog(parent) {
    inputPath = new QLineEdit( );
    inputPath->setPlaceholderText("Select input file");

    inputButton = new QPushButton("Select file...");

    csvPath = new QLineEdit( );
    csvPath->setPlaceholderText("Select output file");

    csvButton = new QPushButton("Select file");

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    auto grid = new QGridLayout();
    grid->setSpacing(6);

    grid->addWidget(inputButton,0,0);
    grid->addWidget(inputPath,0,1);

    grid->addWidget(csvButton,1,0);
    grid->addWidget(csvPath,1,1);

    advancedWidget = new QWidget();
    auto advacedGrid = new QHBoxLayout( advancedWidget );



    saveCombo = new QComboBox( advancedWidget );
    saveCombo->addItem("None");
    saveCombo->addItem("STL");
    saveCombo->addItem("VRML");
    
    advancedButton = new QPushButton("Advanced Options" );
    advancedButton->setStyleSheet(tr("border:1px"));

    percentageBox = new QDoubleSpinBox( advancedWidget );
    percentageBox->setValue(30.0f);
    percentageBox->setMinimum(5.0f);
    percentageBox->setMinimum(100.0f);

    cleanCheckBox = new QCheckBox( advancedWidget );
    cleanCheckBox->setChecked(true);

    auto formLayout1 = new QFormLayout( advancedWidget  );
    formLayout1->addRow(tr("Export: "),saveCombo);
    formLayout1->addRow(tr("OutputResolutionPercentage"),percentageBox);
    formLayout1->addRow(tr("Clean Vrml"),cleanCheckBox);
    formLayout1->setSpacing(6);


    precisionBox = new QSpinBox( advancedWidget);
    precisionBox->setValue(50);
    precisionBox->setMinimum(5);
    precisionBox->setMaximum(150);

    segmentsCheckBox = new QCheckBox( advancedWidget );
    segmentsCheckBox->setChecked(true);

    kernelSizeBox = new QSpinBox(advancedWidget );
    kernelSizeBox->setValue(3);
    kernelSizeBox->setMinimum(1);
    kernelSizeBox->setMaximum(8);

    auto formLayout2 = new QFormLayout( advancedWidget );
    formLayout2->addRow(tr("Precision"),precisionBox);
    formLayout2->addRow(tr("Include Segments"),segmentsCheckBox);
    formLayout2->addRow(tr("Kernel Size"),kernelSizeBox);
    formLayout2->setSpacing(6);



    advacedGrid->addLayout(formLayout1);
    advacedGrid->addLayout(formLayout2);
    advacedGrid->setSpacing(25);

    advancedWidget->setLayout(advacedGrid);


    QFrame* line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    mainLayout = new QVBoxLayout( );
    mainLayout->addItem(grid);
    mainLayout->addWidget(advancedButton);
    mainLayout->addWidget(line);
    mainLayout->addWidget(advancedWidget);
    mainLayout->addStretch(1);
    mainLayout->addWidget(buttonBox);

    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    futureWatcher = new QFutureWatcher<void>();

    connect(inputButton, &QPushButton::released,[=]() {openSelectFileDialog(inputPath,"Select Input File","Imaris Filament Tracer(*.vrml *.wrl *.imx)");});
    connect(csvButton, &QPushButton::released,[=](){openSelectFileDialog(csvPath, "Select output File", "CSV(*.csv)");});
    connect(buttonBox, &QDialogButtonBox::accepted, this, &RepairDialog::onOk);
    connect(buttonBox, &QDialogButtonBox::rejected,[=](){this->close();});
    connect(futureWatcher, &QFutureWatcher<void>::finished,this,&RepairDialog::onProcessFinish);
    connect(advancedButton, &QPushButton::released,this,&RepairDialog::onAdvancedPress);

    setLayout(mainLayout);
    advancedWidget->hide();
    resize(600,100);
}

void RepairDialog::onOk() {
    QString program = "meshreconstructwrapper";
    QString input;
    if (inputPath->text().isEmpty()) {
        QToolTip::showText(inputPath->mapToGlobal(QPoint()), tr("Need a input file"));
        return;
    } else {
        input = inputPath->text();
    }

    QString output;
    if (csvPath->text().isEmpty()) {
        QString filename = input.section(".",0,0);
        output = input+".csv";
    } else {
        output = csvPath->text();
    }
    auto future = QtConcurrent::run([=](){
        QStringList arguments;
        arguments << "-a" << output << "-v" << input << "-s" << saveCombo->currentText() << "-p" << precisionBox->text()
                  << "-r" << QString::number(percentageBox->value()) << "-f" << QString::number(segmentsCheckBox->isChecked()) << "-k" << kernelSizeBox->text()
                  << "-c" << QString::number(cleanCheckBox->isChecked());
        for ( QString string: arguments) {
            std::cout << string.toStdString() << " ";
        }

        QProcess* process = new QProcess();
        process->start(program,arguments);
        process->waitForFinished();
        process->close();
    });
    futureWatcher->setFuture(future);

    progressDialog = new QProgressDialog("Operation in progress", "Cancel", 0, 0, this);
    progressDialog->setValue(0);
    progressDialog->setCancelButton(0);
    progressDialog->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    progressDialog->exec();

}

void RepairDialog::openSelectFileDialog(QLineEdit *dest,const QString& message,const QString& formats) {
    auto file = QFileDialog::getOpenFileName(this,message,QString(),formats);
    dest->setText(file);
}

void RepairDialog::onProcessFinish() {
    progressDialog->setMaximum(1);
    progressDialog->setValue(1);
    QMessageBox msgBox(this);
    msgBox.setText("Task Finished");
    msgBox.setIcon(QMessageBox::Information);
    msgBox.exec();
    this->accept();
}

void RepairDialog::onAdvancedPress() {
    advancedWidget->setVisible(!advancedWidget->isVisible());
    if (!advancedWidget->isVisible()) {
        mainLayout->setSizeConstraint(QLayout::SetFixedSize);
        resize(600,2);
    } else {
        mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    }

}
