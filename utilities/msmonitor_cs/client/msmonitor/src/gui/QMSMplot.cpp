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
