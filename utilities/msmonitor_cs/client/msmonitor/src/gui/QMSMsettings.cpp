#include "gui/QMSMsettings.h"
#include "ui_qmsmsettings.h"

QMSMSettings::QMSMSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QMSMSettings)
{
    ui->setupUi(this);
    connectSignals();
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
}

void QMSMSettings::emit_stop()
{
    emit signal_stop();
}

void QMSMSettings::emit_guiRate(int v)
{
    emit signal_guiRate(v);
}

void QMSMSettings::emit_dataPlot(int v)
{
    emit signal_dataPlot(v);
}
