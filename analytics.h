#ifndef ANALYTICS_H
#define ANALYTICS_H

#include <QMainWindow>

namespace Ui {
class Analytics;
}

class Analytics : public QMainWindow
{
    Q_OBJECT

public:
    explicit Analytics(QWidget *parent = nullptr);
    ~Analytics();

private:
    Ui::Analytics *ui;
};

#endif // ANALYTICS_H
