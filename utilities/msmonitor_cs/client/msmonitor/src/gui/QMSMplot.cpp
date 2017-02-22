#include "include/gui/QMSMplot.h"

QMSMplot::QMSMplot(QWidget* parent):
    QWidget(parent),
    parent(parent),
    mpLegend(new QwtLegend()),
    mpPaintCpu0(new QPen(Qt::darkBlue)),
    mpPaintCpu1(new QPen(Qt::blue)),
    mpPaintGpu0(new QPen(Qt::darkRed)),
    mpPaintGpu1(new QPen(Qt::red)),
    mpPaintFpga0(new QPen(Qt::darkGreen)),
    mpPaintFpga1(new QPen(Qt::green)),
    mpPaintMic0(new QPen(Qt::darkMagenta)),
    mpPaintMic1(new QPen(Qt::magenta)),
    mpPaintSystem( new QPen(Qt::black))
{
    mpLegend->setFrameStyle(QFrame::Box | QFrame::Sunken);
}

QMSMplot::~QMSMplot()
{
    delete mpPlot;
    delete parent;
    delete mpCpu0;
    delete mpCpu1;
    delete mpGpu0;
    delete mpGpu1;
    delete mpFpga0;
    delete mpFpga1;
    delete mpMic0;
    delete mpMic1;
    delete mpSystem;

    delete mpPaintCpu0;
    delete mpPaintCpu1;
    delete mpPaintGpu0;
    delete mpPaintGpu1;
    delete mpPaintFpga0;
    delete mpPaintFpga1;
    delete mpPaintMic0;
    delete mpPaintMic1;
    delete mpPaintSystem;
}

QWidget* QMSMplot::getPlot()
{
    return this->mpPlot;
}

QWidget* QMSMplot::getParent()
{
    return this->parent;
}

void QMSMplot::makeGrid()
{
    QwtPlotGrid* mpGrid = new QwtPlotGrid();
    mpGrid->enableXMin(true);
    mpGrid->enableYMin(true);
    mpGrid->setPen(QPen(Qt::black, 0, Qt::DotLine));
    mpGrid->attach(mpPlot);
}

void QMSMplot::scaleAxis(double xValue)
{
    int32_t first_tick		= 0;
    int32_t second_tick		= 0;
    double time_show_data	= 60;

    first_tick		= ((int)(xValue-time_show_data)/10)*10;
    first_tick		= (first_tick >= 0 && (xValue-time_show_data) > 0) ? first_tick+10 : first_tick;
    second_tick		= ((int)(xValue)/10)*10+10;

    mpPlot->setAxisScale(QwtPlot::xBottom, first_tick, second_tick);
//    mpPlot->setAxisScale(QwtPlot::yLeft, yValueMin, yValueMax);
}
