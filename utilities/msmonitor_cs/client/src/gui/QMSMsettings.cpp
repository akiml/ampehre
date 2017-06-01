/*
 * QMSMsettings.cpp
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


#include "gui/QMSMsettings.h"
#include "ui_qmsmsettings.h"

QMSMSettings::QMSMSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QMSMSettings)
{
    ui->setupUi(this);
    setWindowTitle("Settings");
    connectSignals();
    ui->pushButton_stop->setDisabled(true);
    ui->lineEdit_cpu->setReadOnly(true);
    ui->lineEdit_fpga->setReadOnly(true);
    ui->lineEdit_gpu->setReadOnly(true);
    ui->lineEdit_mic->setReadOnly(true);
    ui->lineEdit_sys->setReadOnly(true);
}

QMSMSettings::~QMSMSettings()
{
    std::cout << "delete settings" << std::endl;
    delete ui;
}

void QMSMSettings::setSaveData(int value)
{
    ui->spinBox_savedata->setValue(value);
}

void QMSMSettings::connectSignals()
{
    connect(ui->pushButton_start, SIGNAL(clicked()), this, SLOT(emit_start()));
    connect(ui->pushButton_stop, SIGNAL(clicked()), this, SLOT(emit_stop()));

    connect(ui->lineEdit_ipaddr, SIGNAL(textChanged(QString)), this, SLOT(emit_ipChanged(QString)));
    connect(ui->spinBox_port, SIGNAL(valueChanged(int)), this, SLOT(emit_portChanged(int)));

    connect(ui->pushButton_exportSettings, SIGNAL(clicked()), this, SLOT(emit_exportConfig()));
    connect(ui->pushButton_loadSettings, SIGNAL(clicked()), this, SLOT(emit_loadConfig()));
    connect(ui->horizontalSlider_guiRate, SIGNAL(valueChanged(int)), this, SLOT(emit_guiRate(int)));
    connect(ui->horizontalSlider_dataPlot, SIGNAL(valueChanged(int)), this, SLOT(emit_dataPlot(int)));
    connect(ui->spinBox_savedata, SIGNAL(valueChanged(int)), this, SLOT(emit_saveData(int)));

}

void QMSMSettings::setFreqLabels(std::vector<uint64_t> &v)
{
    std::ostringstream ss;

    ss << v[FREQ_CPU];
    ui->lineEdit_cpu->setText(QString::fromStdString(ss.str()));
    ss.str("");

    ss << v[FREQ_GPU];
    ui->lineEdit_gpu->setText(QString::fromStdString(ss.str()));
    ss.str("");

    ss << v[FREQ_FPGA];
    ui->lineEdit_fpga->setText(QString::fromStdString(ss.str()));
    ss.str("");

    ss << v[FREQ_MIC];
    ui->lineEdit_mic->setText(QString::fromStdString(ss.str()));
    ss.str("");

    ss << v[FREQ_SYSTEM];
    ui->lineEdit_sys->setText(QString::fromStdString(ss.str()));
    ss.str("");

}

void QMSMSettings::emit_start()
{
    start();
    emit signal_start();
}

void QMSMSettings::emit_saveData(int v)
{
    emit signal_saveData(v);
}


void QMSMSettings::emit_stop()
{
    stop();
    emit signal_stop();
}

void QMSMSettings::start()
{
    ui->pushButton_start->setDisabled(true);
    ui->pushButton_stop->setDisabled(false);
}

void QMSMSettings::stop()
{
    ui->pushButton_start->setDisabled(false);
    ui->pushButton_stop->setDisabled(true);
}

void QMSMSettings::emit_guiRate(int v)
{
    emit signal_guiRate(v);
    QString s = QString::number(v) + " ms";
    ui->label_guiRate->setText(s);
}

void QMSMSettings::emit_dataPlot(int v)
{
    emit signal_dataPlot(v);
    QString s = QString::number(v) + " ms";
    ui->label_dataPlot->setText(s);
}

void QMSMSettings::set_port(int v)
{
    if(v >= 0)
        ui->spinBox_port->setValue(v);
}

void QMSMSettings::set_ip(QString v)
{
    ui->lineEdit_ipaddr->setText(v);
}

void QMSMSettings::emit_ipChanged(QString s)
{
    emit signal_ipChanged(s);
}

void QMSMSettings::emit_portChanged(int v)
{
    emit signal_portChanged(v);
}

void QMSMSettings::emit_exportConfig()
{
    emit signal_exportConfig();
}

void QMSMSettings::emit_loadConfig()
{
    emit signal_loadConfig();
}

int QMSMSettings::get_port()
{
    return ui->spinBox_port->value();
}

QString QMSMSettings::get_ip()
{
    return ui->lineEdit_ipaddr->text();
}

int QMSMSettings::get_maxData()
{
    return ui->spinBox_savedata->value();
}

void QMSMSettings::setSliderPos(std::vector<int>& val)
{
    QString s;
    for(unsigned int i = 0; i < val.size(); i++)
    {
        switch(i)
        {
        case GUI:
            s = QString::number(val[GUI]) + " ms";
            ui->label_guiRate->setText(s);
            ui->horizontalSlider_guiRate->setSliderPosition(val[GUI]);
            break;
        case DATA_PLOT:
            s = QString::number(val[DATA_PLOT]) + " ms";
            ui->label_dataPlot->setText(s);
            ui->horizontalSlider_dataPlot->setSliderPosition(val[DATA_PLOT]);
            break;
        }
    }
}
