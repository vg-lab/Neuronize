//
// Created by ivelascog on 11/12/19.
//

#ifndef NEURONIZE_WIZZARDINPUTONE_H
#define NEURONIZE_WIZZARDINPUTONE_H


#include <QWizardPage>
#include <QRadioButton>
#include <QLineEdit>
#include <QListWidget>

#include <SkelGeneratorUtil/Neuron.h>


class FilePage : public QWizardPage {
    Q_OBJECT
    QRadioButton* radioSWC;
    QRadioButton* radioASC;
    QRadioButton* radioFilament;
public:
    explicit FilePage(QWidget *parent = 0);

private:
    int nextId() const override ;
};

class SWCPage: public QWizardPage {
    Q_OBJECT
    QLineEdit* SWCLineEdit;
    QString& traceFile;
public:
    int nextId() const override;

    bool validatePage() override;

    explicit SWCPage(QString& traceFile_, QWidget* parent = 0);

};

class ASCPage: public QWizardPage {
    Q_OBJECT
    QLineEdit* ASCLineEdit;
    QString& traceFile;

public:
    explicit ASCPage(QString& traceFile_, QWidget* parent = 0);

    int nextId() const override;

    bool validatePage() override;
};

class FilamentPage: public QWizardPage {
    Q_OBJECT
    QListWidget* dendriteList;
    QStringList& filamentFiles;
public:
    explicit FilamentPage (QStringList& filamentFiles_, QWidget* parent = 0);

    bool validatePage() override;
};

class SelectApicalPage: public QWizardPage {
    Q_OBJECT
    QStringList& filamentFiles;
    QListWidget* dendriteList;
    skelgenerator::Neuron*& neuron;
    QListWidgetItem* selectedItem;
public:
    explicit SelectApicalPage(skelgenerator::Neuron*& neuron_, QStringList& filamentfiles_,QWidget *parent);

private:
    void initializePage() override;

    bool validatePage() override;

public:
    int nextId() const override;

private slots:
    void showWarningDialogIncorrectConnections(float &newThreshold);

    void showWarningDialogReaminingSegments(int sobrantes, float &newThreshold);

    void processSkel(const std::string &apical,const std::vector<std::string>& basals,const std::string &imarisVol,const std::string &imarisLongs);
};

class AddLongsPage: public QWizardPage {
    Q_OBJECT
    QPushButton* selectLongsButton;
    QLineEdit* longsPath;
    skelgenerator::Neuron*& neuron;
public:
    explicit AddLongsPage(skelgenerator::Neuron*& neuron_,QWidget* parent = 0);

    bool validatePage() override;

};

class FinishPage:public QWizardPage {
    Q_OBJECT
    QLineEdit* outuputLineEdit;
    QString& outputPath;
public:
    explicit FinishPage(QString& outputPath_, QWidget* parent = 0);

    bool validatePage() override;
};

class WizzardInputOne: public QWizard {
    Q_OBJECT
    QStringList filamentFiles;
    skelgenerator::Neuron* neuron;
    QString traceFile;
    QString outputPath;
public:
    explicit WizzardInputOne(QWidget* parent = 0);
    bool isFilament();

    QString getTraceFile() const;

    skelgenerator::Neuron *getNeuron() const;

    const QString &getOutputPath() const;

    enum Pages {Page_files,Page_SWC,Page_ASC,Page_Filament,Page_Select_Apical,Page_Add_Longs,Page_Finish};
};


#endif //NEURONIZE_WIZZARDINPUTONE_H
