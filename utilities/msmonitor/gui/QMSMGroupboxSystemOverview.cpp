/*
 * QMSMGroupboxSystemOverview.cpp
 * 
 * Copyright (C) 2015, Achim Lösch <achim.loesch@upb.de>, Christoph Knorr <cknorr@mail.uni-paderborn.de>
 * All rights reserved.
 * 
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
 * 
 * encoding: UTF-8
 * tab size: 4
 * 
 * author: Christoph Knorr (cknorr@mail.uni-paderborn.de)
 * created: 03/11/16
 * version: 0.7.4 - add query for currently active processes to libmeasure and system overview gui to msmonitor
 */

#include "QMSMGroupboxSystemOverview.hpp"

namespace Ui {
	QMSMGroupboxSystemOverview::QMSMGroupboxSystemOverview(QWidget* parent, const std::string &resourceName, const std::string &utilization1, const std::string &utilization2,
															const std::string &temperature1, const std::string &temperature2, bool withProcessTable, const std::string columnNames[], int columns, int maxProcessCount):
		QGroupBox(parent),
		mpTableWidgetProcesses(NULL),
		mHasProcessTable(withProcessTable),
		mMaxProcessCount(maxProcessCount),
		mColumnCount(columns) {
		
		setupUi();
		
		const char *parentName = parent->objectName().toStdString().c_str();
		setTitle(QApplication::translate(parentName, resourceName.c_str(), 0, QApplication::UnicodeUTF8));
		mpLabelUtilization->setText(QApplication::translate(parentName, "Utilization:", 0, QApplication::UnicodeUTF8));
		mpLabelUtilization1->setText(QApplication::translate(parentName, utilization1.c_str(), 0, QApplication::UnicodeUTF8));
		mpLabelUtilization2->setText(QApplication::translate(parentName, utilization2.c_str(), 0, QApplication::UnicodeUTF8));
		mpLabelTemperature->setText(QApplication::translate(parentName, "Temperature:", 0, QApplication::UnicodeUTF8));
		mpLabelTemperature1->setText(QApplication::translate(parentName, temperature1.c_str(), 0, QApplication::UnicodeUTF8));
		mpLabelTemperature2->setText(QApplication::translate(parentName, temperature2.c_str(), 0, QApplication::UnicodeUTF8));
		
		if(columnNames != NULL && mHasProcessTable) {
			for(int i = 0; i < mColumnCount; i++) {
				QTableWidgetItem *qtablewidgetitem = mpTableWidgetProcesses->horizontalHeaderItem(i);
				qtablewidgetitem->setText(QApplication::translate(parentName, columnNames[i].c_str(), 0, QApplication::UnicodeUTF8));
			}
			mpTableWidgetProcesses->resizeColumnsToContents();
		} else if(columnNames == NULL && mHasProcessTable) {
			std::string names[] = {"PID", "Name", "User"};
			for(int i = 0; i < 3; i++) {
				QTableWidgetItem *qtablewidgetitem = mpTableWidgetProcesses->horizontalHeaderItem(i);
				qtablewidgetitem->setText(QApplication::translate(parentName, names[i].c_str(), 0, QApplication::UnicodeUTF8));
			}
			mpTableWidgetProcesses->resizeColumnsToContents();
		}
	}

	QMSMGroupboxSystemOverview::~QMSMGroupboxSystemOverview() {
		delete mpPixmapActivity;
		delete mpPixmapTemperature1;
		delete mpPixmapTemperature2;
		delete mpPixmapUtilization1;
		delete mpPixmapUtilization2;
		delete mpColorMap;
		delete mpPainter;
	}
	
	void QMSMGroupboxSystemOverview::setActivity(bool active) {
		mpPixmapActivity->fill(this, 0, 0);
		mpPainter->begin(mpPixmapActivity);
		mpPainter->setRenderHint(QPainter::Antialiasing);
		mpPainter->setPen(Qt::black);
		QRadialGradient radialGradient(16, 16, 16);
		radialGradient.setColorAt(0.0, Qt::white);
		radialGradient.setColorAt(1.0, Qt::black);
		if(active) {
			radialGradient.setColorAt(0.7, Qt::red);
		} else {
			radialGradient.setColorAt(0.7, Qt::green);
		}
		mpPainter->setBrush(radialGradient);
		mpPainter->drawEllipse(1, 1, 30, 30);
		mpPainter->end();
		mpLabelResourceActivity->setPixmap(*mpPixmapActivity);
	}
	
	void QMSMGroupboxSystemOverview::setUtilization(int id, double value) {
		QPixmap *pPixmap;
		QLabel *pLabel;
		
		if(id == 0) {
			pPixmap = mpPixmapUtilization1;
			pLabel = mpLabelPixmapUtilization1;
		} else {
			pPixmap = mpPixmapUtilization2;
			pLabel = mpLabelPixmapUtilization2;
		}
		
		pPixmap->fill(this, 0, 0);
		mpPainter->begin(pPixmap);
		mpPainter->setRenderHint(QPainter::Antialiasing);
		mpPainter->setPen(Qt::black);
		QRect rect = QRect(1, 1, 58, 33);
		mpPainter->drawRect(rect);
		mpPainter->fillRect(rect, mpColorMap->color(QwtDoubleInterval(0,100), value));
		std::ostringstream sstream;
		sstream.precision(1);
		sstream << std::fixed << value << " %";
		mpPainter->drawText(1 ,1, 58, 33, Qt::AlignCenter, sstream.str().c_str());
		mpPainter->end();
		pLabel->setPixmap(*pPixmap);
	}

	void QMSMGroupboxSystemOverview::setTemperature(int id, double minVal, double maxVal, double value) {
		QPixmap *pPixmap;
		QLabel *pLabel;
		
		if(id == 0) {
			pPixmap = mpPixmapTemperature1;
			pLabel = mpLabelPixmapTemperature1;
		} else {
			pPixmap = mpPixmapTemperature2;
			pLabel = mpLabelPixmapTemperature2;
		}
		
		pPixmap->fill(this, 0, 0);
		mpPainter->begin(pPixmap);
		mpPainter->setRenderHint(QPainter::Antialiasing);
		mpPainter->setPen(Qt::black);
		QRect rect = QRect(1, 1, 58, 33);
		mpPainter->drawRect(rect);
		mpPainter->fillRect(rect, mpColorMap->color(QwtDoubleInterval(minVal, maxVal), value));
		std::ostringstream sstream;
		sstream << value << " \u00b0C";
		mpPainter->drawText(1 ,1, 58, 33, Qt::AlignCenter, QString::fromUtf8(sstream.str().c_str()));
		mpPainter->end();
		pLabel->setPixmap(*pPixmap);
	}
	
	QTableWidget* QMSMGroupboxSystemOverview::getProcessTable() {
		return mpTableWidgetProcesses;
	}
	
	int QMSMGroupboxSystemOverview::getProcessTableColumnCount() {
		return mColumnCount;
	}

	int QMSMGroupboxSystemOverview::getProcessTableRowCount() {
		return mMaxProcessCount;
	}
	
	void QMSMGroupboxSystemOverview::setupUi() {
		QFont font;
		font.setBold(true);
		font.setWeight(75);
		this->setObjectName(QString::fromUtf8("groupboxSystemOverview"));
		if(!mHasProcessTable) {
			this->setMaximumSize(QSize(16777215, 100));
		} else {
			this->setMaximumSize(QSize(16777215, 132+mMaxProcessCount*25));
		}
		this->setFont(font);
		this->setStyleSheet(QString::fromUtf8("QGroupBox::title { \n background-color: solid;\n }"));
		this->setFlat(false);
		
		mpVerticalLayoutTopAndTable = new QVBoxLayout(this);
		mpVerticalLayoutTopAndTable->setObjectName(QString::fromUtf8("mpVerticalLayoutTopAndTable"));
		mpHorizontalLayoutTop = new QHBoxLayout();
		mpHorizontalLayoutTop->setObjectName(QString::fromUtf8("mpHorizontalLayoutTop"));
		mpLabelResourceActivity = new QLabel(this);
		mpLabelResourceActivity->setObjectName(QString::fromUtf8("mpLabelResourceActivity"));
		mpLabelResourceActivity->setMinimumSize(QSize(35, 35));
		mpLabelResourceActivity->setMaximumSize(QSize(35, 35));
		mpPixmapActivity = new QPixmap(35, 35);

		mpHorizontalLayoutTop->addWidget(mpLabelResourceActivity);

		mpHorizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

		mpHorizontalLayoutTop->addItem(mpHorizontalSpacer);

		mpLine1 = new QFrame(this);
		mpLine1->setObjectName(QString::fromUtf8("mpLine1"));
		mpLine1->setFrameShape(QFrame::VLine);
		mpLine1->setFrameShadow(QFrame::Sunken);

		mpHorizontalLayoutTop->addWidget(mpLine1);

		mpLabelUtilization = new QLabel(this);
		mpLabelUtilization->setObjectName(QString::fromUtf8("mpLabelUtilization"));
		mpLabelUtilization->setMinimumSize(QSize(0, 50));
		mpLabelUtilization->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);

		mpHorizontalLayoutTop->addWidget(mpLabelUtilization);

		mpVerticalLayoutUtilization1 = new QVBoxLayout();
		mpVerticalLayoutUtilization1->setObjectName(QString::fromUtf8("mpVerticalLayoutUtilization1"));
		mpLabelUtilization1 = new QLabel(this);
		mpLabelUtilization1->setObjectName(QString::fromUtf8("mpLabelUtilization1"));
		mpLabelUtilization1->setAlignment(Qt::AlignCenter);

		mpVerticalLayoutUtilization1->addWidget(mpLabelUtilization1);

		mpLabelPixmapUtilization1 = new QLabel(this);
		mpLabelPixmapUtilization1->setObjectName(QString::fromUtf8("mpLabelPixmapUtilization1"));
		mpLabelPixmapUtilization1->setMinimumSize(QSize(60, 35));
		mpLabelPixmapUtilization1->setMaximumSize(QSize(60, 35));
		mpPixmapUtilization1 = new QPixmap(60, 35);

		mpVerticalLayoutUtilization1->addWidget(mpLabelPixmapUtilization1);

		mpHorizontalLayoutTop->addLayout(mpVerticalLayoutUtilization1);

		mpVerticalLayoutUtilization2 = new QVBoxLayout();
		mpVerticalLayoutUtilization2->setObjectName(QString::fromUtf8("mpVerticalLayoutUtilization2"));
		mpLabelUtilization2 = new QLabel(this);
		mpLabelUtilization2->setObjectName(QString::fromUtf8("mpLabelUtilization2"));
		mpLabelUtilization2->setAlignment(Qt::AlignCenter);

		mpVerticalLayoutUtilization2->addWidget(mpLabelUtilization2);

		mpLabelPixmapUtilization2 = new QLabel(this);
		mpLabelPixmapUtilization2->setObjectName(QString::fromUtf8("mpLabelPixmapUtilization2"));
		mpLabelPixmapUtilization2->setMinimumSize(QSize(60, 35));
		mpLabelPixmapUtilization2->setMaximumSize(QSize(60, 35));
		mpPixmapUtilization2 = new QPixmap(60,35);

		mpVerticalLayoutUtilization2->addWidget(mpLabelPixmapUtilization2);

		mpHorizontalLayoutTop->addLayout(mpVerticalLayoutUtilization2);

		mpLine2 = new QFrame(this);
		mpLine2->setObjectName(QString::fromUtf8("mpLine2"));
		mpLine2->setFrameShape(QFrame::VLine);
		mpLine2->setFrameShadow(QFrame::Sunken);

		mpHorizontalLayoutTop->addWidget(mpLine2);

		mpLabelTemperature = new QLabel(this);
		mpLabelTemperature->setObjectName(QString::fromUtf8("mpLabelTemperature"));
		mpLabelTemperature->setMinimumSize(QSize(0, 50));
		mpLabelTemperature->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);

		mpHorizontalLayoutTop->addWidget(mpLabelTemperature);

		mpVerticalLayoutTemperature1 = new QVBoxLayout();
		mpVerticalLayoutTemperature1->setObjectName(QString::fromUtf8("mpVerticalLayoutTemperature1"));
		mpLabelTemperature1 = new QLabel(this);
		mpLabelTemperature1->setObjectName(QString::fromUtf8("mpLabelTemperature1"));
		mpLabelTemperature1->setAlignment(Qt::AlignCenter);

		mpVerticalLayoutTemperature1->addWidget(mpLabelTemperature1);

		mpLabelPixmapTemperature1 = new QLabel(this);
		mpLabelPixmapTemperature1->setObjectName(QString::fromUtf8("mpLabelPixmapTemperature1"));
		mpLabelPixmapTemperature1->setMinimumSize(QSize(60, 35));
		mpLabelPixmapTemperature1->setMaximumSize(QSize(60, 35));
		mpPixmapTemperature1 = new QPixmap(60,35);

		mpVerticalLayoutTemperature1->addWidget(mpLabelPixmapTemperature1);

		mpHorizontalLayoutTop->addLayout(mpVerticalLayoutTemperature1);

		mpVerticalLayoutTemperature2 = new QVBoxLayout();
		mpVerticalLayoutTemperature2->setObjectName(QString::fromUtf8("mpVerticalLayoutTemperature2"));
		mpLabelTemperature2 = new QLabel(this);
		mpLabelTemperature2->setObjectName(QString::fromUtf8("mpLabelTemperature2"));
		mpLabelTemperature2->setAlignment(Qt::AlignCenter);

		mpVerticalLayoutTemperature2->addWidget(mpLabelTemperature2);

		mpLabelPixmapTemperature2 = new QLabel(this);
		mpLabelPixmapTemperature2->setObjectName(QString::fromUtf8("mpLabelPixmapTemperature2"));
		mpLabelPixmapTemperature2->setMinimumSize(QSize(60, 35));
		mpLabelPixmapTemperature2->setMaximumSize(QSize(60, 35));
		mpPixmapTemperature2 = new QPixmap(60,35);

		mpVerticalLayoutTemperature2->addWidget(mpLabelPixmapTemperature2);

		mpHorizontalLayoutTop->addLayout(mpVerticalLayoutTemperature2);

		mpVerticalLayoutTopAndTable->addLayout(mpHorizontalLayoutTop);

		if(mHasProcessTable) {
			mpTableWidgetProcesses = new QTableWidget(this);
			if (mpTableWidgetProcesses->columnCount() < 2)
				mpTableWidgetProcesses->setColumnCount(mColumnCount);
			for(int j = 0; j < mColumnCount; j++) {
				mpTableWidgetProcesses->setHorizontalHeaderItem(j, new QTableWidgetItem());
			}
			mpTableWidgetProcesses->setObjectName(QString::fromUtf8("mpTableWidgetProcesses"));
			QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
			sizePolicy1.setHorizontalStretch(0);
			sizePolicy1.setVerticalStretch(0);
			sizePolicy1.setHeightForWidth(mpTableWidgetProcesses->sizePolicy().hasHeightForWidth());
			mpTableWidgetProcesses->setSizePolicy(sizePolicy1);
			mpTableWidgetProcesses->verticalHeader()->setDefaultSectionSize(25);

			mpVerticalLayoutTopAndTable->addWidget(mpTableWidgetProcesses);
			
			mpTableWidgetProcesses->verticalHeader()->hide();
			mpTableWidgetProcesses->setRowCount(mMaxProcessCount);
			for(int i = 0; i < mMaxProcessCount; i++) {
				for(int j = 0; j < mColumnCount; j++) {
					mpTableWidgetProcesses->setItem(i, j, new QTableWidgetItem());
				}
			}
			mpTableWidgetProcesses->resizeColumnsToContents();
			mpTableWidgetProcesses->setEditTriggers(QAbstractItemView::NoEditTriggers);
			mpTableWidgetProcesses->horizontalHeader()->setStretchLastSection(true);
		}
		
		mpPainter = new QPainter();
		
		mpColorMap = new QwtLinearColorMap(START_COLOR, STOP_COLOR);
		mpColorMap->addColorStop(FIRST_COLOR_STOP);
		mpColorMap->addColorStop(SECOND_COLOR_STOP);
		mpColorMap->addColorStop(THIRD_COLOR_STOP);
	}
}