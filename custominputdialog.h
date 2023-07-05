#ifndef CUSTOMINPUTDIALOG_H
#define CUSTOMINPUTDIALOG_H

#include <QDialog>
class CustomInputDialog : public QDialog
{
    Q_OBJECT
public:
    CustomInputDialog(QWidget* parent = nullptr);
    QString getText();
};


#endif // CUSTOMINPUTDIALOG_H
