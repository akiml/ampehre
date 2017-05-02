/*
 * QMSMsystemoverview.h
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



#ifndef QMSMSYSTEMOVERVIEW_H
#define QMSMSYSTEMOVERVIEW_H

#include <QMdiSubWindow>
#include "gui/QMSMutilplot.h"
#include "gui/QMSMtemperatureplot.h"

namespace Ui {
class QMSMSystemOverview;
}

class QMSMSystemOverview : public QMdiSubWindow
{
    Q_OBJECT

public:
    explicit QMSMSystemOverview(QWidget *parent = 0);
    ~QMSMSystemOverview();


public slots:
    void update(std::vector<double>& v);
    void updatePID(std::vector<std::string>& str);

private:
    Ui::QMSMSystemOverview *ui;

    void updateUtil(std::vector<double> &v);
    void updateTemp(std::vector<double> &v);


};

#endif // QMSMSYSTEMOVERVIEW_H
