#include "gui/QMSMheatmap.h"


class LinearColorMapRGB: public QwtLinearColorMap
{
public:
    LinearColorMapRGB():
        QwtLinearColorMap( Qt::darkCyan, Qt::red, QwtColorMap::RGB )
    {
        addColorStop( 0.2, Qt::cyan );
        addColorStop( 0.5, Qt::green );
        addColorStop( 0.85, Qt::yellow );
    }
};

class LinearColorMapIndexed: public QwtLinearColorMap
{
public:
    LinearColorMapIndexed():
        QwtLinearColorMap( Qt::darkCyan, Qt::red, QwtColorMap::Indexed )
    {
        addColorStop( 0.2, Qt::cyan );
        addColorStop( 0.5, Qt::green );
        addColorStop( 0.85, Qt::yellow );
    }
};

class HueColorMap: public QwtColorMap
{
public:
    // class backported from Qwt 6.2

    HueColorMap():
        d_hue1(0),
        d_hue2(359),
        d_saturation(150),
        d_value(200)
    {
        updateTable();

    }

    virtual QRgb rgb( const QwtInterval &interval, double value ) const
    {
        if ( qIsNaN(value) )
            return 0u;

        const double width = interval.width();
        if ( width <= 0 )
            return 0u;

        if ( value <= interval.minValue() )
            return d_rgbMin;

        if ( value >= interval.maxValue() )
            return d_rgbMax;

        const double ratio = ( value - interval.minValue() ) / width;
        int hue = d_hue1 + qRound( ratio * ( d_hue2 - d_hue1 ) );

        if ( hue >= 360 )
        {
            hue -= 360;

            if ( hue >= 360 )
                hue = hue % 360;
        }

        return d_rgbTable[hue];
    }

    virtual unsigned char colorIndex( const QwtInterval &, double ) const
    {
        // we don't support indexed colors
        return 0;
    }


private:
    void updateTable()
    {
        for ( int i = 0; i < 360; i++ )
            d_rgbTable[i] = QColor::fromHsv( i, d_saturation, d_value ).rgb();

        d_rgbMin = d_rgbTable[ d_hue1 % 360 ];
        d_rgbMax = d_rgbTable[ d_hue2 % 360 ];
    }

    int d_hue1, d_hue2, d_saturation, d_value;
    QRgb d_rgbMin, d_rgbMax, d_rgbTable[360];
};

class AlphaColorMap: public QwtAlphaColorMap
{
public:
    AlphaColorMap()
    {
        setColor( QColor("SteelBlue") );
    }
};

QMSMHeatmap::QMSMHeatmap( QWidget *parent, int alpha, int intEnd, int intStart, int update, QString caption, int zStart, int zEnd):
    QwtPlot( parent ),
    mpSpectrogram(new QwtPlotSpectrogram()),
    mpMatrix (new QwtMatrixRasterData()),
    mAlpha(alpha),
    mIntervalEnd(intEnd),
    mIntervalStart(intStart),
    mUpdate(update)
{
    mY.push_back(0);
    mpMatrix->setValueMatrix(mY, mY.size());
    mpMatrix->setInterval( Qt::XAxis, QwtInterval( mIntervalStart, mIntervalEnd ) );
    mpMatrix->setInterval( Qt::YAxis, QwtInterval( 0, 1 ) );
    mpMatrix->setInterval( Qt::ZAxis, QwtInterval( zStart, zEnd ) );


    mpSpectrogram->setData( mpMatrix );
    mpSpectrogram->setRenderThreadCount( 0 ); // use system specific thread count
    mpSpectrogram->setCachePolicy( QwtPlotRasterItem::PaintCache );

    QList<double> contourLevels;
    for ( double level = 0.5; level < 10.0; level += 1.0 )
        contourLevels += level;
    mpSpectrogram->setContourLevels( contourLevels );
    mpSpectrogram->attach( this );

    const QwtInterval zInterval = mpSpectrogram->data()->interval( Qt::ZAxis );
    setAxisScale( QwtPlot::yRight, zInterval.minValue(), zInterval.maxValue() );

    // A color bar on the right axis
    QwtScaleWidget *rightAxis = axisWidget( QwtPlot::yRight );
    QwtText txt (caption);
    rightAxis->setTitle( txt );
    rightAxis->setColorBarEnabled( true );

    enableAxis( QwtPlot::yRight );
    enableAxis( QwtPlot::yLeft, false);

    plotLayout()->setAlignCanvasToScales( true );

    setColorMap( QMSMHeatmap::RGBMap );
}

void QMSMHeatmap::updateValues(std::vector<double> &values, int val)
{
    this->mTime.push_back(values[X]);
    switch(val)
    {
    case UtilCpu:
        this->mY.push_back(values[YUtilCpu]);
        break;
    case UtilGpuCore:
        this->mY.push_back(values[YUtilGpuCore]);
        break;
    case UtilGpuMem:
        this->mY.push_back(values[YUtilGpuMem]);
        break;
    case UtilFpga:
        this->mY.push_back(values[YUtilFpga]);
        break;
    case UtilMic:
        this->mY.push_back(values[YUtilMic]);
        break;
    case TempCpu0:
        this->mY.push_back(values[YTempCpu0]);
        break;
    case TempCpu1:
        this->mY.push_back(values[YTempCpu1]);
        break;
    case TempGpu:
        this->mY.push_back(values[YTempGpu]);
        break;
    case TempFpgaCompute:
        this->mY.push_back(values[YTempFpgaM]);
        break;
    case TempFpgaInterface:
        this->mY.push_back(values[YTempFpgaI]);
        break;
    case TempMicDie:
        this->mY.push_back(values[YTempMicDie]);
        break;
    case TempMainBoard:
        this->mY.push_back(values[YTempSystem]);
        break;
    }
}

void QMSMHeatmap::redraw()
{
    mpMatrix->setValueMatrix(mY, mY.size());
   // mpMatrix->setInterval( Qt::XAxis, QwtInterval( mIntervalStart, mIntervalEnd ) );
    mpSpectrogram->setData( mpMatrix );

    int32_t first_tick = 0, second_tick = 0;
    double time_show_data	= 60;
    double xValue = mTime.size();

    first_tick		= ((int)(xValue-time_show_data)/10)*10;
    first_tick		= (first_tick >= 0 && (xValue-time_show_data) > 0) ? first_tick+10 : first_tick;
    second_tick		= ((int)(xValue)/10)*10+10;

    setAxisScale(QwtPlot::xBottom, first_tick, second_tick);
    mIntervalEnd = second_tick;
    mpMatrix->setInterval( Qt::XAxis, QwtInterval( mIntervalStart, mIntervalEnd ) );
    mUpdate++;

    if(mUpdate >= 10)
    {
        replot();
        mUpdate = 0;
    }
}

void QMSMHeatmap::setColorMap( int type )
{
    QwtScaleWidget *axis = axisWidget( QwtPlot::yRight );
    const QwtInterval zInterval = mpSpectrogram->data()->interval( Qt::ZAxis );
    mMapType = type;

    int alpha = mAlpha;
    switch( type )
    {
        case QMSMHeatmap::HueMap:
        {
            mpSpectrogram->setColorMap( new HueColorMap() );
            axis->setColorMap( zInterval, new HueColorMap() );
            break;
        }
        case QMSMHeatmap::AlphaMap:
        {
            alpha = 255;
            mpSpectrogram->setColorMap( new AlphaColorMap() );
            axis->setColorMap( zInterval, new AlphaColorMap() );
            break;
        }
        case QMSMHeatmap::IndexMap:
        {
            mpSpectrogram->setColorMap( new LinearColorMapIndexed() );
            axis->setColorMap( zInterval, new LinearColorMapIndexed() );
            break;
        }
        case QMSMHeatmap::RGBMap:
        default:
        {
            mpSpectrogram->setColorMap( new LinearColorMapRGB() );
            axis->setColorMap( zInterval, new LinearColorMapRGB() );
        }
    }
    mpSpectrogram->setAlpha( alpha );

    replot();

}

void QMSMHeatmap::setAlpha( int alpha )
{
    mAlpha = alpha;
    if ( mMapType != QMSMHeatmap::AlphaMap )
    {
        mpSpectrogram->setAlpha( alpha );
        replot();
    }
}


