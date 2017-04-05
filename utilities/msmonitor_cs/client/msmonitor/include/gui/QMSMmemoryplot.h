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
    QMSMMemoryPlot(int linewidth, int maxData, int width, int height, QWidget *parent);
    virtual ~QMSMMemoryPlot();
    virtual QWidget* getPlot();

public slots:
    virtual void updateValues(std::vector<double>& values);
    virtual void redraw();
    virtual void initPlot();
    virtual void resetPen();
};

#endif // QMSMMemoryPlot_H
