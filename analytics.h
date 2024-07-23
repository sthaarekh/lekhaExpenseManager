#ifndef ANALYTICS_H
#define ANALYTICS_H

#include<QSqlDatabase>
#include    <QMainWindow>

#include    <QtCore>
#include    <QtGui>
#include    <QtWidgets>
#include    <QtCharts>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QBarSeries>
#include <QBarSet>
#include <QVBoxLayout>

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
    void on_homeButton_clicked();
    void setupPieChart();
    void setupBarChart();
    void onSliceHovered(QPieSlice *slice, bool state);
    void onBarHovered(QBarSet *barSet, bool state);



private:
    Ui::Analytics *ui;
    Home *homeWindow;

};

#endif // ANALYTICS_H
