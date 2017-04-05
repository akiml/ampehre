#include "include/CConfig.h"

CConfig::CConfig(const QString path, QWidget* parent):
    parent(parent),
    serverIP("131.234.58.31"),
    serverPort(2900),
    plot(2000),
    heatmap(150),
    gui(500),
    lineWidth(1),
    maxDataRecord(60),
    width(700),
    height(500),
    alpha(255),
    intervalStart(0),
    intervalEnd(100),
    updateTime(10)
{
    importConfig(path);
}

CConfig::~CConfig()
{

}

void CConfig::exportConfig()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Save as"), "/");
    QFile file(filename);
    file.open(QIODevice::WriteOnly);
    QString txt = "";

    txt = QString("serverIP") + QString("=") + serverIP + QString(QChar('\n'));
    file.write(txt.toUtf8());
    txt = QString("serverPort") + QString("=") + QString::number(serverPort) + QString(QChar('\n'));
    file.write(txt.toUtf8());
    txt = QString("plot") + QString("=") + QString::number(plot) + QString(QChar('\n'));
    file.write(txt.toUtf8());
    txt = QString("heatmap") + QString("=") + QString::number(heatmap) + QString(QChar('\n'));
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
    QString filename = dialog.getSaveFileName(NULL, "Create New File","","");
    QFile file(filename);
    file.open(QIODevice::WriteOnly);
    QString txt = "";
    txt = QString("TimeStamp,CPU0,CPU1,GPU0,GPU1,FPGA0,FPGA1,MIC0,MIC1,SYSTEM,\n");
    file.write(txt.toUtf8());
    for(unsigned int i = 0; i < plot->mTimevalues.size(); i ++)
    {
        txt = "";
        txt += QString::number(plot->mTimevalues[i]) + ",";
        if(plot->mCpu0values.size() > i)
            txt += QString::number(plot->mCpu0values[i]) + ",";
        if(plot->mCpu1values.size() > i)
            txt += QString::number(plot->mCpu1values[i]) + ",";
        if(plot->mGpu0values.size() > i)
            txt += QString::number(plot->mGpu0values[i]) + ",";
        if(plot->mGpu1values.size() > i)
            txt += QString::number(plot->mGpu1values[i]) + ",";
        if(plot->mFpga0values.size() > i)
            txt += QString::number(plot->mFpga0values[i]) + ",";
        if(plot->mFpga1values.size() > i)
            txt += QString::number(plot->mFpga1values[i]) + ",";
        if(plot->mMic0values.size() > i)
            txt += QString::number(plot->mMic0values[i]) + ",";
        if(plot->mMic1values.size() > i)
            txt += QString::number(plot->mMic1values[i]) + ",";
        if(plot->mSystemvalues.size() > i)
            txt += QString::number(plot->mSystemvalues[i]) + ",";
        txt += "\n";
        file.write(txt.toUtf8());
    }
    file.close();
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
    qDebug() << line;
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
    else if(var_name == "heatmap")
    {
        heatmap = var.toInt();
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
    else
    {
        qDebug() << "no such config variable...";
    }

}
