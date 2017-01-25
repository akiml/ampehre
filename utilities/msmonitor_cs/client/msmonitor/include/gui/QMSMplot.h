#ifndef QMSMPLOT_H
#define QMSMPLOT_H

#include <QWidget>
#include <QPen>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_legend.h>
#include <vector>

class QMSMplot : public QWidget
{
    Q_OBJECT
public:
    QMSMplot(QWidget* parent);
    virtual ~QMSMplot();
    virtual QWidget* getPlot() = 0;
    virtual QWidget* getParent();

public slots:
    virtual void updateValues(std::vector<double>& values) = 0;
    virtual void redraw() = 0;
    virtual void initPlot(QWidget* parent) = 0;

protected:
    QwtPlot* mpPlot;
    QWidget* parent;
    QwtLegend* mpLegend;

    QwtPlotCurve* mpCpu0;
    QwtPlotCurve* mpCpu1;
    QwtPlotCurve* mpGpu0;
    QwtPlotCurve* mpGpu1;
    QwtPlotCurve* mpFpga0;
    QwtPlotCurve* mpFpga1;
    QwtPlotCurve* mpMic0;
    QwtPlotCurve* mpMic1;
    QwtPlotCurve* mpSystem;

    QPen* mpPaintCpu0;
    QPen* mpPaintCpu1;
    QPen* mpPaintGpu0;
    QPen* mpPaintGpu1;
    QPen* mpPaintFpga0;
    QPen* mpPaintFpga1;
    QPen* mpPaintMic0;
    QPen* mpPaintMic1;
    QPen* mpPaintSystem;


    std::vector<double> mTimevalues;
    std::vector<double> mCpu0values;
    std::vector<double> mCpu1values;
    std::vector<double> mGpu0values;
    std::vector<double> mGpu1values;
    std::vector<double> mFpga0values;
    std::vector<double> mFpga1values;
    std::vector<double> mMic0values;
    std::vector<double> mMic1values;
    std::vector<double> mSystemvalues;

};

#endif // QMSMPLOT_H
