#ifndef QMSMSETTINGS_H
#define QMSMSETTINGS_H

#include <QWidget>

namespace Ui {
class QMSMSettings;
}

class QMSMSettings : public QWidget
{
    Q_OBJECT

public:
    explicit QMSMSettings(QWidget *parent = 0);
    ~QMSMSettings();

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


private:
    Ui::QMSMSettings *ui;
    void connectSignals();
};

#endif // QMSMSETTINGS_H
