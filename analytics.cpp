#include "analytics.h"
#include "ui_analytics.h"
#include "home.h" // Include home.h here
#include <QToolTip> // Include QToolTip

Analytics::Analytics(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Analytics)
{
    ui->setupUi(this);
    setupPieChart();
    setupBarChart();
}

void Analytics::on_homeButton_clicked()
{
    this->hide();
    homeWindow = new Home;
    homeWindow->showFullScreen();
}

Analytics::~Analytics()
{
    delete ui;
}

void Analytics::setupPieChart()
{
    // Create the pie series
    QPieSeries *series = new QPieSeries();
    series->setHoleSize(0.3);

    // Append slices and set colors
    QPieSlice *foodSlice = series->append("Food", 30);
    foodSlice->setColor(QColor(0, 168, 107));

    QPieSlice *rentSlice = series->append("Rent", 10);
    rentSlice->setColor(QColor(15, 82, 186));

    QPieSlice *educationSlice = series->append("Education", 20);
    educationSlice->setColor(QColor(0, 128, 255));

    QPieSlice *healthSlice = series->append("Health", 20);
    healthSlice->setColor(QColor(210, 31, 60));

    QPieSlice *entertainmentSlice = series->append("Entertainment", 20);
    entertainmentSlice->setColor(QColor(239, 130, 13));

    // Connect the hovered signal to the slot
    connect(series, &QPieSeries::hovered, this, &Analytics::onSliceHovered);

    // Create the chart
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Expense Distribution");
    chart->setTheme(QChart::ChartThemeLight);
    chart->setAnimationOptions(QChart::SeriesAnimations);
    // Hide the legend
    chart->legend()->setVisible(false);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    chartView->resize(1200, 1000); // Adjust width and height as needed

    // Add the chart view to an existing layout in the UI
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(chartView);
    ui->pieChartWidget->setLayout(layout);
}

void Analytics::setupBarChart()
{
    // Create the bar series
    QBarSeries *series = new QBarSeries();
    QBarSet *set_1 = new QBarSet("Shubham is Great");
    set_1->append(8400);
    set_1->append(9000);
    set_1->append(7234);
    set_1->append(8000);
    set_1->append(10000);
    series->append(set_1);

    connect(set_1, &QBarSet::hovered, this, [this, set_1](bool state) {
        onBarHovered(set_1, state);
    });
    // Create the chart
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Monthly Expenditure");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    // Set up the X axis
    QStringList subjectNames;
    subjectNames.append("July");
    subjectNames.append("August");
    subjectNames.append("September");
    subjectNames.append("October");
    subjectNames.append("November");
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(subjectNames);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    // Set up the Y axis
    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(0, 15000);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    // Set up the legend
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    chartView->resize(1200, 1000); // Adjust width and height as needed

    // Add the chart view to an existing layout in the UI
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(chartView);
    ui->barChartWidget->setLayout(layout);
}

void Analytics::onSliceHovered(QPieSlice *slice, bool state)
{
    // Explode the slice on hover
    slice->setExploded(state);

    // Display the tooltip with the name and percentage
    if (state)
    {
        double percentage = slice->percentage() * 100; // Get percentage
        QString tooltipText = QString("%1: %2%").arg(slice->label()).arg(percentage, 0, 'f', 1);
        QToolTip::showText(QCursor::pos(), tooltipText);
    }
    else
    {
        QToolTip::hideText(); // Hide tooltip when not hovered
    }
}

void Analytics::onBarHovered(QBarSet *barSet, bool state)
{
    if (state)
    {
        // Get the value of the bar set
        QString tooltipText = QString::number(barSet->at(0)); // Adjust index based on your needs
        QToolTip::showText(QCursor::pos(), tooltipText);
    }
    else
    {
        QToolTip::hideText(); // Hide tooltip when not hovered
    }
}


