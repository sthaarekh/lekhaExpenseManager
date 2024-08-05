#include "ui_analytics.h"
#include "analytics.h"
#include "home.h"
#include "capital.h"

Analytics::Analytics(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Analytics)
{
    ui->setupUi(this);
    // Hide the title bar and borders
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

    // Ensure the background is not painted (no background)
    this->setAttribute(Qt::WA_OpaquePaintEvent, true);

    // Disable the window's resizing functionality
    this->setFixedSize(this->width(), this->height());


    // Connecting to the SQLite database
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("/Users/sthaarekh/Documents/       /lekhaEx/shubham/database/mydb-2.db"); // SQLite database location
    db.open();

    if (!db.open()) {
        qDebug() << "Failed to connect to database analytics: " << db.lastError();
        return;
    }

    QDate currentDate = QDate::currentDate();
    selectedYear = currentDate.year();
    selectedMonth = currentDate.month();
    displayCapital();
    displayLB();
    showAvailableBalance();
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
    homeWindow->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    homeWindow->setAttribute(Qt::WA_OpaquePaintEvent, true);
    homeWindow->setFixedSize(this->width(), this->height());
    homeWindow->show();
}


// Helper function to get month names up to four months before the current month
QStringList getLastFourMonths() {
    QStringList monthNames;
    QDate currentDate = QDate::currentDate();
    for (int i = 1; i < 6; ++i) {
        monthNames.prepend(currentDate.addMonths(-i).toString("MMMM"));
    }
    return monthNames;
}


// Helper function to get the expense data for the last four months
QList<double> getLastFourMonthsData() {
    QList<double> expenses;
    QDate currentDate = QDate::currentDate();
    for (int i = 1; i < 6; ++i) {
        QDate date = currentDate.addMonths(-i);
        double expense = Capital::getTotalExpense(date.year(), date.month());
        expenses.prepend(expense);
    }
    return expenses;
}


void Analytics::setupPieChart()
{
    // Create the pie series
    QPieSeries *series = new QPieSeries();
    series->setHoleSize(0.3);

    // Append slices and set colors
    double foodExpense = Capital::getExpensePercent("food", selectedYear, selectedMonth);
    QPieSlice *foodSlice = series->append("Food", foodExpense);
    foodSlice->setColor(QColor(0, 168, 107));

    double housingExpense = Capital::getExpensePercent("housing", selectedYear, selectedMonth);
    QPieSlice *housingSlice = series->append("Housing", housingExpense);
    housingSlice->setColor(QColor(15, 82, 186));

    double educationExpense = Capital::getExpensePercent("education", selectedYear, selectedMonth);
    QPieSlice *educationSlice = series->append("Education", educationExpense);
    educationSlice->setColor(QColor(0, 128, 255));

    double healthExpense = Capital::getExpensePercent("health", selectedYear, selectedMonth);
    QPieSlice *healthSlice = series->append("Health", healthExpense);
    healthSlice->setColor(QColor(210, 31, 60));

    double transportationExpense = Capital::getExpensePercent("transportation", selectedYear, selectedMonth);
    QPieSlice *transportationSlice = series->append("Transportation", transportationExpense);
    transportationSlice->setColor(QColor(52, 73, 94));

    double entertainmentExpense = Capital::getExpensePercent("entertainment", selectedYear, selectedMonth);
    QPieSlice *entertainmentSlice = series->append("Entertainment", entertainmentExpense);
    entertainmentSlice->setColor(QColor(239, 130, 13));

    double miscellaneousExpense = Capital::getExpensePercent("miscellaneous", selectedYear, selectedMonth);
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
    chart->setBackgroundBrush(Qt::NoBrush);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    chartView->resize(2500, 2000); // Adjust width and height as needed

    // Remove existing layout from the pieChartWidget if any
    QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(ui->pieChartWidget->layout());
    if (layout) {
        QLayoutItem *item;
        while ((item = layout->takeAt(0)) != nullptr) {
            delete item->widget(); // Delete the widget
            delete item; // Delete the layout item
        }
        delete layout; // Delete the layout itself
    }

    // Create a new layout and add the chartView to it
    layout = new QVBoxLayout();
    layout->addWidget(chartView);
    ui->pieChartWidget->setLayout(layout);
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


void Analytics::setupBarChart()
{
    // Create the bar series
    QBarSeries *series = new QBarSeries();

    // Get the last four months and their data
    QStringList subjectNames = getLastFourMonths();
    QList<double> dataValues = getLastFourMonthsData();

    // Create a single bar set for the last four months
    QBarSet *set = new QBarSet("Monthly Expenditure");

    // Append the dynamic values to the bar set
    for (double value : dataValues) {
        set->append(value);
    }
    series->append(set);

    // Create the chart
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Monthly Expenditure");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    // Set up the X axis
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(subjectNames);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    // Set up the Y axis
    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(0, 15000); // Adjust this range as necessary
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    // Set up the legend
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    chartView->resize(1200, 1000); // Adjust width and height as needed

    // Create a widget to hold the layout
    QWidget *chartContainer = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(chartContainer);
    layout->addWidget(chartView);


    // Add the chart container to an existing layout in the UI
    ui->barChartWidget->setLayout(new QVBoxLayout());
    ui->barChartWidget->layout()->addWidget(chartContainer);
}


void Analytics::onBarHovered(QBarSet *barSet, bool state, int index)
{
    if (state)
    {
        // Get the value of the specific bar that is hovered
        QString tooltipText = QString::number(barSet->at(index));
        QToolTip::showText(QCursor::pos(), tooltipText);
    }
    else
    {
        QToolTip::hideText(); // Hide tooltip when not hovered
    }
}



void Analytics::displayCapital(){
    double amount = Capital::getCapital(selectedMonth,selectedYear);
    // Convert the double to QString
    QString amountString = QString::number(amount);
    // Set the text of the QLabel
    ui->capitalDisplay->setText(amountString);
}

bool Analytics::displayLB(){
    double amountL = Capital::getTotalLB("lend");
    double amountB = Capital::getTotalLB("borrow");
    // Convert the double to QString
    QString amountLString = QString::number(amountL);
    QString amountBString = QString::number(amountB);
    // Set the text of the QLabel
    ui->lendDisplay->setText(amountLString);
    ui->borrowDisplay->setText(amountBString);
}

// Function to get the selected month and year in the calender
void Analytics::on_calendarWidget_currentPageChanged(int year, int month)
{
    // Store the selected year and month
    selectedYear = year;
    selectedMonth = month;

    // Update the pie chart
    setupPieChart();
    displayCapital();
}

void Analytics::showAvailableBalance(){
    double amount = Capital::getAvailableBalance();
    ui->avBalance->setText(QString::number(amount));
}


void Analytics::on_pushButton_clicked()
{
    Capital::addCapital();
    Analytics::displayCapital();
    Analytics::showAvailableBalance();
}

