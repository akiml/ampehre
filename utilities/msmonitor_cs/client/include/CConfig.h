/*
 * CConfig.h
 *
 * Copyright (C) 2015, Achim Lösch <achim.loesch@upb.de>, Ahmad El-Ali <aelali@mail.upb.de>
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
 *
 * encoding: UTF-8
 * tab size: 4
 *
 * author: Ahmad El-Ali (aelali@mail.upb.de)
 * created: 11/22/16
 * version: 0.3.0 - extend libmeasure and add application for online monitoring
 *          0.7.0 - modularized measurement struct
 *          0.7.4 - add query for currently active processes to libmeasure and system overview gui to msmonitor
 * 			0.8.0 - client server implementation
 */


#ifndef CCONFIG_H
#define CCONFIG_H

#include <QFile>
#include <QTextStream>
#include <QString>
#include <string>
#include <QObject>
#include "gui/QMSMplot.h"

class CConfig: public QWidget
{
    Q_OBJECT
public:
    CConfig(const QString path, QWidget* parent = 0);
    ~CConfig();

    QWidget* parent;
    QString serverIP;
    int serverPort;
    int plot;
    int gui;
    int lineWidth;
    int maxDataRecord;
    int width;
    int height;
    int alpha;
    int intervalStart;
    int intervalEnd;
    int updateTime;
    int safetyTimeServer;

    void exportConfig();
    void importConfig(const QString path);
    void setEnding(QString& name, const QString ending);

public slots:
    void exportPlotToCSV(QMSMplot* plot);

private:
    void removeSpaces(QString& line);
    void setVariable(const QString line);

};

#endif
