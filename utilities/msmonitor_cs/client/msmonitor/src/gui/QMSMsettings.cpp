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
    ui->pushButton_reset->setDisabled(true);
    ui->lineEdit_cpu->setReadOnly(true);
    ui->lineEdit_fpga->setReadOnly(true);
    ui->lineEdit_gpu->setReadOnly(true);
    ui->lineEdit_mic->setReadOnly(true);
    ui->lineEdit_sys->setReadOnly(true);
}

QMSMSettings::~QMSMSettings()
{
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
    emit signal_start();
    ui->pushButton_start->setDisabled(true);
    ui->pushButton_stop->setDisabled(false);
    ui->pushButton_reset->setDisabled(false);
}

void QMSMSettings::emit_saveData(int v)
{
    emit signal_saveData(v);
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
