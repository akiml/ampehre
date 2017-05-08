#include "gui/QMSMlinearcolormaprgb.h"

LinearColorMapRGB::LinearColorMapRGB():
    QwtLinearColorMap( Qt::darkCyan, Qt::red, QwtColorMap::RGB )
{
    addColorStop( 0.2, Qt::cyan );
    addColorStop( 0.5, Qt::green );
    addColorStop( 0.85, Qt::yellow );
}

LinearColorMapRGB::~LinearColorMapRGB(){}

//class LinearColorMapRGB: public QwtLinearColorMap
//{
//public:
//    LinearColorMapRGB():
//        QwtLinearColorMap( Qt::darkCyan, Qt::red, QwtColorMap::RGB )
//    {
//        addColorStop( 0.2, Qt::cyan );
//        addColorStop( 0.5, Qt::green );
//        addColorStop( 0.85, Qt::yellow );
//    }
//};

//class LinearColorMapIndexed: public QwtLinearColorMap
//{
//public:
//    LinearColorMapIndexed():
//        QwtLinearColorMap( Qt::darkCyan, Qt::red, QwtColorMap::Indexed )
//    {
//        addColorStop( 0.2, Qt::cyan );
//        addColorStop( 0.5, Qt::green );
//        addColorStop( 0.85, Qt::yellow );
//    }
//};

//class HueColorMap: public QwtColorMap
//{
//public:
//    // class backported from Qwt 6.2

//    HueColorMap():
//        d_hue1(0),
//        d_hue2(359),
//        d_saturation(150),
//        d_value(200)
//    {
//        updateTable();

//    }

//    virtual QRgb rgb( const QwtInterval &interval, double value ) const
//    {
//        if ( qIsNaN(value) )
//            return 0u;

//        const double width = interval.width();
//        if ( width <= 0 )
//            return 0u;

//        if ( value <= interval.minValue() )
//            return d_rgbMin;

//        if ( value >= interval.maxValue() )
//            return d_rgbMax;

//        const double ratio = ( value - interval.minValue() ) / width;
//        int hue = d_hue1 + qRound( ratio * ( d_hue2 - d_hue1 ) );

//        if ( hue >= 360 )
//        {
//            hue -= 360;

//            if ( hue >= 360 )
//                hue = hue % 360;
//        }

//        return d_rgbTable[hue];
//    }

//    virtual unsigned char colorIndex( const QwtInterval &, double ) const
//    {
//        // we don't support indexed colors
//        return 0;
//    }


//private:
//    void updateTable()
//    {
//        for ( int i = 0; i < 360; i++ )
//            d_rgbTable[i] = QColor::fromHsv( i, d_saturation, d_value ).rgb();

//        d_rgbMin = d_rgbTable[ d_hue1 % 360 ];
//        d_rgbMax = d_rgbTable[ d_hue2 % 360 ];
//    }

//    int d_hue1, d_hue2, d_saturation, d_value;
//    QRgb d_rgbMin, d_rgbMax, d_rgbTable[360];
//};

//class AlphaColorMap: public QwtAlphaColorMap
//{
//public:
//    AlphaColorMap()
//    {
//        setColor( QColor("SteelBlue") );
//    }
//};
