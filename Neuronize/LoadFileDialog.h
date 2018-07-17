//
// Created by ivelascog on 17/07/18.
//

#ifndef NEURONIZE_LOADFILEDIALOG_H
#define NEURONIZE_LOADFILEDIALOG_H


#include <QDialog>
#include <QRadioButton>
#include <QLineEdit>
#include <QPushButton>

class LoadFileDialog: public QDialog {
     Q_OBJECT

public:
    explicit LoadFileDialog(QWidget* parent = 0);

    const std::string &getFile() const;

private:
    QRadioButton* traces;
    QRadioButton* vrmls;
    QLineEdit* tracePath;
    QLineEdit* apiPath;
    QLineEdit* basalPath;
    QPushButton* tracePathButton;
    QPushButton* apiPathButton;
    QPushButton* basalPathButton;
    QPushButton* okButton;
    QPushButton* cancelButton;
    std::string file;
private slots:
    void openSelectFileDialog(QLineEdit* target,const QString& title,const QString &types,bool multiFile);
    void onRadioChanged(bool b);
    void onOkPressed();
};


#endif //NEURONIZE_LOADFILEDIALOG_H
