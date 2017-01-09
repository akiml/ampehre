/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

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
    QWidget *centralWidget;
    QWidget *widget;
    QTabWidget *tabWidget;
    QWidget *tab;
    QWidget *tab_3;
    QWidget *tab_2;
    QPushButton *pushButton;
    QLabel *label_5;
    QPushButton *pushButton_2;
    QPushButton *pushButton_3;
    QPushButton *pushButton_4;
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
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(1057, 636);
        action_Quit = new QAction(MainWindow);
        action_Quit->setObjectName(QStringLiteral("action_Quit"));
        action_Systemoverview = new QAction(MainWindow);
        action_Systemoverview->setObjectName(QStringLiteral("action_Systemoverview"));
        action_Power = new QAction(MainWindow);
        action_Power->setObjectName(QStringLiteral("action_Power"));
        action_Temperature = new QAction(MainWindow);
        action_Temperature->setObjectName(QStringLiteral("action_Temperature"));
        action_Clock = new QAction(MainWindow);
        action_Clock->setObjectName(QStringLiteral("action_Clock"));
        action_Utilization = new QAction(MainWindow);
        action_Utilization->setObjectName(QStringLiteral("action_Utilization"));
        action_Memory = new QAction(MainWindow);
        action_Memory->setObjectName(QStringLiteral("action_Memory"));
        action_Utilization_2 = new QAction(MainWindow);
        action_Utilization_2->setObjectName(QStringLiteral("action_Utilization_2"));
        action_Temperature_2 = new QAction(MainWindow);
        action_Temperature_2->setObjectName(QStringLiteral("action_Temperature_2"));
        action_About = new QAction(MainWindow);
        action_About->setObjectName(QStringLiteral("action_About"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        widget = new QWidget(centralWidget);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setGeometry(QRect(0, 0, 281, 591));
        tabWidget = new QTabWidget(widget);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tabWidget->setGeometry(QRect(10, 10, 261, 301));
        tab = new QWidget();
        tab->setObjectName(QStringLiteral("tab"));
        tabWidget->addTab(tab, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName(QStringLiteral("tab_3"));
        tabWidget->addTab(tab_3, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QStringLiteral("tab_2"));
        tabWidget->addTab(tab_2, QString());
        pushButton = new QPushButton(widget);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(10, 340, 95, 31));
        label_5 = new QLabel(widget);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(120, 340, 131, 21));
        pushButton_2 = new QPushButton(widget);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));
        pushButton_2->setGeometry(QRect(10, 430, 95, 31));
        pushButton_3 = new QPushButton(widget);
        pushButton_3->setObjectName(QStringLiteral("pushButton_3"));
        pushButton_3->setGeometry(QRect(10, 480, 95, 31));
        pushButton_4 = new QPushButton(widget);
        pushButton_4->setObjectName(QStringLiteral("pushButton_4"));
        pushButton_4->setGeometry(QRect(10, 530, 95, 31));
        MainWindow->setCentralWidget(centralWidget);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1057, 27));
        menu_File = new QMenu(menuBar);
        menu_File->setObjectName(QStringLiteral("menu_File"));
        menu_View = new QMenu(menuBar);
        menu_View->setObjectName(QStringLiteral("menu_View"));
        menu_Curves = new QMenu(menu_View);
        menu_Curves->setObjectName(QStringLiteral("menu_Curves"));
        menu_Heatmaps = new QMenu(menu_View);
        menu_Heatmaps->setObjectName(QStringLiteral("menu_Heatmaps"));
        menu_Help = new QMenu(menuBar);
        menu_Help->setObjectName(QStringLiteral("menu_Help"));
        MainWindow->setMenuBar(menuBar);

        menuBar->addAction(menu_File->menuAction());
        menuBar->addAction(menu_View->menuAction());
        menuBar->addAction(menu_Help->menuAction());
        menu_File->addAction(action_Quit);
        menu_File->addSeparator();
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

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MSMonitor v0.8.0", 0));
        action_Quit->setText(QApplication::translate("MainWindow", "&Quit", 0));
        action_Quit->setShortcut(QApplication::translate("MainWindow", "Ctrl+S", 0));
        action_Systemoverview->setText(QApplication::translate("MainWindow", "&System Overview", 0));
        action_Systemoverview->setShortcut(QApplication::translate("MainWindow", "Ctrl+O", 0));
        action_Power->setText(QApplication::translate("MainWindow", "&Power", 0));
        action_Power->setShortcut(QApplication::translate("MainWindow", "Ctrl+P", 0));
        action_Temperature->setText(QApplication::translate("MainWindow", "&Temperature", 0));
        action_Temperature->setShortcut(QApplication::translate("MainWindow", "Ctrl+T", 0));
        action_Clock->setText(QApplication::translate("MainWindow", "&Clock", 0));
        action_Clock->setShortcut(QApplication::translate("MainWindow", "Ctrl+C", 0));
        action_Utilization->setText(QApplication::translate("MainWindow", "&Utilization", 0));
        action_Utilization->setShortcut(QApplication::translate("MainWindow", "Ctrl+U", 0));
        action_Memory->setText(QApplication::translate("MainWindow", "&Memory", 0));
        action_Memory->setShortcut(QApplication::translate("MainWindow", "Ctrl+M", 0));
        action_Utilization_2->setText(QApplication::translate("MainWindow", "&Utilization", 0));
        action_Utilization_2->setShortcut(QApplication::translate("MainWindow", "Ctrl+Shift+U", 0));
        action_Temperature_2->setText(QApplication::translate("MainWindow", "&Temperature", 0));
        action_Temperature_2->setShortcut(QApplication::translate("MainWindow", "Ctrl+Shift+T", 0));
        action_About->setText(QApplication::translate("MainWindow", "&About", 0));
        action_About->setShortcut(QApplication::translate("MainWindow", "Ctrl+A", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("MainWindow", "Resource", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QApplication::translate("MainWindow", "Data", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("MainWindow", "GUI", 0));
        pushButton->setText(QApplication::translate("MainWindow", "Save", 0));
        label_5->setText(QApplication::translate("MainWindow", "save results to csv", 0));
        pushButton_2->setText(QApplication::translate("MainWindow", "Start", 0));
        pushButton_3->setText(QApplication::translate("MainWindow", "Stop", 0));
        pushButton_4->setText(QApplication::translate("MainWindow", "Reset", 0));
        menu_File->setTitle(QApplication::translate("MainWindow", "&File", 0));
        menu_View->setTitle(QApplication::translate("MainWindow", "&View", 0));
        menu_Curves->setTitle(QApplication::translate("MainWindow", "&Curves", 0));
        menu_Heatmaps->setTitle(QApplication::translate("MainWindow", "&Heatmaps", 0));
        menu_Help->setTitle(QApplication::translate("MainWindow", "&Help", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
