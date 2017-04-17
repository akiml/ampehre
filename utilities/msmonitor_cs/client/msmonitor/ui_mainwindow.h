/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Mon Apr 17 12:36:00 2017
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMdiArea>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *action_Quit;
    QAction *action_Systemoverview;
    QAction *action_Power;
    QAction *action_Temperature;
    QAction *action_Clock;
    QAction *action_Utilization;
    QAction *action_Memory;
    QAction *action_Utilization_2;
    QAction *action_Temperature_2;
    QAction *action_About;
    QAction *action_Settings;
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QMdiArea *mdiArea;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;
    QMenuBar *menuBar;
    QMenu *menu_File;
    QMenu *menu_View;
    QMenu *menu_Curves;
    QMenu *menu_Heatmaps;
    QMenu *menu_Help;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1130, 708);
        MainWindow->setMinimumSize(QSize(1050, 650));
        action_Quit = new QAction(MainWindow);
        action_Quit->setObjectName(QString::fromUtf8("action_Quit"));
        action_Systemoverview = new QAction(MainWindow);
        action_Systemoverview->setObjectName(QString::fromUtf8("action_Systemoverview"));
        action_Power = new QAction(MainWindow);
        action_Power->setObjectName(QString::fromUtf8("action_Power"));
        action_Temperature = new QAction(MainWindow);
        action_Temperature->setObjectName(QString::fromUtf8("action_Temperature"));
        action_Clock = new QAction(MainWindow);
        action_Clock->setObjectName(QString::fromUtf8("action_Clock"));
        action_Utilization = new QAction(MainWindow);
        action_Utilization->setObjectName(QString::fromUtf8("action_Utilization"));
        action_Memory = new QAction(MainWindow);
        action_Memory->setObjectName(QString::fromUtf8("action_Memory"));
        action_Utilization_2 = new QAction(MainWindow);
        action_Utilization_2->setObjectName(QString::fromUtf8("action_Utilization_2"));
        action_Temperature_2 = new QAction(MainWindow);
        action_Temperature_2->setObjectName(QString::fromUtf8("action_Temperature_2"));
        action_About = new QAction(MainWindow);
        action_About->setObjectName(QString::fromUtf8("action_About"));
        action_Settings = new QAction(MainWindow);
        action_Settings->setObjectName(QString::fromUtf8("action_Settings"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        mdiArea = new QMdiArea(centralWidget);
        mdiArea->setObjectName(QString::fromUtf8("mdiArea"));
        mdiArea->setMinimumSize(QSize(1050, 600));

        verticalLayout->addWidget(mdiArea);

        MainWindow->setCentralWidget(centralWidget);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1130, 27));
        menu_File = new QMenu(menuBar);
        menu_File->setObjectName(QString::fromUtf8("menu_File"));
        menu_View = new QMenu(menuBar);
        menu_View->setObjectName(QString::fromUtf8("menu_View"));
        menu_Curves = new QMenu(menu_View);
        menu_Curves->setObjectName(QString::fromUtf8("menu_Curves"));
        menu_Heatmaps = new QMenu(menu_View);
        menu_Heatmaps->setObjectName(QString::fromUtf8("menu_Heatmaps"));
        menu_Help = new QMenu(menuBar);
        menu_Help->setObjectName(QString::fromUtf8("menu_Help"));
        MainWindow->setMenuBar(menuBar);

        menuBar->addAction(menu_File->menuAction());
        menuBar->addAction(menu_View->menuAction());
        menuBar->addAction(menu_Help->menuAction());
        menu_File->addAction(action_Quit);
        menu_File->addSeparator();
        menu_File->addAction(action_Settings);
        menu_View->addAction(menu_Curves->menuAction());
        menu_View->addAction(menu_Heatmaps->menuAction());
        menu_View->addAction(action_Systemoverview);
        menu_Curves->addAction(action_Power);
        menu_Curves->addAction(action_Temperature);
        menu_Curves->addAction(action_Clock);
        menu_Curves->addAction(action_Utilization);
        menu_Curves->addAction(action_Memory);
        menu_Heatmaps->addAction(action_Utilization_2);
        menu_Heatmaps->addAction(action_Temperature_2);
        menu_Help->addAction(action_About);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MSMonitor v0.8.0", 0, QApplication::UnicodeUTF8));
        action_Quit->setText(QApplication::translate("MainWindow", "&Quit", 0, QApplication::UnicodeUTF8));
        action_Quit->setShortcut(QApplication::translate("MainWindow", "Ctrl+Q", 0, QApplication::UnicodeUTF8));
        action_Systemoverview->setText(QApplication::translate("MainWindow", "&System Overview", 0, QApplication::UnicodeUTF8));
        action_Systemoverview->setShortcut(QApplication::translate("MainWindow", "Ctrl+O", 0, QApplication::UnicodeUTF8));
        action_Power->setText(QApplication::translate("MainWindow", "&Power", 0, QApplication::UnicodeUTF8));
        action_Power->setShortcut(QApplication::translate("MainWindow", "Ctrl+P", 0, QApplication::UnicodeUTF8));
        action_Temperature->setText(QApplication::translate("MainWindow", "&Temperature", 0, QApplication::UnicodeUTF8));
        action_Temperature->setShortcut(QApplication::translate("MainWindow", "Ctrl+T", 0, QApplication::UnicodeUTF8));
        action_Clock->setText(QApplication::translate("MainWindow", "&Clock", 0, QApplication::UnicodeUTF8));
        action_Clock->setShortcut(QApplication::translate("MainWindow", "Ctrl+C", 0, QApplication::UnicodeUTF8));
        action_Utilization->setText(QApplication::translate("MainWindow", "&Utilization", 0, QApplication::UnicodeUTF8));
        action_Utilization->setShortcut(QApplication::translate("MainWindow", "Ctrl+U", 0, QApplication::UnicodeUTF8));
        action_Memory->setText(QApplication::translate("MainWindow", "&Memory", 0, QApplication::UnicodeUTF8));
        action_Memory->setShortcut(QApplication::translate("MainWindow", "Ctrl+M", 0, QApplication::UnicodeUTF8));
        action_Utilization_2->setText(QApplication::translate("MainWindow", "&Utilization", 0, QApplication::UnicodeUTF8));
        action_Utilization_2->setShortcut(QApplication::translate("MainWindow", "Ctrl+Shift+U", 0, QApplication::UnicodeUTF8));
        action_Temperature_2->setText(QApplication::translate("MainWindow", "&Temperature", 0, QApplication::UnicodeUTF8));
        action_Temperature_2->setShortcut(QApplication::translate("MainWindow", "Ctrl+Shift+T", 0, QApplication::UnicodeUTF8));
        action_About->setText(QApplication::translate("MainWindow", "&About", 0, QApplication::UnicodeUTF8));
        action_About->setShortcut(QApplication::translate("MainWindow", "Ctrl+A", 0, QApplication::UnicodeUTF8));
        action_Settings->setText(QApplication::translate("MainWindow", "&Settings", 0, QApplication::UnicodeUTF8));
        action_Settings->setShortcut(QApplication::translate("MainWindow", "Ctrl+S", 0, QApplication::UnicodeUTF8));
        menu_File->setTitle(QApplication::translate("MainWindow", "&File", 0, QApplication::UnicodeUTF8));
        menu_View->setTitle(QApplication::translate("MainWindow", "&View", 0, QApplication::UnicodeUTF8));
        menu_Curves->setTitle(QApplication::translate("MainWindow", "&Curves", 0, QApplication::UnicodeUTF8));
        menu_Heatmaps->setTitle(QApplication::translate("MainWindow", "&Heatmaps", 0, QApplication::UnicodeUTF8));
        menu_Help->setTitle(QApplication::translate("MainWindow", "&Help", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
