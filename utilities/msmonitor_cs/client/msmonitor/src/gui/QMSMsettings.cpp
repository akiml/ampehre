#include "gui/QMSMsettings.h"
#include "ui_qmsmsettings.h"

QMSMSettings::QMSMSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QMSMSettings)
{
    ui->setupUi(this);
    connectSignals();
    mFreq.push_back(ui->spinBoxCPU->value());
    mFreq.push_back(ui->spinBoxGPU->value());
    mFreq.push_back(ui->spinBoxFPGA->value());
    mFreq.push_back(ui->spinBoxMIC->value());
    mFreq.push_back(ui->spinBoxSystem->value());
    ui->pushButton_stop->setDisabled(true);
    ui->pushButton_reset->setDisabled(true);
}

QMSMSettings::~QMSMSettings()
{
    delete ui;
}

void QMSMSettings::connectSignals()
{
    connect(ui->pushButton_start, SIGNAL(clicked()), this, SLOT(emit_start()));
    connect(ui->pushButton_stop, SIGNAL(clicked()), this, SLOT(emit_stop()));

    connect(ui->horizontalSlider_guiRate, SIGNAL(valueChanged(int)), this, SLOT(emit_guiRate(int)));
    connect(ui->horizontalSlider_dataPlot, SIGNAL(valueChanged(int)), this, SLOT(emit_dataPlot(int)));

    connect(ui->spinBoxCPU, SIGNAL(valueChanged(int)), this, SLOT(setFreqCpu(int)));
    connect(ui->spinBoxGPU, SIGNAL(valueChanged(int)), this, SLOT(setFreqGpu(int)));
    connect(ui->spinBoxFPGA, SIGNAL(valueChanged(int)), this, SLOT(setFreqFpga(int)));
    connect(ui->spinBoxMIC, SIGNAL(valueChanged(int)), this, SLOT(setFreqMic(int)));
    connect(ui->spinBoxSystem, SIGNAL(valueChanged(int)), this, SLOT(setFreqSys(int)));

}

void QMSMSettings::emit_start()
{
    emit signal_start();
    ui->pushButton_start->setDisabled(true);
    ui->pushButton_stop->setDisabled(false);
    ui->pushButton_reset->setDisabled(false);
}

void QMSMSettings::emit_stop()
{
    emit signal_stop();
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

void QMSMSettings::emit_dataHeatmap(int v)
{
    emit signal_dataHeatmap(v);
    QString s = QString::number(v) + " ms";
    ui->label_dataHeatmap->setText(s);
}

void QMSMSettings::setFreqCpu(int v)
{
    mFreq[FREQ_CPU] = v;
    emit signal_freq(mFreq);
}

void QMSMSettings::setFreqGpu(int v)
{
    mFreq[FREQ_GPU] = v;
    emit signal_freq(mFreq);
}

void QMSMSettings::setFreqFpga(int v)
{
    mFreq[FREQ_FPGA] = v;
    emit signal_freq(mFreq);
}

void QMSMSettings::setFreqMic(int v)
{
    mFreq[FREQ_MIC] = v;
    emit signal_freq(mFreq);
}

void QMSMSettings::setFreqSys(int v)
{
    mFreq[FREQ_SYSTEM] = v;
    emit signal_freq(mFreq);
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
        case DATA_HEAT:
            s = QString::number(val[DATA_HEAT]) + " ms";
            ui->label_dataHeatmap->setText(s);
            ui->horizontalSlider_dataHeatmap->setSliderPosition(val[DATA_HEAT]);
            break;
        }
    }
}
