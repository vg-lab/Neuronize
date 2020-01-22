//
// Created by ivelascog on 11/12/19.
//

#include "WizzardInputOne.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <iostream>
#include <QInputDialog>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>
#include <QProgressDialog>

WizzardInputOne::WizzardInputOne(QWidget* parent): QWizard(parent) {
    this->neuron = nullptr;

    this->setPage(Page_files,new FilePage(this));
    this->setPage(Page_SWC,new SWCPage(traceFile,this));
    this->setPage(Page_ASC, new ASCPage(traceFile,this));
    this->setPage(Page_Filament,new FilamentPage(filamentFiles,this));
    this->setPage(Page_Select_Apical,new SelectApicalPage(neuron,filamentFiles,this));
    this->setPage(Page_Add_Longs,new AddLongsPage(neuron,this));
    this->setPage(Page_Finish, new FinishPage(outputPath,this));
}

bool WizzardInputOne::isFilament() {
    return neuron != nullptr;
}

QString WizzardInputOne::getTraceFile() const {
    return traceFile;
}

skelgenerator::Neuron *WizzardInputOne::getNeuron() const {
    return neuron;
}

const QString &WizzardInputOne::getOutputPath() const {
    return outputPath;
}


FilePage::FilePage(QWidget *parent) : QWizardPage(parent) {
    this->setTitle("Select input file type");
    auto label = new QLabel("select the type of file");

    radioSWC = new QRadioButton("SWC");
    radioASC = new QRadioButton("ASC");
    radioFilament = new QRadioButton("Filament");

    auto layout = new QVBoxLayout;
    layout->addWidget(label);
    layout->addWidget(radioSWC);
    layout->addWidget(radioASC);
    layout->addWidget(radioFilament);

    this->setLayout(layout);
}

int FilePage::nextId() const {
    if (radioSWC->isChecked())
        return WizzardInputOne::Page_SWC;
    if (radioASC->isChecked())
        return WizzardInputOne::Page_ASC;
    if (radioFilament->isChecked())
        return WizzardInputOne::Page_Filament;

}

SWCPage::SWCPage(QString& traceFile_, QWidget *parent) : QWizardPage(parent), traceFile(traceFile_) {
    this->setTitle("Select input SWC file");

    auto button = new QPushButton(tr("&SWC file"));
    SWCLineEdit = new QLineEdit;

    auto layout = new  QHBoxLayout;
    layout->addWidget(button);
    layout->addWidget(SWCLineEdit);
    this->setLayout(layout);
    connect(button,&QPushButton::released,this,[&](){
        auto file = QFileDialog::getOpenFileName(this,tr("Select SWC file"),QString(),"SWC(*.swc *.SWC)");
        SWCLineEdit->setText(file);
    });
}

int SWCPage::nextId() const {
    return WizzardInputOne::Page_Finish;
}

bool SWCPage::validatePage() {
    if (!this->SWCLineEdit->text().isEmpty()) {
        this->traceFile = SWCLineEdit->text();
        return true;
    } else {
        QMessageBox::warning(this,"Missing SWC file","Please fill SWC file field to continue (or go back)");
        return false;
    }
}

ASCPage::ASCPage(QString& traceFile_, QWidget *parent) : QWizardPage(parent), traceFile(traceFile_) {
    this->setTitle("Select input ASC file");
    auto button = new QPushButton(tr("&ASC file"));
    ASCLineEdit = new QLineEdit;

    auto layout = new  QHBoxLayout;
    layout->addWidget(button);
    layout->addWidget(ASCLineEdit);
    this->setLayout(layout);
    connect(button,&QPushButton::released,this,[&](){
        auto file = QFileDialog::getOpenFileName(this,tr("Select ASC file"),QString(),"ASC(*.asc *.ASC)");
        ASCLineEdit->setText(file);
    });
}

int ASCPage::nextId() const {
    return WizzardInputOne::Page_Finish;
}

bool ASCPage::validatePage() {
    if (!this->ASCLineEdit->text().isEmpty()) {
        this->traceFile = ASCLineEdit->text();
        return true;
    } else {
        QMessageBox::warning(this,"Missing ASC file","Please fill ASC file field to continue (or go back)");
        return false;
    }
}

FilamentPage::FilamentPage(QStringList& filamentFiles_, QWidget* parent) : QWizardPage(parent), filamentFiles(filamentFiles_) {
    this->setTitle("Select input VRML files");
    this->setSubTitle("Remember that each file must contain at least one complete neurite");

    this->dendriteList = new QListWidget();
    auto addDendrite = new QPushButton("Add Neurite/s");
    auto removeDendrite = new QPushButton("Remove Neurite/s");

    connect(addDendrite,&QPushButton::released,this,[&](){
        auto files = QFileDialog::getOpenFileNames(this,"Select VRML Neurites file/s",QString(),"VRML(*.vrml *.VRML *.wrl *.WRL)");
        for (const auto& file: files) {
            QFileInfo fileInfo (file);
            auto item = new QListWidgetItem(fileInfo.baseName(),dendriteList);
            item->setData(Qt::ToolTipRole,file);
            std::cout << dendriteList->count() << std::endl;
        }
    });

    connect(removeDendrite,&QPushButton::released,this,[&](){
        auto items = dendriteList->selectedItems();
        if (items.isEmpty()) {
            QMessageBox::warning(this,tr("Neuronize"),tr("Please select a dendrite to remove"));
        }

        for (const auto& item: items) {
            int index = dendriteList->row(item);
            dendriteList->takeItem(index);
        }
    });

    auto buttonLayout = new QVBoxLayout;
    buttonLayout->addWidget(addDendrite);
    buttonLayout->addWidget(removeDendrite);

    auto layout = new QHBoxLayout;
    layout->addItem(buttonLayout);
    layout->addWidget(dendriteList);

    this->setLayout(layout);
}

bool FilamentPage::validatePage() {
    filamentFiles.clear();
    for (size_t i = 0; i < this->dendriteList->count(); i++) {
        auto item = this->dendriteList->item(i);
        auto file = item->data(Qt::ToolTipRole).toString();
        filamentFiles.append(file);
    }
    return QWizardPage::validatePage();
}


SelectApicalPage::SelectApicalPage(skelgenerator::Neuron*&neuron_, QStringList& filamentfiles_,QWidget *parent) : QWizardPage(parent), neuron(neuron_), filamentFiles(filamentfiles_) {

    this->dendriteList = new QListWidget;
    this->neuron = nullptr;
    this->selectedItem = nullptr;
    dendriteList->setSelectionMode(QAbstractItemView::SingleSelection);
    connect(dendriteList,&QListWidget::itemClicked,this, [&](QListWidgetItem* item){
       if ( selectedItem == item) {
           dendriteList->clearSelection();
           dendriteList->clearFocus();
           this->selectedItem = nullptr;
       } else {
           this->selectedItem = item;
       }
    });

    this->setTitle("Please select the apical dendrite if it exists");
    auto layout = new QHBoxLayout;
    layout->addWidget(dendriteList);

    this->setLayout(layout);
}

void SelectApicalPage::initializePage() {
    this->selectedItem = nullptr;
    dendriteList->clear();
    for (const auto& file : filamentFiles) {
        QFileInfo fileInfo (file);
        auto item = new QListWidgetItem(fileInfo.baseName(),dendriteList);
        item->setData(Qt::ToolTipRole,file);
    }
    dendriteList->setCurrentIndex(QModelIndex());
    dendriteList->setCurrentRow(-1);
}

bool SelectApicalPage::validatePage() {
    auto items = this->dendriteList->selectedItems();
    QString apical;
    if (!items.isEmpty()) {
        auto item = items[0];
        apical = item->data(Qt::ToolTipRole).toString();
        this->dendriteList->takeItem(this->dendriteList->row(item));
    }

    std::vector<std::string> basals;
    for (size_t i = 0; i < this->dendriteList->count(); i++) {
        auto item = this->dendriteList->item(i);
        auto file = item->data(Qt::ToolTipRole).toString().toStdString();
        basals.push_back(file);
    }


    QFuture<void> future = QtConcurrent::run(
            [=]() { processSkel(apical.toStdString(),basals,"","" );});
    auto futureWatcher = new QFutureWatcher<void>();
    futureWatcher->setFuture(future);
    auto progresDialog = new QProgressDialog("Generating tracing", "Cancel", 0, 0, this);
    connect(futureWatcher, &QFutureWatcher<void>::finished, this,[&](){
        progresDialog->setMaximum(1);
        progresDialog->setValue(1);
        QMessageBox msgBox(this);

        msgBox.setText("Task Finished");
        msgBox.setIcon(QMessageBox::Information);
        msgBox.exec();
    });
    progresDialog->setValue(0);
    progresDialog->setCancelButton(0);
    progresDialog->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    progresDialog->exec();



    return QWizardPage::validatePage();
}

void SelectApicalPage::showWarningDialogIncorrectConnections(float &newThreshold) {
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
        inputDialog.setInputMode(QInputDialog::DoubleInput);
        inputDialog.setDoubleRange(0.0,40.0);
        inputDialog.setDoubleStep(0.1f);
        inputDialog.setDoubleDecimals(2);
        inputDialog.setDoubleValue(newThreshold);
        inputDialog.exec();
        newThreshold = inputDialog.doubleValue();
    } else {
        newThreshold = -1;
    }
}

void SelectApicalPage::showWarningDialogReaminingSegments(int sobrantes, float &newThreshold) {
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
        inputDialog.setInputMode(QInputDialog::DoubleInput);
        inputDialog.setDoubleRange(0.0,40.0);
        inputDialog.setDoubleStep(0.1f);
        inputDialog.setDoubleDecimals(2);
        inputDialog.setDoubleValue(newThreshold);
        inputDialog.exec();
        newThreshold = inputDialog.doubleValue();


    } else {
        newThreshold = -1;
    }
}

void SelectApicalPage::processSkel(const std::string &apical,const std::vector<std::string>& basals,const std::string &imarisVol,const std::string &imarisLongs) {
    //TODO
//    auto imarisFile = ui.imarisPath->text().toStdString();
//    auto longsFile = ui.longsPath->text().toStdString();

    float newThreshold = 1.1f;
    auto neuron = new skelgenerator::Neuron(apical, basals, imarisVol, imarisLongs, newThreshold);
    bool ignore = false;
    while (neuron->isIncorrectConecctions() || neuron->getReamingSegments() > 0 && !ignore) {
        if (neuron->isIncorrectConecctions()) {
            QMetaObject::invokeMethod(this, "showWarningDialogIncorrectConnections", Qt::BlockingQueuedConnection,
                                      Q_ARG(float &, newThreshold));

        } else {
            if (neuron->getReamingSegments() > 0) {
                QMetaObject::invokeMethod(this, "showWarningDialogReaminingSegments", Qt::BlockingQueuedConnection,
                                          Q_ARG(int, neuron->getReamingSegments()),
                                          Q_ARG(float &, newThreshold));
            }
        }

        ignore = newThreshold < 0;
        if (!ignore) {
            delete (neuron);
            neuron = new skelgenerator::Neuron(apical, basals, imarisVol,imarisLongs, newThreshold);
        }

    }
    this->neuron = neuron;

}

int SelectApicalPage::nextId() const {
    if (this->neuron == nullptr)
        return WizzardInputOne::Page_Add_Longs;
    if (!this->neuron->hasFilamentSpines())
        return WizzardInputOne::Page_Add_Longs;

    return WizzardInputOne::Page_Finish;
}


AddLongsPage::AddLongsPage(skelgenerator::Neuron*& neuron_ ,QWidget *parent) : QWizardPage(parent), neuron(neuron_) {
    this->setTitle("The neuron hasn't spines");
    this->setSubTitle("Do you want to add an Imaris length file to be able to generate the spine information?");

    auto radioYes = new QRadioButton("Yes");
    auto radioNo = new QRadioButton("No");


    selectLongsButton = new QPushButton("Select File");
    longsPath = new QLineEdit();
    selectLongsButton->setEnabled(false);
    selectLongsButton->setEnabled(false);

    auto buttonLayout = new QHBoxLayout;
    buttonLayout->setSpacing(6);
    buttonLayout->addWidget(selectLongsButton);
    buttonLayout->addWidget(longsPath);

    auto yesLayout = new QVBoxLayout;
    yesLayout->addWidget(radioYes);
    yesLayout->addItem(buttonLayout);

    auto layout = new QHBoxLayout;
    layout->addItem(yesLayout);
    layout->addWidget(radioNo);

    this->setLayout(layout);
    connect(radioYes,&QRadioButton::toggled,this,[&](bool b){
        selectLongsButton->setEnabled(b);
        longsPath->setEnabled(b);
    });

    connect(selectLongsButton,&QPushButton::released,this,[&](){
        auto file = QFileDialog::getOpenFileName(this,tr("Select VRML longs file"),QString(),"Imaris(*.vrml *.VRML *.wrl *.WRL)");
        longsPath->setText(file);
    });
}

bool AddLongsPage::validatePage() {
    //TODO checks;
    this->neuron->addSpinesLongs(longsPath->text().toStdString());
    return QWizardPage::validatePage();
}

FinishPage::FinishPage(QString& oututPath_, QWidget *parent) : QWizardPage(parent), outputPath(oututPath_) {
    this->setTitle("The process is over");
    this->setSubTitle("Now select an output directory to store all the generated files and go to the soma deformation stage");

    auto button = new QPushButton(tr("&Output Directory"));
    this->outuputLineEdit = new QLineEdit;

    auto layout = new  QHBoxLayout;
    layout->addWidget(button);
    layout->addWidget(outuputLineEdit);
    this->setLayout(layout);
    connect(button,&QPushButton::released,this,[&](){
        auto file = QFileDialog::getExistingDirectory(this,tr("Select Output Directory"),QString());
        outuputLineEdit->setText(file);
    });
}

bool FinishPage::validatePage() {
    if (!outuputLineEdit->text().isEmpty()) {
        outputPath = outuputLineEdit->text();
        return true;
    } else {
        QMessageBox::warning(this,"Miss output directory","Please fill output directory field to continue");
        return false;
    }
}

