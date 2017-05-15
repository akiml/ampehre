#include "gui/QMSMabout.h"
#include "ui_qmsmabout.h"

QMSMabout::QMSMabout(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QMSMabout)
{
    ui->setupUi(this);
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(close()));
}

QMSMabout::~QMSMabout()
{
    delete ui;
}
