//
// Created by ivelascog on 9/10/19.
//

#ifndef NEURONIZE_EXPORTDIALOG_H
#define NEURONIZE_EXPORTDIALOG_H


#include <QDialog>
#include <QCheckBox>
#include <QDialogButtonBox>

class ExportDialog: public QDialog {
    Q_OBJECT

    std::vector<QCheckBox*> checkBoxs;
    std::vector<std::string> neurons;
    QDialogButtonBox* buttons;

public:
    explicit ExportDialog(QWidget* parent = 0);

private slots:
    void onOk();

};


#endif //NEURONIZE_EXPORTDIALOG_H
