#include "analytics.h"
#include "ui_analytics.h"
#include "home.h" // Include home.h here
#include <QToolTip> // Include QToolTip

Analytics::Analytics(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Analytics)
{
    ui->setupUi(this);
    // Connecting to the SQLite database
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("C:/Programming/project/lekhaExpenseManager/database/mydb.db"); // SQLite database location
    db.open();

    if (!db.open()) {
        qDebug() << "Failed to connect to database: " << db.lastError();
        return;
    }

    setupPieChart();
    setupBarChart();
}
Analytics::~Analytics()
{
    delete ui;
}

void Analytics::on_homeButton_clicked()
{
    this->hide();
    homeWindow = new Home;
    homeWindow->showFullScreen();
}

// Get the total expense from the userrec table
double Analytics::getTotalExpense(){
    QSqlQuery query;
    query.prepare("SELECT amount FROM userrec");
    if (!query.exec()) {
        qDebug() << "Error executing query:" << query.lastError();
        return 0.0;
    }
    double totalAmount = 0.0;
    // Loop through the query results and sum up the amounts
    while(query.next()){
        totalAmount += query.value(0).toDouble();
    }
    return totalAmount;
}


// Get the percentage of expenses for a specific category
double Analytics::getExpensePercent(const QString &category){
    QSqlQuery query;
    query.prepare("SELECT amount FROM userrec WHERE category = :category");
    query.bindValue(":category", category);

    if (!query.exec()) {
        qDebug() << "Error executing query:" << query.lastError();
        return 0.0;
    }

    double categoryAmount = 0.0;
    while (query.next()) {
        categoryAmount += query.value(0).toDouble();
    }

    // Calculate the percentage of the category expense relative to the total expense
    double totalExpense = getTotalExpense();
    qDebug() << totalExpense;
    qDebug() << categoryAmount;
    double percentage = 0.0;
    if (totalExpense > 0) {
        percentage = (categoryAmount / totalExpense) * 100;
    }

    return percentage;
}


void Analytics::setupPieChart()
{
    // Create the pie series
    QPieSeries *series = new QPieSeries();
    series->setHoleSize(0.3);

    // Append slices and set colors
    double foodExpense = getExpensePercent("food");
    QPieSlice *foodSlice = series->append("Food", foodExpense);
    foodSlice->setColor(QColor(0, 168, 107));

    double rentExpense = getExpensePercent("rent");
    QPieSlice *rentSlice = series->append("Rent", rentExpense);
    rentSlice->setColor(QColor(15, 82, 186));

    double educationExpense = getExpensePercent("education");
    QPieSlice *educationSlice = series->append("Education", educationExpense);
    educationSlice->setColor(QColor(0, 128, 255));

    double healthExpense = getExpensePercent("health");
    QPieSlice *healthSlice = series->append("Health", healthExpense);
    healthSlice->setColor(QColor(210, 31, 60));

    double entertainmentExpense = getExpensePercent("entertainment");
    QPieSlice *entertainmentSlice = series->append("Entertainment", entertainmentExpense);
    entertainmentSlice->setColor(QColor(239, 130, 13));

    double miscellaneousExpense = getExpensePercent("miscellaneous");
    QPieSlice *miscellaneousSlice = series->append("Miscellaneous", miscellaneousExpense);
    miscellaneousSlice->setColor(QColor(100, 108, 17));

    // Connect the hovered signal to the slot
    connect(series, &QPieSeries::hovered, this, &Analytics::onSliceHovered);

    // Create the chart
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Expense Distribution");
    chart->setTheme(QChart::ChartThemeLight);
    chart->setAnimationOptions(QChart::SeriesAnimations);
    // Hide the legend
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignRight);

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


