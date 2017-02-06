/********************************************************************************
** Form generated from reading UI file 'qmsmsettings.ui'
**
** Created: Mon Feb 6 15:06:30 2017
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
        tabWidget->setGeometry(QRect(30, 20, 331, 301));
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
        horizontalSlider_dataHeatmap->setMaximum(500);
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

        tabWidget->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(QMSMSettings);
    } // setupUi

    void retranslateUi(QWidget *QMSMSettings)
    {
        QMSMSettings->setWindowTitle(QApplication::translate("QMSMSettings", "Form", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("QMSMSettings", "Resource", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("QMSMSettings", "Plot:", 0, QApplication::UnicodeUTF8));
        label_dataPlot->setText(QApplication::translate("QMSMSettings", "250 ms", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("QMSMSettings", "Heatmap:", 0, QApplication::UnicodeUTF8));
        label_dataHeatmap->setText(QApplication::translate("QMSMSettings", "0 ms", 0, QApplication::UnicodeUTF8));
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
