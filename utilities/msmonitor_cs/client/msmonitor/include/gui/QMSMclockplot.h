#ifndef QMSMCLOCKPLOT_H
#define QMSMCLOCKPLOT_H

#include "QMSMplot.h"
#include<qwt_plot.h>
#include<qwt_plot_curve.h>

#include <vector>

#include "CProtocol.hpp"

class QMSMClockPlot : public QMSMplot
{
public:
    QMSMClockPlot(QWidget *parent);
    virtual ~QMSMClockPlot();
    virtual QWidget* getPlot();

public slots:
    virtual void updateValues(std::vector<double>& values);
    virtual void redraw();

private:
    virtual void initPlot(QWidget *parent);
};

#endif // QMSMCLOCKPLOT_H
