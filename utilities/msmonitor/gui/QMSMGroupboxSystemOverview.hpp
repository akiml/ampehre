/*
 * QMSMGroupboxSystemOverview.hpp
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

#include <QtGui/QGroupBox>
#include <QtGui/QVBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QTableWidget>
#include <QtGui/QApplication>
#include <QtGui/QHeaderView>
#include <QPainter>
#include <qwt_color_map.h>
#include <sstream>

#ifndef __QMSMGROUPBOXSYSTEMOVERVIEW_H__
#define __QMSMGROUPBOXSYSTEMOVERVIEW_H__

namespace Ui {
	class QMSMGroupboxSystemOverview:
	public QGroupBox {
	Q_OBJECT
	
	private:
		QVBoxLayout *mpVerticalLayoutTopAndTable;
		QHBoxLayout *mpHorizontalLayoutTop;
		
		QLabel *mpLabelResourceActivity;
		QPixmap *mpPixmapActivity;
		QSpacerItem *mpHorizontalSpacer;
		
		QFrame *mpLine1;
		QLabel *mpLabelUtilization;
		QVBoxLayout *mpVerticalLayoutUtilization1;
		QLabel *mpLabelUtilization1;
		QLabel *mpLabelPixmapUtilization1;
		QPixmap *mpPixmapUtilization1;
		QVBoxLayout *mpVerticalLayoutUtilization2;
		QLabel *mpLabelUtilization2;
		QLabel *mpLabelPixmapUtilization2;
		QPixmap *mpPixmapUtilization2;
		
		QFrame *mpLine2;
		QLabel *mpLabelTemperature;
		QVBoxLayout *mpVerticalLayoutTemperature1;
		QLabel *mpLabelTemperature1;
		QLabel *mpLabelPixmapTemperature1;
		QPixmap *mpPixmapTemperature1;
		QVBoxLayout *mpVerticalLayoutTemperature2;
		QLabel *mpLabelTemperature2;
		QLabel *mpLabelPixmapTemperature2;
		QPixmap *mpPixmapTemperature2;
		
		QTableWidget *mpTableWidgetProcesses;
		
		QPainter *mpPainter;
		QwtLinearColorMap *mpColorMap;
		
		bool mHasProcessTable;
		int mMaxProcessCount;
		int mColumnCount;
	
	public:
		/**
		 * resourceName:		heading of the groupbox
		 * utilization1/2:		text of the label above the first/second utilization
		 * temperature1/2:		text of the label above the first/second temperature
		 * withProcessTable:	indicates whether a table for the processes is needed
		 * columnNames:			string array with the column headings
		 * columns:				number of columns in the table
		 * maxProcessCount:		number of rows in the table
		 */
		QMSMGroupboxSystemOverview(QWidget* parent, const std::string &resourceName, const std::string &utilization1, const std::string &utilization2,
									const std::string &temperature1, const std::string &temperature2, bool withProcessTable, const  std::string columnNames[] = NULL, int columns = 3, int maxProcessCount = 1);
		~QMSMGroupboxSystemOverview();
		
		void setupUi();
		//set the activity LED
		void setActivity(bool active);
		// set the utilization, id indicates the first (0) or second (1) label
		void setUtilization(int id, double value);
		// set the temperature, id indicates the first (0) or second (1) label
		// minVal is the lowest value and maxVal the highest possible value needed to calculate the color of the Pixmap
		void setTemperature(int id, double minVal, double maxVal, double value);
		
		QTableWidget* getProcessTable();
		int getProcessTableRowCount();
		int getProcessTableColumnCount();
	};
}

#endif /* __QMSMGROUPBOXSYSTEMOVERVIEW_H__ */