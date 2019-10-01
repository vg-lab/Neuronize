//
// Created by ivelascog on 6/06/19.
//

#ifndef NEURONIZE_REPAIRDIALOG_H
#define NEURONIZE_REPAIRDIALOG_H


#include <QDialog>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QComboBox>
#include <QFutureWatcher>
#include <QProgressDialog>
#include <QToolBox>
#include <QDoubleSpinBox>
#include <QCheckBox>

class RepairDialog: public QDialog {
    Q_OBJECT
public:
    RepairDialog(QWidget* parent = 0);

private:
    QLineEdit* csvPath;
    QLineEdit* inputPath;
    QPushButton* csvButton;
    QPushButton* inputButton;
    QPushButton* advancedButton;
    QWidget* advancedWidget;
    QDialogButtonBox* buttonBox;
    QComboBox* saveCombo;
    QFutureWatcher<void>* futureWatcher;
    QProgressDialog* progressDialog;
    QVBoxLayout* mainLayout;
    QDoubleSpinBox* percentageBox;
    QCheckBox* cleanCheckBox;
    QSpinBox* precisionBox;
    QCheckBox* segmentsCheckBox;
    QSpinBox* kernelSizeBox;

private slots:
    void onOk();
    void openSelectFileDialog(QLineEdit* dest, const QString& message, const QString& formats);
    void onProcessFinish();
    void onAdvancedPress();
    void saveFileDialog(QLineEdit *dest, const QString &message, const QString &formats);
};


#endif //NEURONIZE_REPAIRDIALOG_H
