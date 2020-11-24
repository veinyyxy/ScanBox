/********************************************************************************
** Form generated from reading UI file 'MatchersDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MATCHERSDIALOG_H
#define UI_MATCHERSDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QTextBrowser>

QT_BEGIN_NAMESPACE

class Ui_MatchersDialog
{
public:
    QDialogButtonBox *buttonBox;
    QLabel *label;
    QTextBrowser *textBrowser;
    QLabel *label_2;
    QPlainTextEdit *plainTextEdit;
    QLabel *label_3;
    QPlainTextEdit *plainTextEdit_2;

    void setupUi(QDialog *MatchersDialog)
    {
        if (MatchersDialog->objectName().isEmpty())
            MatchersDialog->setObjectName(QStringLiteral("MatchersDialog"));
        MatchersDialog->resize(555, 307);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MatchersDialog->sizePolicy().hasHeightForWidth());
        MatchersDialog->setSizePolicy(sizePolicy);
        buttonBox = new QDialogButtonBox(MatchersDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(10, 270, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        label = new QLabel(MatchersDialog);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(10, 10, 81, 16));
        textBrowser = new QTextBrowser(MatchersDialog);
        textBrowser->setObjectName(QStringLiteral("textBrowser"));
        textBrowser->setGeometry(QRect(390, 30, 151, 221));
        label_2 = new QLabel(MatchersDialog);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(390, 10, 54, 12));
        plainTextEdit = new QPlainTextEdit(MatchersDialog);
        plainTextEdit->setObjectName(QStringLiteral("plainTextEdit"));
        plainTextEdit->setGeometry(QRect(10, 30, 371, 91));
        label_3 = new QLabel(MatchersDialog);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(10, 120, 91, 16));
        plainTextEdit_2 = new QPlainTextEdit(MatchersDialog);
        plainTextEdit_2->setObjectName(QStringLiteral("plainTextEdit_2"));
        plainTextEdit_2->setGeometry(QRect(10, 140, 371, 111));

        retranslateUi(MatchersDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), MatchersDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), MatchersDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(MatchersDialog);
    } // setupUi

    void retranslateUi(QDialog *MatchersDialog)
    {
        MatchersDialog->setWindowTitle(QApplication::translate("MatchersDialog", "Dialog", nullptr));
        label->setText(QApplication::translate("MatchersDialog", "Matcher URL:", nullptr));
        textBrowser->setHtml(QApplication::translate("MatchersDialog", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'SimSun'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; background-color:#f7f7f7;\"><span style=\" font-family:'Source Code Pro,sans-serif'; color:#008800;\">https://*.google.com/*</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; background-color:#f7f7f7;\"><span style=\" font-family:'Source Code Pro,sans-serif'; color:#008800;\">*://*.chromium.org/*</span></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-family:'Source Code Pro,san"
                        "s-serif'; color:#008800; background-color:#f7f7f7;\"><br /></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-family:'Source Code Pro,sans-serif'; color:#008800; background-color:#f7f7f7;\"><br /></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-family:'Source Code Pro,sans-serif'; color:#008800; background-color:#f7f7f7;\"><br /></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-family:'Source Code Pro,sans-serif'; color:#008800; background-color:#f7f7f7;\"><br /></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-family:'Source Code Pro,sans-serif'; color:#008800; background-color:#f7f7f7"
                        ";\"><br /></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-family:'Source Code Pro,sans-serif'; color:#008800; background-color:#f7f7f7;\"><br /></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; background-color:#f7f7f7;\"><span style=\" font-family:'Source Code Pro,sans-serif'; color:#008800;\">&quot;chrome-extension://bkemkcjhgfgodgbbhmkpefeaibolebme/&quot;</span></p></body></html>", nullptr));
        label_2->setText(QApplication::translate("MatchersDialog", "Example:", nullptr));
        label_3->setText(QApplication::translate("MatchersDialog", "Extension Key:", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MatchersDialog: public Ui_MatchersDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MATCHERSDIALOG_H
