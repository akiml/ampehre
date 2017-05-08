#ifndef QMSMLINEARCOLORMAPRGB_H
#define QMSMLINEARCOLORMAPRGB_H

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

class LinearColorMapRGB : public QwtLinearColorMap
{
public:
    LinearColorMapRGB();
    ~LinearColorMapRGB();
};

#endif // QMSMLINEARCOLORMAPRGB_H
