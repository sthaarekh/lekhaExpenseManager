#ifndef CAPITAL_H
#define CAPITAL_H

#include <QSqlError>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDate>
#include <QDebug>
#include "analytics.h"

class Capital {
public:
    Capital(Analytics& analytics);
    static bool insertCapitalForMonth(double capital);
    static bool recordExpense(double amount);
    static double getCapital(int month, int year);
    static double getTotalExpense(int year, int month);
    static double getExpensePercent(const QString &category, int year, int month);
    static double getTotalLB(const QString &tag);
    static void checkAndInsertCapitalForMonth();
    static double getAvailableBalance();
    static bool addCapital();
    static void updateAvailableBalance(double newBalance);
    static void editAvailableBalance(double amount, const QString &tag);
    static bool storeBudgetS(int month, int year);
    static double getBudgetS(int year, int month);
protected slots:


private:
    QSqlDatabase db;
    Analytics& analyticsRef;


};

#endif
