#ifndef QMSMTEMPERATUREPLOT_H
#define QMSMTEMPERATUREPLOT_H

#include "QMSMplot.h"
#include<qwt_plot.h>
#include<qwt_plot_curve.h>

#include <vector>

#include "CProtocol.hpp"

class QMSMTemperaturePlot :public QMSMplot
{
public:
    QMSMTemperaturePlot(QWidget* parent);
    virtual ~QMSMTemperaturePlot();
    virtual QWidget* getPlot();

public slots:
    virtual void updateValues(std::vector<double>& values);
    virtual void redraw();
    virtual void initPlot(QWidget* parent);
};

#endif // QMSMTEMPERATUREPLOT_H
