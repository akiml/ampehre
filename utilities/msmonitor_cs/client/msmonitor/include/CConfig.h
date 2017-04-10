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
