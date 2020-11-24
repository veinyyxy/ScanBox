/********************************************************************************
** Form generated from reading UI file 'ScanFacade.ui'
**
** Created by: Qt User Interface Compiler version 5.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SCANFACADE_H
#define UI_SCANFACADE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QListView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ScanFacadeClass
{
public:
    QAction *actionload;
    QAction *actionunload;
    QAction *actionopen;
    QAction *actionclose;
    QAction *actionaquire_DS_list;
    QAction *actionopen_2;
    QAction *actionclose_2;
    QAction *action_show_setup_dialog;
    QAction *actionStartScan;
    QAction *actionprevious;
    QAction *actionnext;
    QAction *actioninsert;
    QAction *actionpackage;
    QAction *actionUpload;
    QAction *actionClear;
    QAction *actionReplace;
    QAction *actionDelete;
    QAction *actionStart;
    QAction *actionStop;
    QAction *actionExit;
    QAction *actionInsert_Extenal_Image_I;
    QAction *action_3;
    QAction *actionwindow;
    QAction *actionwindow_2;
    QAction *actionfile;
    QAction *actiondisable;
    QAction *actionMatchers;
    QWidget *centralWidget;
    QGridLayout *gridLayout;
    QGridLayout *gridLayout_2;
    QListView *listView;
    QGraphicsView *graphicsView;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QPushButton *cleanButton;
    QSpacerItem *horizontalSpacer;
    QPlainTextEdit *plainTextEdit;
    QMenuBar *menuBar;
    QMenu *menu;
    QMenu *menuDS;
    QMenu *menu_2;
    QMenu *menu_3;
    QMenu *menu_4;
    QMenu *menuServer;
    QMenu *menu_Chrome;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *ScanFacadeClass)
    {
        if (ScanFacadeClass->objectName().isEmpty())
            ScanFacadeClass->setObjectName(QStringLiteral("ScanFacadeClass"));
        ScanFacadeClass->resize(1187, 801);
        actionload = new QAction(ScanFacadeClass);
        actionload->setObjectName(QStringLiteral("actionload"));
        actionunload = new QAction(ScanFacadeClass);
        actionunload->setObjectName(QStringLiteral("actionunload"));
        actionopen = new QAction(ScanFacadeClass);
        actionopen->setObjectName(QStringLiteral("actionopen"));
        actionclose = new QAction(ScanFacadeClass);
        actionclose->setObjectName(QStringLiteral("actionclose"));
        actionaquire_DS_list = new QAction(ScanFacadeClass);
        actionaquire_DS_list->setObjectName(QStringLiteral("actionaquire_DS_list"));
        actionopen_2 = new QAction(ScanFacadeClass);
        actionopen_2->setObjectName(QStringLiteral("actionopen_2"));
        actionclose_2 = new QAction(ScanFacadeClass);
        actionclose_2->setObjectName(QStringLiteral("actionclose_2"));
        action_show_setup_dialog = new QAction(ScanFacadeClass);
        action_show_setup_dialog->setObjectName(QStringLiteral("action_show_setup_dialog"));
        actionStartScan = new QAction(ScanFacadeClass);
        actionStartScan->setObjectName(QStringLiteral("actionStartScan"));
        actionprevious = new QAction(ScanFacadeClass);
        actionprevious->setObjectName(QStringLiteral("actionprevious"));
        actionnext = new QAction(ScanFacadeClass);
        actionnext->setObjectName(QStringLiteral("actionnext"));
        actioninsert = new QAction(ScanFacadeClass);
        actioninsert->setObjectName(QStringLiteral("actioninsert"));
        actionpackage = new QAction(ScanFacadeClass);
        actionpackage->setObjectName(QStringLiteral("actionpackage"));
        actionUpload = new QAction(ScanFacadeClass);
        actionUpload->setObjectName(QStringLiteral("actionUpload"));
        actionClear = new QAction(ScanFacadeClass);
        actionClear->setObjectName(QStringLiteral("actionClear"));
        actionReplace = new QAction(ScanFacadeClass);
        actionReplace->setObjectName(QStringLiteral("actionReplace"));
        actionDelete = new QAction(ScanFacadeClass);
        actionDelete->setObjectName(QStringLiteral("actionDelete"));
        actionStart = new QAction(ScanFacadeClass);
        actionStart->setObjectName(QStringLiteral("actionStart"));
        actionStop = new QAction(ScanFacadeClass);
        actionStop->setObjectName(QStringLiteral("actionStop"));
        actionExit = new QAction(ScanFacadeClass);
        actionExit->setObjectName(QStringLiteral("actionExit"));
        actionInsert_Extenal_Image_I = new QAction(ScanFacadeClass);
        actionInsert_Extenal_Image_I->setObjectName(QStringLiteral("actionInsert_Extenal_Image_I"));
        action_3 = new QAction(ScanFacadeClass);
        action_3->setObjectName(QStringLiteral("action_3"));
        actionwindow = new QAction(ScanFacadeClass);
        actionwindow->setObjectName(QStringLiteral("actionwindow"));
        actionwindow_2 = new QAction(ScanFacadeClass);
        actionwindow_2->setObjectName(QStringLiteral("actionwindow_2"));
        actionwindow_2->setCheckable(true);
        actionfile = new QAction(ScanFacadeClass);
        actionfile->setObjectName(QStringLiteral("actionfile"));
        actionfile->setCheckable(true);
        actiondisable = new QAction(ScanFacadeClass);
        actiondisable->setObjectName(QStringLiteral("actiondisable"));
        actiondisable->setCheckable(true);
        actionMatchers = new QAction(ScanFacadeClass);
        actionMatchers->setObjectName(QStringLiteral("actionMatchers"));
        centralWidget = new QWidget(ScanFacadeClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        gridLayout = new QGridLayout(centralWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(1, 1, 1, 1);
        gridLayout_2 = new QGridLayout();
        gridLayout_2->setSpacing(6);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        listView = new QListView(centralWidget);
        listView->setObjectName(QStringLiteral("listView"));
        listView->setMinimumSize(QSize(200, 0));
        listView->setMaximumSize(QSize(300, 16777215));

        gridLayout_2->addWidget(listView, 1, 0, 1, 1);

        graphicsView = new QGraphicsView(centralWidget);
        graphicsView->setObjectName(QStringLiteral("graphicsView"));

        gridLayout_2->addWidget(graphicsView, 1, 1, 1, 1);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        cleanButton = new QPushButton(centralWidget);
        cleanButton->setObjectName(QStringLiteral("cleanButton"));

        horizontalLayout->addWidget(cleanButton);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayout);

        plainTextEdit = new QPlainTextEdit(centralWidget);
        plainTextEdit->setObjectName(QStringLiteral("plainTextEdit"));
        plainTextEdit->setMinimumSize(QSize(0, 0));
        plainTextEdit->setMaximumSize(QSize(16777215, 150));

        verticalLayout->addWidget(plainTextEdit);


        gridLayout_2->addLayout(verticalLayout, 2, 0, 1, 2);


        gridLayout->addLayout(gridLayout_2, 0, 1, 1, 1);

        ScanFacadeClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(ScanFacadeClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1187, 23));
        menu = new QMenu(menuBar);
        menu->setObjectName(QStringLiteral("menu"));
        menuDS = new QMenu(menu);
        menuDS->setObjectName(QStringLiteral("menuDS"));
        menu_2 = new QMenu(menuBar);
        menu_2->setObjectName(QStringLiteral("menu_2"));
        menu_3 = new QMenu(menuBar);
        menu_3->setObjectName(QStringLiteral("menu_3"));
        menu_4 = new QMenu(menu_3);
        menu_4->setObjectName(QStringLiteral("menu_4"));
        menuServer = new QMenu(menuBar);
        menuServer->setObjectName(QStringLiteral("menuServer"));
        menu_Chrome = new QMenu(menuBar);
        menu_Chrome->setObjectName(QStringLiteral("menu_Chrome"));
        ScanFacadeClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(ScanFacadeClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        ScanFacadeClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(ScanFacadeClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        ScanFacadeClass->setStatusBar(statusBar);

        menuBar->addAction(menuServer->menuAction());
        menuBar->addAction(menu->menuAction());
        menuBar->addAction(menu_2->menuAction());
        menuBar->addAction(menu_Chrome->menuAction());
        menuBar->addAction(menu_3->menuAction());
        menu->addAction(actionload);
        menu->addAction(actionopen);
        menu->addAction(actionaquire_DS_list);
        menu->addSeparator();
        menu->addAction(menuDS->menuAction());
        menu->addSeparator();
        menu->addAction(actionunload);
        menu->addAction(actionclose);
        menuDS->addSeparator();
        menu_2->addAction(actionopen_2);
        menu_2->addAction(action_show_setup_dialog);
        menu_2->addSeparator();
        menu_2->addAction(actionclose_2);
        menu_3->addAction(actionStartScan);
        menu_3->addSeparator();
        menu_3->addAction(actionprevious);
        menu_3->addAction(actionnext);
        menu_3->addSeparator();
        menu_3->addAction(actionInsert_Extenal_Image_I);
        menu_3->addAction(actioninsert);
        menu_3->addAction(actionReplace);
        menu_3->addAction(actionDelete);
        menu_3->addAction(actionClear);
        menu_3->addSeparator();
        menu_3->addAction(actionpackage);
        menu_3->addAction(actionUpload);
        menu_3->addSeparator();
        menu_3->addAction(menu_4->menuAction());
        menu_4->addAction(actionwindow_2);
        menu_4->addAction(actionfile);
        menu_4->addAction(actiondisable);
        menuServer->addAction(actionStart);
        menuServer->addAction(actionStop);
        menuServer->addSeparator();
        menuServer->addAction(actionExit);
        menu_Chrome->addAction(actionMatchers);

        retranslateUi(ScanFacadeClass);

        QMetaObject::connectSlotsByName(ScanFacadeClass);
    } // setupUi

    void retranslateUi(QMainWindow *ScanFacadeClass)
    {
        ScanFacadeClass->setWindowTitle(QApplication::translate("ScanFacadeClass", "ScanFacade", nullptr));
        actionload->setText(QApplication::translate("ScanFacadeClass", "Load DSM(&L)", nullptr));
#ifndef QT_NO_TOOLTIP
        actionload->setToolTip(QApplication::translate("ScanFacadeClass", "\350\277\236\346\216\245\346\225\260\346\215\256\346\272\220\347\256\241\347\220\206\345\231\250", nullptr));
#endif // QT_NO_TOOLTIP
        actionunload->setText(QApplication::translate("ScanFacadeClass", "Unload DSM(&U)", nullptr));
        actionopen->setText(QApplication::translate("ScanFacadeClass", "Open DSM(&O)", nullptr));
#ifndef QT_NO_TOOLTIP
        actionopen->setToolTip(QApplication::translate("ScanFacadeClass", "\346\211\223\345\274\200\346\225\260\346\215\256\346\272\220\347\256\241\347\220\206\345\231\250", nullptr));
#endif // QT_NO_TOOLTIP
        actionclose->setText(QApplication::translate("ScanFacadeClass", "Close DSM(&C)", nullptr));
        actionaquire_DS_list->setText(QApplication::translate("ScanFacadeClass", "Aquire DS list(&D)", nullptr));
#ifndef QT_NO_TOOLTIP
        actionaquire_DS_list->setToolTip(QApplication::translate("ScanFacadeClass", "\350\216\267\345\276\227\346\225\260\346\215\256\346\272\220\345\210\227\350\241\250", nullptr));
#endif // QT_NO_TOOLTIP
        actionopen_2->setText(QApplication::translate("ScanFacadeClass", "Open(&O)", nullptr));
#ifndef QT_NO_TOOLTIP
        actionopen_2->setToolTip(QApplication::translate("ScanFacadeClass", "\346\211\223\345\274\200\346\225\260\346\215\256\346\272\220", nullptr));
#endif // QT_NO_TOOLTIP
        actionclose_2->setText(QApplication::translate("ScanFacadeClass", "Close", nullptr));
        action_show_setup_dialog->setText(QApplication::translate("ScanFacadeClass", "Show setup dialog(&S)", nullptr));
#ifndef QT_NO_TOOLTIP
        action_show_setup_dialog->setToolTip(QApplication::translate("ScanFacadeClass", "\346\230\276\347\244\272\346\225\260\346\215\256\346\272\220\347\232\204\351\205\215\347\275\256\345\257\271\350\257\235\346\241\206", nullptr));
#endif // QT_NO_TOOLTIP
        actionStartScan->setText(QApplication::translate("ScanFacadeClass", "Start Scan..(&S)", nullptr));
        actionprevious->setText(QApplication::translate("ScanFacadeClass", "Previous(&<)", nullptr));
        actionnext->setText(QApplication::translate("ScanFacadeClass", "Next(&>)", nullptr));
        actioninsert->setText(QApplication::translate("ScanFacadeClass", "Insert(&I)", nullptr));
        actionpackage->setText(QApplication::translate("ScanFacadeClass", "Package(&P)...", nullptr));
#ifndef QT_NO_TOOLTIP
        actionpackage->setToolTip(QApplication::translate("ScanFacadeClass", "\346\211\223\345\214\205\345\216\213\347\274\251\347\232\204\345\233\276\347\211\207", nullptr));
#endif // QT_NO_TOOLTIP
        actionUpload->setText(QApplication::translate("ScanFacadeClass", "Upload(&U)", nullptr));
        actionClear->setText(QApplication::translate("ScanFacadeClass", "Clear(&C)", nullptr));
        actionReplace->setText(QApplication::translate("ScanFacadeClass", "Replace(R)", nullptr));
        actionDelete->setText(QApplication::translate("ScanFacadeClass", "Delete(&D)", nullptr));
        actionStart->setText(QApplication::translate("ScanFacadeClass", "Start(&S)", nullptr));
        actionStop->setText(QApplication::translate("ScanFacadeClass", "Stop(&T)", nullptr));
        actionExit->setText(QApplication::translate("ScanFacadeClass", "Exit(&E)", nullptr));
        actionInsert_Extenal_Image_I->setText(QApplication::translate("ScanFacadeClass", "Insert Extenal Image(&I)", nullptr));
        action_3->setText(QApplication::translate("ScanFacadeClass", "\346\226\207\344\273\266", nullptr));
        actionwindow->setText(QApplication::translate("ScanFacadeClass", "window", nullptr));
        actionwindow_2->setText(QApplication::translate("ScanFacadeClass", "Window", nullptr));
        actionfile->setText(QApplication::translate("ScanFacadeClass", "File", nullptr));
        actiondisable->setText(QApplication::translate("ScanFacadeClass", "Disable", nullptr));
        actionMatchers->setText(QApplication::translate("ScanFacadeClass", "Matches(&M)...", nullptr));
        cleanButton->setText(QApplication::translate("ScanFacadeClass", "Clear Logging", nullptr));
        menu->setTitle(QApplication::translate("ScanFacadeClass", "Data Source Manger(&M)", nullptr));
        menuDS->setTitle(QApplication::translate("ScanFacadeClass", "DS", nullptr));
        menu_2->setTitle(QApplication::translate("ScanFacadeClass", "Date Source(&D)", nullptr));
        menu_3->setTitle(QApplication::translate("ScanFacadeClass", "Scan Image(&N)", nullptr));
        menu_4->setTitle(QApplication::translate("ScanFacadeClass", "logging(&L)", nullptr));
        menuServer->setTitle(QApplication::translate("ScanFacadeClass", "Server(&S)", nullptr));
        menu_Chrome->setTitle(QApplication::translate("ScanFacadeClass", "Chrome(&C)", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ScanFacadeClass: public Ui_ScanFacadeClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SCANFACADE_H
