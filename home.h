#ifndef HOME_H
#define HOME_H

#include"analytics.h"
#include <QDialog>
#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QApplication>
#include <QMessageBox>
#include <QString>
#include <QWidget>
#include <QMessageBox>
#include <QDebug>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>

namespace Ui {
class Home;
}

class Home : public QMainWindow
{
    Q_OBJECT

public:
    explicit Home(QWidget *parent = nullptr);
    QMessageBox msgBox;
    ~Home();

private slots:
    void on_pushButton_clicked();
    void on_lendButton_clicked();
    void on_borrowButton_clicked();
    void on_pushText_clicked();


private:
    Ui::Home *ui;
    Analytics *analytics;
    QSqlDatabase db;
    void insertData(int amount, const QString &description, const QString &tag);
    QString fetchData();
    void displayData();
    void deleteItem();
};

#endif // HOME_H
