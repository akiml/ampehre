#ifndef QMSMSETTINGS_H
#define QMSMSETTINGS_H

#include <QWidget>
#include <vector>
#include <iostream>
#include <sstream>
#include <inttypes.h>

enum SLIDER
{
  GUI, DATA_PLOT, SLIDER_VALUESIZE
};

enum FREQ
{
    FREQ_CPU, FREQ_GPU, FREQ_FPGA, FREQ_MIC, FREQ_SYSTEM, FREQ_SIZE
};


namespace Ui {
class QMSMSettings;
}

class QMSMSettings : public QWidget
{
    Q_OBJECT

public:
    explicit QMSMSettings(QWidget *parent = 0);
    ~QMSMSettings();

    void setSliderPos(std::vector<int>& val);

signals:
    void signal_start();
    void signal_stop();

    void signal_guiRate(int);
    void signal_dataPlot(int);

public slots:
    void emit_start();
    void emit_stop();

    void emit_guiRate(int v);
    void emit_dataPlot(int v);

    void setFreqLabels(std::vector<uint64_t> &v);

private:
    Ui::QMSMSettings *ui;
    void connectSignals();
};

#endif // QMSMSETTINGS_H
