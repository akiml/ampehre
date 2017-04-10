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
    void setSaveData(int value);

signals:
    void signal_start();
    void signal_stop();

    void signal_guiRate(int);
    void signal_dataPlot(int);
    void signal_saveData(int);
    void signal_portChanged(int);
    void signal_ipChanged(QString);
    void signal_loadConfig();
    void signal_exportConfig();

public slots:
    void emit_start();
    void emit_stop();
    void stop();
    void start();

    void emit_guiRate(int v);
    void emit_dataPlot(int v);
    void emit_saveData(int v);
    void emit_ipChanged(QString s);
    void emit_portChanged(int v);
    void emit_loadConfig();
    void emit_exportConfig();

    void set_port(int v);
    void set_ip(QString v);

    int get_port();
    QString get_ip();
    int get_maxData();

    void setFreqLabels(std::vector<uint64_t> &v);

private:
    Ui::QMSMSettings *ui;
    void connectSignals();
};

#endif // QMSMSETTINGS_H
