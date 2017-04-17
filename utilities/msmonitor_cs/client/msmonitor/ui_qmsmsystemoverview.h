/********************************************************************************
** Form generated from reading UI file 'qmsmsystemoverview.ui'
**
** Created: Mon Apr 17 12:36:00 2017
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QMSMSYSTEMOVERVIEW_H
#define UI_QMSMSYSTEMOVERVIEW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QSpacerItem>
#include <QtGui/QTableWidget>
#include <QtGui/QTextEdit>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QMSMSystemOverview
{
public:
    QVBoxLayout *verticalLayout_5;
    QGroupBox *groupBox_cpu;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_6;
    QGroupBox *groupBox_cpu_util;
    QHBoxLayout *horizontalLayout;
    QTextEdit *textEdit_cpu_util;
    QSpacerItem *horizontalSpacer_2;
    QGroupBox *groupBox_cpu_temp;
    QHBoxLayout *horizontalLayout_3;
    QTextEdit *textEdit_cpu_temp0;
    QTextEdit *textEdit_cpu_temp1;
    QGroupBox *groupBox_gpu;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout_4;
    QGroupBox *groupBox_gpu_util;
    QHBoxLayout *horizontalLayout_9;
    QTextEdit *textEdit_gpu_core;
    QTextEdit *textEdit_gpu_mem;
    QGroupBox *groupBox_gpu_temp;
    QHBoxLayout *horizontalLayout_10;
    QTextEdit *textEdit_gpu_temp;
    QSpacerItem *horizontalSpacer_4;
    QTableWidget *tableWidget_gpu;
    QGroupBox *groupBox_fpga;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_5;
    QGroupBox *groupBox_fpga_util;
    QHBoxLayout *horizontalLayout_12;
    QTextEdit *textEdit_fpga_util;
    QSpacerItem *horizontalSpacer_6;
    QGroupBox *groupBox_fpga_temp;
    QHBoxLayout *horizontalLayout_11;
    QTextEdit *textEdit_fpga_comp;
    QTextEdit *textEdit_fpga_inter;
    QTableWidget *tableWidget_fpga;
    QGroupBox *groupBox_mic;
    QVBoxLayout *verticalLayout_4;
    QHBoxLayout *horizontalLayout_8;
    QGroupBox *groupBox_mic_util;
    QHBoxLayout *horizontalLayout_2;
    QTextEdit *textEdit_mic_util;
    QSpacerItem *horizontalSpacer;
    QGroupBox *groupBox_mic_temp;
    QHBoxLayout *horizontalLayout_7;
    QTextEdit *textEdit_mic_temp;
    QSpacerItem *horizontalSpacer_3;

    void setupUi(QWidget *QMSMSystemOverview)
    {
        if (QMSMSystemOverview->objectName().isEmpty())
            QMSMSystemOverview->setObjectName(QString::fromUtf8("QMSMSystemOverview"));
        QMSMSystemOverview->resize(960, 770);
        verticalLayout_5 = new QVBoxLayout(QMSMSystemOverview);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        groupBox_cpu = new QGroupBox(QMSMSystemOverview);
        groupBox_cpu->setObjectName(QString::fromUtf8("groupBox_cpu"));
        verticalLayout_2 = new QVBoxLayout(groupBox_cpu);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        groupBox_cpu_util = new QGroupBox(groupBox_cpu);
        groupBox_cpu_util->setObjectName(QString::fromUtf8("groupBox_cpu_util"));
        horizontalLayout = new QHBoxLayout(groupBox_cpu_util);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        textEdit_cpu_util = new QTextEdit(groupBox_cpu_util);
        textEdit_cpu_util->setObjectName(QString::fromUtf8("textEdit_cpu_util"));
        textEdit_cpu_util->setReadOnly(true);

        horizontalLayout->addWidget(textEdit_cpu_util);

        horizontalSpacer_2 = new QSpacerItem(100, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);


        horizontalLayout_6->addWidget(groupBox_cpu_util);

        groupBox_cpu_temp = new QGroupBox(groupBox_cpu);
        groupBox_cpu_temp->setObjectName(QString::fromUtf8("groupBox_cpu_temp"));
        horizontalLayout_3 = new QHBoxLayout(groupBox_cpu_temp);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        textEdit_cpu_temp0 = new QTextEdit(groupBox_cpu_temp);
        textEdit_cpu_temp0->setObjectName(QString::fromUtf8("textEdit_cpu_temp0"));
        textEdit_cpu_temp0->setReadOnly(true);

        horizontalLayout_3->addWidget(textEdit_cpu_temp0);

        textEdit_cpu_temp1 = new QTextEdit(groupBox_cpu_temp);
        textEdit_cpu_temp1->setObjectName(QString::fromUtf8("textEdit_cpu_temp1"));
        textEdit_cpu_temp1->setReadOnly(true);

        horizontalLayout_3->addWidget(textEdit_cpu_temp1);


        horizontalLayout_6->addWidget(groupBox_cpu_temp);


        verticalLayout_2->addLayout(horizontalLayout_6);


        verticalLayout_5->addWidget(groupBox_cpu);

        groupBox_gpu = new QGroupBox(QMSMSystemOverview);
        groupBox_gpu->setObjectName(QString::fromUtf8("groupBox_gpu"));
        verticalLayout_3 = new QVBoxLayout(groupBox_gpu);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        groupBox_gpu_util = new QGroupBox(groupBox_gpu);
        groupBox_gpu_util->setObjectName(QString::fromUtf8("groupBox_gpu_util"));
        horizontalLayout_9 = new QHBoxLayout(groupBox_gpu_util);
        horizontalLayout_9->setObjectName(QString::fromUtf8("horizontalLayout_9"));
        textEdit_gpu_core = new QTextEdit(groupBox_gpu_util);
        textEdit_gpu_core->setObjectName(QString::fromUtf8("textEdit_gpu_core"));
        textEdit_gpu_core->setReadOnly(true);

        horizontalLayout_9->addWidget(textEdit_gpu_core);

        textEdit_gpu_mem = new QTextEdit(groupBox_gpu_util);
        textEdit_gpu_mem->setObjectName(QString::fromUtf8("textEdit_gpu_mem"));
        textEdit_gpu_mem->setReadOnly(true);

        horizontalLayout_9->addWidget(textEdit_gpu_mem);


        horizontalLayout_4->addWidget(groupBox_gpu_util);

        groupBox_gpu_temp = new QGroupBox(groupBox_gpu);
        groupBox_gpu_temp->setObjectName(QString::fromUtf8("groupBox_gpu_temp"));
        horizontalLayout_10 = new QHBoxLayout(groupBox_gpu_temp);
        horizontalLayout_10->setObjectName(QString::fromUtf8("horizontalLayout_10"));
        textEdit_gpu_temp = new QTextEdit(groupBox_gpu_temp);
        textEdit_gpu_temp->setObjectName(QString::fromUtf8("textEdit_gpu_temp"));
        textEdit_gpu_temp->setReadOnly(true);

        horizontalLayout_10->addWidget(textEdit_gpu_temp);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_10->addItem(horizontalSpacer_4);


        horizontalLayout_4->addWidget(groupBox_gpu_temp);


        verticalLayout_3->addLayout(horizontalLayout_4);

        tableWidget_gpu = new QTableWidget(groupBox_gpu);
        if (tableWidget_gpu->columnCount() < 2)
            tableWidget_gpu->setColumnCount(2);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tableWidget_gpu->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tableWidget_gpu->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        if (tableWidget_gpu->rowCount() < 5)
            tableWidget_gpu->setRowCount(5);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        tableWidget_gpu->setVerticalHeaderItem(0, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        tableWidget_gpu->setVerticalHeaderItem(1, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        tableWidget_gpu->setVerticalHeaderItem(2, __qtablewidgetitem4);
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        tableWidget_gpu->setVerticalHeaderItem(3, __qtablewidgetitem5);
        QTableWidgetItem *__qtablewidgetitem6 = new QTableWidgetItem();
        tableWidget_gpu->setVerticalHeaderItem(4, __qtablewidgetitem6);
        tableWidget_gpu->setObjectName(QString::fromUtf8("tableWidget_gpu"));

        verticalLayout_3->addWidget(tableWidget_gpu);


        verticalLayout_5->addWidget(groupBox_gpu);

        groupBox_fpga = new QGroupBox(QMSMSystemOverview);
        groupBox_fpga->setObjectName(QString::fromUtf8("groupBox_fpga"));
        verticalLayout = new QVBoxLayout(groupBox_fpga);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        groupBox_fpga_util = new QGroupBox(groupBox_fpga);
        groupBox_fpga_util->setObjectName(QString::fromUtf8("groupBox_fpga_util"));
        horizontalLayout_12 = new QHBoxLayout(groupBox_fpga_util);
        horizontalLayout_12->setObjectName(QString::fromUtf8("horizontalLayout_12"));
        textEdit_fpga_util = new QTextEdit(groupBox_fpga_util);
        textEdit_fpga_util->setObjectName(QString::fromUtf8("textEdit_fpga_util"));
        textEdit_fpga_util->setReadOnly(true);

        horizontalLayout_12->addWidget(textEdit_fpga_util);


        horizontalLayout_5->addWidget(groupBox_fpga_util);

        horizontalSpacer_6 = new QSpacerItem(100, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_6);

        groupBox_fpga_temp = new QGroupBox(groupBox_fpga);
        groupBox_fpga_temp->setObjectName(QString::fromUtf8("groupBox_fpga_temp"));
        horizontalLayout_11 = new QHBoxLayout(groupBox_fpga_temp);
        horizontalLayout_11->setObjectName(QString::fromUtf8("horizontalLayout_11"));
        textEdit_fpga_comp = new QTextEdit(groupBox_fpga_temp);
        textEdit_fpga_comp->setObjectName(QString::fromUtf8("textEdit_fpga_comp"));
        textEdit_fpga_comp->setReadOnly(true);

        horizontalLayout_11->addWidget(textEdit_fpga_comp);

        textEdit_fpga_inter = new QTextEdit(groupBox_fpga_temp);
        textEdit_fpga_inter->setObjectName(QString::fromUtf8("textEdit_fpga_inter"));
        textEdit_fpga_inter->setReadOnly(true);

        horizontalLayout_11->addWidget(textEdit_fpga_inter);


        horizontalLayout_5->addWidget(groupBox_fpga_temp);


        verticalLayout->addLayout(horizontalLayout_5);

        tableWidget_fpga = new QTableWidget(groupBox_fpga);
        if (tableWidget_fpga->columnCount() < 3)
            tableWidget_fpga->setColumnCount(3);
        QTableWidgetItem *__qtablewidgetitem7 = new QTableWidgetItem();
        tableWidget_fpga->setHorizontalHeaderItem(0, __qtablewidgetitem7);
        QTableWidgetItem *__qtablewidgetitem8 = new QTableWidgetItem();
        tableWidget_fpga->setHorizontalHeaderItem(1, __qtablewidgetitem8);
        QTableWidgetItem *__qtablewidgetitem9 = new QTableWidgetItem();
        tableWidget_fpga->setHorizontalHeaderItem(2, __qtablewidgetitem9);
        if (tableWidget_fpga->rowCount() < 1)
            tableWidget_fpga->setRowCount(1);
        QTableWidgetItem *__qtablewidgetitem10 = new QTableWidgetItem();
        tableWidget_fpga->setVerticalHeaderItem(0, __qtablewidgetitem10);
        tableWidget_fpga->setObjectName(QString::fromUtf8("tableWidget_fpga"));

        verticalLayout->addWidget(tableWidget_fpga);


        verticalLayout_5->addWidget(groupBox_fpga);

        groupBox_mic = new QGroupBox(QMSMSystemOverview);
        groupBox_mic->setObjectName(QString::fromUtf8("groupBox_mic"));
        verticalLayout_4 = new QVBoxLayout(groupBox_mic);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        groupBox_mic_util = new QGroupBox(groupBox_mic);
        groupBox_mic_util->setObjectName(QString::fromUtf8("groupBox_mic_util"));
        horizontalLayout_2 = new QHBoxLayout(groupBox_mic_util);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        textEdit_mic_util = new QTextEdit(groupBox_mic_util);
        textEdit_mic_util->setObjectName(QString::fromUtf8("textEdit_mic_util"));
        textEdit_mic_util->setReadOnly(true);

        horizontalLayout_2->addWidget(textEdit_mic_util);

        horizontalSpacer = new QSpacerItem(50, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);


        horizontalLayout_8->addWidget(groupBox_mic_util);

        groupBox_mic_temp = new QGroupBox(groupBox_mic);
        groupBox_mic_temp->setObjectName(QString::fromUtf8("groupBox_mic_temp"));
        horizontalLayout_7 = new QHBoxLayout(groupBox_mic_temp);
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        textEdit_mic_temp = new QTextEdit(groupBox_mic_temp);
        textEdit_mic_temp->setObjectName(QString::fromUtf8("textEdit_mic_temp"));
        textEdit_mic_temp->setReadOnly(true);

        horizontalLayout_7->addWidget(textEdit_mic_temp);

        horizontalSpacer_3 = new QSpacerItem(100, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_7->addItem(horizontalSpacer_3);


        horizontalLayout_8->addWidget(groupBox_mic_temp);


        verticalLayout_4->addLayout(horizontalLayout_8);


        verticalLayout_5->addWidget(groupBox_mic);


        retranslateUi(QMSMSystemOverview);

        QMetaObject::connectSlotsByName(QMSMSystemOverview);
    } // setupUi

    void retranslateUi(QWidget *QMSMSystemOverview)
    {
        QMSMSystemOverview->setWindowTitle(QApplication::translate("QMSMSystemOverview", "Form", 0, QApplication::UnicodeUTF8));
        groupBox_cpu->setTitle(QApplication::translate("QMSMSystemOverview", "CPU", 0, QApplication::UnicodeUTF8));
        groupBox_cpu_util->setTitle(QApplication::translate("QMSMSystemOverview", "Utilization", 0, QApplication::UnicodeUTF8));
        groupBox_cpu_temp->setTitle(QApplication::translate("QMSMSystemOverview", "Temperature", 0, QApplication::UnicodeUTF8));
        textEdit_cpu_temp0->setHtml(QApplication::translate("QMSMSystemOverview", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Cantarell'; font-size:11pt; font-weight:400; font-style:normal;\">\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">CPU 0</p></body></html>", 0, QApplication::UnicodeUTF8));
        textEdit_cpu_temp1->setHtml(QApplication::translate("QMSMSystemOverview", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Cantarell'; font-size:11pt; font-weight:400; font-style:normal;\">\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">CPU 1</p></body></html>", 0, QApplication::UnicodeUTF8));
        groupBox_gpu->setTitle(QApplication::translate("QMSMSystemOverview", "GPU", 0, QApplication::UnicodeUTF8));
        groupBox_gpu_util->setTitle(QApplication::translate("QMSMSystemOverview", "Utilization", 0, QApplication::UnicodeUTF8));
        textEdit_gpu_core->setHtml(QApplication::translate("QMSMSystemOverview", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Cantarell'; font-size:11pt; font-weight:400; font-style:normal;\">\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Core</p></body></html>", 0, QApplication::UnicodeUTF8));
        textEdit_gpu_mem->setHtml(QApplication::translate("QMSMSystemOverview", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Cantarell'; font-size:11pt; font-weight:400; font-style:normal;\">\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Memory</p></body></html>", 0, QApplication::UnicodeUTF8));
        groupBox_gpu_temp->setTitle(QApplication::translate("QMSMSystemOverview", "Temperature", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem = tableWidget_gpu->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("QMSMSystemOverview", "PID", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem1 = tableWidget_gpu->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QApplication::translate("QMSMSystemOverview", "Name", 0, QApplication::UnicodeUTF8));
        groupBox_fpga->setTitle(QApplication::translate("QMSMSystemOverview", "FPGA", 0, QApplication::UnicodeUTF8));
        groupBox_fpga_util->setTitle(QApplication::translate("QMSMSystemOverview", "Utilization", 0, QApplication::UnicodeUTF8));
        groupBox_fpga_temp->setTitle(QApplication::translate("QMSMSystemOverview", "Temperature", 0, QApplication::UnicodeUTF8));
        textEdit_fpga_comp->setHtml(QApplication::translate("QMSMSystemOverview", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Cantarell'; font-size:11pt; font-weight:400; font-style:normal;\">\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Compute</p></body></html>", 0, QApplication::UnicodeUTF8));
        textEdit_fpga_inter->setHtml(QApplication::translate("QMSMSystemOverview", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Cantarell'; font-size:11pt; font-weight:400; font-style:normal;\">\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Interface</p></body></html>", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem2 = tableWidget_fpga->horizontalHeaderItem(0);
        ___qtablewidgetitem2->setText(QApplication::translate("QMSMSystemOverview", "PID", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem3 = tableWidget_fpga->horizontalHeaderItem(1);
        ___qtablewidgetitem3->setText(QApplication::translate("QMSMSystemOverview", "Name", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem4 = tableWidget_fpga->horizontalHeaderItem(2);
        ___qtablewidgetitem4->setText(QApplication::translate("QMSMSystemOverview", "User", 0, QApplication::UnicodeUTF8));
        groupBox_mic->setTitle(QApplication::translate("QMSMSystemOverview", "MIC", 0, QApplication::UnicodeUTF8));
        groupBox_mic_util->setTitle(QApplication::translate("QMSMSystemOverview", "Utilization", 0, QApplication::UnicodeUTF8));
        groupBox_mic_temp->setTitle(QApplication::translate("QMSMSystemOverview", "Temperature", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class QMSMSystemOverview: public Ui_QMSMSystemOverview {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QMSMSYSTEMOVERVIEW_H
