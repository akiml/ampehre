#include "gui/QMSMplot.h"
#include "ui_qmsmplot.h"

QMSMplot::QMSMplot(QWidget* parent):
    QWidget(parent),
    ui(new Ui::QMSMplot),
    mLineWidth(1),
    parent(parent),
    mpLegend(new QwtLegend()),
    mpPaintCpu0(new QPen(Qt::darkBlue, mLineWidth)),
    mpPaintCpu1(new QPen(Qt::blue, mLineWidth)),
    mpPaintGpu0(new QPen(Qt::darkRed, mLineWidth)),
    mpPaintGpu1(new QPen(Qt::red, mLineWidth)),
    mpPaintFpga0(new QPen(Qt::darkGreen, mLineWidth)),
    mpPaintFpga1(new QPen(Qt::green, mLineWidth)),
    mpPaintMic0(new QPen(Qt::darkMagenta, mLineWidth)),
    mpPaintMic1(new QPen(Qt::magenta, mLineWidth)),
    mpPaintSystem( new QPen(Qt::black, mLineWidth))
{
    ui->setupUi(this);
    mpPlot = ui->qwtPlot;
    mpLegend->setFrameStyle(QFrame::Box | QFrame::Sunken);

    connect(ui->spinBox, SIGNAL(valueChanged(int)), this, SLOT(resetLineWidth(int)));
    connect(ui->pushButtonScreenshot, SIGNAL(clicked()), this, SLOT(screenshot()));
}

QMSMplot::~QMSMplot()
{
    delete ui;
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

void QMSMplot::screenshot()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                "/",
                                tr("Images (*.png *.xpm *.jpg)"));

    QPixmap pixmap (mpPlot->geometry().width(),mpPlot->geometry().height());
    QPainter * painter=new QPainter(&pixmap);
    QwtPlotRenderer rend;
    rend.render(mpPlot,painter,mpPlot->geometry());
    pixmap.save(fileName);
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

void QMSMplot::resetLineWidth(int lValue)
{
    if( lValue > 0 ){
        mLineWidth = lValue;
        mpPaintCpu0->setWidth(lValue);
        mpPaintCpu1->setWidth(lValue);
        mpPaintGpu0->setWidth(lValue);
        mpPaintGpu1->setWidth(lValue);
        mpPaintFpga0->setWidth(lValue);
        mpPaintFpga1->setWidth(lValue);
        mpPaintMic0->setWidth(lValue);
        mpPaintMic1->setWidth(lValue);
        mpPaintSystem->setWidth(lValue);

        resetPen();
    }

}

void QMSMplot::resetPen()
{
    //nothing to do here
}
