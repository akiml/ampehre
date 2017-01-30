/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Mon Jan 30 10:45:49 2017
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
#include <QtGui/QLabel>
#include <QtGui/QMainWindow>
#include <QtGui/QMdiArea>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QSlider>
#include <QtGui/QStatusBar>
#include <QtGui/QTabWidget>
#include <QtGui/QToolBar>
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
    QWidget *centralWidget;
    QWidget *settingsWidget;
    QTabWidget *tabWidget;
    QWidget *tab;
    QWidget *tab_3;
    QLabel *label;
    QSlider *horizontalSlider_dataPlot;
    QLabel *label_dataPlot;
    QLabel *label_2;
    QSlider *horizontalSlider_dataHeatmap;
    QLabel *label_dataHeatmap;
    QWidget *tab_2;
    QLabel *label_3;
    QSlider *horizontalSlider_guiRate;
    QLabel *label_guiRate;
    QPushButton *pushButton_save;
    QLabel *label_5;
    QPushButton *pushButton_start;
    QPushButton *pushButton_stop;
    QPushButton *pushButton_reset;
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
        MainWindow->resize(1050, 650);
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
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        settingsWidget = new QWidget(centralWidget);
        settingsWidget->setObjectName(QString::fromUtf8("settingsWidget"));
        settingsWidget->setGeometry(QRect(0, 0, 331, 581));
        settingsWidget->setMinimumSize(QSize(320, 560));
        tabWidget = new QTabWidget(settingsWidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setGeometry(QRect(10, 10, 301, 301));
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        tabWidget->addTab(tab, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName(QString::fromUtf8("tab_3"));
        label = new QLabel(tab_3);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(0, 20, 41, 21));
        horizontalSlider_dataPlot = new QSlider(tab_3);
        horizontalSlider_dataPlot->setObjectName(QString::fromUtf8("horizontalSlider_dataPlot"));
        horizontalSlider_dataPlot->setGeometry(QRect(70, 20, 160, 18));
        horizontalSlider_dataPlot->setMinimum(10);
        horizontalSlider_dataPlot->setMaximum(500);
        horizontalSlider_dataPlot->setSingleStep(10);
        horizontalSlider_dataPlot->setPageStep(20);
        horizontalSlider_dataPlot->setSliderPosition(250);
        horizontalSlider_dataPlot->setOrientation(Qt::Horizontal);
        label_dataPlot = new QLabel(tab_3);
        label_dataPlot->setObjectName(QString::fromUtf8("label_dataPlot"));
        label_dataPlot->setGeometry(QRect(240, 20, 61, 21));
        label_2 = new QLabel(tab_3);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(0, 70, 66, 21));
        horizontalSlider_dataHeatmap = new QSlider(tab_3);
        horizontalSlider_dataHeatmap->setObjectName(QString::fromUtf8("horizontalSlider_dataHeatmap"));
        horizontalSlider_dataHeatmap->setGeometry(QRect(70, 70, 160, 18));
        horizontalSlider_dataHeatmap->setMinimum(10);
        horizontalSlider_dataHeatmap->setMaximum(200);
        horizontalSlider_dataHeatmap->setSingleStep(10);
        horizontalSlider_dataHeatmap->setPageStep(20);
        horizontalSlider_dataHeatmap->setOrientation(Qt::Horizontal);
        label_dataHeatmap = new QLabel(tab_3);
        label_dataHeatmap->setObjectName(QString::fromUtf8("label_dataHeatmap"));
        label_dataHeatmap->setGeometry(QRect(240, 70, 61, 21));
        tabWidget->addTab(tab_3, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        label_3 = new QLabel(tab_2);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(0, 20, 61, 21));
        horizontalSlider_guiRate = new QSlider(tab_2);
        horizontalSlider_guiRate->setObjectName(QString::fromUtf8("horizontalSlider_guiRate"));
        horizontalSlider_guiRate->setGeometry(QRect(70, 20, 160, 18));
        horizontalSlider_guiRate->setMinimum(10);
        horizontalSlider_guiRate->setMaximum(500);
        horizontalSlider_guiRate->setSingleStep(10);
        horizontalSlider_guiRate->setPageStep(20);
        horizontalSlider_guiRate->setSliderPosition(250);
        horizontalSlider_guiRate->setOrientation(Qt::Horizontal);
        label_guiRate = new QLabel(tab_2);
        label_guiRate->setObjectName(QString::fromUtf8("label_guiRate"));
        label_guiRate->setGeometry(QRect(240, 20, 61, 21));
        tabWidget->addTab(tab_2, QString());
        pushButton_save = new QPushButton(settingsWidget);
        pushButton_save->setObjectName(QString::fromUtf8("pushButton_save"));
        pushButton_save->setGeometry(QRect(20, 520, 95, 31));
        label_5 = new QLabel(settingsWidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(130, 520, 131, 21));
        pushButton_start = new QPushButton(settingsWidget);
        pushButton_start->setObjectName(QString::fromUtf8("pushButton_start"));
        pushButton_start->setGeometry(QRect(20, 370, 95, 31));
        pushButton_stop = new QPushButton(settingsWidget);
        pushButton_stop->setObjectName(QString::fromUtf8("pushButton_stop"));
        pushButton_stop->setGeometry(QRect(200, 370, 95, 31));
        pushButton_reset = new QPushButton(settingsWidget);
        pushButton_reset->setObjectName(QString::fromUtf8("pushButton_reset"));
        pushButton_reset->setGeometry(QRect(200, 420, 95, 31));
        mdiArea = new QMdiArea(centralWidget);
        mdiArea->setObjectName(QString::fromUtf8("mdiArea"));
        mdiArea->setGeometry(QRect(330, 0, 1973, 700));
        MainWindow->setCentralWidget(centralWidget);
        settingsWidget->raise();
        mdiArea->raise();
        tabWidget->raise();
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1050, 27));
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
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MSMonitor v0.8.0", 0, QApplication::UnicodeUTF8));
        action_Quit->setText(QApplication::translate("MainWindow", "&Quit", 0, QApplication::UnicodeUTF8));
        action_Quit->setShortcut(QApplication::translate("MainWindow", "Ctrl+S", 0, QApplication::UnicodeUTF8));
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
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("MainWindow", "Resource", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MainWindow", "Plot:", 0, QApplication::UnicodeUTF8));
        label_dataPlot->setText(QApplication::translate("MainWindow", "250 ms", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("MainWindow", "Heatmap:", 0, QApplication::UnicodeUTF8));
        label_dataHeatmap->setText(QApplication::translate("MainWindow", "0 ms", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QApplication::translate("MainWindow", "Data", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("MainWindow", "redraw:", 0, QApplication::UnicodeUTF8));
        label_guiRate->setText(QApplication::translate("MainWindow", "250 ms", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("MainWindow", "GUI", 0, QApplication::UnicodeUTF8));
        pushButton_save->setText(QApplication::translate("MainWindow", "Save", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("MainWindow", "save results to csv", 0, QApplication::UnicodeUTF8));
        pushButton_start->setText(QApplication::translate("MainWindow", "Start", 0, QApplication::UnicodeUTF8));
        pushButton_stop->setText(QApplication::translate("MainWindow", "Stop", 0, QApplication::UnicodeUTF8));
        pushButton_reset->setText(QApplication::translate("MainWindow", "Reset", 0, QApplication::UnicodeUTF8));
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
