#ifndef QMSMPLOT_H
#define QMSMPLOT_H

#include <QWidget>
#include <QPen>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_legend.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_renderer.h>
#include <qwt_plot_magnifier.h>
#include <qwt_plot_panner.h>
#include <vector>
#include <QVBoxLayout>
#include <QPixmap>
#include <QPainter>
#include <QFileDialog>


namespace Ui {
class QMSMplot;
}


class QMSMplot : public QWidget
{
    Q_OBJECT
public:
    explicit QMSMplot(QWidget* parent);
    virtual ~QMSMplot();
    virtual QWidget* getPlot() = 0;
    virtual QWidget* getParent();


public slots:
    virtual void updateValues(std::vector<double>& values) = 0;
    virtual void redraw() = 0;
    virtual void initPlot() = 0;
    virtual void makeGrid();
    virtual void scaleAxis(double xValue);
    virtual void resetLineWidth(int lValue);
    virtual void resetPen();
    virtual void screenshot();
    virtual void makeZoomable();
    virtual void setMaxData(int v);


protected:
    Ui::QMSMplot* ui;
    int mLineWidth;
    int maxData;

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

    QwtPlotMagnifier* mpMagnifier;
    QwtPlotPanner* mpPanner;


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
