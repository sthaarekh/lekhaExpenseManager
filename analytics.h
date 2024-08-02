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
#include <QToolTip>
#include <QDate>
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
    void onBarHovered(QBarSet *barSet, bool state, int index);
    double getExpensePercent(const QString &category, int year, int month);
    double getTotalExpense(int year, int month);


    void on_calendarWidget_currentPageChanged(int year, int month);

private:
    Ui::Analytics *ui;
    Home *homeWindow;
    QSqlDatabase db;
    int selectedYear;
    int selectedMonth;
};

#endif // ANALYTICS_H
