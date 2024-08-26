#include "capital.h"
#include "analytics.h"
#include "home.h"
#include <QInputDialog>
#include <QMessageBox>


Capital::Capital(Analytics& analytics) : analyticsRef(analytics) {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("/Users/sthaarekh/Documents/       /lekhaFinal/database/mydb.db");
    if (!db.open()) {
        qDebug() << "Error: Connection with database failed in capital";
    }

    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS capital (id INTEGER PRIMARY KEY AUTOINCREMENT, amount REAL, month INTEGER, year INTEGER)");
    checkAndInsertCapitalForMonth();
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
    double amount = QInputDialog::getDouble(nullptr, "Add Capital", "Enter amount to add to the capital:", 0, 0, 1000000, 0, &ok);
    if (ok) {
        double newCapital = currentCapital + amount;
        double newBalance = amount + getAvailableBalance();
        updateAvailableBalance(newBalance);

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


void Capital::checkAndInsertCapitalForMonth() {
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

        } else {
            // Capital for the current month is not available
            bool ok;
            double capital = QInputDialog::getDouble(nullptr, "Enter Capital", "Enter capital for the current month:", 0, 0, 1000000, 2, &ok);

            if (ok) {
                // Insert the capital into the database
                insertCapitalForMonth(capital);
                updateAvailableBalance(capital);
                storeBudgetS(currentMonth, currentYear);
            } else {
                QMessageBox::critical(nullptr, "Error", "Please enter a valid capital amount.");
            }
        }
    } else {
        qDebug() << "Failed to check existing capital records:" << query.lastError();
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


// Get the total expense from the userrec table
double Capital::getTotalExpense(int year, int month) {
    QSqlQuery query;
    query.prepare("SELECT SUM(amount) FROM userrec WHERE "
                  "strftime('%Y', timestamp) = :year AND strftime('%m', timestamp) = :month AND payMode= 'self'");
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
    QSqlQuery query1;
    query1.prepare("SELECT amount FROM userrec WHERE payMode = 'borrow'");
    if (!query.exec()) {
        qDebug() << "Error executing query at getTotalLB:" << query.lastError();
        return 0.0;
    }

    double totalAmount = 0.0;
    while (query.next()) {
        totalAmount += query.value(0).toDouble();
    }
    if(tag=="borrow"){
        if (!query1.exec()) {
            qDebug() << "Error executing query at getTotalLend:" << query.lastError();
            return 0.0;
        }
        while (query1.next()) {
            totalAmount += query1.value(0).toDouble();
        }
    }
    return totalAmount;
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

void Capital::editAvailableBalance(double amount, const QString &tag){
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
    else if(tag=="expense"){
        availableBalance -= amount;
    }
    updateAvailableBalance(availableBalance);
}

void Capital::updateAvailableBalance(double newBalance){
    QSqlQuery query;
    query.prepare("UPDATE balance SET balance = :balance WHERE id =1 ");
    query.bindValue(":balance", newBalance);
    if (query.exec()) {
        Home::callAvailableBalance();
    } else {
        qDebug()<< "Failed to insert balance:" << query.lastError();
    }
}

double Capital::getBudgetS(int year, int month){
    QSqlQuery query;
    query.prepare("SELECT budgetS FROM capital WHERE month = :month AND year = :year");
    query.bindValue(":month", month);
    query.bindValue(":year", year);
    if(!query.exec()){
        qDebug() << "Failed to get savings:" << query.lastError();
        return 0.0; // Return a default value on error
    }
    if (query.next()) {
        double budgetS = query.value(0).toDouble();
        return budgetS;
    } else {
        qDebug() << "No record found";
        return 0.0; // Return a default value if no record is found
    }
}


bool Capital::storeBudgetS(int month, int year){
    QSqlQuery query;
    query.prepare("UPDATE capital SET budgetS = :budgetS WHERE month = :month AND year = :year");
    query.bindValue(":month", month);
    query.bindValue(":year", year);

    double newBudget = getCapital(month, year) - getTotalExpense(year, month);
    qDebug()<<newBudget;
    if(newBudget<0){
        newBudget = 0.0;
    }

    query.bindValue(":budgetS", newBudget);
    if (query.exec()) {
        qDebug() << "Budget for the month updated successfully.";
        return true;
    } else {
        qDebug() << "Failed to update budget:" << query.lastError();
        return false;
    }
}
