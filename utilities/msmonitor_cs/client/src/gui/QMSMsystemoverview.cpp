/*
 * QMSMsystemoverview.cpp
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


#include "gui/QMSMsystemoverview.h"
#include "ui_qmsmsystemoverview.h"

QMSMSystemOverview::QMSMSystemOverview(QWidget *parent) :
    QMdiSubWindow(parent),
    ui(new Ui::QMSMSystemOverview)
{
    delete this->layout();
    ui->setupUi(this);
    ui->tableWidget_fpga->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget_gpu->horizontalHeader()->setStretchLastSection(true);

    ui->tableWidget_fpga->setItem(0,0,new QTableWidgetItem(""));
    ui->tableWidget_fpga->setItem(0,1,new QTableWidgetItem(""));
    ui->tableWidget_fpga->setItem(0,2,new QTableWidgetItem(""));

    ui->tableWidget_gpu->setItem(0,0,new QTableWidgetItem(""));
    ui->tableWidget_gpu->setItem(0,1,new QTableWidgetItem(""));
    ui->tableWidget_gpu->setItem(1,0,new QTableWidgetItem(""));
    ui->tableWidget_gpu->setItem(1,1,new QTableWidgetItem(""));
    ui->tableWidget_gpu->setItem(2,0,new QTableWidgetItem(""));
    ui->tableWidget_gpu->setItem(2,1,new QTableWidgetItem(""));
    ui->tableWidget_gpu->setItem(3,0,new QTableWidgetItem(""));
    ui->tableWidget_gpu->setItem(3,1,new QTableWidgetItem(""));
    ui->tableWidget_gpu->setItem(4,0,new QTableWidgetItem(""));
    ui->tableWidget_gpu->setItem(4,1,new QTableWidgetItem(""));

}

QMSMSystemOverview::~QMSMSystemOverview()
{
    delete ui;
    std::cout << "delete sysoverview" << std::endl;
}

void QMSMSystemOverview::update(std::vector<double> &v)
{
    updateUtil(v);
    updateTemp(v);
}

void QMSMSystemOverview::updatePID(std::vector<std::string>& str)
{
    std::string tmp = "";
    for(unsigned int i = 0; i < str.size(); i++)
    {
        if(str[i][0] == 'f')
        {
//            int z = i;
            unsigned int k = 2;
            while(k < str[i].size() && str[i][k] != ' ')
            {
                tmp += str[i][k];
                k++;
            }
            ui->tableWidget_fpga->item(0,0)->setText(QString::fromStdString(tmp));
            tmp = "";
            k++;

            while(k < str[i].size() && str[i][k] != ' ')
            {
                tmp += str[i][k];
                k++;
            }
            ui->tableWidget_fpga->item(0,1)->setText(QString::fromStdString(tmp));
            tmp = "";
            k++;

            while(k < str[i].size() && str[i][k] != ' ')
            {
                tmp += str[i][k];
                k++;
            }
            ui->tableWidget_fpga->item(0,2)->setText(QString::fromStdString(tmp));
            tmp = "";
        }
        else if(str[i][0] == 'g')
        {
            int z = i;
            unsigned int k = 2;
            while(k < str[i].size() && str[i][k] != ' ')
            {
                tmp += str[i][k];
                k++;
            }
            ui->tableWidget_gpu->item(i-z,0)->setText(QString::fromStdString(tmp));
            tmp = "";
            k++;

            while(k < str[i].size() && str[i][k] != ' ')
            {
                tmp += str[i][k];
                k++;
            }
            ui->tableWidget_gpu->item(i-z,1)->setText(QString::fromStdString(tmp));
            tmp = "";
        }
    }
}

void QMSMSystemOverview::updateUtil(std::vector<double> &v)
{
    if(v.size() > 0)
    {
        QString s = QString::number((int)v[YUtilCpu]);
        ui->textEdit_cpu_util->setPlainText(s);

        s = QString::number((int)v[YUtilGpuCore]);
        ui->textEdit_gpu_core->setPlainText(s);

        s = QString::number((int)v[YUtilGpuMem]);
        ui->textEdit_gpu_mem->setPlainText(s);

        s = QString::number((int)v[YUtilFpga]);
        ui->textEdit_fpga_util->setPlainText(s);

        s = QString::number((int)v[YUtilMic]);
        ui->textEdit_mic_util->setPlainText(s);
    }
}

void QMSMSystemOverview::updateTemp(std::vector<double> &v)
{
    if(v.size() > 0)
    {
        QString s = QString::number((int)v[YTempCpu0]);
        ui->textEdit_cpu_temp0->setPlainText(s);

        s = QString::number((int)v[YTempCpu1]);
        ui->textEdit_cpu_temp1->setPlainText(s);

        s = QString::number((int)v[YTempGpu]);
        ui->textEdit_gpu_temp->setPlainText(s);

        s = QString::number((int)v[YTempFpgaI]);
        ui->textEdit_fpga_inter->setPlainText(s);

        s = QString::number((int)v[YTempFpgaM]);
        ui->textEdit_fpga_comp->setPlainText(s);

        s = QString::number((int)v[YTempMicDie]);
        ui->textEdit_mic_temp->setPlainText(s);
    }
}
