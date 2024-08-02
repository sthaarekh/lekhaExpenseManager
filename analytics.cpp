#include "analytics.h"
#include "ui_analytics.h"
#include "home.h" // Include home.h here

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
        qDebug() << "Failed to connect to database: " << db.lastError();
        return;
    }

    QDate currentDate = QDate::currentDate();
    selectedYear = currentDate.year();
    selectedMonth = currentDate.month();

    setupPieChart();
    setupBarChart();
}
// Helper function to get month names up to four months before the current month
QStringList getLastFourMonths() {
    QStringList monthNames;
    QDate currentDate = QDate::currentDate();
    for (int i = 0; i < 5; ++i) {
        monthNames.prepend(currentDate.addMonths(-i).toString("MMMM"));
    }
    return monthNames;
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


// Get the total expense from the userrec table
double Analytics::getTotalExpense(int year, int month) {
    QSqlQuery query;
    query.prepare("SELECT SUM(amount) FROM userrec WHERE "
                  "strftime('%Y', timestamp) = :year AND strftime('%m', timestamp) = :month");
    query.bindValue(":year", QString::number(year));
    query.bindValue(":month", QString("%1").arg(month, 2, 10, QChar('0')));  // Format month as two digits

    if (!query.exec()) {
        qDebug() << "Error executing query:" << query.lastError();
        return 0.0;
    }

    double totalExpense = 0.0;
    if (query.next()) {
        totalExpense = query.value(0).toDouble();
    }
    qDebug()<<"Total Expense:"<<totalExpense;
    return totalExpense;
}


// Get the percentage of expenses for a specific category
double Analytics::getExpensePercent(const QString &category, int year, int month) {
    QSqlQuery query;
    query.prepare("SELECT amount FROM userrec WHERE category = :category AND "
                  "strftime('%Y', timestamp) = :year AND strftime('%m', timestamp) = :month");
    query.bindValue(":category", category);
    query.bindValue(":year", QString::number(year));
    query.bindValue(":month", QString("%1").arg(month, 2, 10, QChar('0')));  // Format month as two digits

    if (!query.exec()) {
        qDebug() << "Error executing query:" << query.lastError();
        return 0.0;
    }

    double categoryAmount = 0.0;
    while (query.next()) {
        categoryAmount += query.value(0).toDouble();
    }

    // Calculate the percentage of the category expense relative to the total expense
    double totalExpense = getTotalExpense(year, month);
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
    double foodExpense = getExpensePercent("food", selectedYear, selectedMonth);
    qDebug() << foodExpense;
    QPieSlice *foodSlice = series->append("Food", foodExpense);
    foodSlice->setColor(QColor(0, 168, 107));

    double rentExpense = getExpensePercent("rent", selectedYear, selectedMonth);
    QPieSlice *rentSlice = series->append("Rent", rentExpense);
    rentSlice->setColor(QColor(15, 82, 186));

    double educationExpense = getExpensePercent("education", selectedYear, selectedMonth);
    QPieSlice *educationSlice = series->append("Education", educationExpense);
    educationSlice->setColor(QColor(0, 128, 255));

    double healthExpense = getExpensePercent("health", selectedYear, selectedMonth);
    QPieSlice *healthSlice = series->append("Health", healthExpense);
    healthSlice->setColor(QColor(210, 31, 60));

    double entertainmentExpense = getExpensePercent("entertainment", selectedYear, selectedMonth);
    QPieSlice *entertainmentSlice = series->append("Entertainment", entertainmentExpense);
    entertainmentSlice->setColor(QColor(239, 130, 13));

    double miscellaneousExpense = getExpensePercent("miscellaneous", selectedYear, selectedMonth);
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


void Analytics::setupBarChart()
{
    // // Create the bar series
    // QBarSeries *series = new QBarSeries();
    // QBarSet *set_1 = new QBarSet("Shubham is Great");
    // set_1->append(8400);
    // set_1->append(9000);
    // set_1->append(7234);
    // set_1->append(8000);
    // set_1->append(10000);
    // series->append(set_1);

    // Create the bar series
    QBarSeries *series = new QBarSeries();

    // Get the last four months
    QStringList subjectNames = getLastFourMonths();

    // Create individual bar sets for each month
    for (int i = 0; i < subjectNames.size(); ++i) {
        QBarSet *set = new QBarSet(subjectNames.at(i));
        // Append a single value to each bar set
        if (i == 0) {
            set->append(8400);
        } else if (i == 1) {
            set->append(9000);
        } else if (i == 2) {
            set->append(7234);
        } else if (i == 3) {
            set->append(8000);
        }
        // Append each bar set to the series
        series->append(set);
    }

    // Create an additional bar set for the last value
    QBarSet *set_5 = new QBarSet(subjectNames.at(0));
    set_5->append(10000);
    series->append(set_5);

    // Adjust the bar width to create space between the sets
    series->setBarWidth(1); // Adjust this value to create the desired margin

    // Set colors for each bar set if desired
    for (int i = 0; i < series->count(); ++i) {
        QBarSet *set = series->barSets().at(i);
        if (i == 0) {
            set->setColor(QColor(100, 150, 17));
        } else if (i == 1) {
            set->setColor(QColor(200, 100, 50));
        } else if (i == 2) {
            set->setColor(QColor(50, 100, 200));
        } else if (i == 3) {
            set->setColor(QColor(150, 50, 100));
        } else if (i == 4) {
            set->setColor(QColor(100, 50, 150));
        }
    }

    // Connect the hovered signal of each bar within the bar set
    auto connectHoveredSignal = [this](QBarSet* set) {
        for (int i = 0; i < set->count(); ++i) {
            connect(set, &QBarSet::hovered, this, [this, set, i](bool state) {
                onBarHovered(set, state, i);
            });
        }
    };

    for (int i = 0; i < series->count(); ++i) {
        QBarSet *set = series->barSets().at(i);
        connectHoveredSignal(set);
    }

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


// Function to get the selected month and year in the calender
void Analytics::on_calendarWidget_currentPageChanged(int year, int month)
{
    qDebug() << "selected month" << month;
    qDebug() << "selected year" << year;

    // Store the selected year and month
    selectedYear = year;
    selectedMonth = month;

    // Update the pie chart
    setupPieChart();
}


