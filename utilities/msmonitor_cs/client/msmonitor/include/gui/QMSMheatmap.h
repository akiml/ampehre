#ifndef QMSMHEATMAP_H
#define QMSMHEATMAP_H

#include <qwt_plot.h>
#include <qwt_plot_spectrogram.h>
#include <qnumeric.h>
#include <qwt_color_map.h>
#include <qwt_plot_spectrogram.h>
#include <qwt_scale_widget.h>
#include <qwt_scale_draw.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_renderer.h>
#include <qwt_matrix_raster_data.h>
#include <qwt_plot_rescaler.h>
#include <vector>
#include <QVector>
#include "CProtocol.hpp"

enum YValue
{
    UtilCpu,
    UtilGpuCore,
    UtilGpuMem,
    UtilFpga,
    UtilMic,
    TempCpu0,
    TempCpu1,
    TempGpu,
    TempFpgaCompute,
    TempFpgaInterface,
    TempMicDie,
    TempMainBoard
};

class QMSMHeatmap: public QwtPlot
{
    Q_OBJECT

public:
    enum ColorMap
    {
        RGBMap,
        IndexMap,
        HueMap,
        AlphaMap
    };
    QMSMHeatmap(QWidget * , QString caption, int zStart, int zEnd);

public Q_SLOTS:
    void setColorMap( int );
    void setAlpha( int );
    void updateValues(std::vector<double>& values, int val);
    void redraw();

private:
    QwtPlotSpectrogram* mpSpectrogram;
    QwtMatrixRasterData* mpMatrix;

    int mMapType;
    int mAlpha;
    int mIntervalEnd;
    int mIntervalStart;
    QVector<double> mTime;
    QVector<double> mY;
};


#endif // QMSMHEATMAP_H
