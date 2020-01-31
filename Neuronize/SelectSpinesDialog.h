//
// Created by ivelascog on 31/1/20.
//

#ifndef NEURONIZE_SELECTSPINESDIALOG_H
#define NEURONIZE_SELECTSPINESDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QRadioButton>
#include <QDialogButtonBox>
class SelectSpinesDialog: public QDialog {
    Q_OBJECT

public:
    explicit SelectSpinesDialog(bool hasSpines,QWidget* parent = 0);
    enum OPTIONS {THIS_SPINES,DEFAULT_SPINES,NEW_FILE_SPINES};

private:
    QRadioButton* thisSpinesRadio;
    QRadioButton* defaultSpinesRadio;
    QRadioButton* otherSpinesRadio;
    QDialogButtonBox* buttons;

    QString newFilePath;
    OPTIONS option;
public:
    const QString &getNewFilePath() const;

    OPTIONS getOption() const;

private slots:
    void onOk();
};


#endif //NEURONIZE_SELECTSPINESDIALOG_H
