//
// Created by ivelascog on 6/06/19.
//

#include <QGridLayout>
#include <QFileDialog>
#include <QToolTip>
#include <QtConcurrent/QtConcurrent>
#include <QMessageBox>
#include <QSpacerItem>
#include <QSizePolicy>
#include <QFormLayout>
#include <iostream>
#include "RepairWidget.h"
#include "neuronize.h"

#ifdef _WIN32
#define RUN std::string("src\\run.bat")
#else
#define RUN std::string("src/run.sh")
#endif


RepairWidget::RepairWidget(QWidget *parent) : QWidget(parent) {
    setupUi();
    setupConnections();
    initUi();
}

void RepairWidget::initUi() const {
    advancedWidget->hide();
    folderOutputEdit->setEnabled(false);
    folderOutputButton->setEnabled(false);
    folderInputEdit->setEnabled(false);
    folderInputButton->setEnabled(false);
    fileRadio->setChecked(true);
}

void RepairWidget::setupConnections() {
    futureWatcher = new QFutureWatcher<void>();

    connect(inputButton, &QPushButton::released, [=]() {
        openSelectFileDialog(inputPath, "Select Input File", "Imaris Filament Tracer(*.vrml *.wrl *.imx)");
    });
    connect(csvButton, &QPushButton::released, [=]() { saveFileDialog(csvPath, "Select output File", "CSV(*.csv)"); });
    connect(repairButton, &QPushButton::released, this, &RepairWidget::onOk);
    connect(futureWatcher, &QFutureWatcher<void>::finished, this, &RepairWidget::onProcessFinish);
    connect(advancedButton, &QPushButton::released, this, &RepairWidget::onAdvancedPress);
    connect(fileRadio, &QRadioButton::toggled, this, &RepairWidget::onRadioChanged);
    connect(folderInputButton, &QPushButton::released, this,
            [=]() { openFolder(folderInputEdit, "Open input folder"); });
    connect(folderOutputButton, &QPushButton::released, this,
            [=]() { openFolder(folderOutputEdit, "Open outout folder"); });
}

void RepairWidget::setupUi() {
    inputPath = new QLineEdit( );
    inputPath->setPlaceholderText("Select input file");

    inputButton = new QPushButton("Select file...");

    csvPath = new QLineEdit( );
    csvPath->setPlaceholderText("Select output file");

    csvButton = new QPushButton("Select file...");
    fileRadio = new QRadioButton("File");
    folderRadio = new QRadioButton("Folder");

    folderInputButton = new QPushButton("Select folder...");
    folderOutputButton = new QPushButton("Select folder...");

    folderInputEdit = new QLineEdit();
    folderInputEdit->setPlaceholderText("Select input folder");

    folderOutputEdit = new QLineEdit();
    folderOutputEdit->setPlaceholderText("Select output folder");

    auto grid = new QGridLayout();
    grid->setSpacing(6);
    grid->addWidget(fileRadio, 0, 0);

    grid->addWidget(inputButton, 0, 1);
    grid->addWidget(inputPath, 0, 2);

    grid->addWidget(csvButton, 1, 1);
    grid->addWidget(csvPath, 1, 2);

    grid->addWidget(folderRadio, 2, 0);

    grid->addWidget(folderInputButton, 2, 1);
    grid->addWidget(folderInputEdit, 2, 2);

    grid->addWidget(folderOutputButton, 3, 1);
    grid->addWidget(folderOutputEdit, 3, 2);


    advancedWidget = new QWidget();
    auto advacedGrid = new QHBoxLayout(advancedWidget);

    saveCombo = new QComboBox(advancedWidget);
    saveCombo->addItem("None");
    saveCombo->addItem("STL");
    saveCombo->addItem("VRML");

    advancedButton = new QPushButton("Advanced Options" );
    advancedButton->setStyleSheet(tr("border:1px"));

    percentageBox = new QDoubleSpinBox(advancedWidget);
    percentageBox->setValue(30.0f);
    percentageBox->setMinimum(5.0f);
    percentageBox->setMaximum(100.0f);

    cleanCheckBox = new QCheckBox(advancedWidget);
    cleanCheckBox->setChecked(true);

    auto formLayout1 = new QFormLayout(advancedWidget);

    auto exportLabel = new QLabel("Export: ");
    exportLabel->setToolTip("Determines the formats of the output meshes");
    formLayout1->addRow(exportLabel, saveCombo);

    auto resolutionLabel = new QLabel("OutputResolutionPercentage");
    resolutionLabel->setToolTip("Level of simplification applied to exported meshes");
    formLayout1->addRow(resolutionLabel, percentageBox);

    auto cleanLabel = new QLabel("Clean VRML");
    cleanLabel->setToolTip("Decides if the file is preprocessed. This can be useful for non-Imaris files");
    formLayout1->addRow(cleanLabel, cleanCheckBox);
    formLayout1->setSpacing(6);


    precisionBox = new QSpinBox(advancedWidget);
    precisionBox->setValue(50);
    precisionBox->setMinimum(5);
    precisionBox->setMaximum(150);

    segmentsCheckBox = new QCheckBox(advancedWidget);
    segmentsCheckBox->setChecked(true);

    kernelSizeBox = new QSpinBox(advancedWidget);
    kernelSizeBox->setValue(3);
    kernelSizeBox->setMinimum(1);
    kernelSizeBox->setMaximum(8);

    auto formLayout2 = new QFormLayout(advancedWidget);

    auto precisionLabel = new QLabel("Precision");
    precisionLabel->setToolTip("Determines the accuracy of the repaired meshes. Please, note that higher values require more memory.");
    formLayout2->addRow(precisionLabel, precisionBox);

    auto includeLabel = new QLabel("Include Segments");
    includeLabel->setToolTip("Mark if you want to repair also the dendritics segments (not only the spines)");
    formLayout2->addRow(includeLabel, segmentsCheckBox);

    auto kernelLabel = new QLabel("Kernel Size");
    kernelLabel->setToolTip("Controls the aggressiveness of the smoothing and of the union of disconnected parts");
    formLayout2->addRow(kernelLabel, kernelSizeBox);
    formLayout2->setSpacing(6);

    advacedGrid->addLayout(formLayout1);
    advacedGrid->addLayout(formLayout2);
    advacedGrid->setSpacing(25);

    advancedWidget->setLayout(advacedGrid);

    repairButton = new QPushButton("Repair");

    QFrame* line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    mainLayout = new QVBoxLayout( );
    mainLayout->addItem(grid);
    mainLayout->addWidget(advancedButton);
    mainLayout->addWidget(line);
    mainLayout->addWidget(advancedWidget);
    mainLayout->addStretch(1);
    mainLayout->addWidget(repairButton);
    setLayout(mainLayout);

}

void RepairWidget::onOk() {
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

        QString envPath = Neuronize::configPath + "/" + "env";

        QStringList arguments;
        arguments << "-a" << "\"" + output + "\"" << "-v" << "\"" + input + "\"" << "-s" << saveCombo->currentText() << "-p" << precisionBox->text()
                  << "-r" << QString::number(percentageBox->value()) << "-f" << QString::number(segmentsCheckBox->isChecked()) << "-k" << kernelSizeBox->text()
                  << "-c" << QString::number(cleanCheckBox->isChecked());
        auto test = arguments.join(" ").toStdString();
        std::cout << test << std::endl;
        std::string command = QCoreApplication::applicationDirPath().toStdString() + "/" + RUN + " " + envPath.toStdString() + " " + arguments.join(" ").toStdString();
        std::system(command.c_str());


    });
    futureWatcher->setFuture(future);

    progressDialog = new QProgressDialog("Operation in progress", "Cancel", 0, 0, this);
    progressDialog->setValue(0);
    progressDialog->setCancelButton(0);
    progressDialog->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    progressDialog->exec();

}

void RepairWidget::openSelectFileDialog(QLineEdit *dest, const QString &message, const QString &formats) {
    auto file = QFileDialog::getOpenFileName(this,message,QString(),formats);
    dest->setText(file);
}

void RepairWidget::saveFileDialog(QLineEdit *dest, const QString &message, const QString &formats) {
    auto file = QFileDialog::getSaveFileName(this,message,QString(),formats);
    dest->setText(file);
}

void RepairWidget::onProcessFinish() {
    progressDialog->setMaximum(1);
    progressDialog->setValue(1);
    QMessageBox msgBox(this);
    msgBox.setText("Task Finished");
    msgBox.setIcon(QMessageBox::Information);
    msgBox.exec();
}

void RepairWidget::onAdvancedPress() {
    advancedWidget->setVisible(!advancedWidget->isVisible());
}

void RepairWidget::onRadioChanged(bool b) {
    inputButton->setEnabled(b);
    inputPath->setEnabled(b);
    csvButton->setEnabled(b);
    csvPath->setEnabled(b);

    folderInputButton->setEnabled(!b);
    folderInputEdit->setEnabled(!b);
    folderOutputButton->setEnabled(!b);
    folderOutputEdit->setEnabled(!b);
}

void RepairWidget::openFolder(QLineEdit *dest, const QString &message) {
    auto folder = QFileDialog::getExistingDirectory(this, message, QString());
    dest->setText(folder);
}
