#include <custominputdialog.h>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
//#define ENABLE_DEBUG_OUTPUT

CustomInputDialog::CustomInputDialog(QWidget* parent) : QDialog (parent)
{
    setWindowTitle("输入标签");

    // create widget
    QLabel* label = new QLabel("标签:");
    QLineEdit* lineEdit = new QLineEdit;
    QPushButton* saveButton = new QPushButton("保存");
    QPushButton* cancelButton = new QPushButton("取消");

    // connect signal and slot function
    connect(saveButton, &QPushButton::clicked, this, &CustomInputDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &CustomInputDialog::reject);

    // layout manage
    QGridLayout* layout = new QGridLayout(this);
    //QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(label, 0, 0);
    layout->addWidget(lineEdit, 1, 0);
    layout->addWidget(saveButton, 2, 0);
    layout->addWidget(cancelButton, 2, 1);

    setLayout(layout);
}

QString CustomInputDialog::getText()
{
    QLineEdit* lineEdit = findChild<QLineEdit*>();
    if (lineEdit)
        return lineEdit->text();
    else
        return QString();
}
