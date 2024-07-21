#ifndef ANALYTICS_H
#define ANALYTICS_H

#include <QMainWindow>

#include    <QtCore>
#include    <QtGui>
#include    <QtWidgets>
#include    <QtCharts>

class Home;
namespace Ui {
class Analytics;
}

class Analytics : public QMainWindow
{
    Q_OBJECT

public:
    explicit Analytics(QWidget *parent = nullptr);
    ~Analytics();

private slots:
    void on_pushButton_clicked();

private:
    Ui::Analytics *ui;
    Home *homeWindow;

};

#endif // ANALYTICS_H
