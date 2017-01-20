#ifndef QMSMPLOT_H
#define QMSMPLOT_H

#include <QWidget>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <vector>

class QMSMplot : public QWidget
{
    Q_OBJECT
public:
    QMSMplot(QWidget* parent);
    virtual ~QMSMplot();
    virtual QWidget* getPlot() = 0;

public slots:
    virtual void updateValues(std::vector<double>& values) = 0;
    virtual void redraw() = 0;

protected:
    virtual void initPlot(QWidget *parent) = 0;

    QwtPlot* mpPlot;

    QwtPlotCurve* mpCpu0;
    QwtPlotCurve* mpCpu1;
    QwtPlotCurve* mpGpu;
    QwtPlotCurve* mpFpga;
    QwtPlotCurve* mpMic;
    QwtPlotCurve* mpSystem;

    QwtPlotCurve* mpTempCpu0;
    QwtPlotCurve* mpTempCpu1;
    QwtPlotCurve* mpTempGpu;
    QwtPlotCurve* mpTempFpgaM;
    QwtPlotCurve* mpTempFpgaI;
    QwtPlotCurve* mpTempMicDie;
    QwtPlotCurve* mpTempSystem;

    QwtPlotCurve* mpClockCpu0;
    QwtPlotCurve* mpClockCpu1;
    QwtPlotCurve* mpClockGpuCore;
    QwtPlotCurve* mpClockGpuMem;
    QwtPlotCurve* mpClockMicCore;
    QwtPlotCurve* mpClockMicMem;

//    QwtPlotCurve* mpUtilCpu;
//    QwtPlotCurve* mpUtilGpuCore;
//    QwtPlotCurve* mpUtilGpuMem;
//    QwtPlotCurve* mpUtilFpga;
//    QwtPlotCurve* mpUtilMic;

//    QwtPlotCurve* mpMemoryCpu;
//    QwtPlotCurve* mpSwapCpu;
//    QwtPlotCurve* mpMemoryGpu;
//    QwtPlotCurve* mpMemoryMic;


    std::vector<double> mTimevalues;
    std::vector<double> mCpu0values;
    std::vector<double> mCpu1values;
    std::vector<double> mGpuvalues;
    std::vector<double> mFpgavalues;
    std::vector<double> mMicvalues;
    std::vector<double> mSystemvalues;

    std::vector<double> mTempCpu0values;
    std::vector<double> mTempCpu1values;
    std::vector<double> mTempGpuvalues;
    std::vector<double> mTempFpgaIvalues;
    std::vector<double> mTempFpgaMvalues;
    std::vector<double> mTempMicDievalues;
    std::vector<double> mTempSystemvalues;

    std::vector<double> mClockCpu0values;
    std::vector<double> mClockCpu1values;
    std::vector<double> mClockGpuCorevalues;
    std::vector<double> mClockGpuMemvalues;
    std::vector<double> mClockMicCorevalues;
    std::vector<double> mClockMicMemvalues;

//    std::vector<double> mUtilCpuvalues;
//    std::vector<double> mUtilGpuCorevalues;
//    std::vector<double> mUtilGpuMemvalues;
//    std::vector<double> mUtilFpgavalues;
//    std::vector<double> mUtilMicvalues;

//    std::vector<double> mMemoryCpuvalues;
//    std::vector<double> mSwapCpuvalues;
//    std::vector<double> mMemoryGpuvalues;
//    std::vector<double> mMemoryMicvalues;

};

#endif // QMSMPLOT_H
