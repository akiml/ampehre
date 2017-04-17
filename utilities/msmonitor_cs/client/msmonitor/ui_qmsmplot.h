/********************************************************************************
** Form generated from reading UI file 'qmsmplot.ui'
**
** Created: Mon Apr 17 12:36:00 2017
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QMSMPLOT_H
#define UI_QMSMPLOT_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include "qwt_plot.h"

QT_BEGIN_NAMESPACE

class Ui_QMSMplot
{
public:
    QVBoxLayout *verticalLayoutMeasurement;
    QwtPlot *qwtPlot;
    QHBoxLayout *horizontalLayoutButtons;
    QHBoxLayout *horizontalLayoutSpinBox;
    QLabel *label;
    QSpinBox *spinBox;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButtonScreenshot;
    QPushButton *pushButtonCSV;

    void setupUi(QWidget *QMSMplot)
    {
        if (QMSMplot->objectName().isEmpty())
            QMSMplot->setObjectName(QString::fromUtf8("QMSMplot"));
        QMSMplot->resize(640, 480);
        QMSMplot->setMinimumSize(QSize(640, 480));
        verticalLayoutMeasurement = new QVBoxLayout(QMSMplot);
        verticalLayoutMeasurement->setObjectName(QString::fromUtf8("verticalLayoutMeasurement"));
        qwtPlot = new QwtPlot(QMSMplot);
        qwtPlot->setObjectName(QString::fromUtf8("qwtPlot"));

        verticalLayoutMeasurement->addWidget(qwtPlot);

        horizontalLayoutButtons = new QHBoxLayout();
        horizontalLayoutButtons->setObjectName(QString::fromUtf8("horizontalLayoutButtons"));
        horizontalLayoutSpinBox = new QHBoxLayout();
        horizontalLayoutSpinBox->setObjectName(QString::fromUtf8("horizontalLayoutSpinBox"));
        label = new QLabel(QMSMplot);
        label->setObjectName(QString::fromUtf8("label"));
        QFont font;
        font.setPointSize(11);
        font.setBold(false);
        font.setWeight(50);
        label->setFont(font);

        horizontalLayoutSpinBox->addWidget(label);

        spinBox = new QSpinBox(QMSMplot);
        spinBox->setObjectName(QString::fromUtf8("spinBox"));
        spinBox->setMinimum(1);
        spinBox->setMaximum(10);

        horizontalLayoutSpinBox->addWidget(spinBox);

        horizontalSpacer = new QSpacerItem(70, 20, QSizePolicy::Preferred, QSizePolicy::Minimum);

        horizontalLayoutSpinBox->addItem(horizontalSpacer);

        pushButtonScreenshot = new QPushButton(QMSMplot);
        pushButtonScreenshot->setObjectName(QString::fromUtf8("pushButtonScreenshot"));

        horizontalLayoutSpinBox->addWidget(pushButtonScreenshot);

        pushButtonCSV = new QPushButton(QMSMplot);
        pushButtonCSV->setObjectName(QString::fromUtf8("pushButtonCSV"));

        horizontalLayoutSpinBox->addWidget(pushButtonCSV);


        horizontalLayoutButtons->addLayout(horizontalLayoutSpinBox);


        verticalLayoutMeasurement->addLayout(horizontalLayoutButtons);


        retranslateUi(QMSMplot);

        QMetaObject::connectSlotsByName(QMSMplot);
    } // setupUi

    void retranslateUi(QWidget *QMSMplot)
    {
        QMSMplot->setWindowTitle(QString());
        label->setText(QApplication::translate("QMSMplot", "Line width:", 0, QApplication::UnicodeUTF8));
        pushButtonScreenshot->setText(QApplication::translate("QMSMplot", "Screenshot", 0, QApplication::UnicodeUTF8));
        pushButtonCSV->setText(QApplication::translate("QMSMplot", "Export to CSV", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class QMSMplot: public Ui_QMSMplot {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QMSMPLOT_H
