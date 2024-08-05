#include "capital.h"
#include "analytics.h"
#include <QInputDialog>
#include <QMessageBox>


Capital::Capital(Analytics& analytics) : analyticsRef(analytics) {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("/Users/sthaarekh/Documents/       /lekhaEx/shubham/database/mydb-2.db");

    if (!db.open()) {
        qDebug() << "Error: Connection with database failed in capital";
    } else {
        qDebug() << "Database: connection ok";
    }

    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS capital (id INTEGER PRIMARY KEY AUTOINCREMENT, amount REAL, month INTEGER, year INTEGER, savings REAL)");
    checkAndInsertCapitalForMonth();
    getSavings();
    // addCapital();
    getTotalLB("borrow");
}


bool Capital::insertCapitalForMonth(double capital) {
    QDate currentDate = QDate::currentDate();
    int currentMonth = currentDate.month();
    int currentYear = currentDate.year();

    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM capital WHERE month = :month AND year = :year");
    query.bindValue(":month", currentMonth);
    query.bindValue(":year", currentYear);

    if (query.exec()) {
        if (query.next() && query.value(0).toInt() == 0) {
            // No record for this month, insert the capital
            query.prepare("INSERT INTO capital (amount, month, year) VALUES (:amount, :month, :year)");
            query.bindValue(":amount", capital);
            query.bindValue(":month", currentMonth);
            query.bindValue(":year", currentYear);

            if (query.exec()) {
                qDebug() << "Capital for the month inserted successfully.";
                return true;
            } else {
                qDebug() << "Failed to insert capital:" << query.lastError();
                return false;
            }
        } else {
            qDebug() << "Capital for the current month already exists.";
            return false;
        }
    } else {
        qDebug() << "Failed to check existing capital records:" << query.lastError();
        return false;
    }
}

bool Capital::addCapital() {
    QDate currentDate = QDate::currentDate();
    int currentMonth = currentDate.month();
    int currentYear = currentDate.year();

    double currentCapital = getCapital(currentMonth, currentYear);
    if (currentCapital == 0.0) {
        // No record for this month, ask user to insert the capital
        bool ok;
        double capital = QInputDialog::getDouble(nullptr, "Insert Capital", "Insert capital for the current month:", 0, 0, 1000000, 2, &ok);
        if (ok) {
            return insertCapitalForMonth(capital);
        } else {
            return false;
        }
    }

    bool ok;
    double amount = QInputDialog::getDouble(nullptr, "Add Capital", "Enter amount to add to the capital:", 0, 0, 1000000, 2, &ok);
    if (ok) {
        double newCapital = currentCapital + amount;
        QSqlQuery query;
        query.prepare("UPDATE capital SET amount = :amount WHERE month = :month AND year = :year");
        query.bindValue(":amount", newCapital);
        query.bindValue(":month", currentMonth);
        query.bindValue(":year", currentYear);

        if (query.exec()) {
            qDebug() << "Capital updated successfully.";
            return true;
        } else {
            qDebug() << "Failed to update capital:" << query.lastError();
            return false;
        }
    } else {
        return false;
    }
}

bool Capital::checkAndInsertCapitalForMonth() {
    QDate currentDate = QDate::currentDate();
    int currentMonth = currentDate.month();
    int currentYear = currentDate.year();

    QSqlQuery query;
    query.prepare("SELECT amount FROM capital WHERE month = :month AND year = :year");
    query.bindValue(":month", currentMonth);
    query.bindValue(":year", currentYear);

    if (query.exec()) {
        if (query.next()) {
            // Capital for the current month is available
            return true;
        } else {
            // Capital for the current month is not available
            bool ok;
            double capital = QInputDialog::getDouble(nullptr, "Enter Capital", "Enter capital for the current month:", 0, 0, 1000000, 2, &ok);

            if (ok) {
                // Insert the capital into the database
                return insertCapitalForMonth(capital);
            } else {
                QMessageBox::critical(nullptr, "Error", "Please enter a valid capital amount.");
                return false;
            }
        }
    } else {
        qDebug() << "Failed to check existing capital records:" << query.lastError();
        return false;
    }
}


// Function to get the capital amount for the specified month and year
double Capital::getCapital(int month, int year) {
    QSqlQuery query;
    query.prepare("SELECT amount FROM capital WHERE month = :month AND year = :year");
    query.bindValue(":month", month);
    query.bindValue(":year", year);

    if (query.exec() && query.next()) { // Make sure to call query.next() to move to the first record
        double capital = query.value(0).toDouble();
        return capital;
    } else {
        qDebug() << "Failed to retrieve capital:" << query.lastError();
        return 0.0;
    }
}

// Function to get the savings for the current month and year
double Capital::getSavings() {
    QDate currentDate = QDate::currentDate();
    int month = currentDate.month();
    int year = currentDate.year();

    double totalExpense = Capital::getTotalExpense(year, month);
    double capital = Capital::getCapital(month, year);
    double savings = capital - totalExpense;

    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM capital WHERE month = :month AND year = :year");
    query.bindValue(":month", month);
    query.bindValue(":year", year);

    if (!query.exec()) {
        qDebug() << "Error checking existing record:" << query.lastError();
        return 0;
    }

    query.next();
    int count = query.value(0).toInt();

    if (count == 0) {
        // No record for this month, insert the capital
        query.prepare("INSERT INTO capital (month, year, savings) VALUES (:month, :year, :savings)");
    } else {
        // Record already exists, update the savings
        query.prepare("UPDATE capital SET savings = :savings WHERE month = :month AND year = :year");
    }

    query.bindValue(":month", month);
    query.bindValue(":year", year);
    query.bindValue(":savings", savings);

    if (query.exec()) {
        qDebug() << "Savings for the month updated successfully.";
        return savings;
    } else {
        qDebug() << "Failed to update savings:" << query.lastError();
        return 0;
    }
}



// Get the total expense from the userrec table
double Capital::getTotalExpense(int year, int month) {
    QSqlQuery query;
    query.prepare("SELECT SUM(amount) FROM userrec WHERE "
                  "strftime('%Y', timestamp) = :year AND strftime('%m', timestamp) = :month");
    query.bindValue(":year", QString::number(year));
    query.bindValue(":month", QString("%1").arg(month, 2, 10, QChar('0')));  // Format month as two digits

    if (!query.exec()) {
        qDebug() << "Error executing query at getToalExpense:" << query.lastError();
        return 0.0;
    }

    double totalExpense = 0.0;
    while (query.next()) {
        totalExpense += query.value(0).toDouble();
    }
    return totalExpense;
}


// Get the percentage of expenses for a specific category
double Capital::getExpensePercent(const QString &category, int year, int month) {
    QSqlQuery query;
    query.prepare("SELECT amount FROM userrec WHERE category = :category AND "
                  "strftime('%Y', timestamp) = :year AND strftime('%m', timestamp) = :month");
    query.bindValue(":category", category);
    query.bindValue(":year", QString::number(year));
    query.bindValue(":month", QString("%1").arg(month, 2, 10, QChar('0')));  // Format month as two digits

    if (!query.exec()) {
        qDebug() << "Error executing query at getExpensePercent:" << query.lastError();
        return 0.0;
    }

    double categoryAmount = 0.0;
    while (query.next()) {
        categoryAmount += query.value(0).toDouble();
    }

    // Calculate the percentage of the category expense relative to the total expense
    double totalExpense = Capital::getTotalExpense(year, month);
    double percentage = 0.0;
    if (totalExpense > 0) {
        percentage = (categoryAmount / totalExpense) * 100;
    }

    return percentage;
}

// Get the total lend,borrow amount from the lendboorow table
double Capital::getTotalLB(const QString &tag) {
    QSqlQuery query;
    query.prepare("SELECT amount FROM lendBorrow WHERE tag = :tag");
    query.bindValue(":tag", tag);

    if (!query.exec()) {
        qDebug() << "Error executing query at getTotalLend:" << query.lastError();
        return 0.0;
    }

    double totalLend = 0.0;
    while (query.next()) {
        totalLend += query.value(0).toDouble();
    }
    qDebug()<<"Total Lend:"<<totalLend;
    return totalLend;
}

double Capital::getAvailableBalance(){
    QSqlQuery query;
    query.prepare("SELECT balance FROM balance WHERE id = 1");
    if(!query.exec()){
        qDebug() << "Failed to get availableBalance:" << query.lastError();
        return 0.0; // Return a default value on error
    }
    if (query.next()) {
        double availableBalance = query.value(0).toDouble();
        return availableBalance;
    } else {
        qDebug() << "No record found";
        return 0.0; // Return a default value if no record is found
    }
}

double Capital::editAvailableBalance(double amount, const QString &tag){
    double availableBalance = Capital::getAvailableBalance();
    if(tag=="lend"){
        availableBalance -= amount;
    }
    else if(tag=="borrow"){
        availableBalance += amount;
    }
    else if(tag=="lendR"){
        availableBalance += amount;
    }
    else if(tag=="borrowR"){
        availableBalance -= amount;
    }
    updateAvailableBalance(availableBalance);
}

bool Capital::updateAvailableBalance(double newBalance){
        QSqlQuery query;
        query.prepare("UPDATE balance SET balance = :balance WHERE id =1 ");
        query.bindValue(":balance", newBalance);
        if (query.exec()) {
                qDebug() << newBalance<< "Balance for the month inserted successfully.";
                return true;
            } else {
                qDebug() << "Failed to insert balance:" << query.lastError();
                return false;
            }
}
