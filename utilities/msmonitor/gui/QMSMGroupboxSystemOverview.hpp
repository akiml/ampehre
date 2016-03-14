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
 * version: 0.7.4 - add query for currently active processes to libmeasure and show them in msmonitor
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
		QMSMGroupboxSystemOverview(QWidget* parent, const std::string &resourceName, const std::string &utilization1, const std::string &utilization2,
									const std::string &temperature1, const std::string &temperature2, bool withProcessTable, const  std::string columnNames[] = NULL, int columns = 3, int maxProcessCount = 1);
		~QMSMGroupboxSystemOverview();
		
		void setupUi();
		void setActivity(bool active);
		void setUtilization(int id, double value);
		void setTemperature(int id, double minVal, double maxVal, double value);
		QTableWidget* getProcessTable();
		int getProcessTableRowCount();
		int getProcessTableColumnCount();
	};
}

#endif /* __QMSMGROUPBOXSYSTEMOVERVIEW_H__ */