#include "analytics.h"
#include "ui_analytics.h"
#include "home.h" // Include home.h here
#include <QToolTip> // Include QToolTip

Analytics::Analytics(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Analytics)
{
    ui->setupUi(this);
    setupChart();
}

void Analytics::on_pushButton_clicked()
{
    this->hide();
    homeWindow = new Home;
    homeWindow->showFullScreen();
}

Analytics::~Analytics()
{
    delete ui;
}

void Analytics::setupChart()
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

    // Hide the legend
    chart->legend()->setVisible(false);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    chartView->resize(1200, 1000); // Adjust width and height as needed

    // Add the chart view to an existing layout in the UI
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(chartView);
    ui->chartWidget->setLayout(layout);
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

