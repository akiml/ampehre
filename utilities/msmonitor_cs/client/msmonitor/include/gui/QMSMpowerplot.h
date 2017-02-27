#ifndef QMSMPOWERPLOT_H
#define QMSMPOWERPLOT_H

#include "QMSMplot.h"
#include<qwt_plot.h>
#include<qwt_plot_curve.h>

#include <vector>

#include "CProtocol.hpp"

class QMSMPowerPlot :public QMSMplot
{

public:
    QMSMPowerPlot(QWidget *parent);
    virtual ~QMSMPowerPlot();
    virtual QWidget* getPlot();

public slots:
    virtual void updateValues(std::vector<double>& values);
    virtual void redraw();
    virtual void initPlot();
    virtual void resetPen();

};

#endif // QMSMPOWERPLOT_H
