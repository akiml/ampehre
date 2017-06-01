#ifndef QMSMABOUT_H
#define QMSMABOUT_H

#include <QWidget>

namespace Ui {
class QMSMabout;
}

class QMSMabout : public QWidget
{
    Q_OBJECT

public:
    explicit QMSMabout(QWidget *parent = 0);
    ~QMSMabout();

private:
    Ui::QMSMabout *ui;
};

#endif // QMSMABOUT_H
