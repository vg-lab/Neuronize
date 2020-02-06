//
// Created by ivelascog on 10/1/20.
//

#ifndef NEURONIZE_NEURONSETDIALOG_H
#define NEURONIZE_NEURONSETDIALOG_H


#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QDialogButtonBox>

class NeuronSetDialog: public QDialog {
    Q_OBJECT

    QPushButton* inputDirectoryButton;
    QPushButton* outputDirectoryButton;
    QLineEdit* inputDirectoryPath;
    QLineEdit* outputDirectoryPath;
    QLineEdit* baseNameLineEdit;
    QSpinBox* subdivisionSpinbox;
    QDialogButtonBox* buttons;

    QString inputDir;
public:
    const QString &getInputDir() const;

    const QString &getOutputDir() const;

    const QString &getBaseName() const;

    int getSubdivisions() const;

private:
    QString outputDir;
    QString baseName;
    int subdivisions;


public:
    explicit NeuronSetDialog(QWidget* parent = 0);

private slots:
    void onOk();

    void openFolder(QLineEdit *dest, const QString &message);
};


#endif //NEURONIZE_NEURONSETDIALOG_H
