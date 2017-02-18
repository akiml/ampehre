/********************************************************************************
** Form generated from reading UI file 'qmsmsettings.ui'
**
** Created: Sat Feb 18 19:01:12 2017
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QMSMSETTINGS_H
#define UI_QMSMSETTINGS_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSlider>
#include <QtGui/QTabWidget>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QMSMSettings
{
public:
    QTabWidget *tabWidget;
    QWidget *tab;
    QLabel *label_4;
    QLabel *label_6;
    QLabel *label_7;
    QLabel *label_8;
    QLabel *label_9;
    QLabel *label_10;
    QLabel *label_11;
    QLabel *label_12;
    QLabel *label_13;
    QLabel *label_14;
    QLineEdit *lineEdit_cpu;
    QLineEdit *lineEdit_gpu;
    QLineEdit *lineEdit_fpga;
    QLineEdit *lineEdit_mic;
    QLineEdit *lineEdit_sys;
    QWidget *tab_3;
    QLabel *label;
    QSlider *horizontalSlider_dataPlot;
    QLabel *label_dataPlot;
    QWidget *tab_2;
    QLabel *label_3;
    QSlider *horizontalSlider_guiRate;
    QLabel *label_guiRate;
    QPushButton *pushButton_start;
    QPushButton *pushButton_stop;
    QPushButton *pushButton_reset;
    QPushButton *pushButton_save;
    QLabel *label_5;

    void setupUi(QWidget *QMSMSettings)
    {
        if (QMSMSettings->objectName().isEmpty())
            QMSMSettings->setObjectName(QString::fromUtf8("QMSMSettings"));
        QMSMSettings->resize(400, 640);
        tabWidget = new QTabWidget(QMSMSettings);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setGeometry(QRect(30, 10, 331, 301));
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        label_4 = new QLabel(tab);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(80, 20, 51, 21));
        label_6 = new QLabel(tab);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(80, 70, 51, 21));
        label_7 = new QLabel(tab);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(80, 120, 51, 21));
        label_8 = new QLabel(tab);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setGeometry(QRect(80, 170, 41, 21));
        label_9 = new QLabel(tab);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setGeometry(QRect(80, 220, 61, 21));
        label_10 = new QLabel(tab);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        label_10->setGeometry(QRect(260, 20, 21, 21));
        label_11 = new QLabel(tab);
        label_11->setObjectName(QString::fromUtf8("label_11"));
        label_11->setGeometry(QRect(260, 70, 21, 21));
        label_12 = new QLabel(tab);
        label_12->setObjectName(QString::fromUtf8("label_12"));
        label_12->setGeometry(QRect(260, 120, 21, 21));
        label_13 = new QLabel(tab);
        label_13->setObjectName(QString::fromUtf8("label_13"));
        label_13->setGeometry(QRect(260, 170, 21, 21));
        label_14 = new QLabel(tab);
        label_14->setObjectName(QString::fromUtf8("label_14"));
        label_14->setGeometry(QRect(260, 220, 21, 21));
        lineEdit_cpu = new QLineEdit(tab);
        lineEdit_cpu->setObjectName(QString::fromUtf8("lineEdit_cpu"));
        lineEdit_cpu->setGeometry(QRect(140, 10, 113, 33));
        lineEdit_gpu = new QLineEdit(tab);
        lineEdit_gpu->setObjectName(QString::fromUtf8("lineEdit_gpu"));
        lineEdit_gpu->setGeometry(QRect(140, 60, 113, 33));
        lineEdit_fpga = new QLineEdit(tab);
        lineEdit_fpga->setObjectName(QString::fromUtf8("lineEdit_fpga"));
        lineEdit_fpga->setGeometry(QRect(140, 110, 113, 33));
        lineEdit_mic = new QLineEdit(tab);
        lineEdit_mic->setObjectName(QString::fromUtf8("lineEdit_mic"));
        lineEdit_mic->setGeometry(QRect(140, 160, 113, 33));
        lineEdit_sys = new QLineEdit(tab);
        lineEdit_sys->setObjectName(QString::fromUtf8("lineEdit_sys"));
        lineEdit_sys->setGeometry(QRect(140, 210, 113, 33));
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
        horizontalSlider_guiRate->setMaximum(1000);
        horizontalSlider_guiRate->setSingleStep(10);
        horizontalSlider_guiRate->setPageStep(20);
        horizontalSlider_guiRate->setSliderPosition(250);
        horizontalSlider_guiRate->setOrientation(Qt::Horizontal);
        label_guiRate = new QLabel(tab_2);
        label_guiRate->setObjectName(QString::fromUtf8("label_guiRate"));
        label_guiRate->setGeometry(QRect(240, 20, 61, 21));
        tabWidget->addTab(tab_2, QString());
        pushButton_start = new QPushButton(QMSMSettings);
        pushButton_start->setObjectName(QString::fromUtf8("pushButton_start"));
        pushButton_start->setGeometry(QRect(30, 350, 95, 31));
        pushButton_stop = new QPushButton(QMSMSettings);
        pushButton_stop->setObjectName(QString::fromUtf8("pushButton_stop"));
        pushButton_stop->setGeometry(QRect(150, 350, 95, 31));
        pushButton_reset = new QPushButton(QMSMSettings);
        pushButton_reset->setObjectName(QString::fromUtf8("pushButton_reset"));
        pushButton_reset->setGeometry(QRect(270, 350, 95, 31));
        pushButton_save = new QPushButton(QMSMSettings);
        pushButton_save->setObjectName(QString::fromUtf8("pushButton_save"));
        pushButton_save->setGeometry(QRect(30, 420, 95, 31));
        label_5 = new QLabel(QMSMSettings);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(150, 430, 131, 21));

        retranslateUi(QMSMSettings);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(QMSMSettings);
    } // setupUi

    void retranslateUi(QWidget *QMSMSettings)
    {
        QMSMSettings->setWindowTitle(QApplication::translate("QMSMSettings", "Form", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("QMSMSettings", "CPU:", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("QMSMSettings", "GPU:", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("QMSMSettings", "FPGA:", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("QMSMSettings", "MIC:", 0, QApplication::UnicodeUTF8));
        label_9->setText(QApplication::translate("QMSMSettings", "System:", 0, QApplication::UnicodeUTF8));
        label_10->setText(QApplication::translate("QMSMSettings", "ms", 0, QApplication::UnicodeUTF8));
        label_11->setText(QApplication::translate("QMSMSettings", "ms", 0, QApplication::UnicodeUTF8));
        label_12->setText(QApplication::translate("QMSMSettings", "ms", 0, QApplication::UnicodeUTF8));
        label_13->setText(QApplication::translate("QMSMSettings", "ms", 0, QApplication::UnicodeUTF8));
        label_14->setText(QApplication::translate("QMSMSettings", "ms", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("QMSMSettings", "Resource", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("QMSMSettings", "Plot:", 0, QApplication::UnicodeUTF8));
        label_dataPlot->setText(QApplication::translate("QMSMSettings", "250 ms", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QApplication::translate("QMSMSettings", "Data", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("QMSMSettings", "redraw:", 0, QApplication::UnicodeUTF8));
        label_guiRate->setText(QApplication::translate("QMSMSettings", "250 ms", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("QMSMSettings", "GUI", 0, QApplication::UnicodeUTF8));
        pushButton_start->setText(QApplication::translate("QMSMSettings", "Start", 0, QApplication::UnicodeUTF8));
        pushButton_stop->setText(QApplication::translate("QMSMSettings", "Stop", 0, QApplication::UnicodeUTF8));
        pushButton_reset->setText(QApplication::translate("QMSMSettings", "Reset", 0, QApplication::UnicodeUTF8));
        pushButton_save->setText(QApplication::translate("QMSMSettings", "Save", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("QMSMSettings", "save results to csv", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class QMSMSettings: public Ui_QMSMSettings {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QMSMSETTINGS_H
