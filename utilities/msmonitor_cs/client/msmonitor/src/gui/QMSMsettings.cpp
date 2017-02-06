#include "gui/QMSMsettings.h"
#include "ui_qmsmsettings.h"

QMSMSettings::QMSMSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QMSMSettings)
{
    ui->setupUi(this);
    connectSignals();
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
