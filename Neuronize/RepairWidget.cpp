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
#include <QDesktopServices>
#include <iostream>
#include "RepairWidget.h"
#include "neuronize.h"

#ifdef _WIN32
#define RUN "src/run.bat"
#else
#define RUN "src/run.sh"
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
    saveCombo->addItem("Obj");
    saveCombo->addItem("Stl");
    saveCombo->setCurrentIndex(1);

    advancedButton = new QPushButton("Advanced Options" );
    advancedButton->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Minimum);
    //advancedButton->setStyleSheet(tr("border:1px"));

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

    auto kernelLabel = new QLabel("Union level");
    kernelLabel->setToolTip("Modify the size of kernel used by the Dilate&Erode operation. Greater values have more possibilities of unify parts, but values too greats can produce bad results");
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
    mainLayout->setAlignment(advancedButton,Qt::AlignHCenter);
    mainLayout->addWidget(line);
    mainLayout->addWidget(advancedWidget);
    mainLayout->addWidget(repairButton);
    mainLayout->addStretch(1);
    setLayout(mainLayout);

}

void RepairWidget::onOk() {
    QStringList arguments;
    QString input;
    QString output;
    QFuture<void> future;
    if (fileRadio->isChecked()) {
        if (inputPath->text().isEmpty()) {
            QToolTip::showText(inputPath->mapToGlobal(QPoint()), tr("Need a input file"));
            return;
        } else {
            input = inputPath->text();
        }

        if (csvPath->text().isEmpty()) {
            QToolTip::showText(csvPath->mapToGlobal(QPoint()), tr("Need a output file"));
            return;
        } else {
            output = csvPath->text();
        }

        future = QtConcurrent::run([&](){
            QFileInfo fi (input);
            QString saveFormat;
            QString outputMeshPath;
            QString tmpPath;

            if (saveCombo->currentIndex() == 0) {
                saveFormat = "Obj";
                outputMeshPath = Neuronize::tmpPath + "/Meshes";
                tmpPath = outputMeshPath;
            } else {
                saveFormat = saveCombo->currentText();
                outputMeshPath = output +"/Meshes";
            }

            repairFile(output,input,saveFormat,precisionBox->text().toInt(),percentageBox->value(),
                    segmentsCheckBox->isChecked(),kernelSizeBox->text().toInt(),cleanCheckBox, tmpPath);

            addToBBDD(outputMeshPath, saveFormat);
        });

    } else {
        if (folderInputEdit->text().isEmpty()) {
            QToolTip::showText(folderInputEdit->mapToGlobal(QPoint()), tr("Need a input Directory"));
            return;
        } else {
            input = folderInputEdit->text();
        }

        if (folderOutputEdit->text().isEmpty()) {
            QToolTip::showText(folderOutputEdit->mapToGlobal(QPoint()), tr("Need a output Directory"));
            return;
        } else {
            output = folderOutputEdit->text();
        }

        future = QtConcurrent::run([&]() {
            QFileInfo fi(input);
            QString saveFormat;
            QString outputMeshPath;
            QString tmpPath;

            if (saveCombo->currentIndex() == 0) {
                saveFormat = "Obj";
                outputMeshPath = Neuronize::tmpPath + "/Meshes";
                tmpPath = outputMeshPath;
            } else {
                saveFormat = saveCombo->currentText();
                outputMeshPath = output + "/Meshes";
            }

            repairDir(output, input, saveFormat, precisionBox->text().toInt(), percentageBox->value(),
                       segmentsCheckBox->isChecked(), kernelSizeBox->text().toInt(), cleanCheckBox, tmpPath);

            addToBBDD(outputMeshPath, saveFormat);
        });
    }

    futureWatcher->setFuture(future);

    progressDialog = new QProgressDialog("Repairing Meshes", "Cancel", 0, 0, this);
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
    auto openFolderButton = msgBox.addButton(tr("go to output directory"), QMessageBox::ActionRole);
    auto okButton = msgBox.addButton(QMessageBox::Ok);
    msgBox.exec();

    if (msgBox.clickedButton() == openFolderButton) {
        QString path;

        if (fileRadio->isChecked()) {
            QFileInfo fi(csvPath->text());
            path = fi.absoluteDir().absolutePath();
        } else {
            QFileInfo fi(folderOutputEdit->text());
            path = fi.absoluteDir().absolutePath();
        }

        QDesktopServices::openUrl(QUrl::fromLocalFile(path));
    }

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

void RepairWidget::addToBBDD(const QString& path,const QString& extension) {
    QDirIterator it(path,QStringList() << "*_O." + extension, QDir::Files | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    Neuronize::bbdd.openTransaction();
    while (it.hasNext()) {
        auto originalFile = it.next();
        int pos = originalFile.lastIndexOf('.');
        auto repairedFile = originalFile;
        repairedFile.replace(pos-1,1,"R");

        QFileInfo fileInfo (originalFile);
        auto spineName = fileInfo.baseName();
        spineName = spineName.left(spineName.size() - 2);

        Neuronize::bbdd.addSpineImaris(originalFile.toStdString(),repairedFile.toStdString(),extension.toStdString(),spineName.toStdString());
    }
    Neuronize::bbdd.closeTransaction();
}

int
RepairWidget::repairFile(const QString &outputFile, const QString &inputFile, const QString &saveFormat,
                         int precision, float reduction, bool includeSegments, int kernelSize,
                         bool clean, const QString& exportPath) {
    QStringList arguments;

    arguments << "-a" << "\"" + outputFile + "\"" << "-v" << "\"" + inputFile + "\"" <<  "-s" << saveFormat << "-p" << QString::number(precision)
              << "-r" << QString::number(reduction) << "-f" << QString::number(includeSegments) << "-k" << QString::number(kernelSize)
              << "-c" << QString::number(clean);

    if (!exportPath.isEmpty()) {
        arguments << "-e" << "\"" + exportPath + "\"";
    }

    QString command = "\"" + QCoreApplication::applicationDirPath() + "/" + RUN + "\" " + Neuronize::envPath + " " + arguments.join(" ");
    std::cout << command.toStdString() << std::endl;
    return std::system(command.toStdString().c_str());

}

int RepairWidget::repairDir(const QString &outputDir, const QString &inputDir, const QString &saveFormat,
                             int precision, float reduction, bool includeSegments, int kernelSize,
                             bool clean, const QString& exportPath) {

    QStringList arguments;

    arguments << "-o" << "\"" + outputDir + "\"" << "-w" << "\"" + inputDir + "\"" <<"-s" << saveFormat << "-p" << QString::number(precision)
              << "-r" << QString::number(reduction) << "-f" << QString::number(includeSegments) << "-k" << QString::number(kernelSize)
              << "-c" << QString::number(clean);

    if (!exportPath.isEmpty()) {
       arguments << "-e" << "\"" + exportPath + "\"";
    }

    QString command = "\"\"" + QCoreApplication::applicationDirPath() + "/" + RUN + "\" " + Neuronize::envPath + " " + arguments.join(" ") + "\"";
    std::cout << command.toStdString() << std::endl;
    return std::system(command.toStdString().c_str());

}
