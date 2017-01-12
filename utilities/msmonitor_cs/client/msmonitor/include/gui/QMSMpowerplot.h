#ifndef QMSMPOWERPLOT_H
#define QMSMPOWERPLOT_H

#include<QWidget>

#include<qwt_plot.h>
#include<qwt_plot_curve.h>

class QMSMPowerPlot : public QWidget
{
public:
    QMSMPowerPlot(QWidget *parent);
    virtual ~QMSMPowerPlot();
    QWidget* getPlot();

private:
    virtual void initPlot(QWidget *parent);

    QwtPlot* plot;
    QwtPlotCurve* cpu0;
    QwtPlotCurve* cpu1;
    QwtPlotCurve* gpu;
    QwtPlotCurve* fpga;
    QwtPlotCurve* mic;
    QwtPlotCurve* system;
};

#endif // QMSMPOWERPLOT_H
