/*
 * CConfig.cpp
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

#include "include/CConfig.h"

CConfig::CConfig(const QString path, QWidget* parent):
    parent(parent),
    serverIP("131.234.58.31"),
    serverPort(2900),
    plot(2000),
    gui(500),
    lineWidth(1),
    maxDataRecord(60),
    width(700),
    height(500),
    alpha(255),
    intervalStart(0),
    intervalEnd(100),
    updateTime(10),
    safetyTimeServer(500)
{
    importConfig(path);
}

CConfig::~CConfig()
{

}

void CConfig::exportConfig()
{
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::AnyFile);
    QString filename = dialog.getSaveFileName(NULL, "Create New File","","*.conf");
    setEnding(filename, ".conf");
    QFile file(filename);
    file.open(QIODevice::WriteOnly);
    QString txt = "";

    txt = QString("serverIP") + QString("=") + serverIP + QString(QChar('\n'));
    file.write(txt.toUtf8());
    txt = QString("serverPort") + QString("=") + QString::number(serverPort) + QString(QChar('\n'));
    file.write(txt.toUtf8());
    txt = QString("plot") + QString("=") + QString::number(plot) + QString(QChar('\n'));
    file.write(txt.toUtf8());
    txt = QString("gui") + QString("=") + QString::number(gui) + QString(QChar('\n'));
    file.write(txt.toUtf8());
    txt = QString("lineWidth") + QString("=") + QString::number(lineWidth) + QString(QChar('\n'));
    file.write(txt.toUtf8());
    txt = QString("maxDataRecord") + QString("=") + QString::number(maxDataRecord) + QString(QChar('\n'));
    file.write(txt.toUtf8());
    txt = QString("width") + QString("=") + QString::number(width) + QString(QChar('\n'));
    file.write(txt.toUtf8());
    txt = QString("height") + QString("=") + QString::number(height) + QString(QChar('\n'));
    file.write(txt.toUtf8());
    txt = QString("alpha") + QString("=") + QString::number(alpha) + QString(QChar('\n'));
    file.write(txt.toUtf8());
    txt = QString("intervalStart") + QString("=") + QString::number(intervalStart) + QString(QChar('\n'));
    file.write(txt.toUtf8());
    txt = QString("intervalEnd") + QString("=") + QString::number(intervalEnd) + QString(QChar('\n'));
    file.write(txt.toUtf8());
    txt = QString("updateTime") + QString("=") + QString::number(updateTime) + QString(QChar('\n'));
    file.write(txt.toUtf8());

    file.close();
}

void CConfig::importConfig(const QString path)
{
    QFile config(path);
    if(config.exists())
    {
        qDebug()<< "config file found!";
        config.open(QIODevice::ReadOnly);
        QString line;
        QTextStream in(&config);
        while(!in.atEnd())
        {
            line = in.readLine();
            if(line[0] == QChar('#') || line[0] == QChar('\n') || line == ""){ //no comments and new lines
                continue;
            }
            else
            {
                qDebug() << line;
                removeSpaces(line);
                setVariable(line);
            }
        }


    }
    else
        qDebug() << "[FATAL]: no config file detected, taking default values...";
}

void CConfig::exportPlotToCSV(QMSMplot *plot)
{
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::AnyFile);
    QString filename = dialog.getSaveFileName(NULL, "Create New File","","*.csv");
    setEnding(filename, ".csv");
    QFile file(filename);
    file.open(QIODevice::WriteOnly);
    QString txt = "";
    switch(plot->mType)
    {
    case MEMORY:
        txt = QString("TimeStamp;CPU0;CPU1;GPU0;MIC0;") + "\n";
        break;
    case POWER:
        txt = QString("TimeStamp;CPU0;CPU1;GPU0;FPGA0;MIC0;SYSTEM;") + "\n";
        txt += QString("[s];[W];[W];[W];[W];[W];[W];") + "\n";
        break;
    case UTIL:
        txt = QString("TimeStamp;CPU0;GPU0;GPU1;FPGA0;MIC0;") + "\n";
        break;
    case TEMP:
        txt = QString("TimeStamp;CPU0;CPU1;GPU0;FPGA0;FPGA1;MIC0;SYSTEM") + "\n";
        txt += QString("[s];[C];[C];[C];[C];[C];[C];[C]") + "\n";
        break;
    case CLOCK:
        txt = QString("TimeStamp;CPU0;CPU1;GPU0;GPU1;MIC0;MIC1;") + "\n";
        break;
    }
    //change txt depending on task
    file.write(txt.toUtf8());
    for(unsigned int i = 0; i < plot->mTimevalues.size(); i ++)
    {
        txt = "";
        txt += QString::number(plot->mTimevalues[i]) + ";";
        if(plot->mCpu0values.size() > i)
            txt += QString::number(plot->mCpu0values[i]) + ";";
        if(plot->mCpu1values.size() > i)
            txt += QString::number(plot->mCpu1values[i]) + ";";
        if(plot->mGpu0values.size() > i)
            txt += QString::number(plot->mGpu0values[i]) + ";";
        if(plot->mGpu1values.size() > i)
            txt += QString::number(plot->mGpu1values[i]) + ";";
        if(plot->mFpga0values.size() > i)
            txt += QString::number(plot->mFpga0values[i]) + ";";
        if(plot->mFpga1values.size() > i)
            txt += QString::number(plot->mFpga1values[i]) + ";";
        if(plot->mMic0values.size() > i)
            txt += QString::number(plot->mMic0values[i]) + ";";
        if(plot->mMic1values.size() > i)
            txt += QString::number(plot->mMic1values[i]) + ";";
        if(plot->mSystemvalues.size() > i)
            txt += QString::number(plot->mSystemvalues[i]) + ";";
        txt += "\n";
        file.write(txt.toUtf8());
    }
    file.close();
}

void CConfig::setEnding(QString &name, const QString ending)
{
    if(!name.endsWith(ending))
    {
        name += ending;
    }
}

void CConfig::removeSpaces(QString &line)
{
    for(int i = 0; i < line.length(); i++)
    {
        if(line[i]==' ')
        {
            line = line.remove(i, 1);
        }
    }
}


void CConfig::setVariable(const QString line)
{
    QString var_name = "";
    QString var = "";
    int i = 0;
    while(line[i] != '=')
    {
        var_name += line[i];
        i++;
    }
    i++;
    while(i < line.size())
    {
        var += line[i];
        i++;
    }
    if(var_name == "serverIP")
    {
        serverIP = var;
    }
    else if(var_name == "serverPort")
    {
        serverPort = var.toInt();
    }
    else if(var_name == "plot")
    {
        plot = var.toInt();
    }
    else if(var_name == "gui")
    {
        gui = var.toInt();
    }
    else if(var_name == "lineWidth")
    {
        lineWidth = var.toInt();
    }
    else if(var_name == "maxDataRecord")
    {
        maxDataRecord = var.toInt();
    }
    else if(var_name == "width")
    {
        width = var.toInt();
    }
    else if(var_name == "height")
    {
        height = var.toInt();
    }
    else if(var_name == "alpha")
    {
        alpha = var.toInt();
    }
    else if(var_name == "intervalStart")
    {
        intervalStart = var.toInt();
    }
    else if(var_name == "intervalEnd")
    {
        intervalEnd = var.toInt();
    }
    else if(var_name == "updateTime")
    {
        updateTime = var.toInt();
    }
    else if(var_name == "safetyTimeServer")
    {
        safetyTimeServer = var.toInt();
    }
    else
    {
        qDebug() << "no such config variable...";
    }

}
