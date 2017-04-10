#ifndef QMSMSYSTEMOVERVIEW_H
#define QMSMSYSTEMOVERVIEW_H

#include <QWidget>
#include "gui/QMSMutilplot.h"
#include "gui/QMSMtemperatureplot.h"

namespace Ui {
class QMSMSystemOverview;
}

class QMSMSystemOverview : public QWidget
{
    Q_OBJECT

public:
    explicit QMSMSystemOverview(QWidget *parent = 0);
    ~QMSMSystemOverview();


public slots:
    void update(std::vector<double>& v);

private:
    Ui::QMSMSystemOverview *ui;

    void updateUtil(std::vector<double> &v);
    void updateTemp(std::vector<double> &v);


};

#endif // QMSMSYSTEMOVERVIEW_H
