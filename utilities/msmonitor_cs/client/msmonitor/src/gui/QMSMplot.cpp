#include "include/gui/QMSMplot.h"

QMSMplot::QMSMplot(QWidget* parent):
    QWidget(parent)
{
}

QMSMplot::~QMSMplot(){}

QWidget* QMSMplot::getPlot()
{
    return this->mpPlot;
}
