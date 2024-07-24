#ifndef ANALYTICS_H
#define ANALYTICS_H

#include <QMainWindow>
#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QtCharts>
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
    double getExpensePercent(const QString &category);
    double getTotalExpense();


private:
    Ui::Analytics *ui;
    Home *homeWindow;
    QSqlDatabase db;
};

#endif // ANALYTICS_H
