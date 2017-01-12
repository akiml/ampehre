#ifndef QMSMPOWERPLOT_H
#define QMSMPOWERPLOT_H

#include<QWidget>
#include<qwt_plot.h>
#include<qwt_plot_curve.h>

#include <vector>

#include "CProtocol.hpp"

class QMSMPowerPlot : public QWidget
{
    Q_OBJECT
public:
    QMSMPowerPlot(QWidget *parent);
    virtual ~QMSMPowerPlot();
    QWidget* getPlot();

public slots:
    void redraw(std::vector<double>& values);

private:
    virtual void initPlot(QWidget *parent);

    QwtPlot* mpPlot;
    QwtPlotCurve* mpCpu0;
    QwtPlotCurve* mpCpu1;
    QwtPlotCurve* mpGpu;
    QwtPlotCurve* mpFpga;
    QwtPlotCurve* mpMic;
    QwtPlotCurve* mpSystem;
    std::vector<double> mTimevalues;
    std::vector<double> mCpu0values;
    std::vector<double> mCpu1values;
    std::vector<double> mGpuvalues;
    std::vector<double> mFpgavalues;
    std::vector<double> mMicvalues;
    std::vector<double> mSystemvalues;
};

#endif // QMSMPOWERPLOT_H
