#ifndef QMSMMemoryPlot_H
#define QMSMMemoryPlot_H

#include "QMSMplot.h"
#include<qwt_plot.h>
#include<qwt_plot_curve.h>

#include <vector>

#include "CProtocol.hpp"

class QMSMMemoryPlot : public QMSMplot
{
public:
    QMSMMemoryPlot(QWidget *parent);
    virtual ~QMSMMemoryPlot();
    virtual QWidget* getPlot();

public slots:
    virtual void updateValues(std::vector<double>& values);
    virtual void redraw();
    virtual void initPlot(QWidget* parent);
};

#endif // QMSMMemoryPlot_H
