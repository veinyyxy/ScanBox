/********************************************************************************
** Form generated from reading UI file 'DeviceDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DEVICEDIALOG_H
#define UI_DEVICEDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListView>

QT_BEGIN_NAMESPACE

class Ui_DeiveDialog
{
public:
    QDialogButtonBox *buttonBox;
    QListView *listView;
    QLabel *label;

    void setupUi(QDialog *DeiveDialog)
    {
        if (DeiveDialog->objectName().isEmpty())
            DeiveDialog->setObjectName(QStringLiteral("DeiveDialog"));
        DeiveDialog->resize(270, 263);
        buttonBox = new QDialogButtonBox(DeiveDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(60, 230, 141, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        listView = new QListView(DeiveDialog);
        listView->setObjectName(QStringLiteral("listView"));
        listView->setGeometry(QRect(10, 30, 256, 192));
        label = new QLabel(DeiveDialog);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(10, 10, 81, 16));

        retranslateUi(DeiveDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), DeiveDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), DeiveDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(DeiveDialog);
    } // setupUi

    void retranslateUi(QDialog *DeiveDialog)
    {
        DeiveDialog->setWindowTitle(QApplication::translate("DeiveDialog", "Device Dialog", nullptr));
        label->setText(QApplication::translate("DeiveDialog", "Device List:", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DeiveDialog: public Ui_DeiveDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DEVICEDIALOG_H
