//
// Created by ivelascog on 17/07/18.
//

#ifndef NEURONIZE_LOADFILEDIALOG_H
#define NEURONIZE_LOADFILEDIALOG_H


#include <QDialog>
#include <QRadioButton>
#include <QLineEdit>
#include <QPushButton>
#include <QFutureWatcher>
#include <QtWidgets/QProgressDialog>
#include <SkelGeneratorUtil/Neuron.h>
#include <QDialogButtonBox>
#include <QCheckBox>



class LoadFileDialog: public QDialog {
     Q_OBJECT



public:
    explicit LoadFileDialog(const std::string &tmpDir,QWidget* parent = 0);
    const std::string &getFile() const;
    skelgenerator::Neuron *getNeuron() const;

private:
    QRadioButton* traces;
    QRadioButton* vrmls;
    QLineEdit* tracePath;
    QLineEdit* apiPath;
    QLineEdit* basalPath;
    QLineEdit* ascPath;
    QPushButton* ascButton;
    QPushButton* tracePathButton;
    QPushButton* apiPathButton;
    QPushButton* basalPathButton;
    QCheckBox* saveCheckBox;
    QDialogButtonBox* buttonBox;
    std::string file;
    std::string tmpDir;
    QFutureWatcher<void>* futureWatcher;
    QProgressDialog *progresDialog;
    skelgenerator::Neuron* neuron;


private slots:
    void openSelectFileDialog(QLineEdit* target,const QString& title,const QString &types,bool multiFile);
    void openSaveFileDialog(QLineEdit *target,const QString& title, const QString &types);
    void onRadioChanged(bool b);
    void onOkPressed();
    void onSaveChanged(int state);

    void onProcessFinish();
    void processSkel(const std::string &fileName);

    void showWarningDialogIncorrectConnections(int &newThreshold);
    void showWarningDialogReaminingSegments(int sobrantes, int &newThreshold);



};


#endif //NEURONIZE_LOADFILEDIALOG_H
