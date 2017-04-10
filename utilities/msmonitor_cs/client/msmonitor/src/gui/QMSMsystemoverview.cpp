#include "gui/QMSMsystemoverview.h"
#include "ui_qmsmsystemoverview.h"

QMSMSystemOverview::QMSMSystemOverview(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QMSMSystemOverview)
{
    ui->setupUi(this);
}

QMSMSystemOverview::~QMSMSystemOverview()
{
    delete ui;
}

void QMSMSystemOverview::update(std::vector<double> &v)
{
    updateUtil(v);
    updateTemp(v);
}

void QMSMSystemOverview::updateUtil(std::vector<double> &v)
{
    qDebug() << "Util: "<<v[YUtilCpu];
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

void QMSMSystemOverview::updateTemp(std::vector<double> &v)
{
    qDebug() << "Temp Cpu0: " << v[YTempCpu0];

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
