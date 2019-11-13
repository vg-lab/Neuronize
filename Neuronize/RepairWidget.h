//
// Created by ivelascog on 6/06/19.
//

#ifndef NEURONIZE_REPAIRWIDGET_H
#define NEURONIZE_REPAIRWIDGET_H


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
#include <QtWidgets/QRadioButton>

class RepairWidget : public QWidget {
    Q_OBJECT
public:
    explicit RepairWidget(QWidget *parent = 0);
    static int repairFile(const QString &outputFile,
            const QString &inputFile,
            const QString &saveFormat,
            int precision,
            float reduction,
            bool includeSegments,
            int kernelSize,
            bool clean,
            const QString& exportPath = QString());

    static int repairDir(const QString &outputDir,
                          const QString &inputDir,
                          const QString &saveFormat,
                          int precision,
                          float reduction,
                          bool includeSegments,
                          int kernelSize,
                          bool clean,
                         const QString& exportPath = QString());

private:
    QLineEdit* csvPath;
    QLineEdit* inputPath;
    QPushButton* csvButton;
    QPushButton* inputButton;
    QPushButton* advancedButton;
    QPushButton *repairButton;
    QWidget* advancedWidget;
    QComboBox* saveCombo;
    QFutureWatcher<void>* futureWatcher;
    QProgressDialog* progressDialog;
    QVBoxLayout* mainLayout;
    QDoubleSpinBox* percentageBox;
    QCheckBox* cleanCheckBox;
    QSpinBox* precisionBox;
    QCheckBox* segmentsCheckBox;
    QSpinBox* kernelSizeBox;
    QRadioButton *fileRadio;
    QRadioButton *folderRadio;
    QPushButton *folderInputButton;
    QPushButton *folderOutputButton;
    QLineEdit *folderInputEdit;
    QLineEdit *folderOutputEdit;

private slots:
    void onOk();
    void openSelectFileDialog(QLineEdit* dest, const QString& message, const QString& formats);

    void openFolder(QLineEdit *dest, const QString &message);
    void onProcessFinish();
    void onAdvancedPress();
    void saveFileDialog(QLineEdit *dest, const QString &message, const QString &formats);

    void addToBBDD(const QString& string,const QString& qString);

    void onRadioChanged(bool b);

    void setupUi();

    void setupConnections();

    void initUi() const;
};


#endif //NEURONIZE_REPAIRWIDGET_H
